
obj-m := anamem.o

#qgpu-objs := ana_mem.o
ccflags-y += -save-temps=obj -g0 -s -Wall -Werror


KERNELSRC := /lib/modules/`uname -r`/build

modules:
	make -C $(KERNELSRC) M=$(shell pwd)
clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f modules.order Module.symvers
	rm -rf .tmp_versions
