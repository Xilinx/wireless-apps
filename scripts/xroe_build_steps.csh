#!/bin/tcsh
set sourced=($_)
if ("$sourced" != "") then
    set THISFILE=`readlink -f $sourced[2]`
	set OUPUT_DIR=$sourced[3]
	set HDF_DIR=$sourced[4]
else
    set THISFILE=`readlink -f $0`
	set OUPUT_DIR=$1
	set HDF_DIR=$2
endif

set DELIVERYDIR=`dirname $THISFILE`

petalinux-create --type project --template zynqMP -n $OUPUT_DIR
cd $OUPUT_DIR/
petalinux-config --get-hw-description $HDF_DIR --oldconfig

petalinux-create -t modules -n xlnx-ptp-timer  --enable
rm ./project-spec/meta-user/recipes-modules/xlnx-ptp-timer/files/xlnx-ptp-timer.c
rm ./project-spec/meta-user/recipes-modules/xlnx-ptp-timer/xlnx-ptp-timer.bb
cp $DELIVERYDIR/../src/xlnx-ptp-timer/* ./project-spec/meta-user/recipes-modules/xlnx-ptp-timer/files/
cp $DELIVERYDIR/../yocto-recipes/src-bb/xlnx-ptp-timer/xlnx-ptp-timer.bb  ./project-spec/meta-user/recipes-modules/xlnx-ptp-timer/

petalinux-create -t apps -n xroe-app  --enable 
rm ./project-spec/meta-user/recipes-apps/xroe-app/files/xroe-app.c
rm ./project-spec/meta-user/recipes-apps/xroe-app/xroe-app.bb
cp $DELIVERYDIR/../src/xroe-app/* ./project-spec/meta-user/recipes-apps/xroe-app/files/
cp $DELIVERYDIR/../yocto-recipes/src-bb/xroe-app/xroe-app.bb  ./project-spec/meta-user/recipes-apps/xroe-app/

petalinux-create -t apps --template install --name xroescripts --enable
rm  ./project-spec/meta-user/recipes-apps/xroescripts/files/xroescripts
cp $DELIVERYDIR/../src/xroe-scripts/* ./project-spec/meta-user/recipes-apps/xroescripts/files/
cp $DELIVERYDIR/../yocto-recipes/src-bb/xroe-scripts/xroescripts.bb  ./project-spec/meta-user/recipes-apps/xroescripts/

petalinux-create -t apps --template install -n xroe-auto-start --enable
cp $DELIVERYDIR/../src/xroe-auto-start/* ./project-spec/meta-user/recipes-apps/xroe-auto-start/files/
cp $DELIVERYDIR/../yocto-recipes/src-bb/xroe-auto-start/xroe-auto-start.bb  ./project-spec/meta-user/recipes-apps/xroe-auto-start/

cp $DELIVERYDIR/../yocto-recipes/meta/system-user.dtsi  ./project-spec/meta-user/recipes-bsp/device-tree/files/

cp -pr $DELIVERYDIR/../yocto-recipes/kernel ./project-spec/meta-user/recipes-kernel

echo 'IMAGE_INSTALL_append = " linuxptp"' >> ./project-spec/meta-user/recipes-core/images/petalinux-image-full.bbappend
perl -p -i -e 's/^# user packages.*\n\K/CONFIG_linuxptp=y\n/' ./project-spec/configs/rootfs_config
perl -p -i -e 's/^# CONFIG_python.*\n\K/CONFIG_python=y\n/' ./project-spec/configs/rootfs_config
perl -p -i -e 's/^# CONFIG_python-unittest.*\n\K/CONFIG_python-unittest=y\n/' ./project-spec/configs/rootfs_config
perl -p -i -e 's/^# CONFIG_perl.*\n\K/CONFIG_perl=y\n/' ./project-spec/configs/rootfs_config
perl -p -i -e 's/^# CONFIG_perl-lib.*\n\K/CONFIG_perl-lib=y\n/' ./project-spec/configs/rootfs_config
perl -p -i -e 's/^# CONFIG_netcat.*\n\K/CONFIG_netcat=y\n/' ./project-spec/configs/rootfs_config
perl -p -i -e 's/^# CONFIG_tcpdump.*\n\K/CONFIG_tcpdump=y\n/' ./project-spec/configs/rootfs_config

perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_MACHINE_NAME.*/CONFIG_SUBSYSTEM_MACHINE_NAME="zcu102-rev1.0"/' ./project-spec/configs/config

petalinux-config --oldconfig
petalinux-build
petalinux-package --boot --fsbl images/linux/zynqmp_fsbl.elf --fpga ./images/linux/system.bit --pmufw images/linux/pmufw.elf --u-boot --force

