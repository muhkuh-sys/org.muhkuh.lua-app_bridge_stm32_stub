#include <stdio.h>

#include "hash/sha384.h"


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

/* Custom commands. */
#define STM32BOOTCMD_HashMemory        0xC0U


typedef union PTR_UNION
{
	unsigned long ul;
	unsigned char *puc;
	unsigned short *pus;
	unsigned long *pul;
} PTR_T;


/* Define some peripherals here. */
typedef struct STM32H7xx_GPIO_AREA_Ttag
{
	volatile uint32_t  ulMode;
	volatile uint32_t  ulOType;
	volatile uint32_t  ulOSpeed;
	volatile uint32_t  ulPuPd;
	volatile uint32_t  ulId;
	volatile uint32_t  ulOd;
	volatile uint32_t  ulBs;
	volatile uint32_t  ulLck;
	volatile uint32_t  ulAfL;
	volatile uint32_t  ulAfH;
} STM32H7xx_GPIO_AREA_T;


typedef struct STM32H7xx_UART_AREA_Ttag
{
	volatile uint32_t  ulCR1;
	volatile uint32_t  ulCR2;
	volatile uint32_t  ulCR3;
	volatile uint32_t  ulBRR;
	volatile uint32_t  ulGTPR;
	volatile uint32_t  ulRTOR;
	volatile uint32_t  ulRQR;
	volatile uint32_t  ulISR;
	volatile uint32_t  ulICR;
	volatile uint32_t  ulRDR;
	volatile uint32_t  ulTDR;
	volatile uint32_t  ulPRESC;
} STM32H7xx_UART_AREA_T;


typedef struct STM32H7xx_RCC_AREA_Ttag
{
	volatile uint32_t CR;
	volatile uint32_t ICSCR;
	volatile uint32_t CRRCR;
	volatile uint32_t RESERVED0;
	volatile uint32_t CFGR;
	volatile uint32_t RESERVED1;
	volatile uint32_t D1CFGR;
	volatile uint32_t D2CFGR;
	volatile uint32_t D3CFGR;
	volatile uint32_t RESERVED2;
	volatile uint32_t PLLCKSELR;
	volatile uint32_t PLLCFGR;
	volatile uint32_t PLL1DIVR;
	volatile uint32_t PLL1FRACR;
	volatile uint32_t PLL2DIVR;
	volatile uint32_t PLL2FRACR;
	volatile uint32_t PLL3DIVR;
	volatile uint32_t PLL3FRACR;
	volatile uint32_t RESERVED3;
	volatile uint32_t D1CCIPR;
	volatile uint32_t D2CCIP1R;
	volatile uint32_t D2CCIP2R;
	volatile uint32_t D3CCIPR;
	volatile uint32_t RESERVED4;
	volatile uint32_t CIER;
	volatile uint32_t CIFR;
	volatile uint32_t CICR;
	volatile uint32_t RESERVED5;
	volatile uint32_t BDCR;
	volatile uint32_t CSR;
	volatile uint32_t RESERVED6;
	volatile uint32_t AHB3RSTR;
	volatile uint32_t AHB1RSTR;
	volatile uint32_t AHB2RSTR;
	volatile uint32_t AHB4RSTR;
	volatile uint32_t APB3RSTR;
	volatile uint32_t APB1LRSTR;
	volatile uint32_t APB1HRSTR;
	volatile uint32_t APB2RSTR;
	volatile uint32_t APB4RSTR;
	volatile uint32_t GCR;
	volatile uint32_t RESERVED7;
	volatile uint32_t D3AMR;
	volatile uint32_t RESERVED8[9];
	volatile uint32_t RSR;
	volatile uint32_t AHB3ENR;
	volatile uint32_t AHB1ENR;
	volatile uint32_t AHB2ENR;
	volatile uint32_t AHB4ENR;
	volatile uint32_t APB3ENR;
	volatile uint32_t APB1LENR;
	volatile uint32_t APB1HENR;
	volatile uint32_t APB2ENR;
	volatile uint32_t APB4ENR;
	volatile uint32_t RESERVED9;
	volatile uint32_t AHB3LPENR;
	volatile uint32_t AHB1LPENR;
	volatile uint32_t AHB2LPENR;
	volatile uint32_t AHB4LPENR;
	volatile uint32_t APB3LPENR;
	volatile uint32_t APB1LLPENR;
	volatile uint32_t APB1HLPENR;
	volatile uint32_t APB2LPENR;
	volatile uint32_t APB4LPENR;
	volatile uint32_t RESERVED10[4];
} STM32H7xx_RCC_AREA_T;

