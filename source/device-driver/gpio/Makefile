obj-m := gpio-foo-dynamic.o

KDIR := /root/qemu/kernel/lsk-4.14

all:
		make -C ${KDIR} M=$(PWD) modules

clean:
		make -C ${KDIR} M=$(PWD) clean
