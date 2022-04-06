#
# This file is the xroeScripts recipe.
#

SUMMARY = "Simple xroeScripts application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://xroe-config-XXV.sh          \
           file://xroe-config-XXV-ptp.sh      \
           file://xroe-ptp4l.cfg              \
           file://xroe-ptp4lsyncE.cfg         \
           file://xroe-ptp_mas.sh             \
           file://xroe-ptp_slv.sh             \
           file://xroe-ptp_slv_synce.sh       \
           file://xroe-report-mod-install.sh  \
           file://xroe-printk_to.sh           \
           file://xroe-xxv-down.sh            \
           file://xroe-xxv-up.sh              \
           file://xroe-run-ptpSyncE.sh        \
           file://xroe-help                   \
           file://xroe-help.txt               \
           file://xroe-ptp_killall.sh         \
           file://xroe-startup.sh             \
	"

## This is insane, but require to stop Yocto blowing up. This prevent the attempted
## stripping of and exe which we have likely benn generated with Yocto and are hence
## already stripped of debug symbols
## https://forums.xilinx.com/t5/Embedded-Linux/adding-pre-built-application-to-roots/td-p/793669
INSANE_SKIP:${PN} = "ldflags"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_PACKAGE_STRIP = "1"

S = "${WORKDIR}"

do_install() {
	     install -d ${D}/${bindir}
	     install -m 0755 ${S}/xroe-config-XXV.sh         ${D}/${bindir}
	     install -m 0755 ${S}/xroe-config-XXV-ptp.sh     ${D}/${bindir}
	     install -m 0755 ${S}/xroe-ptp4l.cfg             ${D}/${bindir}
	     install -m 0755 ${S}/xroe-ptp4lsyncE.cfg        ${D}/${bindir}
	     install -m 0755 ${S}/xroe-ptp_mas.sh            ${D}/${bindir}
	     install -m 0755 ${S}/xroe-ptp_slv.sh            ${D}/${bindir}
	     install -m 0755 ${S}/xroe-ptp_slv_synce.sh      ${D}/${bindir}
	     install -m 0755 ${S}/xroe-report-mod-install.sh ${D}/${bindir}
	     install -m 0755 ${S}/xroe-ptp_killall.sh        ${D}/${bindir}
	     install -m 0755 ${S}/xroe-printk_to.sh          ${D}/${bindir}
	     install -m 0755 ${S}/xroe-xxv-down.sh           ${D}/${bindir}
	     install -m 0755 ${S}/xroe-xxv-up.sh             ${D}/${bindir}
	     install -m 0755 ${S}/xroe-run-ptpSyncE.sh       ${D}/${bindir}
	     install -m 0755 ${S}/xroe-help                  ${D}/${bindir}
	     install -m 0555 ${S}/xroe-help.txt              ${D}/${bindir}
	     install -m 0555 ${S}/xroe-startup.sh            ${D}/${bindir}
}
