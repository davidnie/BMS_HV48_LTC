/***********************************************************************
filename: BSP_global.c
function:  configuration all device

***********************************************************************/
#include "main.h"

/*
 
 */
void BSP_Init(void)
{
	NVIC_Configuration();
	DIO_Configuration();
	CAN1_Configuration();
	ADC_Configuration();
	SPI_LTC_Init();
	SPI_FLASH_Init();
	
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	 /*enable RNG */
	 RNG_Cmd(ENABLE);

	WDG_Init();

	SC_POWER_ON;

}

