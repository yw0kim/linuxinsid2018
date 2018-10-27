WORK_DIR=${HOME}"/linuxinside/qemu_prac"
KERNEL_DIR=${WORK_DIR}"/lsk-4.14"
KERNEL_IMG=${KERNEL_DIR}"/arch/arm64/boot/Image"
ROOTFS="rootfs.ext4"
DTB_FILE="virt.dtb"

qemu-system-aarch64 -smp 2 -m 1024 -cpu cortex-a57 -nographic \
		-machine virt \
		-kernel ${KERNEL_IMG} \
		-append 'root=/dev/vda rw rootwait mem=1024M console=ttyAMA0,38400n8' \
		-device virtio-blk-device,drive=disk \
		-device ivshmem-doorbell,chardev=myivshmem,vectors=1 \
		-chardev socket,path=/thmp/ivshmem_socket,id=myivshmem \
		-drive if=none,id=disk,file=${ROOTFS},format=raw

#reset
