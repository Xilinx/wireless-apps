# RoE framer scripts

## **Build Vivado Project in GUI**
Start vivado choose a supported board/part, place the IP on the IPI canvas and run the Block Automation as detailed in PG312.
See [ADVANCED.md](ADVANCED.md) for Vivado command line examples.

## **Petalinux eCPRI/1914 Framer** (Oran_Mode == 0)

Usage: 
```console
xroe_build_petalinux.csh <project_dir_path> <HDF_dir_path> <zcu102|zcu111> <om0>
```
This script will populate a project directory with a given XROE_framer-based hardware image, then configure and build the project.
Build output will be in `<project_dir_path>/images/linux/image.ub` (PetaLinux image) and `<project_dir_path>/images/linux/BOOT.BIN` (bootloader image).
Copy these to the root directory of an SD card and insert the card into the ZCU102 SD card slot to boot the board.

## Advanced
See [ADVANCED.md](ADVANCED.md) for TCL scripting and debug help.

## Adding a board
See [ADD_YOUR_BOARD.md](ADD_YOUR_BOARD.md), out of the box boards are shown below.


| Mode/Board | | zcu102 | zcu111 |
| --- | --- | :---:  | :---: |
| | | |
| 2018.3 OM0 (10G) | | Y | N |
| | | |
| 2019.2 OM0 (10G) | | Y | Y |
| 2019.2 OM0 (25G) | | NA | Y |
| 2020.1 OM0 (10G) | | Y | Y |
| 2020.1 OM0 (25G) | | NA | Y |
