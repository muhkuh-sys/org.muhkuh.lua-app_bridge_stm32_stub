/***************************************************************************
 *   Copyright (C) 2020 by Christoph Thelen                                *
 *   doc_bacardi@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "main_module.h"

#include <string.h>

#include "pad_control.h"
#include "rdy_run.h"
#include "systime.h"
#include "uart.h"

/*-------------------------------------------------------------------------*/

#define STM32BOOT_ACK                  0x79U
#define STM32BOOT_NACK                 0x1fU

#define STM32BOOTCMD_Get               0x00U
#define STM32BOOTCMD_GetVersion        0x01U
#define STM32BOOTCMD_GetID             0x02U
#define STM32BOOTCMD_ReadMemory        0x11U
#define STM32BOOTCMD_Go                0x21U
#define STM32BOOTCMD_WriteMemory       0x31U
#define STM32BOOTCMD_Erase             0x43U
#define STM32BOOTCMD_ExtendedErase     0x44U
#define STM32BOOTCMD_WriteProtect      0x63U
#define STM32BOOTCMD_WriteUnprotect    0x73U
#define STM32BOOTCMD_ReadoutProtect    0x82U
#define STM32BOOTCMD_ReadoutUnprotect  0x92U


//SPI_CFG_T s_tSpiCfg;

typedef union PTR_UNION
{
	unsigned long ul;
	unsigned char *puc;
	unsigned short *pus;
	unsigned long *pul;
} PTR_T;

/*
typedef struct HISPI_PARAMETER_STRUCT
{
	BOOT_SPI_CONFIGURATION_T tSpiConfiguration;
	unsigned int uiUnit;
	unsigned int uiChipSelect;
} HISPI_PARAMETER_T;
*/

extern unsigned char __buffer_start__[];


/*-------------------------------------------------------------------------*/


static const unsigned char aucPadCtrlUartAppIndex[2] =
{
	PAD_AREG2OFFSET(mmio, 1),    /* MMIO1 */
	PAD_AREG2OFFSET(mmio, 2)     /* MMIO2 */
};



static const unsigned char aucPadCtrlUartAppConfig[2] =
{
	PAD_CONFIGURATION(PAD_DRIVING_STRENGTH_Low, PAD_PULL_Enable, PAD_INPUT_Enable),
	PAD_CONFIGURATION(PAD_DRIVING_STRENGTH_Low, PAD_PULL_Enable, PAD_INPUT_Enable)
};



static void setup_padctrl(void)
{
	pad_control_apply(aucPadCtrlUartAppIndex, aucPadCtrlUartAppConfig, sizeof(aucPadCtrlUartAppIndex));
}


/*-------------------------------------------------------------------------*/

/* NOTE: This project needs a custom uart_init function. The communication uses a parity of "even".
 *       The routine in the platform library sets up "no" parity.
 */
static void uart_initialize(void)
{
	unsigned long ulValue;
	HOSTDEF(ptUartAppArea);
	HOSTDEF(ptAsicCtrlArea);
	HOSTDEF(ptMmioCtrlArea);


	/* Disable the UART. */
	ptUartAppArea->ulUartcr = 0;

	/* Use baud rate mode 2. */
	ptUartAppArea->ulUartcr_2 = HOSTMSK(uartcr_2_Baud_Rate_Mode);

	/* Set the baud rate. */
	ulValue = UART_BAUDRATE_DIV(UART_BAUDRATE_115200);
	ptUartAppArea->ulUartlcr_l = ulValue & 0xffU;
	ptUartAppArea->ulUartlcr_m = ulValue >> 8;

	/* Set the UART to 8E1, FIFO enabled. */
	ulValue  = HOSTMSK(uartlcr_h_WLEN);
	ulValue |= HOSTMSK(uartlcr_h_FEN);
	ulValue |= HOSTMSK(uartlcr_h_PEN);
	ulValue |= HOSTMSK(uartlcr_h_EPS);
	ptUartAppArea->ulUartlcr_h = ulValue;

	/* Disable all drivers. */
	ptUartAppArea->ulUartdrvout = 0;

	/* Disable RTS/CTS mode. */
	ptUartAppArea->ulUartrts = 0;

	/* Enable the UART. */
	ptUartAppArea->ulUartcr = HOSTMSK(uartcr_uartEN);

	/* Setup MMIO2 as UART_APP_RXD. */
	ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
	ptMmioCtrlArea->aulMmio_cfg[2] = NX90_MMIO_CFG_UART_APP_RXD;

	/* Enable the drivers. */
	ulValue = HOSTMSK(uartdrvout_DRVTX);
	ptUartAppArea->ulUartdrvout = ulValue;

	/* Setup MMIO1 as UART_APP_TXD. */
	ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
	ptMmioCtrlArea->aulMmio_cfg[1] = NX90_MMIO_CFG_UART_APP_TXD;
}



