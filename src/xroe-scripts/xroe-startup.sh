#!/bin/sh
USERSDROOTDIR=/run/media/mmcblk0p1/xroe/
PRESTARTUPSLEEPPERIOD=2
POSTSTARTUPSLEEPPERIOD=1
LOGGER="/usr/bin/logger -t $0"

sleep $PRESTARTUPSLEEPPERIOD
$LOGGER "********************************************************************"
$LOGGER "**  Xilinx roe_framer demonstration platform application loader   **"
$LOGGER "********************************************************************"
sleep $POSTSTARTUPSLEEPPERIOD

## ----------------------------------------------------------------------------
## Check if there is a xroe Directory on the SD card. If there is run from 
## this instead of the onboard scripts.
## ----------------------------------------------------------------------------
if [ ! -e $USERSDROOTDIR ]; then
	USERSDROOTDIR=/usr/bin
fi

## ----------------------------------------------------------------------------
## Path to main runscript
## ----------------------------------------------------------------------------
USERBOOTRUNSCRIPTFILE=$USERSDROOTDIR/runXroe.bash

## ----------------------------------------------------------------------------
## Start the runManager if it exists. The user can stop it running by creating
## an empty directory on the SD card, 
## /run/media/mmcblk0p1/xroe
## ----------------------------------------------------------------------------
if [ ! -e $USERBOOTRUNSCRIPTFILE ]; then
   echo "File $USERBOOTRUNSCRIPTFILE does not exist."
else
   echo "User startup script $USERBOOTRUNSCRIPTFILE exists. Executing,"
   $USERBOOTRUNSCRIPTFILE
fi

## Forever loop
while [ 1 ];
   do
   sleep 10
done
