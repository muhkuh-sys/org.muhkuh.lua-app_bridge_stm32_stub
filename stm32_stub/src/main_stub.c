#include <stdio.h>

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

#define STM32H7xx_DEF_ptRccArea STM32H7xx_RCC_AREA_T * const ptRccArea = (STM32H7xx_RCC_AREA_T * const)Addr_STM32H7xx_rcc;


void stub_main(void) __attribute__ ((noreturn));
void stub_main(void)
{
	STM32H7xx_DEF_ptRccArea;
	STM32H7xx_DEF_ptGpioIArea;
	unsigned long ulValue;


	/* Enable the clocks for the GPIO I peripheral. */
	ulValue = ptRccArea->AHB4ENR;
	ulValue |= 1U << 7U;
	ptRccArea->AHB4ENR = ulValue;

	/* Switch GPIO I0 to output. */
	ulValue = ptGpioIArea->ulMode;
	ulValue &= 0xfffffffcU;
	ulValue |= 0x00000001U;
	ptGpioIArea->ulMode = ulValue;
	/* Set GPIO I0 to 0 (LED on). */
	ptGpioIArea->ulOd = 0;

	/* WFE has a strage side effect. The program crashes at some strange address. Maybe a vector call at Power Down? */
	while(1)
	{
//		__asm__("WFE");
	}
}