/* Remove all elements from the receive fifo. */
static void uart_clean_receive_fifo(void)
{
	HOSTDEF(ptUartAppArea);
	unsigned long ulValue;


	do
	{
		/* Check for data in the FIFO. */
		ulValue  = ptUartAppArea->ulUartfr;
		ulValue &= HOSTMSK(uartfr_RXFE);
		if( ulValue==0 )
		{
			/* The FIFO is not empty, get the received byte. */
			ptUartAppArea->ulUartdr;
		}
	} while( ulValue==0 );
}



static void uart_send(unsigned char ucData)
{
	HOSTDEF(ptUartAppArea);
	unsigned long ulValue;


	/* Wait until there is space in the FIFO. */
	do
	{
		ulValue  = ptUartAppArea->ulUartfr;
		ulValue &= HOSTMSK(uartfr_TXFF);
	} while( ulValue!=0 );

	ptUartAppArea->ulUartdr = ucData;
}



static unsigned long uart_receive(unsigned char *pucData, unsigned long ulTimeoutInMs)
{
	HOSTDEF(ptUartAppArea);
	unsigned long ulResult;
	unsigned long ulTimer;
	unsigned long ulValue;
	int iTimerElapsed;


	ulResult = STM32_RESULT_UartTimout;

	/* Get the current system tick. */
	ulTimer = systime_get_ms();
	do
	{
		/* Is data in the receive FIFO? */
		ulValue  = ptUartAppArea->ulUartfr;
		ulValue &= HOSTMSK(uartfr_RXFE);
		/* Timer elapsed? */
		if( ulTimeoutInMs!=0 )
		{
			iTimerElapsed = systime_elapsed(ulTimer, ulTimeoutInMs);
		}
	} while( ulValue!=0 && iTimerElapsed==0 );

	/* Is data in the FIFO? */
	if( ulValue==0 )
	{
		ulValue  = ptUartAppArea->ulUartdr;
		ulValue &= 0xffU;
		if( pucData!=NULL )
		{
			*pucData = (unsigned char)ulValue;
		}
		ulResult = STM32_RESULT_Ok;
	}

	return ulResult;
}



static void stm32boot_send_command(unsigned char ucCommand)
{
	uart_send(ucCommand);
	uart_send(ucCommand^0xff);
}



static unsigned long stm32boot_wait_for_ack(unsigned long ulTimeoutInMs)
{
	unsigned long ulResult;
	unsigned char ucData;


	do
	{
		/* Wait for ACK/NACK. */
		/* FIXME: This discards silently all other characters. The flow chart looks like it should by done like this, but is this really OK? */
		ulResult = uart_receive(&ucData, ulTimeoutInMs);
	} while( ulResult==STM32_RESULT_Ok && ucData!=STM32BOOT_ACK && ucData!=STM32BOOT_NACK );

	if( ulResult==STM32_RESULT_Ok )
	{
		/* Is this an ACK? */
		if( ucData==STM32BOOT_ACK )
		{
			ulResult = STM32_RESULT_Ok;
		}
		else
		{
			ulResult = STM32_RESULT_NACK;
		}
	}

	return ulResult;
}



