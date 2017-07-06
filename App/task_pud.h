/***********************************************************************
***********************************************************************/
#ifndef _APP_TASK_PUD_H_
#define _APP_TASK_PUD_H_

#define TASK_PUD_STK_SIZE	400
extern OS_STK Stk_Task_PUD[TASK_PUD_STK_SIZE];

extern OS_EVENT  *sem_addr_acquired;			//get vilid address
extern void Task_PUD(void *pdata);

#endif

