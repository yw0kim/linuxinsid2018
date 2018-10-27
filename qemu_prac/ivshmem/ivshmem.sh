WORK_DIR=${HOME}"/linuxinside"
QEMU_DIR=${HOME}"/qemu"
KERNEL_DIR=${QEMU_DIR}"/kernel/lsk-4.14"
KERNEL_IMG=${KERNEL_DIR}"/arch/arm64/boot/Image"
ROOTFS=${HOME}"/qemu/rootfs"
DTB_FILE="virt.dtb"

qemu-system-aarch64 -smp 3 -m 4096 -cpu cortex-a57 -nographic \
		-machine virt \
		-kernel ${KERNEL_IMG} \
		-append 'root=/dev/vda rw rootwait mem=1024M console=ttyAMA0,38400n8' \
		-device virtio-blk-device,drive=disk \
		-device ivshmem-doorbell,chardev=myivshmem,vectors=1 \
		-chardev socket,path=/tmp/ivshmem_socket,id=myivshmem \
		-drive if=none,id=disk,file=${ROOTFS},format=raw \
		-device virtio-net-device,netdev=mynet1 \
		-netdev tap,id=mynet1,ifname=tap0,script=no,downscript=no
#reset