static void stm32boot_send_xor_data(const unsigned char *pucData, unsigned int sizData)
{
	const unsigned char *pucCnt;
	const unsigned char *pucEnd;
	unsigned char ucXor;
	unsigned char ucData;


	pucCnt = pucData;
	pucEnd = pucData + sizData;
	ucXor = 0x00;
	while( pucCnt<pucEnd )
	{
		ucData = *(pucCnt++);
		ucXor = ucXor ^ ucData;
		uart_send(ucData);
	}
	uart_send(ucXor);
}



static unsigned long execute_command(unsigned char ucCommand, unsigned char *pucData, unsigned int uiDataMaxSize, unsigned int *puiDataReceived)
{
	unsigned long ulResult;
	unsigned char ucData;
	unsigned int uiReceiveLen;
	unsigned int uiReceivePos;


	/* Send the command. */
	stm32boot_send_command(ucCommand);

	/* Wait for ACK/NACK with a timeout of 1 second. */
	ulResult = stm32boot_wait_for_ack(1000);
	if( ulResult==STM32_RESULT_Ok )
	{
		/* Get the number of bytes to receive.
		 * NOTE: The STM32 bootloadersends the number of bytes - 1 .
		 */
		ulResult = uart_receive(&ucData, 250);
		if( ulResult==STM32_RESULT_Ok )
		{
			uiReceivePos = 0;
			uiReceiveLen = ucData + 1U;
			/* Loop over all bytes. */
			while( uiReceivePos<uiReceiveLen )
			{
				/* Get the next byte. */
				ulResult = uart_receive(&ucData, 250);
				if( ulResult==STM32_RESULT_Ok )
				{
					/* Is enough space left in the buffer? */
					if( uiReceivePos<uiDataMaxSize )
					{
						pucData[uiReceivePos] = ucData;
					}
					++uiReceivePos;
				}
				else
				{
					break;
				}
			}

			if( ulResult==STM32_RESULT_Ok )
			{
				if( puiDataReceived!=NULL )
				{
					*puiDataReceived = uiReceivePos;
				}
			}

			/* Wait for ACK/NACK with a timeout of 250ms. */
			ulResult = stm32boot_wait_for_ack(250);
		}
	}

	return ulResult;
}



static unsigned long execute_command_read_memory(unsigned long ulAddress __attribute__((unused)), unsigned long *pulData __attribute__((unused)))
{
	/* FIXME: Continue here. */
	return STM32_RESULT_UnalignedAddress;
}



static unsigned long execute_command_write_memory(unsigned long ulAddress __attribute__((unused)), unsigned long ulData __attribute__((unused)))
{
	/* FIXME: Continue here. */
	return STM32_RESULT_UnalignedAddress;
}




static unsigned long install_stub(void)
{
	unsigned long ulResult;
	unsigned char aucCpuId[2];
	unsigned char aucBuffer[256];
	unsigned int uiDataReceived;
	unsigned int fHasCmdReadMemory;
	unsigned int fHasCmdWriteMemory;
	unsigned int fHasCmdGo;
	unsigned int uiCnt;
	unsigned char ucData;


	/* NOTE: The STM32 must be already reset. */

	/* Send 0x7f and expect an ACK. */
	uart_send(0x7fU);
	ulResult = stm32boot_wait_for_ack(1000);
	if( ulResult==STM32_RESULT_Ok )
	{
		/* Get the CPU ID.
		 * Send the "GetID" command and expect the result 0x04 0x50.
		 */
		ulResult = execute_command(STM32BOOTCMD_GetID, aucCpuId, sizeof(aucCpuId), &uiDataReceived);
		if( ulResult==STM32_RESULT_Ok )
		{
			if( uiDataReceived!=2 )
			{
				ulResult = STM32_RESULT_UnexpectedSizeOfResult | (uiDataReceived<<8U);
			}
			else
			{
				/* The CPU ID must be 0x04 0x50. */
				if( aucCpuId[0]==0x04 && aucCpuId[1]==0x50 )
				{
					/* Get the list of supported commands.
					 * Send the "Get" command and look for the "ReadMemory", "WriteMemory" and "Go" commands.
					 */
					fHasCmdReadMemory = 0;
					fHasCmdWriteMemory = 0;
					fHasCmdGo = 0;
					ulResult = execute_command(STM32BOOTCMD_Get, aucBuffer, sizeof(aucBuffer), &uiDataReceived);
					if( ulResult==STM32_RESULT_Ok )
					{
						for(uiCnt=0; uiCnt<uiDataReceived; ++uiCnt)
						{
							ucData = aucBuffer[uiCnt];
							if( ucData==STM32BOOTCMD_ReadMemory )
							{
								fHasCmdReadMemory = 1U;
							}
							else if( ucData==STM32BOOTCMD_WriteMemory )
							{
								fHasCmdWriteMemory = 1U;
							}
							else if( ucData==STM32BOOTCMD_Go )
							{
								fHasCmdGo = 1U;
							}
						}
						if( fHasCmdReadMemory!=0U && fHasCmdWriteMemory!=0U && fHasCmdGo!=0U )
						{
							ulResult = 11U;
						}
						else
						{
							ulResult = STM32_RESULT_RequiredCommandsUnavailable;
						}
					}
				}
				else
				{
					ulResult = STM32_RESULT_UnexpectedCpuID;
				}
			}
		}
	}

	return ulResult;
}



