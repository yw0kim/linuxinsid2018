#!/bin/bash

# copy from default kernel config
cp /boot/config-$(uname -r) ./.config

# rootfs create
wget https://releases.linaro.org/debian/images/developer-arm64/18.04/linaro-stretch-developer-20180416-89.tar.gz
dd if=/dev/zero of=rootfs.ext4 bs=1M count=4000
mkfs.ext4 -F rootfs.ext4
mkdir mnt1
mount rootfs.ext4 mnt1 -o loop
rm -rf mnt1/lost+found
tar -xzf linaro-stretch-developer-20180416-89.tar.gz -C mnt1/
mv mnt1/binary/* mnt1/
rmdir mnt1/binary
umount mnt1

