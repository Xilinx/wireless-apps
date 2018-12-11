#!/bin/sh
if [ ! -v $1 ]
then
   echo "echo $1 to /proc/sys/kernel/printk"
   echo $1 > /proc/sys/kernel/printk
else
   echo "No value given, exiting."
fi
