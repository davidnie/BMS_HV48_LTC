#include "main.h"
#include "Task_LED.h"
#include "Task_Main.h"
OS_STK Stk_Task_LED[TASK_LED_STK_SIZE];
static LED_STAT_T led_stat=LED_RED_FLASH_1S;
static unsigned int lpcnt;

//OS_EVENT  *sem_addr_acquired;			//get vilid address
void Task_LED(void *pdata);

/***********************************************************************
note: this task control LED status
***********************************************************************/
void Task_LED(void *pdata)
{	
	while(1){
		OSTimeDly(100);//100ms
		if(lpcnt++>65500) lpcnt=0;;
		switch(led_stat){
			case LED_RED_ON:
				LEDR_ON;
				break;
			case LED_GREEN_ON:
				LEDG_ON;
				break;
			case LED_RED_FLASH_100ms:
				LEDR_Toggle;
				break;
			case LED_RED_FLASH_1S:
				if(lpcnt%10==0){
					LEDR_Toggle;
				}
				break;
			default:
				led_stat=0;
		}
		
	}
}

void LEDStat_Set(LED_STAT_T stat){
	led_stat=stat;
}

