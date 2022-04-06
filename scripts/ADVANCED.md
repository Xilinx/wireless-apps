## **Petalinux** (advanced)
### Creating a BSP
If you wish to create a BSP from this project, you can do so using the following command.
```console
petalinux-package --bsp -p <PATH_TO_PROJECT> --output MY.BSP
```

### Unpacking RPM
If you build custom apps and wish to manually copy the executeable, you will need to recover it from its RPM file. Use the following command to do this.
```console
rpm2cpio myrpmfile.rpm | cpio -idmv
```

### Having issues?
#### Xilinx Petalinux help
https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/18842475/PetaLinux+Yocto+Tips

#### Checking the generated device tree
If there are issues when adding blocks to the design, check the device tree. You can decompile the device tree using dtc and check that it was assembled as you require.
```console
## You can find the dtc executable in your PL repo at
## .../petalinux-v2019.1-final/components/yocto/source/arm/buildtools/sysroots/x86_64-petalinux-linux/usr/bin/
dtc  -I dtb -O dts -o ./deviceTreeDebugView.dts ./images/linux/system.dtb
```

## **Vivado TCL Script** (advanced)
The following script is provided as an example. It is similar to how Xilinx test the IP and can help maintain consistency between your design runs. It provides hooks for extension. The end result will be the same as the GUI flow asour TCL scripts use the IP block automation.

Usage: 
```console
vivado -mode tcl -source ./xroe_build_vivado.tcl
```
Once loaded, review the printed help. When you are familiar with the flow and want to fully script the build, you can call vivado with launch arguments.
```console
## This style uses a set of strings concatenated together to tell the 
## script what mode it should built in. Note. case is ignored.
## 
## 1st TCLARGS = board to use (zcu111|zcu102)
## 2nd TCLARGS = Ip Mode to select (om0|om5)
## 3rd TCLARGS = Command mode to run (impl)(exit)(nodate)
## 4rd TCLARGS = IP Directory        (Path to local IP repo.) [optional]

vivado -mode tcl -source ./xroe_build_vivado.tcl -tclargs om5Impl -tclargs zcu102  
```
### **Vivado TCL going deeper**
Also provided is `xil_vivado_utils.tcl` should you wish to augment or further explore the vivado design. Both supplied scripts
can be TCL "sourced" in your own flow and extended to achieve your end design.

## Build Everything
### Vivado
To run all the builds serially in a script, the following sequence can be called.
```console
vivado -mode tcl -source ./xroe_build_vivado.tcl -tclargs zcu102 -tclargs om0    -tclargs implNodateExit
vivado -mode tcl -source ./xroe_build_vivado.tcl -tclargs zcu111 -tclargs om0    -tclargs implNodateExit
vivado -mode tcl -source ./xroe_build_vivado.tcl -tclargs zcu111 -tclargs om0_25 -tclargs implNodateExit
```
### Petalinux
Once complete run the Petalinux builds (where "20XX_X" is the release, e.g. 2022.1).
Note the **../** is used as Petalinux needs to create a project and cd up and out to get to the XSA directory.
```console
./xroe_build_petalinux.csh om0_z102_0    ../zcu102_om0_20XX_X/zcu102_om0_20XX_X.sdk        zcu102 om0
./xroe_build_petalinux.csh om0_z111_0    ../zcu111_om0_20XX_X/zcu111_om0_20XX_X.sdk        zcu111 om0
./xroe_build_petalinux.csh om0_z111_25g0 ../zcu111_om0_25_20XX_X/zcu111_om0_25_20XX_X.sdk  zcu111 om0_25g

```
