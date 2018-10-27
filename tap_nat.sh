#!/bin/sh

WHOAMI=$(whoami)
DEV_ETH=enp0s3
DEV_WIFI=wlp2s0
DEV_TAP=tap0
IP_ETH=$(ip addr show ${DEV_ETH} | awk '/inet / {print $2}' | cut -d/ -f1)
IP_TAP=$(ip addr show ${DEV_TAP} | awk '/inet / {print $2}' | cut -d/ -f1)

if [ "$IP_TAP" == "20.0.2.4" ]; then
	echo tap0 already installed.
	exit
fi

if [ "$IP_ETH" == "" ]; then
	DEV=$DEV_WIFI
else
	DEV=$DEV_ETH
fi

DEV=enp0s3

echo setup NAT network: tap0 to ${DEV}
sudo tunctl -d ${DEV_TAP}
sudo tunctl -u ${WHOAMI}
sudo ifconfig ${DEV_TAP} 20.0.2.4/24 up
sudo sysctl -w net.ipv4.ip_forward=1
sudo iptables -t nat -A POSTROUTING -o ${DEV} -j MASQUERADE
