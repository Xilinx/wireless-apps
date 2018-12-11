// SPDX-License-Identifier: GPL-2.0 
/*-----------------------------------------------------------------------------
* C Header bank BASE definitions
*------------------------------------------------------------------------------
*/
#define ROE_RADIO_CFG_BASE_ADDR 0x0 /* 0 */
#define ROE_RADIO_SOURCE_BASE_ADDR 0x1000 /* 4096 */

/*-----------------------------------------------------------------------------
* C Header bank register definitions for bank roe_radio_cfg 
* with prefix radio_ @ address 0x0
*------------------------------------------------------------------------------
*/
/* Type = roInt */
#define RADIO_ID_ADDR 0x0 /* 0 */
#define RADIO_ID_MASK 0xffffffff /* 4294967295 */
#define RADIO_ID_OFFSET 0x0 /* 0 */
#define RADIO_ID_WIDTH 0x20 /* 32 */
#define RADIO_ID_DEFAULT 0x120001 /* 1179649 */

/* Type = rw */
#define RADIO_TIMEOUT_ENABLE_ADDR 0x4 /* 4 */
#define RADIO_TIMEOUT_ENABLE_MASK 0x1 /* 1 */
#define RADIO_TIMEOUT_ENABLE_OFFSET 0x0 /* 0 */
#define RADIO_TIMEOUT_ENABLE_WIDTH 0x1 /* 1 */
#define RADIO_TIMEOUT_ENABLE_DEFAULT 0x0 /* 0 */

/* Type = ro */
#define RADIO_TIMEOUT_STATUS_ADDR 0x8 /* 8 */
#define RADIO_TIMEOUT_STATUS_MASK 0x1 /* 1 */
#define RADIO_TIMEOUT_STATUS_OFFSET 0x0 /* 0 */
#define RADIO_TIMEOUT_STATUS_WIDTH 0x1 /* 1 */
#define RADIO_TIMEOUT_STATUS_DEFAULT 0x1 /* 1 */

/* Type = rw */
#define RADIO_TIMEOUT_VALUE_ADDR 0xc /* 12 */
#define RADIO_TIMEOUT_VALUE_MASK 0xfff /* 4095 */
#define RADIO_TIMEOUT_VALUE_OFFSET 0x0 /* 0 */
#define RADIO_TIMEOUT_VALUE_WIDTH 0xc /* 12 */
#define RADIO_TIMEOUT_VALUE_DEFAULT 0x80 /* 128 */

/* Type = rw */
#define RADIO_GPIO_CDC_LEDMODE2_ADDR 0x10 /* 16 */
#define RADIO_GPIO_CDC_LEDMODE2_MASK 0x1 /* 1 */
#define RADIO_GPIO_CDC_LEDMODE2_OFFSET 0x0 /* 0 */
#define RADIO_GPIO_CDC_LEDMODE2_WIDTH 0x1 /* 1 */
#define RADIO_GPIO_CDC_LEDMODE2_DEFAULT 0x0 /* 0 */

/* Type = rw */
#define RADIO_GPIO_CDC_LEDGPIO_ADDR 0x10 /* 16 */
#define RADIO_GPIO_CDC_LEDGPIO_MASK 0x30 /* 48 */
#define RADIO_GPIO_CDC_LEDGPIO_OFFSET 0x4 /* 4 */
#define RADIO_GPIO_CDC_LEDGPIO_WIDTH 0x2 /* 2 */
#define RADIO_GPIO_CDC_LEDGPIO_DEFAULT 0x0 /* 0 */

/* Type = roSig */
#define RADIO_GPIO_CDC_DIPSTATUS_ADDR 0x14 /* 20 */
#define RADIO_GPIO_CDC_DIPSTATUS_MASK 0xff /* 255 */
#define RADIO_GPIO_CDC_DIPSTATUS_OFFSET 0x0 /* 0 */
#define RADIO_GPIO_CDC_DIPSTATUS_WIDTH 0x8 /* 8 */
#define RADIO_GPIO_CDC_DIPSTATUS_DEFAULT 0x0 /* 0 */

