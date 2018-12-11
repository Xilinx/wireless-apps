#
# This file is the xroe-auto-start recipe.
#
SUMMARY = "Simple xroe-auto-start application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"

LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://xroe-auto-start  \
           file://runXroe.bash     \
"

S = "${WORKDIR}"

RDEPENDS_${PN} += "bash"
FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

inherit update-rc.d

INITSCRIPT_NAME   = "xroe-auto-start"
INITSCRIPT_PARAMS = "start 99 S ."

do_install() {
   install -d ${D}${sysconfdir}/init.d
   install -m 0755 ${S}/xroe-auto-start ${D}${sysconfdir}/init.d/xroe-auto-start
   install -d ${D}/${bindir}
   install -m 0755 ${S}/runXroe.bash    ${D}/${bindir}
}

FILES_${PN} += "${sysconfdir}/*"
