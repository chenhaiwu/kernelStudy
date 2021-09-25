#include <linux/vfio.h>
#include <linux/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// #include <fcntl.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/eventfd.h>

#include <unistd.h>

#include <time.h>

// #define DMA_DRIVER
// #define IRQ

#define VFIO_CONTAINER "/dev/vfio/vfio"

#define VFIO_DMA_MAP_FLAG_EXEC		(1 << 2)


static void dump_hex(const unsigned char *buf, int len)
{
	for (int i = 0; i < len; i++) {
		printf("%02x ", buf[i]);
		if ((i + 1) % 8 == 0)
			printf("\n");
	}
	printf("\n");
}

static void modify_hex(const unsigned char *buf, int len)
{
	/* ramdon modify the data, just plus 1. */
	for (int i = 0; i < len; i++) {
		buf[i]++;
	}
}

static void vfio_irqfd_clean(int device, unsigned int index)
{
    struct vfio_irq_set irq_set = {
		.argsz = sizeof(irq_set),
		.flags = VFIO_IRQ_SET_DATA_NONE | VFIO_IRQ_SET_ACTION_TRIGGER,
		.index = index,
		.start = 0,
		.count = 0,
	};

	int ret = ioctl(device, VFIO_DEVICE_SET_IRQS, &irq_set);

	if (ret) {
		printf("Failure in %s for IRQ %d\n", __func__, index);
		exit(1);
	}
}

static void vfio_irqfd_init(int device, unsigned int index, int fd)
{
	struct vfio_irq_set *irq_set;
	int32_t *pfd;
	int ret, argsz;

	argsz = sizeof(*irq_set) + sizeof(*pfd);
	irq_set = malloc(argsz);

	if (!irq_set) {
		printf("Failure in %s allocating memory\n", __func__);
		exit(1);
	}

	irq_set->argsz = argsz;
	irq_set->flags = VFIO_IRQ_SET_DATA_EVENTFD | VFIO_IRQ_SET_ACTION_TRIGGER;
	irq_set->index = index;
	irq_set->start = 0;
	irq_set->count = 1;
	pfd = (int32_t *)&irq_set->data;
	*pfd = fd;

	ret = ioctl(device, VFIO_DEVICE_SET_IRQS, irq_set);
	free(irq_set);

	if (ret) {
		printf("Failure in %s for IRQ %d\n", __func__, index);
		exit(1);
	}
}

void done_callback(void *user_data)
{
	printf("done!\n");

	char *ptr = (char *)user_data;
	printf("the message is: %s\n", ptr);
}