/* Type = wPlsH */
#define RADIO_SW_TRIGGER_ADDR 0x20 /* 32 */
#define RADIO_SW_TRIGGER_MASK 0x1 /* 1 */
#define RADIO_SW_TRIGGER_OFFSET 0x0 /* 0 */
#define RADIO_SW_TRIGGER_WIDTH 0x1 /* 1 */
#define RADIO_SW_TRIGGER_DEFAULT 0x0 /* 0 */

/* Type = rw */
#define RADIO_CDC_ENABLE_ADDR 0x24 /* 36 */
#define RADIO_CDC_ENABLE_MASK 0x1 /* 1 */
#define RADIO_CDC_ENABLE_OFFSET 0x0 /* 0 */
#define RADIO_CDC_ENABLE_WIDTH 0x1 /* 1 */
#define RADIO_CDC_ENABLE_DEFAULT 0x0 /* 0 */

/* Type = roSig */
#define RADIO_CDC_ERROR_ADDR 0x24 /* 36 */
#define RADIO_CDC_ERROR_MASK 0x2 /* 2 */
#define RADIO_CDC_ERROR_OFFSET 0x1 /* 1 */
#define RADIO_CDC_ERROR_WIDTH 0x1 /* 1 */
#define RADIO_CDC_ERROR_DEFAULT 0x0 /* 0 */

/* Type = roSig */
#define RADIO_CDC_STATUS_ADDR 0x24 /* 36 */
#define RADIO_CDC_STATUS_MASK 0x4 /* 4 */
#define RADIO_CDC_STATUS_OFFSET 0x2 /* 2 */
#define RADIO_CDC_STATUS_WIDTH 0x1 /* 1 */
#define RADIO_CDC_STATUS_DEFAULT 0x0 /* 0 */

/* Type = rw */
#define RADIO_CDC_LOOPBACK_ADDR 0x28 /* 40 */
#define RADIO_CDC_LOOPBACK_MASK 0x1 /* 1 */
#define RADIO_CDC_LOOPBACK_OFFSET 0x0 /* 0 */
#define RADIO_CDC_LOOPBACK_WIDTH 0x1 /* 1 */
#define RADIO_CDC_LOOPBACK_DEFAULT 0x0 /* 0 */

/* Type = rw */
#define RADIO_SINK_ENABLE_ADDR 0x2c /* 44 */
#define RADIO_SINK_ENABLE_MASK 0x1 /* 1 */
#define RADIO_SINK_ENABLE_OFFSET 0x0 /* 0 */
#define RADIO_SINK_ENABLE_WIDTH 0x1 /* 1 */
#define RADIO_SINK_ENABLE_DEFAULT 0x1 /* 1 */

/* Type = roSig */
#define RADIO_CDC_ERROR_31_0_ADDR 0x30 /* 48 */
#define RADIO_CDC_ERROR_31_0_MASK 0xffffffff /* 4294967295 */
#define RADIO_CDC_ERROR_31_0_OFFSET 0x0 /* 0 */
#define RADIO_CDC_ERROR_31_0_WIDTH 0x20 /* 32 */
#define RADIO_CDC_ERROR_31_0_DEFAULT 0x0 /* 0 */

/* Type = roSig */
#define RADIO_CDC_ERROR_63_32_ADDR 0x34 /* 52 */
#define RADIO_CDC_ERROR_63_32_MASK 0xffffffff /* 4294967295 */
#define RADIO_CDC_ERROR_63_32_OFFSET 0x0 /* 0 */
#define RADIO_CDC_ERROR_63_32_WIDTH 0x20 /* 32 */
#define RADIO_CDC_ERROR_63_32_DEFAULT 0x0 /* 0 */