static unsigned long module_command_read32(unsigned long ulAddress, unsigned long *pulData)
{
	unsigned long ulResult;


	if( (ulAddress&3)!=0 )
	{
		ulResult = STM32_RESULT_UnalignedAddress;
	}
	else
	{
		ulResult = execute_command_read_memory(ulAddress, pulData);
	}

	return ulResult;
}



static unsigned long module_command_write32(unsigned long ulAddress, unsigned long ulData)
{
	unsigned long ulResult;


	if( (ulAddress&3)!=0 )
	{
		ulResult = STM32_RESULT_UnalignedAddress;
	}
	else
	{
		ulResult = execute_command_write_memory(ulAddress, ulData);
	}

	return ulResult;
}



static unsigned long module_command_sequence(unsigned long ulSequenceSize)
{
	unsigned long ulResult;
	unsigned char *pucSequenceCnt;
	unsigned char *pucSequenceEnd;
	unsigned char *pucOutCnt;
	unsigned long ulSizeLeft;
	STM32_COMMAND_T tCommand;
	unsigned long ulRegisterAddress;
	unsigned long ulData;


	ulResult = STM32_RESULT_Ok;

	pucOutCnt = __buffer_start__;

	/* Loop over all sequences. */
	pucSequenceCnt = __buffer_start__;
	pucSequenceEnd = pucSequenceCnt + ulSequenceSize;
	while( pucSequenceCnt<pucSequenceEnd )
	{
		ulResult = STM32_RESULT_InvalidSequenceCommand;
		tCommand = (STM32_COMMAND_T)(pucSequenceCnt[0]);
		switch(tCommand)
		{
		case STM32_COMMAND_Initialize:
			/* The "initialize" command can not be used in a sequence. */
			break;

		case STM32_COMMAND_ReadData32:
		case STM32_COMMAND_WriteData32:
			ulResult = STM32_RESULT_Ok;
			break;

		case STM32_COMMAND_RunSequence:
			/* The "run sequence" command can not be used in a sequence. */
			break;
		}
		if( ulResult==STM32_RESULT_Ok )
		{
			ulSizeLeft = (unsigned long)(pucSequenceEnd - pucSequenceCnt);

			ulResult = STM32_RESULT_InvalidSequenceCommand;
			switch(tCommand)
			{
			case STM32_COMMAND_Initialize:
				/* The "initialize" command can not be used in a sequence. */
				break;

			case STM32_COMMAND_ReadData32:
				/* The ReadData32 command needs 5 bytes. */
				if( ulSizeLeft<5U )
				{
					ulResult = STM32_RESULT_NotEnoughSequenceData;
				}
				else
				{
					ulRegisterAddress  = (unsigned long)(pucSequenceCnt[1]);
					ulRegisterAddress |= (unsigned long)(pucSequenceCnt[2] <<  8U);
					ulRegisterAddress |= (unsigned long)(pucSequenceCnt[3] << 16U);
					ulRegisterAddress |= (unsigned long)(pucSequenceCnt[4] << 24U);

					if( (ulRegisterAddress&3)!=0 )
					{
						ulResult = STM32_RESULT_UnalignedAddress;
					}
					else
					{
						ulResult = module_command_read32(ulRegisterAddress, &ulData);
						if( ulResult==STM32_RESULT_Ok )
						{
							pucOutCnt[0] = (unsigned char)( ulData        & 0xffU);
							pucOutCnt[1] = (unsigned char)((ulData >>  8) & 0xffU);
							pucOutCnt[2] = (unsigned char)((ulData >> 16) & 0xffU);
							pucOutCnt[3] = (unsigned char)((ulData >> 24) & 0xffU);

							pucSequenceCnt += 5U;
							pucOutCnt += 4U;
						}
					}
				}
				break;

			case STM32_COMMAND_WriteData32:
				/* The WriteData32 command needs 9 bytes. */
				if( ulSizeLeft<9U )
				{
					ulResult = STM32_RESULT_NotEnoughSequenceData;
				}
				else
				{
					ulRegisterAddress  = (unsigned long)(pucSequenceCnt[1]);
					ulRegisterAddress |= (unsigned long)(pucSequenceCnt[2] <<  8U);
					ulRegisterAddress |= (unsigned long)(pucSequenceCnt[3] << 16U);
					ulRegisterAddress |= (unsigned long)(pucSequenceCnt[4] << 24U);

					ulData  = (unsigned long)(pucSequenceCnt[5]);
					ulData |= (unsigned long)(pucSequenceCnt[6] <<  8U);
					ulData |= (unsigned long)(pucSequenceCnt[7] << 16U);
					ulData |= (unsigned long)(pucSequenceCnt[8] << 24U);

					if( (ulRegisterAddress&3)!=0 )
					{
						ulResult = STM32_RESULT_UnalignedAddress;
					}
					else
					{
						ulResult = module_command_write32(ulRegisterAddress, ulData);
						if( ulResult==STM32_RESULT_Ok )
						{
							pucSequenceCnt += 9U;
						}
					}
				}
				break;

			case STM32_COMMAND_RunSequence:
				/* The "run sequence" command can not be used in a sequence. */
				break;
			}

			if( ulResult!=STM32_RESULT_Ok )
			{
				break;
			}
		}
	}

	return ulResult;
}



