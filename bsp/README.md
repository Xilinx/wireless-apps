# RoE framer BSPs

Build instructions (adapt for correct Petalinux version):

To build the Xilinx Radio over Ethernet Framer Petalinux project you will need the following installed on your build machine:
- Petalinux 20XX.X
- A BSP (zcuXXX_omX.bsp)

Set up the Petalinux environment with the following command:
    source <petalinux_directory>/settings.csh

Build the project with the following commands:

    petalinux-create -t project -n <PROJECT_DIR_NAME> -s zcuXXX_omX.bsp

This will create a directory called <PROJECT_DIR_NAME> in the current directory populated with the BSP and ready to build.

    cd <PROJECT_DIR_NAME>
    petalinux-build
    
This will create a PetaLinux image: <PROJECT_DIR_NAME>/images/linux/image.ub

    petalinux-package --boot --fsbl images/linux/zynqmp_fsbl.elf --fpga ./images/linux/system.bit --pmufw images/linux/pmufw.elf --u-boot --force

This will create a bootloader image: <PROJECT_DIR_NAME>/images/linux/BOOT.BIN

Copy these two files to the root directory of an SD card and place in the SD card socket of the zcuXXX board.

## Xilinx ROE_Framer Address Space

|IP|Base Addr|Range|Path|Comment|
|:---|:---|---:|:---|:---|
|roe_framer|0xA0000000|64K|datapath/framer_datapath/def_subsPtp_x_5/s_axi/Reg              | |
|AXI DMA   |0xA0010000|4K |datapath/xxv_eth_subs/subs_2_arm_mm_dma/axi_dma_0/S_AXI_LITE/Reg| |     
|Ethernet 0 |0xA0020000|64K|datapath/xxv_eth_subs/xxv_wrap/xxv_ethernet_0/s_axi_0/Reg       | |
|Ethernet 1 |0xA0030000|64K|As above with xxv_ethernet_<n>                                  | |
|Ethernet 2 |0xA0040000|64K|| |
|Ethernet 3 |0xA0050000|64K|| |
|RoeRadio   |0xA0060000|64K|datapath/framer_datapath/roe_radio_ctrl_0/s_axi/reg0| |
|RX TS Fifo |0xA0070000|64K|datapath/xxv_eth_subs/xxv_wrap/support_1588_2step/rx_ts_fifo/axi_fifo_mm_s_0/S_AXI/Mem0      |          Hole in non-PTP build|
|Timer1588  |0xA0080000|64K|datapath/xxv_eth_subs/xxv_wrap/support_1588_2step/timer1588_subs/timer_1588_v2_0_0/s_axi/reg0   |             Hole in non-PTP build|
|TX TS Fifo |0xA0090000|64K|datapath/xxv_eth_subs/xxv_wrap/support_1588_2step/tx_ts_fifo/axi_fifo_mm_s_0/S_AXI/Mem0         |       Hole in non-PTP build|