/* Type = roSig */
#define RADIO_CDC_ERROR_95_64_ADDR 0x38 /* 56 */
#define RADIO_CDC_ERROR_95_64_MASK 0xffffffff /* 4294967295 */
#define RADIO_CDC_ERROR_95_64_OFFSET 0x0 /* 0 */
#define RADIO_CDC_ERROR_95_64_WIDTH 0x20 /* 32 */
#define RADIO_CDC_ERROR_95_64_DEFAULT 0x0 /* 0 */

/* Type = roSig */
#define RADIO_CDC_ERROR_127_96_ADDR 0x3c /* 60 */
#define RADIO_CDC_ERROR_127_96_MASK 0xffffffff /* 4294967295 */
#define RADIO_CDC_ERROR_127_96_OFFSET 0x0 /* 0 */
#define RADIO_CDC_ERROR_127_96_WIDTH 0x20 /* 32 */
#define RADIO_CDC_ERROR_127_96_DEFAULT 0x0 /* 0 */

/* Type = roSig */
#define RADIO_CDC_STATUS_31_0_ADDR 0x40 /* 64 */
#define RADIO_CDC_STATUS_31_0_MASK 0xffffffff /* 4294967295 */
#define RADIO_CDC_STATUS_31_0_OFFSET 0x0 /* 0 */
#define RADIO_CDC_STATUS_31_0_WIDTH 0x20 /* 32 */
#define RADIO_CDC_STATUS_31_0_DEFAULT 0x0 /* 0 */

/* Type = roSig */
#define RADIO_CDC_STATUS_63_32_ADDR 0x44 /* 68 */
#define RADIO_CDC_STATUS_63_32_MASK 0xffffffff /* 4294967295 */
#define RADIO_CDC_STATUS_63_32_OFFSET 0x0 /* 0 */
#define RADIO_CDC_STATUS_63_32_WIDTH 0x20 /* 32 */
#define RADIO_CDC_STATUS_63_32_DEFAULT 0x0 /* 0 */

/* Type = roSig */
#define RADIO_CDC_STATUS_95_64_ADDR 0x48 /* 72 */
#define RADIO_CDC_STATUS_95_64_MASK 0xffffffff /* 4294967295 */
#define RADIO_CDC_STATUS_95_64_OFFSET 0x0 /* 0 */
#define RADIO_CDC_STATUS_95_64_WIDTH 0x20 /* 32 */
#define RADIO_CDC_STATUS_95_64_DEFAULT 0x0 /* 0 */

/* Type = roSig */
#define RADIO_CDC_STATUS_127_96_ADDR 0x4c /* 76 */
#define RADIO_CDC_STATUS_127_96_MASK 0xffffffff /* 4294967295 */
#define RADIO_CDC_STATUS_127_96_OFFSET 0x0 /* 0 */
#define RADIO_CDC_STATUS_127_96_WIDTH 0x20 /* 32 */
#define RADIO_CDC_STATUS_127_96_DEFAULT 0x0 /* 0 */


/*-----------------------------------------------------------------------------
* C Header bank register definitions for bank roe_radio_source 
* with prefix fram_ @ address 0x1000
*------------------------------------------------------------------------------
*/
/* Type = rwpdef */
#define FRAM_PACKET_DATA_SIZE_ADDR 0x1000 /* 4096 */
#define FRAM_PACKET_DATA_SIZE_MASK 0x7f /* 127 */
#define FRAM_PACKET_DATA_SIZE_OFFSET 0x0 /* 0 */
#define FRAM_PACKET_DATA_SIZE_WIDTH 0x7 /* 7 */
#define FRAM_PACKET_DATA_SIZE_DEFAULT 0x0 /* 0 */

/* Type = rwpdef */
#define FRAM_PAUSE_DATA_SIZE_ADDR 0x1004 /* 4100 */
#define FRAM_PAUSE_DATA_SIZE_MASK 0x7f /* 127 */
#define FRAM_PAUSE_DATA_SIZE_OFFSET 0x0 /* 0 */
#define FRAM_PAUSE_DATA_SIZE_WIDTH 0x7 /* 7 */
#define FRAM_PAUSE_DATA_SIZE_DEFAULT 0x0 /* 0 */


