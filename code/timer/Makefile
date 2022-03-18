# For temporary debugging, set this to 1 -- development only!
TMP_DEBUG	:= 0

ifeq ($(TMP_DEBUG), 1)
	ccflags-y += -Wall -std=gnu99
else
	ccflags-y += -g -Wall -Werror
endif

FIND_PATTERN	:= -name .git -prune -o			\
		   -name "*.mod.c" -prune -o		\
		   -name "*.[chsS]" -type f -print


obj-m		:= timer.o
qgpu-objs	:= timer.o

KDIR := /lib/modules/`uname -r`/build


.PHONY: default tags clean mrproper

default: modules

modules:
	$(MAKE) C=1 -C $(KDIR) M=$(shell pwd)

tags:
	@echo "Building tags.."
	@find . $(FIND_PATTERN) |xargs ctags --extra=+f -a
	@echo "Building cscope.."
	@find . $(FIND_PATTERN) > cscope.files
	@cscope -Rqbk

clean:
	rm -f *.o *~ .depend .*.cmd *.ko *.mod built-in.a *.mod.c
	rm -f modules.order Module.symvers
	rm -rf .tmp_versions

mrproper: clean
	rm -f tags cscope.*

