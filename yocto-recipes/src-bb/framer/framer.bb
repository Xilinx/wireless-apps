SUMMARY = "Recipe for  build an external framer Linux kernel module"
SECTION = "PETALINUX/modules"
LICENSE = "GPLv2"
#LIC_FILES_CHKSUM = "file://COPYING;md5=65dd37ccb3e888dc57e47d925b80b38a"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"
#LIC_FILES_CHKSUM = "file://COPYING;md5=00187214765b8e7e1ce425ca82385ae1"

inherit module

SRC_URI = "file://Makefile \
           file://xroe_framer.c \
           file://sysfs_xroe.c \
           file://sysfs_xroe_framer_stats.c \
           file://sysfs_xroe_framer_deframer.c \
           file://sysfs_xroe_framer_cfg.c \
           file://sysfs_xroe_framer_shared.c \
           file://xroe_framer.h \
           file://roe_framer_ctrl.h \
	   file://COPYING \
          "

S = "${WORKDIR}"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.
