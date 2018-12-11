#!/bin/sh
xroe-config-XXV-ptp.sh
ptp4l -m -A -i eth1 -f /usr/bin/xroe-ptp4lsyncE.cfg -s
