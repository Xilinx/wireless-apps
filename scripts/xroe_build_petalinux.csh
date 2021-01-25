#!/bin/tcsh
set sourced=($_)
if ("$sourced" != "") then
    set THISFILE=`readlink -f $sourced[2]`
	set OUPUT_DIR=$sourced[3]
	set HDF_DIR=$sourced[4]
	set BOARD=$sourced[5]
	set MODE=$sourced[6]
else
    set THISFILE=`readlink -f $0`
	set OUPUT_DIR=$1
	set HDF_DIR=$2
	set BOARD=$3
	set MODE=$4
endif

set DELIVERYDIR=`dirname $THISFILE`
set mode_is_oran=`echo $MODE | sed -n /om5/p | wc -l`



petalinux-create --type project --template zynqMP -n $OUPUT_DIR

cd $OUPUT_DIR/
petalinux-config --get-hw-description $HDF_DIR --silentconfig

#if ("$MODE" == "om5") then
if ( $mode_is_oran == 1 ) then

  echo "xroe: building for OM5 - ORAN mode"
  petalinux-create -t modules -n framer  --enable
  rm ./project-spec/meta-user/recipes-modules/framer/files/framer.c
  rm ./project-spec/meta-user/recipes-modules/framer/framer.bb
  cp $DELIVERYDIR/../src/local-staging/xroeframer-driver/* ./project-spec/meta-user/recipes-modules/framer/files/
  cp $DELIVERYDIR/../yocto-recipes/src-bb/framer/framer.bb  ./project-spec/meta-user/recipes-modules/framer/

else

  echo "xroe: building for OM0 - Framer only mode"
  petalinux-create -t modules -n xtraffic --enable
  rm ./project-spec/meta-user/recipes-modules/xtraffic/files/*.c
  rm ./project-spec/meta-user/recipes-modules/xtraffic/xtraffic.bb
  cp $DELIVERYDIR/../src/xtraffic/* ./project-spec/meta-user/recipes-modules/xtraffic/files/
  cp $DELIVERYDIR/../yocto-recipes/src-bb/xtraffic/xtraffic.bb  ./project-spec/meta-user/recipes-modules/xtraffic/

endif

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
if ("$BOARD" == "zcu102") then
  cat $DELIVERYDIR/../yocto-recipes/meta/system-user_102.dtsi >> ./project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi
endif
if ("$BOARD" == "zcu111") then
  cat $DELIVERYDIR/../yocto-recipes/meta/system-user_111.dtsi >> ./project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi
endif

mkdir -p project-spec/meta-user/recipes-kernel/
cp -pr $DELIVERYDIR/../yocto-recipes/kernel/* ./project-spec/meta-user/recipes-kernel/

#if ("$MODE" == "om5") then
if ( $mode_is_oran == 1 ) then
  sed -i '/CONFIG_XROE_FRAMER/d' ./project-spec/meta-user/recipes-kernel/linux/linux-xlnx/fragment1.cfg
endif

echo "xroe: Update init-ifupdown, this enables DHCP on the correct eth port"
mkdir project-spec/meta-user/recipes-core
mkdir project-spec/meta-user/recipes-core/init-ifupdown/
cp ../../yocto-recipes/src-bb/init-ifupdown/init-ifupdown_%.bbappend ./project-spec/meta-user/recipes-core/init-ifupdown/init-ifupdown_%.bbappend

echo 'IMAGE_INSTALL_append = " linuxptp"' >> ./project-spec/meta-user/conf/petalinuxbsp.conf

echo "xroe: Update config files using PERL inline replace, provide scripting resource for test and development"
perl -p -i -e 's/^# user packages.*\n\K/CONFIG_linuxptp=y\n/' ./project-spec/configs/rootfs_config
perl -p -i -e 's/^# CONFIG_python.*\n\K/CONFIG_python=y\n/' ./project-spec/configs/rootfs_config
perl -p -i -e 's/^# CONFIG_python-unittest.*\n\K/CONFIG_python-unittest=y\n/' ./project-spec/configs/rootfs_config
perl -p -i -e 's/^# CONFIG_perl.*\n\K/CONFIG_perl=y\n/' ./project-spec/configs/rootfs_config
perl -p -i -e 's/^# CONFIG_perl-lib.*\n\K/CONFIG_perl-lib=y\n/' ./project-spec/configs/rootfs_config
perl -p -i -e 's/^# CONFIG_netcat.*\n\K/CONFIG_netcat=y\n/' ./project-spec/configs/rootfs_config
perl -p -i -e 's/^# CONFIG_tcpdump.*\n\K/CONFIG_tcpdump=y\n/' ./project-spec/configs/rootfs_config
perl -p -i -e 's/^# CONFIG_python-argparse.*\n\K/CONFIG_python-argparse=y\n/' ./project-spec/configs/rootfs_config
perl -p -i -e 's/^# CONFIG_git\s.*\n\K/CONFIG_git=y\n/' ./project-spec/configs/rootfs_config

echo "xroe: Enable GCC & G++ on board. Allows easier development for simple C programs"
perl -p -i -e 's/^# CONFIG_packagegroup-core-buildessential\s.*\n\K/CONFIG_packagegroup-core-buildessential=y\n/' ./project-spec/configs/rootfs_config
perl -p -i -e 's/^# CONFIG_libgcc\s.*\n\K/CONFIG_libgcc=y\n/' ./project-spec/configs/rootfs_config
perl -p -i -e 's/^# CONFIG_libgcc-dbg\s.*\n\K/CONFIG_libgcc-dbg=y\n/' ./project-spec/configs/rootfs_config
perl -p -i -e 's/^# CONFIG_libgcc-dev\s.*\n\K/CONFIG_libgcc-dev=y\n/' ./project-spec/configs/rootfs_config
perl -p -i -e 's/^# CONFIG_libstdcPLUSPLUS-dev\s.*\n\K/CONFIG_libstdcPLUSPLUS-dev=y\n/' ./project-spec/configs/rootfs_config
perl -p -i -e 's/^# CONFIG_libstdcPLUSPLUS\s.*\n\K/CONFIG_libstdcPLUSPLUS=y\n/' ./project-spec/configs/rootfs_config

echo "xroe: Enable peekpoke"
perl -p -i -e 's/^# CONFIG_peekpoke.*\n\K/CONFIG_peekpoke=y\n/' ./project-spec/configs/rootfs_config

## 
if ("$BOARD" == "zcu111") then
  echo "xroe: zcu111 support"
  perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_MACHINE_NAME.*/CONFIG_SUBSYSTEM_MACHINE_NAME="zcu111-reva"/' ./project-spec/configs/config
  perl -p -i -e 's/^.*YOCTO_MACHINE_NAME.*/YOCTO_MACHINE_NAME="zcu111-zynqmp"/' ./project-spec/configs/config
  perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_UBOOT_CONFIG_TARGET.*/CONFIG_SUBSYSTEM_UBOOT_CONFIG_TARGET="xilinx_zynqmp_virt_defconfig"/' ./project-spec/configs/config
