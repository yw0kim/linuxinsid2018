obj-m += helloadd.o
helloadd-objs := hello-2.o add.o

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
		make -C /lib/moduels/$(shell uname -r)/build M=$(PWD) clean
