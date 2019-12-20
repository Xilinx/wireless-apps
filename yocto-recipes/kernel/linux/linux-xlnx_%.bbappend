SRC_URI += "file://fragment1.cfg \
"

SRC_URI_append += "file://0001-Signed-off-by-Xu-Dong-xud-xilinx.com.patch"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"