#define Addr_STM32H7xx_gpioa 0x58020000U
#define Addr_STM32H7xx_gpiob 0x58020400U
#define Addr_STM32H7xx_gpioc 0x58020800U
#define Addr_STM32H7xx_gpiod 0x58020C00U
#define Addr_STM32H7xx_gpioe 0x58021000U
#define Addr_STM32H7xx_gpiof 0x58021400U
#define Addr_STM32H7xx_gpiog 0x58021800U
#define Addr_STM32H7xx_gpioh 0x58021C00U
#define Addr_STM32H7xx_gpioi 0x58022000U
#define Addr_STM32H7xx_gpioj 0x58022400U
#define Addr_STM32H7xx_gpiok 0x58022800U

#define Addr_STM32H7xx_usart3 0x40004800U

#define Addr_STM32H7xx_rcc 0x58024400U

#define STM32H7xx_DEF_ptGpioAArea STM32H7xx_GPIO_AREA_T * const ptGpioAArea = (STM32H7xx_GPIO_AREA_T * const)Addr_STM32H7xx_gpioa;
#define STM32H7xx_DEF_ptGpioBArea STM32H7xx_GPIO_AREA_T * const ptGpioBArea = (STM32H7xx_GPIO_AREA_T * const)Addr_STM32H7xx_gpiob;
#define STM32H7xx_DEF_ptGpioCArea STM32H7xx_GPIO_AREA_T * const ptGpioCArea = (STM32H7xx_GPIO_AREA_T * const)Addr_STM32H7xx_gpioc;
#define STM32H7xx_DEF_ptGpioDArea STM32H7xx_GPIO_AREA_T * const ptGpioDArea = (STM32H7xx_GPIO_AREA_T * const)Addr_STM32H7xx_gpiod;
#define STM32H7xx_DEF_ptGpioEArea STM32H7xx_GPIO_AREA_T * const ptGpioEArea = (STM32H7xx_GPIO_AREA_T * const)Addr_STM32H7xx_gpioe;
#define STM32H7xx_DEF_ptGpioFArea STM32H7xx_GPIO_AREA_T * const ptGpioFArea = (STM32H7xx_GPIO_AREA_T * const)Addr_STM32H7xx_gpiof;
#define STM32H7xx_DEF_ptGpioGArea STM32H7xx_GPIO_AREA_T * const ptGpioGArea = (STM32H7xx_GPIO_AREA_T * const)Addr_STM32H7xx_gpiog;
#define STM32H7xx_DEF_ptGpioHArea STM32H7xx_GPIO_AREA_T * const ptGpioHArea = (STM32H7xx_GPIO_AREA_T * const)Addr_STM32H7xx_gpioh;
#define STM32H7xx_DEF_ptGpioIArea STM32H7xx_GPIO_AREA_T * const ptGpioIArea = (STM32H7xx_GPIO_AREA_T * const)Addr_STM32H7xx_gpioi;
#define STM32H7xx_DEF_ptGpioJArea STM32H7xx_GPIO_AREA_T * const ptGpioJArea = (STM32H7xx_GPIO_AREA_T * const)Addr_STM32H7xx_gpioj;
#define STM32H7xx_DEF_ptGpioKArea STM32H7xx_GPIO_AREA_T * const ptGpioKArea = (STM32H7xx_GPIO_AREA_T * const)Addr_STM32H7xx_gpiok;

