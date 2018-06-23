ARCH=x86
CROSS_COMPILE=
obj-m := platform_device.o

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
		make -C /lib/moduels/$(shell uname -r)/build M=$(PWD) clean
