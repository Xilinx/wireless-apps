# RoE framer scripts

**xroe_build_steps.csh**

Usage: `xroe_build_steps.csh <project_dir_path> <HDF_dir_path>`

This script will populate a project directory with a given XROE_framer-based hardware image, then configure and build the project.
Build output will be in `<project_dir_path>/images/linux/image.ub` (PetaLinux image) and `<project_dir_path>/images/linux/BOOT.BIN` (bootloader image).
Copy these to the root directory of an SD card and insert the card into the ZCU102 SD card slot to boot the board.

## Creating a BSP
If you wish to create a BSP from this project, you can do so using the following command.
```console
petalinux-package --bsp -p <PATH_TO_PROJECT> --output MY.BSP
```

## Unpacking RPM
If you build custom apps and wish to manually copy the executeable, you will need to recover it from its RPM file. Use the following command to do this.
```console
rpm2cpio myrpmfile.rpm | cpio -idmv
```