endif

if ("$BOARD" == "zcu102") then
  echo "xroe: zcu102 support"
  perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_MACHINE_NAME.*/CONFIG_SUBSYSTEM_MACHINE_NAME="zcu102-rev1.0"/' ./project-spec/configs/config
  perl -p -i -e 's/^.*YOCTO_MACHINE_NAME.*/YOCTO_MACHINE_NAME="zcu102-zynqmp"/' ./project-spec/configs/config
  perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_UBOOT_CONFIG_TARGET.*/CONFIG_SUBSYSTEM_UBOOT_CONFIG_TARGET="xilinx_zynqmp_virt_defconfig"/' ./project-spec/configs/config
endif

echo "xroe: PL Config"
petalinux-config --silentconfig

echo "xroe: PL Build"
petalinux-build

if ("$BOARD" == "zcu111") then
  echo "xroe: PL Package"
  petalinux-package --boot --fsbl --fpga --pmufw --u-boot --force
else 
  echo "xroe: PL Package"
  petalinux-package --boot --fsbl --fpga --pmufw --u-boot --force
endif

echo "xroe: Create BSP one level up."
petalinux-package --bsp -p ./ --output ./../${BOARD}_${MODE}.bsp

echo "## Use internal build managment using. Copies BOOT.bin/image.ub your home directory."
echo ""
echo "cp ../../image-management/buildSide/copyBoot ."
echo ""
echo "## To create a BSP"
echo ""
echo "petalinux-package --bsp -p ./ --output ../../bsp/2019.2/${BOARD}_${MODE}.bsp"
echo ""