int main(int argc, char **argv)
{
	int container, group, device;
	unsigned int i;

	struct vfio_group_status group_status = { .argsz = sizeof(group_status) };
	struct vfio_iommu_type1_info iommu_info = { .argsz = sizeof(iommu_info) };
	// source memory area the DMA controller will read from
	struct vfio_iommu_type1_dma_map dma_map_src = { .argsz = sizeof(dma_map_src) };
	// destination memory area the DMA controller will read to
	struct vfio_iommu_type1_dma_map dma_map_dst = { .argsz = sizeof(dma_map_dst) };
	/*
	 * memory area where the DMA controller will grub the instructions
	 * to execute. We will tell to the controller how to reach these
	 * instructions through the DEBUG registers.
	 */
	struct vfio_iommu_type1_dma_map dma_map_inst = { .argsz = sizeof(dma_map_inst) };

	struct vfio_device_info device_info = { .argsz = sizeof(device_info) };

	int ret;

	if (argc != 3) {
		printf("Usage: ./vfio-dt /dev/vfio/${group_id} device_id\n");
		return 2;
	}

	/* Create a new container */
	container = open(VFIO_CONTAINER, O_RDWR);

	if (ioctl(container, VFIO_GET_API_VERSION) != VFIO_API_VERSION) {
		printf("Unknown API version\n");
		return 1;
	}

	if (!ioctl(container, VFIO_CHECK_EXTENSION, VFIO_TYPE1_IOMMU)) {
		printf("Doesn't support the IOMMU driver we want\n");
		return 1;
	}

	/* Open the group */
	group = open(argv[1], O_RDWR);

	/* Test the group is viable and available */
	ioctl(group, VFIO_GROUP_GET_STATUS, &group_status);

	if (!(group_status.flags & VFIO_GROUP_FLAGS_VIABLE)) {
		printf("Group is not viable (not all devices bound for vfio)\n");
		return 1;
	}

	/* Add the group to the container */
	ioctl(group, VFIO_GROUP_SET_CONTAINER, &container);

	/* Enable the IOMMU model we want */
	ioctl(container, VFIO_SET_IOMMU, VFIO_TYPE1_IOMMU);

	/* Get addition IOMMU info */
	ioctl(container, VFIO_IOMMU_GET_INFO, &iommu_info);

	// easy and safer map
	int size_to_map = 4 * 1024;

	// source map for the dma copy
	dma_map_src.vaddr = (u64)((uintptr_t)mmap(NULL, size_to_map, PROT_READ | PROT_WRITE,
			     MAP_PRIVATE | MAP_ANONYMOUS, 0, 0));
	dma_map_src.size = size_to_map;
	dma_map_src.iova = 0;
	dma_map_src.flags = VFIO_DMA_MAP_FLAG_READ | VFIO_DMA_MAP_FLAG_WRITE
						   | VFIO_DMA_MAP_FLAG_EXEC;

	// destination map for the dma copy
	dma_map_dst.vaddr = (u64)((uintptr_t)mmap(NULL, size_to_map, PROT_READ | PROT_WRITE,
			     MAP_PRIVATE | MAP_ANONYMOUS, 0, 0));
	dma_map_dst.size = size_to_map;
	dma_map_dst.iova = dma_map_src.size;
	dma_map_dst.flags = VFIO_DMA_MAP_FLAG_READ | VFIO_DMA_MAP_FLAG_WRITE
						   | VFIO_DMA_MAP_FLAG_EXEC;

	// memory which stores the commands executed by the dma controller
	int cmds_len = size_to_map;
	dma_map_inst.vaddr = (u64)((uintptr_t)mmap(NULL, cmds_len, PROT_READ | PROT_WRITE,
			     MAP_PRIVATE | MAP_ANONYMOUS, 0, 0));
	dma_map_inst.size = cmds_len;
	dma_map_inst.iova = dma_map_src.size + dma_map_dst.size;
	dma_map_inst.flags = VFIO_DMA_MAP_FLAG_READ | VFIO_DMA_MAP_FLAG_WRITE
						    | VFIO_DMA_MAP_FLAG_EXEC;

	ret = ioctl(container, VFIO_IOMMU_MAP_DMA, &dma_map_src);
	ret |= ioctl(container, VFIO_IOMMU_MAP_DMA, &dma_map_dst);
	ret |= ioctl(container, VFIO_IOMMU_MAP_DMA, &dma_map_inst);

	if(ret) {
		printf("Could not map DMA memory\n");
		return 1;
	}

	/* Get a file descriptor for the device */
	device = ioctl(group, VFIO_GROUP_GET_DEVICE_FD, argv[2]);
	printf("=== VFIO device file descriptor %d ===\n", device);

	/* Test and setup the device */
	ret = ioctl(device, VFIO_DEVICE_GET_INFO, &device_info);

	if(ret) {
		printf("Could not get VFIO device\n");
		return 1;
	}

	printf("Device has %d region(s):\n", device_info.num_regions);

	struct vfio_region_info reg = { .argsz = sizeof(reg) };
	uchar *base_regs;

	/* try to test bar0 */
	reg.index = 0;
	ret = ioctl(device, VFIO_DEVICE_GET_REGION_INFO, &reg);

	if(ret) {
		printf("Couldn't get region %d info\n", reg.index);
		return 1;
	}

	printf("- Region %d: size=0x%llx offset=0x%llx flags=0x%x\n",
			reg.index,
			reg.size,
			reg.offset,
			reg.flags );

	base_regs = (uchar *)mmap(NULL, reg.size, PROT_READ | PROT_WRITE, MAP_SHARED,
			device, reg.offset);

	if (base_regs != MAP_FAILED)
		printf("  - Successful MMAP to address %p\n", base_regs);

	/* now we do mmio write to bar space using maping */
	printf("do mmio opt:\n");
	dump_hex(base_regs, 32);
	modify_hex(base_regs, 32);
	dump_hex(base_regs, 32);
	

#if DMA_DRIVER
#ifdef IRQ
	struct vfio_irq_info irq = { .argsz = sizeof(irq) };
	struct vfio_irq_set set = { .argsz = sizeof(set) };

	irq.index = 0;

	ret = ioctl(device, VFIO_DEVICE_GET_IRQ_INFO, &irq);

	if (ret) {
		printf("Couldn't get IRQ %d info\n", irq.index);
		return 1;
	}

	printf("- IRQ %d: range of %d, flags=0x%x\n",
			irq.index,
			irq.count,
			irq.flags );

	// Let's play with that interrupt a bit
	unsigned long long int e;

	int irqfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (irqfd < 0)
		return 1;

	vfio_irqfd_init(device, irq.index, irqfd);

	// init the controller before adding irq
	pl330_vfio_init(base_regs);
	// add the irq to the pl330 controller
	pl330_vfio_add_irq(irqfd, irq.index);

	// we should get 0 triggered interrupts
	ret = read(irqfd, &e, sizeof(e));
	if (ret != -1 || errno != EAGAIN) {
		printf("IRQ %d shouldn't trigger yet.\n", irq.index);
		return 1;
	}
#endif

	int *src_ptr = (int *)((uintptr_t)dma_map_src.vaddr);
	int *dst_ptr = (int *)((uintptr_t)dma_map_dst.vaddr);

	// fill with random data
	int c;
	int tot = dma_map_src.size/sizeof(*src_ptr);
	srand(time(NULL));
	for(c = 0; c < tot; c++) {
		src_ptr[c] = rand();
	}

	printf("source value: 0x%x\n", *src_ptr);
	printf("destination value: 0x%x\n", *dst_ptr);

	printf("start thread\n");

	// irq handler after setting up irqs
	pl330_vfio_start_irq_handler();

	struct req_config config;
	pl330_vfio_mem2mem_defconfig(&config);

	config.iova_src = dma_map_src.iova;
	config.iova_dst = dma_map_dst.iova;
	config.size	= dma_map_src.size;
	config.int_fin  = true;

	int channel_id;
	channel_id = pl330_vfio_request_channel();
	if(channel_id < 0) {
		printf("fail! No channels available!\n");
		return -1;
	} else {
		printf("channel %d allocated\n", channel_id);
		config.chan_id = channel_id;
	}

	config.callback = done_callback;
	char msg[] = "transfer completed";
	config.user_data = msg;

	generate_cmds_from_request((uchar *)((uintptr_t)dma_map_inst.vaddr), &config);
	pl330_vfio_submit_req((uchar *)((uintptr_t)dma_map_inst.vaddr), dma_map_inst.iova,
								&config);

	for(c = 0; c < tot; c++) {
		if(src_ptr[c] != dst_ptr[c]) {
			printf("test failed! - %d - 0x%x - 0x%x\n", c, src_ptr[c], dst_ptr[c]);
		}
	}

	pl330_vfio_reset();

	/*
	 * check result
	 */
	printf("source value: 0x%x\n", *((uint *)src_ptr));
	printf("destination value: 0x%x\n", *((uint *)dst_ptr));

	// halt controller: check the various thread are finished and remove TODO
#ifdef IRQ
	vfio_irqfd_clean(device, irq.index);

	close(irqfd);
#endif

	pl330_vfio_release_channel(channel_id);
#endif

	ioctl(device, VFIO_DEVICE_RESET);

	return 0;
}

