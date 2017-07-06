/***********************************************************************
name: main.c
function: entry point
***********************************************************************/
#include "main.h"
#include "Task_EFlash.h"
#include "Task_Main.h"
#include "Task_sysCheck.h"
#include "Task_wifi.h"
#include "mesa.h"
#include "Task_ltc.h"
#include "task_pud.h"
#include "task_led.h"
SYS_CONFIG_CONST_ST gSysCfg;

uint32_t main_loop_count=0;
int main(void)
{
	//board initlize
	BSP_Init();
	//battery System parameter initlize
	SCInit();

	//OS initlize
	OSInit();
	//create first task
	OSTaskCreate(	Task_StartUp,            		    				//pointer to routine
                  	(void *) 0,												//pointer of argument pass to the task
					(OS_STK *)&Stk_TaskStartUp[TASK_STARTUP_STK_SIZE - 1],	//top address of stack to the task
					(INT8U) OS_USER_PRIO_LOWEST);							//lowest priority 59 

	//clear timer
	OSTimeSet(0);

	//start OS 
	OSStart();

	return 0;
}
//allocte start task stack
OS_STK Stk_TaskStartUp[TASK_STARTUP_STK_SIZE];


void Task_StartUp(void *pdata)
{

	//init os timer
	/* OS_TICKS_PER_SEC  is UCOS-II tick per second */
	SysTick_Config(SystemCoreClock/OS_TICKS_PER_SEC - 1);	 
 
	OSTaskCreate(Task_FaultCheck, (void *)0, &Stk_TaskFaultCheck[TASK_FAULT_CHECK_STK_SIZE-1], OS_USER_PRIO_GET(4));
	OSTaskCreate(Task_Main, (void *)0, &Stk_Task_Main[TASK_MAIN_STK_SIZE-1], OS_USER_PRIO_GET(5));
	OSTaskCreate(Task_CAN1, (void *)0, &Stk_Task_CAN1[TASK_TEST_CAN1_STK_SIZE-1], OS_USER_PRIO_GET(7));
	OSTaskCreate(Task_LTC, (void *)0, &Stk_Task_LTC[TASK_LTC_STK_SIZE-1], OS_USER_PRIO_GET(19));
	//OSTaskCreate(Task_ADC, (void *)0, &Stk_Task_a[TASK_CAN2_REV_STK_SIZE-1], OS_USER_PRIO_GET(20));
 	OSTaskCreate(Task_CANSend, (void *)0, &Stk_Task_CAN_Send[TASK_CAN2_REV_STK_SIZE-1], OS_USER_PRIO_GET(23));
	OSTaskCreate(Task_PUD, (void *)0, &Stk_Task_PUD[TASK_PUD_STK_SIZE-1], OS_USER_PRIO_GET(24));
 	OSTaskCreate(Task_LED, (void *)0, &Stk_Task_LED[TASK_LED_STK_SIZE-1], OS_USER_PRIO_GET(50));
	//OSTaskCreate(Task_EFlash, (void *)0, &Stk_Task_EFlash[400-1], OS_USER_PRIO_GET(32));
	while (1)
	{
		//guard routine
		//updateBP_BM();
		/* flash a LED every 1 second to show system is runing */
		main_loop_count++;
		OSTimeDly(1000);//1000ms
	}
}
