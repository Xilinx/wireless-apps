#
# This file is the xroe-app recipe.
#

SUMMARY = "Simple xroe-app application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://xroe-app.c \
		file://ecpri.c \
		file://ip.c \
		file://stats.c \
		file://comms.c \
		file://client.c \
		file://parser.c \
		file://enable.c \
		file://disable.c \
		file://restart.c \
		file://framing.c \
		file://radio_ctrl.c \
		file://ecpri_proto.c \
		file://xroe_api.c \
		file://commands.h \
		file://xroe_types.h \
		file://xroefram_str.h \
		file://ecpri_str.h \
		file://ip_str.h \
		file://stats_str.h \
		file://enable_str.h \
		file://disable_str.h \
		file://restart_str.h \
		file://framing_str.h \
		file://radio_ctrl_str.h \
		file://comms.h \
		file://client.h \
		file://roe_framer_ctrl.h \
		file://roe_radio_ctrl.h \
		file://ecpri_proto.h \
		file://parser.h \
		file://xroe_api.h \
	   file://Makefile \
		  "

S = "${WORKDIR}"

do_compile() {
	     oe_runmake
}

do_install() {
	     install -d ${D}${bindir}
	     install -m 0755 xroe-app ${D}${bindir}
}
