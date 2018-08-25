KERNELDIR = /lib/modules/$(shell uname -r)/build

obj-m = foo-char.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
ARCH := x86
CROSS_COMPILE :=

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf .*.cmd
	rm -rf *.o
