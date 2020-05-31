#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_CNT 10
#define MAP_LEN (PAGE_CNT * 4096)

int main(void)
{
    int fd, i;
    char *uaddr;

    fd = open("/dev/my_memmap_dev", O_RDWR);
    if (fd < 0) {
        printf("open dev fail\n");
        return 0;
    }
    uaddr = mmap(0, MAP_LEN, PROT_WRITE, MAP_SHARED | MAP_LOCKED, fd, 0);
    sprintf(uaddr, "write my context from write userspace");


    while (1) {
        sleep(1);
    }
    return 0;
}