# init-ifupdown_%.bbappend content
  
do_install:append() {
  sed -i '/iface eth0 inet dhcp/ a auto eth1' ${D}${sysconfdir}/network/interfaces
}

