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
