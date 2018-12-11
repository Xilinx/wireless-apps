#!/bin/sh

echo "Setting up the 25G Ethernet link..."
ip link set dev eth1 up
ip addr add 192.168.99.10/24 dev eth1

ip addr

echo "...done!"
