#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_CNT 1000
#define MAP_LEN (PAGE_CNT * 4096)

int main(void)
{
    int fd, i;
    char *uaddr;

    fd = open("/dev/my_memmap_dev", O_RDONLY);
    if (fd < 0) {
        printf("open dev fail\n");
        return 0;
    }
    uaddr = mmap(0, MAP_LEN, PROT_READ, MAP_PRIVATE | MAP_LOCKED, fd, 0);
    for (i = 0; i < 3; i++) {
        printf("i=%d, uaddr: %lx,  context: %-10s\n", i, (unsigned long)uaddr + 4096 * i, uaddr + 4096 * i);
    }

    // for (i = 880; i < PAGE_CNT; i++) {
    //     printf("i=%d, uaddr: %lx,  context: %-10s\n", i, (unsigned long)uaddr + 4096 * i, uaddr + 4096 * i);
    // }

    while (1) {
        sleep(1);
    }
    return 0;
}