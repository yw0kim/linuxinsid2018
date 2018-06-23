# FROM ubuntu:18.04
FROM ubuntu:16.04
# ENV setting
ENV LINARO_HOME /opt/gcc-linaro-7.2.1-2017.11-x86_64_aarch64-linux-gnu
ENV ARCH arm64
ENV CROSS_COMPILE aarch64-linux-gnu-
ENV PATH $LINARO_HOME/bin:$PATH
ENV HOME /root
WORKDIR $HOME
# for tz-data install
ENV DEBIAN_FRONTEND noninteractive 

# apt repo set
RUN sed -i 's/archive.ubuntu.com/ftp.daumkakao.com/g' /etc/apt/sources.list
# RUN sed -i 's/archive.ubuntu.com/ftp.jaist.ac.jp/g' /etc/apt/sources.list # ubuntu 18.04

RUN touch /etc/apt/apt.conf.d/99fixbadproxy 
RUN echo "Acquire::http::Pipeline-Depth 0;" >> /etc/apt/apt.conf.d/99fixbadproxy 
RUN echo "Acquire::http::No-Cache true;" >> /etc/apt/apt.conf.d/99fixbadproxy 
RUN echo "Acquire::BrokenProxy true;" >> /etc/apt/apt.conf.d/99fixbadproxy 
RUN apt-get update -o Acquire::CompressionTypes::Order::=gz 
RUN apt-get clean 
RUN rm -rf /var/lib/apt/lists/* 
RUN apt-get update

# build env deploy
RUN apt-get install -y apt-utils tzdata 
RUN ln -fs /usr/share/zoneinfo/Asia/Seoul /etc/localtime
RUN dpkg-reconfigure --frontend noninteractive tzdata

RUN apt-get update
# RUN apt-get install -y python-minimal # ubuntu 18.04
RUN apt-get install -y build-essential libncurses5 libncurses5-dev kernel-package libncurses-dev bc git libssl-dev

# QEMU build
RUN apt-get install -y pkg-config zlib1g-dev libglib2.0-dev zlib1g-dev libpixman-1-dev flex bison
RUN git clone git://git.qemu.org/qemu.git qemu.git
WORKDIR qemu.git
RUN bash configure --target-list=aarch64-softmmu
RUN make -j 8
RUN make install
WORKDIR $HOME

# linaro toolchain install
RUN apt-get install -y wget
RUN wget https://releases.linaro.org/components/toolchain/binaries/latest/aarch64-linux-gnu/gcc-linaro-7.2.1-2017.11-x86_64_aarch64-linux-gnu.tar.xz
RUN tar -xvf gcc-linaro-7.2.1-2017.11-x86_64_aarch64-linux-gnu.tar.xz
RUN mv gcc-linaro-7.2.1-2017.11-x86_64_aarch64-linux-gnu /opt/

# linaro kernel compile
RUN git clone --depth 1 -b linux-linaro-lsk-v4.14 https://git.linaro.org/kernel/linux-linaro-stable.git lsk-4.14
WORKDIR lsk-4.14
COPY .config $HOME/lsk-4.14/.config
RUN make oldconfig
RUN make -j 8
WORKDIR $HOME

# copy linaro bootfs
COPY rootfs.ext4 $HOME/rootfs.ext4

# run qemu 
CMD qemu-system-aarch64 -smp 2 -m 1024 -cpu cortex-a57 -nographic -machine virt -kernel lsk-4.14/arch/arm64/boot/Image -append 'root=/dev/vda rw rootwait mem=1024M console=ttyAMA0,38400n8' -device virtio-blk-device,drive=disk -drive if=none,id=disk,file=rootfs.ext4,format=raw


