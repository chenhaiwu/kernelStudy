obj-m:=myrmmod.o

CURRENT_PATH:=$(shell pwd)
LINUX_KERNEL:=$(shell uname -r)
#因为我已经是自己编译的全量代码，所以不再需要只有headers的包，而是已有全量包如下
#当然如果没有全量编译的话，又缺少headers的话，可以通过apt-get install linux-headers-$(shell uname -r)来下载
#LINUX_KERNEL_PATH:=/usr/src/linux-headers-$(LINUX_KERNEL)
LINUX_KERNEL_PATH:=/usr/src/linux-$(LINUX_KERNEL)

all:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules
clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean