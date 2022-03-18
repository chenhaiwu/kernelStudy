/* For the size of the file. */
#include <sys/stat.h>
/* This contains the mmap calls. */
#include <sys/mman.h> 
/* These are for error printing. */
#include <errno.h>
#include <string.h>
#include <stdarg.h>
/* This is for open. */
#include <fcntl.h>
#include <stdio.h>
/* For exit. */
#include <stdlib.h>
/* For the final part of the example. */
#include <ctype.h>

int main(int argc, char **argv)
{
	int fd = 0;
	int ret = 0;
	char *src, *dst;
    unsigned int operation = 0;
	unsigned int opt_data = 0;

	/*打开设备文件*/
	fd = open("/sys/bus/pci/devices/0000\:00\:08.0/resource0", O_RDONLY | O_SYNC);//_O_RDONLY
	//mmap returns a userspace address
	//0, 4096 tells it to pull one page
	printf("open suc\n");
	if ((src = mmap (0, 0x100000, PROT_READ, MAP_SHARED, fd, 0)) == (caddr_t) -1)
	{
		printf ("mmap /dev/nvidia0 failed: %s", strerror (errno));
		return 0;
	}
	printf("PCI BAR0 0x0000 = 0x%x\n", *((unsigned short *) src));

	munmap(src, 0x100000);

done:
	close(fd);
	return ret;
}



	// fd = open("/dev/nvidia0", O_RDWR);
	// if (fd < 0) {
	// 	printf("Open nvidia0 Error!\n");
	// 	ret = -2;
	// 	goto done;
	// }

	// printf("Open nvidia0 suc\n");

	// /* mmap the input file */
	// if ((src = mmap (0xd0000000, 0x100000, PROT_READ, MAP_SHARED, fd, 0)) == (caddr_t) -1)
	// {
	// 	printf ("mmap /dev/nvidia0 failed: %s", strerror (errno));
	// 	return 0;
	// }