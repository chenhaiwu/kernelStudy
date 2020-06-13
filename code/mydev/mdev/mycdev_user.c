#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


#define DEV_NAME "/dev/my_dev"

int main()
{
    char buf[64];
    int fd;

    fd = open(DEV_NAME, O_RDWR);
    if (fd < 0) {
        printf("open fail ...fd =%d, \n", fd);
        return -1;
    }

    read(fd, buf, 64);
    printf("read from kernel : %s\n", buf);
    
    sprintf(buf, "write from user......");
    write(fd, buf, 64);
    close(fd);
    return 0;
}