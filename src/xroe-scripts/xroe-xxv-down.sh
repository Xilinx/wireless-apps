#!/bin/sh

#ipAddr="192.168.99.10"
eTag="eth1"

if [ ! -v $1 ]
then
   eTag=$1
else
   echo "No Eth Interface given using ${eTag}."
fi

echo "Set XXV Ethernet link ${eTag} down..."
ip link set dev ${eTag} down

#ip addr
