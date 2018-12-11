#!/bin/sh

#ipAddr="192.168.99.10"
eTag="eth1"

if [ ! -v $1 ]
then
   eTag=$1
else
   echo "No Eth Interface given using ${eTag}."
fi

echo "Set XXV Ethernet link ${eTag} up..."
ip link set dev ${eTag} up

#ip addr
