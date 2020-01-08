## Vivado
1. Open a Vivado project and select the part/board of interest
2. Create and IPI design and add the framer IP
3. Run "Block automation"
4. Add constraints for all the pins, refer to the Board User guide and the included examples.
5. Check clocking is compatible, refer to boards User Guide and amend if necessary
5. Generate Bitstream

## Petalinux
1. Generate an XSA from the Vivado project
2. Review the Petalinux script where there are board type checks, zcu102 for example
3. Update the content with appropriate updates for your board. If you dont know the values, you can build first and review/update later.
4. Build

## Issues
The roe_framer example system should be targetable at any MPSOC board. Internally it is only tested on the zcu102 & zcu111. Use the following links below for any porting issues. 

### Vivado & Petalinux
<a href="https://forums.xilinx.com/" target="_blank">Xilinx Forum</a>

### PetaLinux Userguides
The PetaLinux landing page can be found <a href="https://www.xilinx.com/products/design-tools/embedded-software/petalinux-sdk.html" target="_blank">here</a>.
The two most commonly used guides are the <a href="https://www.xilinx.com/support/documentation/sw_manuals/xilinx2019_2/ug1144-petalinux-tools-reference-guide.pdf" target="_blank">Reference</a> and <a href="https://www.xilinx.com/support/documentation/sw_manuals/xilinx2019_2/ug1157-petalinux-tools-command-line-guide.pdf" target="_blank">Command Line</a> documents.
