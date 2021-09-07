#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/unistd.h>
#include <linux/spinlock.h>
#include <linux/version.h>
#include <linux/ktime.h>



#define THIS_DESCRIPTION "Tencent QoS Container GPU Virtualization Driver"
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haiwu Chen <haiwuchen@tencent.com>");
MODULE_DESCRIPTION(THIS_DESCRIPTION);

static inline void *qgpu_vmap_pfn(resource_size_t phys_addr, unsigned long size)
{
	unsigned int i = 0;
	void *vaddr;
	struct page **pages = NULL;
	unsigned long n_page = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	phys_addr_t phys_addr_start = phys_addr - offset_in_page(phys_addr);

	pages = kmalloc_array(n_page, sizeof(*pages), GFP_KERNEL);
	if (!pages)
		return NULL;

	for (i = 0; i < n_page; i++) {
		phys_addr_t addr = phys_addr_start + i * PAGE_SIZE;

		pages[i] = pfn_to_page(addr >> PAGE_SHIFT);
	}

	vaddr = vmap(pages, n_page, VM_IOREMAP, pgprot_noncached(PAGE_KERNEL_IO));

	kfree(pages);
	return vaddr;
}

static inline void qgpu_vunmap(const void *addr)
{
	if (is_vmalloc_addr(addr))
		vunmap(addr);
}

static inline unsigned int os_readl(void __iomem *addr, u64 offset)
{
	return readl(addr + offset);
}

static inline void os_writel(unsigned int val, void __iomem *addr, u64 offset)
{
	writel(val, addr + offset);
}


#define GPU_BAR0_PA		0xFC000000 //QGPU2
//#define GPU_BAR0_PA		0xFD000000	//TEST ENV
#define GPU_MAP_LEN		(0x1000000)
#define GPU_OFFSET		(0xbb0080)
#define TRY_TIMES		(10000)
#define MAX_MMIO_CYCLES	(1000)
#define TEST_MMIO_WRITE
#define TEST_MMIO_READ
#define TEST_MMIO_WRITE_FOR_ASS
#define TEST_MMIO_WRITE_FOE_TEST


static int is_mmio_tracing_by_hypervisor(void *addr, int disable_irq)
{
	u64 cur_cycle = 0;
	unsigned long flags;
	u64 tsc1, tsc2;

	if (disable_irq)
		local_irq_save(flags);
	tsc1 = get_cycles();
	os_writel(0x1000, addr, GPU_OFFSET);
	tsc2 = get_cycles();
	if (disable_irq)
		local_irq_restore(flags);
	cur_cycle = tsc2 - tsc1;

	if (cur_cycle > MAX_MMIO_CYCLES)
		return 1;
	else
		return 0;
}

