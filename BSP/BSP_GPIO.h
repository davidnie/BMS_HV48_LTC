/***********************************************************************
filename£ºBSP_GPIO.h
function: initlize and name GPIO
note:
***********************************************************************/
#ifndef _LED_H_
#define _LED_H_

#include "config.h"

	#define LED_R			GPIOC , GPIO_Pin_11
	#define LEDR_ON 		GPIO_SetBits(LED_R)
	#define LEDR_OFF 		GPIO_ResetBits(LED_R)
	#define LEDR_Toggle		GPIO_ToggleBits(LED_R)
#define LED_G			GPIOC , GPIO_Pin_12
#define LEDG_ON 		GPIO_SetBits(LED_G)
#define LEDG_OFF		GPIO_ResetBits(LED_G)
#define LEDG_Toggle 	GPIO_ToggleBits(LED_G)

#define WAKEUP_TRANS			GPIOB , GPIO_Pin_11
#define WAKEUP_TRANS_ON 		GPIO_SetBits(WAKEUP_TRANS)
#define WAKEUP_TRANS_OFF		GPIO_ResetBits(WAKEUP_TRANS)

#define DRY_SIG			GPIOB , GPIO_Pin_10
#define DRY_SIG_ON 		GPIO_ResetBits(DRY_SIG)
#define DRY_SIG_OFF		GPIO_SetBits(DRY_SIG)
#define DRY_SG_TOGGLE 	GPIO_ToggleBits(DRY_SIG);
#define DRY_SECOND_TRIG GPIO_ReadInputDataBit(DRY_SIG)

#define DRY_SIG_B			GPIOC , GPIO_Pin_10
#define DRY_SIG_B_ON 		GPIO_ResetBits(DRY_SIG_B)
#define DRY_SIG_B_OFF		GPIO_SetBits(DRY_SIG_B)
#define DRY_SIG_B_TOGGLE 	GPIO_ToggleBits(DRY_SIG_B)


#define SC_POWER			GPIO_ReadOutputDataBit(GPIOD , GPIO_Pin_6)
#define SC_POWER_OFF 		GPIO_ResetBits(GPIOD , GPIO_Pin_6)
#define SC_POWER_ON 		GPIO_SetBits(GPIOD , GPIO_Pin_6)


#define SW_LOOP			GPIOD , GPIO_Pin_8
#define SW_LOOP_STAT	GPIO_ReadOutputDataBit(SW_LOOP)

#define LTC_WDT			GPIOE , GPIO_Pin_7
#define LTC_WDT_STAT	GPIO_ReadOutputDataBit(LTC_WDT)

#define LTC_BLC_TEN			GPIOE , GPIO_Pin_15
#define LTC_BLC_TEN_STAT GPIO_ReadOutputDataBit(LTC_BLC_TEN)
#define LTC_BLC_TEN_EN		GPIO_ResetBits(LTC_BLC_TEN)
#define LTC_BLC_TEN_DIS		GPIO_SetBits(LTC_BLC_TEN)

#define ISODO_1			GPIOB , GPIO_Pin_6
#define ISODO_1_STAT	!GPIO_ReadOutputDataBit(ISODO_1)
#define ISODO_ON_1 		GPIO_ResetBits(ISODO_1)
#define ISODO_OFF_1 		GPIO_SetBits(ISODO_1)
#define ISODO_2			GPIOB , GPIO_Pin_7
#define ISODO_2_STAT	!GPIO_ReadOutputDataBit(ISODO_2)
#define ISODO_ON_2 		GPIO_ResetBits(ISODO_2)
#define ISODO_OFF_2 		GPIO_SetBits(ISODO_2)
#define ISODO_3			GPIOD , GPIO_Pin_7
#define ISODO_3_STAT	!GPIO_ReadOutputDataBit(ISODO_3)
#define ISODO_ON_3 		GPIO_ResetBits(ISODO_3)
#define ISODO_OFF_3 		GPIO_SetBits(ISODO_3)

#define DI_WAKE_BY_BUTTON			GPIO_ReadInputDataBit(GPIOA , GPIO_Pin_15)

#define ADDR_1 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_0);
#define ADDR_2 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1);
#define ADDR_3 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2);
#define ADDR_4 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3);
#define ADDR_5 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4);
#define ADDR_6 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_6);



#define AC_PRESENT GPIO_ReadInputDataBit(DI_12V_1)


void GPIO_Configuration(void);
void LED_Blink(void);
void One_LED_ON(unsigned char led_num);
static LED_Delay(uint32_t nCount);

#endif
