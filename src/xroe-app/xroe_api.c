// SPDX-License-Identifier: BSD-3-Clause
/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.
 *
 ******************************************************************************/ 

/** 
* @file xroe_api.c
* @addtogroup framer_driver_api
* @{
*
*  Radio over Ethernet Framer driver API library 
*
*
******************************************************************************/

/***************************** Include Files *********************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <errno.h>
#include <syslog.h>
#include <sys/ioctl.h>
#include <roe_framer_ctrl.h>

/* IOCTL commands */
/* Use 0xF5 as magic number */
#define XROE_FRAMER_MAGIC_NUMBER  0xF5

#define XROE_FRAMER_IOSET		_IOW(XROE_FRAMER_MAGIC_NUMBER,  0, uint32_t)
#define XROE_FRAMER_IOGET 	_IOR(XROE_FRAMER_MAGIC_NUMBER,  1, uint32_t)

struct ioctl_arguments {
         uint32_t *offset;
         uint32_t *value;
 };

/*****************************************************************************/
/**
*
* Reads bytes from anywhere in the framer address space.
* Reads bytes from the address given and writes them into pRead.
*
* @param [in]	addr   Address in framer address space (0 base) to read from
* @param [out]	pRead  Pointer to use to store output
* @param [in]	length Number of bytes to read
*
* @return
*		- 0 on success
*		- EIO on device open failure
*		- EFAULT on pread() failure
*
******************************************************************************/
int IP_API_Read(int addr, uint8_t *pRead, int length)
{
	int fd=0;
	int read = 0;
	int ret = 0;
	fd=open("/dev/xroe/ip", O_RDONLY);

	if(fd>0)
	{
		read = pread(fd, pRead, length, addr);
		if(!read)
		{
			ret = EFAULT;
		}

		close(fd);
	}
	else
	{
		ret = EIO;
	}
	
	return ret;
}

/*****************************************************************************/
/**
*
* Writes bytes to anywhere in the framer address space.
* Writes length bytes to addr from pWrite.
*
* @param [in]	addr   Address in framer address space (0 base) to write to
* @param [in]	pWrite Pointer to read from
* @param [in]	length Number of bytes to write
*
* @return
*		- 0 on success
*		- EIO on device open failure
*		- EFAULT on pwrite() failure
*
******************************************************************************/
int IP_API_Write(int addr, uint8_t *pWrite, int length)
{
	int fd=0;
	int write = 0;
	int ret = 0;
	fd=open("/dev/xroe/ip", O_WRONLY);

	if(fd>0)
	{
		write = pwrite(fd, pWrite, length, addr);
		if(!write)
		{
			ret = EFAULT;
		}

		close(fd);
	}
	else
	{
		ret = EIO;
	}

	return ret;
}

/*****************************************************************************/
/**
*
* Reads 32-bits from anywhere in the framer address space.
* Reads 32-bits from the address given, shifts and masks them
* as per Offset and Mask, and writes them into pRead.
*
* @param [in]  	addr   Address in framer address space (0 base) to read from
* @param [out] 	pRead  Pointer to use to store output
* @param [in]  	Mask   Mask to use to mask output before shifting
* @param [in]  	Offset Number of bits to shift output down by after masking
*
* @return
*		- 0 on success
*		- EIO on device open failure
*		- EFAULT on pread() failure
*
******************************************************************************/
int IP_API_Read_Register(int addr, unsigned int *pRead, int Mask, int Offset)
{
	int fd=0;
	unsigned int buf;
	int ret = 0;
	struct ioctl_arguments args;

	fd=open("/dev/xroe/ip", O_WRONLY);

	if(fd>0)
	{
		args.offset = (uint32_t *)&addr;
		args.value = (uint32_t *)&buf;
		
		ret = ioctl(fd, XROE_FRAMER_IOGET, &args);
		if(!ret)
		{
			*pRead = (buf & Mask) >> Offset;
		}

		close(fd);
	}
	return ret;
}


