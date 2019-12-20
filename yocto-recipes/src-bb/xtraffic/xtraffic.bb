SUMMARY = "Recipe for  build an external xtraffic Linux kernel module"
SECTION = "PETALINUX/modules"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"

inherit module

SRC_URI = "file://Makefile \
           file://roe_radio_ctrl.h \
           file://xroe_traffic.h \
           file://xroe_traffic.c \
           file://xroe_traffic_utils.c \
           file://sysfs_xroe_traffic_config.c \
      	   file://COPYING \
          "

S = "${WORKDIR}"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.
