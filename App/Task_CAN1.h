/***********************************************************************
***********************************************************************/
#ifndef _APP_TASK_CAN1_H_
#define _APP_TASK_CAN1_H_


#define TASK_CO_EB_STK_SIZE	2000
void Task_CO_EB_Slave(void *pdata);
extern OS_STK Stk_Task_CO_EB[TASK_CO_EB_STK_SIZE];
extern OS_EVENT  *sem_addr_claim_rev_flag;			//CAN1 get address information from upper controller


#define TASK_TEST_CAN1_STK_SIZE	2000
extern OS_STK Stk_Task_CAN1[TASK_TEST_CAN1_STK_SIZE];
void Task_CAN1(void *pdata);

extern OS_EVENT  *sem_CAN1_rec_flag;			//CAN1接收完一桢数据信号量定义

#endif