unsigned long module(unsigned long ulParameter0, unsigned long ulParameter1, unsigned long ulParameter2, unsigned long ulParameter3 __attribute__((unused)))
{
	unsigned long ulResult;
	PTR_T tPtr;


	if( ulParameter0==STM32_COMMAND_Initialize )
	{
		/* Initialize has no parameter */
		setup_padctrl();
		uart_initialize();
		uart_clean_receive_fifo();
		ulResult = install_stub();
	}
	else if( ulParameter0==STM32_COMMAND_ReadData32 )
	{
		/* ReadData32 has 2 parameter:
		 *  ulParameter1 = address in STM32 memory
		 *  ulParameter2 = destination address in netX APP memory for the data
		 */
		tPtr.ul = ulParameter2;
		ulResult = module_command_read32(ulParameter1, tPtr.pul);
	}
	else if( ulParameter0==STM32_COMMAND_WriteData32 )
	{
		/* WriteData32 has 2 parameter:
		 *  ulParameter1 = address in STM32 memory
		 *  ulParameter2 = data to write to the STM32
		 */
		ulResult = module_command_write32(ulParameter1, ulParameter2);
	}
	else if( ulParameter0==STM32_COMMAND_RunSequence )
	{
		/* RunSequence has 1 parameter:
		 * ulParameter1 = the start address of the sequence data
		 */
		ulResult = module_command_sequence(ulParameter1);
	}
	else
	{
		ulResult = STM32_RESULT_UnknownCommand;
	}

	return ulResult;
}

/*-----------------------------------*/