/*****************************************************************************/
/**
*
* Writes 32-bits to anywhere in the framer address space.
* Writes 32-bits from Write to the address given, after
* shifting and masking them as per Offset and Mask.
*
* @param [in]  	addr   Address in framer address space (0 base) to write to
* @param [in] 	Write  Value to write
* @param [in]  	Mask   Mask to use to mask input after shifting
* @param [in]  	Offset Number of bits to shift input up by before masking
*
* @return
*		- 0 on success
*		- EIO on device open failure
*		- EFAULT on pwrite() failure
*
******************************************************************************/
int IP_API_Write_Register(int addr, unsigned int Write, int Mask, int Offset)
{
	int fd = 0;
	int buf;
	int ret = 0;
	struct ioctl_arguments args;

	fd=open("/dev/xroe/ip", O_WRONLY);

	if(fd>0)
	{
		args.offset = (uint32_t *)&addr;
		args.value = (uint32_t *)&buf;

		buf = (Write << Offset) & Mask;
		ret = ioctl(fd, XROE_FRAMER_IOSET, &args);

		close(fd);
	}

	return ret;
}

/*****************************************************************************/
/**
*
* Reads 32-bits from the stats address space.
* Reads 32-bits from the address given and writes them into pRead.
*
* @param [in]  	addr   Address in stats address space (0 base) to read from
* @param [out] 	pRead  Pointer to use to store output
*
* @return
*		- 0 on success
*		- EIO on device open failure
*		- EFAULT on ioctl() failure
*
******************************************************************************/
int STATS_API_Read_Register(int addr, unsigned int *pRead)
{
	int fd = 0;
	unsigned int buf;
	int ret = 0;
	struct ioctl_arguments args;

	fd = open("/dev/xroe/stats", O_RDONLY);

	if(fd>0)
	{
		args.offset = (uint32_t *)&addr;
		args.value = (uint32_t *)&buf;
		
		ret = ioctl(fd, XROE_FRAMER_IOGET, &args);
		
		if (!ret)
		{
			*pRead = buf;
		}
		else
		{
			ret = EFAULT;
		}

		close(fd);
	}
	else
	{
		ret = EIO;
	}

	return ret;
}

/*****************************************************************************/
/**
*
* Enables or disables the framer.
* Writes true or false to framer restart sysfs variable to enable.
* or disable the framer
*
* @param [in]	restart Non-zero to disable (assert restart), zero to enable
*
* @return
*		- 0 on success
*		- -1 on device open failure
*		- EFAULT on write() failure
*
******************************************************************************/
int FRAMER_API_Framer_Restart(int restart)
{
	int fd;
	int w;
	int ret = -1;
	fd = open("/sys/kernel/xroe/framer_restart", O_WRONLY);

	if(fd>0)
	{
		if(restart)
		{
			w = write (fd, "true", 1);
		}
		else
		{
			w = write (fd, "false", 1);
		}
		
		if(w)
		{
			ret = 0;
		}
		else
		{
			ret = EFAULT;
		}
		close(fd);
	}

	return ret;
}

/*****************************************************************************/
/**
*
* Enables or disables the deframer.
* Writes true or false to deframer restart sysfs variable to 
* enable or disable the deframer.
*
* @param [in]	restart Non-zero to disable (assert restart), zero to enable
*
* @return
*		- 0 on success
*		- -1 on device open failure
*		- EFAULT on write() failure
*
******************************************************************************/
int FRAMER_API_Deframer_Restart(int restart)
{
	int fd;
	int w;
	int ret = -1;
	fd = open("/sys/kernel/xroe/deframer_restart", O_WRONLY);

	if(fd>0)
	{
		if(restart)
		{
			w = write (fd, "true", 1);
		}
		else
		{
			w = write (fd, "false", 1);
		}
		
		if(w)
		{
			ret = 0;
		}
		else
		{
			ret = EFAULT;
		}
		close(fd);
	}

	return ret;
}

/*****************************************************************************/
/**
* Reads bytes from anywhere in the radio_ctrl address space.
* Reads bytes from the address given and writes them into pRead.
*
* @param [in]  addr   Address in radio_ctrl address space (0 base) to read from
* @param [out] pRead  Pointer to use to store output
* @param [in]  length Number of bytes to read
*
* @return     	
*		- 0 on success
*		- EIO on device open failure
*		- EFAULT on pread() failure
*
******************************************************************************/
int RADIO_CTRL_API_Read(int addr, uint8_t *pRead, int length)
{
	int fd=0;
	int read = 0;
	int ret = 0;

	fd=open("/dev/xroe/radio_ctrl", O_RDONLY);

	if(fd>0)
	{
		read = pread(fd, pRead, length, addr);
		if(!read)
		{
			ret = EFAULT;
		}

		close(fd);
	}
	else
	{
		ret = EIO;
	}

	return ret;
}

