/***********************************************************************
***********************************************************************/
#ifndef _APP_TASK_LED_H_
#define _APP_TASK_LED_H_

#define TASK_LED_STK_SIZE	400
extern OS_STK Stk_Task_LED[TASK_LED_STK_SIZE];

//extern OS_EVENT  *sem_addr_acquired;			//get vilid address
extern void Task_LED(void *pdata);
typedef enum{
	LED_RED_ON,
	LED_GREEN_ON,
	LED_BOTH_ON,
		LED_RED_FLASH_1S,
		LED_RED_FLASH_500ms,
		LED_RED_FLASH_200ms,
		LED_RED_FLASH_100ms,
		LED_GREEN_FLASH_1S,
		LED_GREEN_FLASH_500ms,
		LED_GREEN_FLASH_200ms,
		LED_GREEN_FLASH_100ms,
		LED_BOTH_FLASH_1S,
		LED_BOTH_FLASH_500ms
}LED_STAT_T;
extern LED_STAT_T led_stat;
#endif