#define STM32H7xx_DEF_ptUsart3Area STM32H7xx_UART_AREA_T * const ptUsart3Area = (STM32H7xx_UART_AREA_T * const)Addr_STM32H7xx_usart3;

#define STM32H7xx_DEF_ptRccArea STM32H7xx_RCC_AREA_T * const ptRccArea = (STM32H7xx_RCC_AREA_T * const)Addr_STM32H7xx_rcc;


static void setup_gpio_and_uart(void)
{
	STM32H7xx_DEF_ptGpioBArea;
//	STM32H7xx_DEF_ptGpioIArea;
	STM32H7xx_DEF_ptUsart3Area;
	unsigned long ulValue;


	/* Switch GPIO B10 and B11 to peripheral. */
	ulValue  = ptGpioBArea->ulMode;
	ulValue &= 0xff0fffffU;
	ulValue |= 0x00a00000U;
	ptGpioBArea->ulMode = ulValue;

	/* Set the speed of the pins to high. */
	ulValue  = ptGpioBArea->ulOSpeed;
	ulValue &= 0xff0fffffU;
	ulValue |= 0x00f00000U;
	ptGpioBArea->ulOSpeed = ulValue;

	/* Set the alternate function for B10 and B11 to 7. */
	ulValue  = ptGpioBArea->ulAfH;
	ulValue &= 0xffff00ffU;
	ulValue |= 0x00007700U;
	ptGpioBArea->ulAfH = ulValue;
#if 0
	/* Switch GPIO I0 to output. */
	ulValue  = ptGpioIArea->ulMode;
	ulValue &= 0xfffffffcU;
	ulValue |= 0x00000001U;
	ptGpioIArea->ulMode = ulValue;

	/* Switch the LED off. */
	ptGpioIArea->ulOd = 1U;
#endif
	/*
	 * Setup the UART.
	 */
	/* Disable the UART. */
	ptUsart3Area->ulCR1 = 0;

	ptUsart3Area->ulCR2 = 0;
	ptUsart3Area->ulCR3 = 0;
	ptUsart3Area->ulBRR = 0x0000022cU;
	ptUsart3Area->ulGTPR = 0;
	ptUsart3Area->ulRTOR = 0;
	ptUsart3Area->ulRQR = 0;
	ptUsart3Area->ulPRESC = 0;
	/* Enable the UART. */
	ptUsart3Area->ulCR1 = 0x2000140dU;
}


#if 0
static void led_on(void)
{
	STM32H7xx_DEF_ptGpioIArea;


	ptGpioIArea->ulOd = 0U;

	while(1) {};
}
#endif


static unsigned char uart_get(void)
{
	STM32H7xx_DEF_ptUsart3Area;
	unsigned long ulValue;


	/* Wait for RX FIFO not empty. */
	do
	{
		ulValue  = ptUsart3Area->ulISR;
		ulValue &= 1U << 5U;
	} while( ulValue==0 );

	/* Get the data. */
	ulValue = ptUsart3Area->ulRDR;
	ulValue &= 0x000000ffU;

	return (unsigned char)ulValue;
}



static void uart_put(unsigned char ucData)
{
	STM32H7xx_DEF_ptUsart3Area;
	unsigned long ulValue;


	/* Wait for space in the FIFO. */
	do
	{
		ulValue  = ptUsart3Area->ulISR;
		ulValue &= 1U << 7U;
	} while( ulValue==0 );

	ptUsart3Area->ulTDR = (unsigned long)ucData;
}



static unsigned char get_inv_data(void)
{
	unsigned char ucCmd0;
	unsigned char ucCmd1;


	/* Get the first byte. */
	ucCmd1 = uart_get();
	do
	{
		ucCmd0 = ucCmd1;
		ucCmd1 = uart_get();
	} while( ucCmd0!=(ucCmd1^0xffU) );

	return ucCmd0;
}



