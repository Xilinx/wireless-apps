# Xilinx wireless-apps software repository

Software, BSPs etc. for 5G wireless IP and PetaLinux

The directory structure is:
- **bsp** Contains PetaLinux BSP files, ready to be built or serve as the starting point for a project.
- **scripts** Build and configuration scripts to help create a PetaLinux project.
- **src** Source code and end-user scripts.
- **yocto-recipes** Recipes to build the source code into a PetaLinux project.

## Get started 

There are three options to test and build the Demonstration Subsystem.
- **SDCard** Try the pre-built example avaiable from the <a href="https://www.xilinx.com/member/ecpri.html" target="_blank">eCPRI Lounge</a> (registration required). 

- **BSP** Build using the BSP in PetaLinux, see README in **bsp** directory.
```console
git clone https://github.com/Xilinx/wireless-apps.git
cd bsp
more README.md
```
- **HDF** Build from a Vivado HDF using PetaLinux. This provides the most fexibility. See README in **scripts** directory.
```console
git clone https://github.com/Xilinx/wireless-apps.git
cd scripts
more README.md
```

## Post Boot
Once the system has booted, the default username and password are both **root**. To help you get started view the onboard help using
```console
xroe-help
```

## PetaLinux 

The PetaLinux landing page can be found <a href="https://www.xilinx.com/products/design-tools/embedded-software/petalinux-sdk.html" target="_blank">here</a>.
The two most commonly used guides are the <a href="https://www.xilinx.com/support/documentation/sw_manuals/xilinx2019_2/ug1144-petalinux-tools-reference-guide.pdf" target="_blank">Reference</a> and <a href="https://www.xilinx.com/support/documentation/sw_manuals/xilinx2019_2/ug1157-petalinux-tools-command-line-guide.pdf" target="_blank">Command Line</a> documents.