static int __init ana_mem_init(void)
{
	unsigned long bar0_pa = GPU_BAR0_PA;
	int i = 0;
	
	struct timespec64 ts1, ts2, dts;
	u64 delta;
	u64 tsc1, tsc2;
	u64 cycle_max = 0, cycle_min = 8000000, ave_cycle = 0;
	u64 tsc_max = 0, tsc_min = 800000000000, ave_time = 0;

	unsigned long flags;

	void *bar0_va = qgpu_vmap_pfn(bar0_pa, GPU_MAP_LEN);
	if (!bar0_va) {
		pr_err("vmap pfn fail\n");
		return -1;
	}
	pr_info("bar0_pa:0x%lx, bar0_va:0x%lx\n", bar0_pa, (unsigned long)bar0_va);

	while (0) {
		unsigned int reg_val = 0, reg_val2 = 0;
		ave_time = 0;
		cycle_max = 0;
		cycle_min = 8000000;
		tsc_max = 0;
		tsc_min = 800000000000;
		ave_cycle = 0;
		for (i = 0; i < TRY_TIMES; i++) {
			preempt_disable();
			local_irq_save(flags);
			ktime_get_real_ts64(&ts1);
			tsc1 = get_cycles();
			reg_val = os_readl(bar0_va, GPU_OFFSET);
			reg_val2 = os_readl(bar0_va, GPU_OFFSET + 4);
			tsc2 = get_cycles();
			ktime_get_real_ts64(&ts2);
			dts = timespec64_sub(ts2, ts1);
			delta = timespec64_to_ns(&dts);
			local_irq_restore(flags);
			preempt_enable();
			if (cycle_max < (tsc2 - tsc1) / 2) {
				cycle_max = (tsc2 - tsc1) / 2;
			}
			if (cycle_min > (tsc2 - tsc1) / 2) {
				cycle_min = (tsc2 - tsc1) / 2;
			}
			if (tsc_max < delta / 2) {
				tsc_max = delta / 2;
			}
			if (tsc_min > delta / 2) {
				tsc_min = delta / 2;
			}
			ave_time += delta / 2;
			ave_cycle += (tsc2 - tsc1) / 2;

			if (tsc2 - tsc1 > 200)
				pr_info("UNNORMAL READ Time:%d, readback high32:0x%08x, low32:0x%08x, from 0x%x, cur time:%lld ns, cur cycle:%lld\n", i, reg_val2, reg_val, GPU_OFFSET, delta / 2, (tsc2 - tsc1) / 2);
		}

		pr_info("=================print all READ statistic info==================\n");
		pr_info("===Time:%d, readback high32:0x%08x, low32:0x%08x, from 0x%x\n", i, reg_val2, reg_val, GPU_OFFSET);
		pr_info("===read one mmio value in time max: %lld ns, min: %lld ns, ave:%lld ns, cycles max:%lld, min:%lld, ave:%lld\n",
			tsc_max, tsc_min, ave_time / TRY_TIMES, cycle_max, cycle_min, ave_cycle / TRY_TIMES);

		break;
	}

	while (0) {
		ave_time = 0;
		cycle_max = 0;
		cycle_min = 8000000;
		tsc_max = 0;
		tsc_min = 800000000000;
		ave_cycle = 0;
		for (i = 0; i < TRY_TIMES; i++) {
			preempt_disable();
			local_irq_save(flags);
			ktime_get_real_ts64(&ts1);
			tsc1 = get_cycles();
			os_writel(0x1000, bar0_va, GPU_OFFSET);
	//		os_writel(0x2000, bar0_va, GPU_OFFSET);
			tsc2 = get_cycles();
			ktime_get_real_ts64(&ts2);
			dts = timespec64_sub(ts2, ts1);
			delta = timespec64_to_ns(&dts);
			local_irq_restore(flags);
			preempt_enable();
			if (cycle_max < (tsc2 - tsc1) / 2) {
				cycle_max = (tsc2 - tsc1) / 2;
			}      
			if (cycle_min > (tsc2 - tsc1) / 2) {
				cycle_min = (tsc2 - tsc1) / 2;
			}
			if (tsc_max < delta / 2) {
				tsc_max = delta / 2;
			}
			if (tsc_min > delta / 2) {
				tsc_min = delta / 2;
			}
			ave_time += delta / 2;
			ave_cycle += (tsc2 - tsc1) / 2;

			if (tsc2 - tsc1 > 200)
				pr_info("UNNORMAL WRITE Time:%d, to 0x%x, cur time:%lld ns, cur cycle:%lld\n", i, GPU_OFFSET, delta / 2, (tsc2 - tsc1) / 2);
		}
		pr_info("=================print all WRITE statistic info==================\n");
		pr_info("===Time:%d, write to 0x%x\n", i,  GPU_OFFSET);
		pr_info("===write one mmio value in time max: %lld ns, min: %lld ns, ave:%lld ns, cycles max:%lld, min:%lld, ave:%lld\n",
			tsc_max, tsc_min, ave_time / TRY_TIMES, cycle_max, cycle_min, ave_cycle / TRY_TIMES);

		break;
	}

#ifdef TEST_MMIO_WRITE_FOR_ASS
		while (1) {
			u64 cur_cycle = 0;
			int disable_irq = 0;

			cycle_max = 0;
			cycle_min = 8000000;
			ave_cycle = 0;
			for (i = 0; i < TRY_TIMES; i++) {
				if (disable_irq)
					local_irq_save(flags);
				tsc1 = get_cycles();
				os_writel(0x1000, bar0_va, GPU_OFFSET);
				tsc2 = get_cycles();
				if (disable_irq)
					local_irq_restore(flags);
				cur_cycle = tsc2 - tsc1;
				cycle_max = cycle_max > cur_cycle ? cycle_max : cur_cycle;
				cycle_min = cycle_min < cur_cycle ? cycle_min : cur_cycle;
				ave_cycle += cur_cycle;
		
				if (cur_cycle > 400)
					pr_info("UNNORMAL WRITE Time:%d, to 0x%x, cur cycle:%lld\n", i, GPU_OFFSET, cur_cycle);
			}
		
			pr_info("=================print all WRITE statistic info==================\n");
			pr_info("===Time:%d, write offset to 0x%x, cycles max:%lld, min:%lld, ave:%lld\n", i,  GPU_OFFSET, cycle_max, cycle_min, ave_cycle / TRY_TIMES);
			break;
		}
#endif

	if (is_mmio_tracing_by_hypervisor(bar0_va, 1)) {
		pr_info("*****mmio is tracing by hypervisor*****\n");
	} else {
		pr_info("*****mmio is NOT tracing by hypervisor*****\n");
	}

	if (bar0_va) {
		qgpu_vunmap(bar0_va);
		bar0_va = NULL;
	}

	return 0;
}

static void __exit ana_mem_exit(void)
{
	pr_info("Bye ana_mem\n");
}

module_init(ana_mem_init);
module_exit(ana_mem_exit);
