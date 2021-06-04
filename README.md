# Xilinx wireless-apps software repository

Software for eCPRI 5G wireless IP and PetaLinux

The directory structure is:
- **src** Source code and end-user scripts.
- **scripts** Build and configuration scripts to help create a PetaLinux project.
- **yocto-recipes** Recipes to build the source code into a PetaLinux project.
- **image-managment** Build and SDSide Perl script to mange BOOT.bin and image.ub copy and selection.

## Get started 

There are three options to test and build the Demonstration Subsystem.
- **SDCard** Try the pre-built example avaiable from the <a href="https://www.xilinx.com/member/ecpri.html" target="_blank">eCPRI Lounge</a> (registration required). 

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

## O-RAN
The present github repository relates to eCPRI IP. For O-RAN IP, please refer to the github repository <a href="https://github.com/Xilinx/wireless-xorif" target="_blank">here</a>.