static int get_xor_data(unsigned char *pucBuffer, unsigned int sizBuffer, unsigned char ucXorInit)
{
	int iResult;
	unsigned int uiCnt;
	unsigned char ucData;
	unsigned char ucXor;


	uiCnt = 0;
	ucXor = ucXorInit;
	while( uiCnt<sizBuffer )
	{
		/* Get the next data byte and store it into the buffer. */
		ucData = uart_get();
		pucBuffer[uiCnt] = ucData;

		/* Update the checksum. */
		ucXor ^= ucData;

		++uiCnt;
	}

	/* Get the checksum. */
	ucData = uart_get();
	iResult = 0;
	if( ucData!=ucXor )
	{
		iResult = -1;
	}
	return iResult;
}



static void send_ack(void)
{
	uart_put(STM32BOOT_ACK);
}



static void send_nack(void)
{
	uart_put(STM32BOOT_NACK);
}



static unsigned long aulBuffer[256 / sizeof(unsigned long)];



static void cmd_read_memory(void)
{
	int iResult;
	unsigned char aucAddressMSB[4];
	PTR_T tPtr;
	unsigned char ucSizeInBytesMinus1;
	unsigned int uiSizeInBytes;
	unsigned int uiSizeInDW;
	unsigned int uiCnt;
	unsigned char *pucBuffer;


	/* ACK the command. */
	send_ack();

	/* Get the address (4 bytes in MSB order). */
	iResult = get_xor_data(aucAddressMSB, sizeof(aucAddressMSB), 0x00);
	if( iResult!=0 )
	{
		send_nack();
	}
	else
	{
		/* Convert the MSB address to a LSB pointer. */
		tPtr.ul  =  (unsigned long)(aucAddressMSB[3]);
		tPtr.ul |= ((unsigned long)(aucAddressMSB[2])) <<  8U;
		tPtr.ul |= ((unsigned long)(aucAddressMSB[1])) << 16U;
		tPtr.ul |= ((unsigned long)(aucAddressMSB[0])) << 24U;

		/* Is the pointer 4 byte aligned? */
		if( (tPtr.ul & 3U)!=0 )
		{
			send_nack();
		}
		else
		{
			/* ACK the address. */
			send_ack();

			/* Get the number of bytes to read - 1. */
			ucSizeInBytesMinus1 = get_inv_data();
			uiSizeInBytes = ucSizeInBytesMinus1 + 1U;
			/* Is the size a multiple of 4? */
			if( (uiSizeInBytes & 3U)!=0 )
			{
				send_nack();
			}
			else
			{
				/* ACK the number of bytes. */
				send_ack();

				uiSizeInDW = uiSizeInBytes / sizeof(unsigned long);

				/* Copy the data with 4 byte accesses. */
				for(uiCnt=0; uiCnt<uiSizeInDW; ++uiCnt)
				{
					aulBuffer[uiCnt] = tPtr.pul[uiCnt];
				}

				pucBuffer = (unsigned char*)aulBuffer;
				for(uiCnt=0; uiCnt<uiSizeInBytes; ++uiCnt)
				{
					uart_put(pucBuffer[uiCnt]);
				}
			}
		}
	}
}



