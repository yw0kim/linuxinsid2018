ARCH=x86
CROSS_COMPILE=
obj-m := pinctrl-my.o

all:
	        make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	        make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