/*****************************************************************************/
/**
* Writes bytes to anywhere in the radio_ctrl address space.
* Writes length bytes from pWrite to addr.
*
* @param [in]	addr   Address in radio_ctrl address space (0 base) to write to
* @param [in]	pWrite Pointer to read from
* @param [in]	length Number of bytes to write
*
* @return		
*		- 0 on success
*		- EIO on device open failure
*		- EFAULT on pwrite() failure
*
******************************************************************************/
int RADIO_CTRL_API_Write(int addr, uint8_t *pWrite, int length)
{
	int fd=0;
	int write = 0;
	int ret = 0;
	fd=open("/dev/xroe/radio_ctrl", O_WRONLY);

	if(fd>0)
	{
		write = pwrite(fd, pWrite, length, addr);
		if(!write)
		{
			ret = EFAULT;
		}

		close(fd);
	}
	else
	{
		ret = EIO;
	}

	return ret;
}

/*****************************************************************************/
/**
* Reads 32-bits from anywhere in the radio_ctrl address space.
* Reads 32-bits from addr to pRead, shifted and masked as per 
* Offset and Mask.
*
* @param [in]  addr   Address in radio_ctrl address space (0 base) to read from
* @param [in]  pRead  Pointer to use to store output
* @param [in]  Mask   Mask to use to mask output before shifting
* @param [in]  Offset Number of bits to shift output down by after masking
*
* @return
*		- 0 on success
*		- EIO on device open failure
*		- EFAULT on pread() failure
*
******************************************************************************/
int RADIO_CTRL_API_Read_Register(int addr, unsigned int *pRead, int Mask, int Offset)
{
	int buf;
	int ret = 0;

	ret = RADIO_CTRL_API_Read(addr, (uint8_t *)&buf, sizeof(buf));
	if(!ret)
	{
		*pRead = (buf & Mask) >> Offset;;
	}

	return ret;
}

/*****************************************************************************/
/**
*
* Writes 32-bits to anywhere in the radio_ctrl address space.
* Performs a Read/Modify/Write of 32-bits from pWrite to addr,
* shifted and masked as per Offset and Mask.
*
* @param [in]	addr   Address in radio_ctrl address space (0 base) to write to
* @param [in]	value  Value to write
* @param [in]	Mask   Mask to use to mask input after shifting
* @param [in]	Offset Number of bits to shift input up by before masking
*
* @return		
*		- 0 on success
*		- EIO on device open failure
*		- EFAULT on pwrite() failure
*
******************************************************************************/
int RADIO_CTRL_API_Write_Register(int addr, unsigned int value, int Mask, int Offset)
{
	unsigned int ReadRegisterValue = 0, WriteRegisterValue = 0, Delta = 0, Buffer = 0;
	int ret = 0;
	int WorkingAddr = 0;

	WorkingAddr = addr;
	ret = RADIO_CTRL_API_Read_Register(WorkingAddr, &ReadRegisterValue, 0xffffffff, 0);
	printf("ReadRegisterValue: %d\n", ReadRegisterValue);

	if(!ret)
	{
		Buffer = (value << Offset);
		WriteRegisterValue = ReadRegisterValue&~Mask;
		Delta = Buffer&Mask;
		WriteRegisterValue |= Delta;
		printf("WriteRegisterValue: %d\n", WriteRegisterValue);
		ret = RADIO_CTRL_API_Write(WorkingAddr, (uint8_t *)&WriteRegisterValue, sizeof(WriteRegisterValue));
	}

	return ret;
}

/*****************************************************************************/
/**
*
* Resets the XXV Ethernet interface.
* Asserts then de-asserts the XXV Ethernet reset.
*
* @return
*		- 0 on success
*		- -1 on device open failure
*		- EFAULT on write() failure
*
******************************************************************************/
int XXV_API_Reset(void)
{
	int fd;
	int w;
	int ret = -1;
	fd = open("/sys/kernel/xroe/xxv_reset", O_WRONLY);

	if (fd>0)
	{
		w = write(fd, "true", 1);
		if (w)
		{
			ret = 0;
		}
		else
		{
			ret = EFAULT;
		}

		w = write(fd, "false", 1);
		if (w)
		{
			ret = 0;
		}
		else
		{
			ret = EFAULT;
		}
		close(fd);
	}

	return ret;
}
/** @} */