static void cmd_write_memory(void)
{
	int iResult;
	unsigned char aucAddressMSB[4];
	PTR_T tPtr;
	unsigned char ucSizeInBytesMinus1;
	unsigned int uiSizeInBytes;
	unsigned int uiSizeInDW;
	unsigned int uiCnt;
	unsigned char *pucBuffer;


	/* ACK the command. */
	send_ack();

	/* Get the address (4 bytes in MSB order). */
	iResult = get_xor_data(aucAddressMSB, sizeof(aucAddressMSB), 0x00);
	if( iResult!=0 )
	{
		send_nack();
	}
	else
	{
		/* Convert the MSB address to a LSB pointer. */
		tPtr.ul  =  (unsigned long)(aucAddressMSB[3]);
		tPtr.ul |= ((unsigned long)(aucAddressMSB[2])) <<  8U;
		tPtr.ul |= ((unsigned long)(aucAddressMSB[1])) << 16U;
		tPtr.ul |= ((unsigned long)(aucAddressMSB[0])) << 24U;

		/* Is the pointer 4 byte aligned? */
		if( (tPtr.ul & 3U)!=0 )
		{
			send_nack();
		}
		else
		{
			/* ACK the address. */
			send_ack();

			/* Get the number of bytes to read - 1. */
			ucSizeInBytesMinus1 = uart_get();
			uiSizeInBytes = ucSizeInBytesMinus1 + 1U;
			/* Receive all data bytes. */
			pucBuffer = (unsigned char*)aulBuffer;
			iResult = get_xor_data(pucBuffer, uiSizeInBytes, ucSizeInBytesMinus1);
			if( iResult!=0 )
			{
				send_nack();
			}
			else
			{
				/* Is the number of bytes a multiple of 4? */
				if( (uiSizeInBytes & 3U)!=0 )
				{
					send_nack();
				}
				else
				{
					/* ACK the size and data. */
					send_ack();

					uiSizeInDW = uiSizeInBytes / sizeof(unsigned long);

					/* Copy the data with 4 byte accesses. */
					for(uiCnt=0; uiCnt<uiSizeInDW; ++uiCnt)
					{
						tPtr.pul[uiCnt] = aulBuffer[uiCnt];
					}
				}
			}
		}
	}
}



/* Define a buffer for the hash function.
 * This is used in the simulated malloc.
 */
unsigned char aucSha512Buffer[256];
unsigned char aucDigest[48];

static void cmd_hash_memory(void)
{
	int iResult;
	unsigned char aucAddressAndSizeMSB[8];
	PTR_T tPtr;
	unsigned long ulSizeInBytes;
	unsigned int uiCnt;


	/* ACK the command. */
	send_ack();

	/* Get the address and size (2 * 4 bytes in MSB order). */
	iResult = get_xor_data(aucAddressAndSizeMSB, sizeof(aucAddressAndSizeMSB), 0x00);
	if( iResult!=0 )
	{
		send_nack();
	}
	else
	{
		/* Convert the MSB address to a LSB pointer. */
		tPtr.ul  =  (unsigned long)(aucAddressAndSizeMSB[3]);
		tPtr.ul |= ((unsigned long)(aucAddressAndSizeMSB[2])) <<  8U;
		tPtr.ul |= ((unsigned long)(aucAddressAndSizeMSB[1])) << 16U;
		tPtr.ul |= ((unsigned long)(aucAddressAndSizeMSB[0])) << 24U;

		ulSizeInBytes  =  (unsigned long)(aucAddressAndSizeMSB[7]);
		ulSizeInBytes |= ((unsigned long)(aucAddressAndSizeMSB[6])) <<  8U;
		ulSizeInBytes |= ((unsigned long)(aucAddressAndSizeMSB[5])) << 16U;
		ulSizeInBytes |= ((unsigned long)(aucAddressAndSizeMSB[4])) << 24U;

		/* Ack the address and size. */
		send_ack();

		sha384Compute(tPtr.puc, ulSizeInBytes, aucDigest);

		/* Send the digest. */
		for(uiCnt=0; uiCnt<sizeof(aucDigest); ++uiCnt)
		{
			uart_put(aucDigest[uiCnt]);
//			uart_put(aucBigBuffer[uiCnt]);
		}
	}
}



void stub_main(void) __attribute__ ((noreturn));
void stub_main(void)
{
	unsigned char ucCommand;


	setup_gpio_and_uart();

	while(1)
	{
		/* Get a command. */
		ucCommand = get_inv_data();
		if( ucCommand==STM32BOOTCMD_ReadMemory )
		{
			cmd_read_memory();
		}
		else if( ucCommand==STM32BOOTCMD_WriteMemory )
		{
			cmd_write_memory();
		}
		else if( ucCommand==STM32BOOTCMD_HashMemory )
		{
			cmd_hash_memory();
		}
		else
		{
			/* Unknown command. */
			send_nack();
		}
	}
#if 0
	/* WFE has a strage side effect. The program crashes at some strange address. Maybe a vector call at Power Down? */
	while(1)
	{
//		__asm__("WFE");
	}
#endif
}
