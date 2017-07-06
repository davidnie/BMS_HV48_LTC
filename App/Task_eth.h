/***********************************************************************
***********************************************************************/
#ifndef _APP_TASK_ETH_H_
#define _APP_TASK_ETH_H_

#define TASK_MESA_STK_SIZE	400
#define TASK_MBTCP_STK_SIZE	2000
#define TASK_HTTP_STK_SIZE	1000

extern OS_STK Stk_Task_Mesa[TASK_MESA_STK_SIZE];
extern OS_STK Stk_Task_Mbtcp[TASK_MBTCP_STK_SIZE];
extern OS_STK Stk_Task_Http[TASK_HTTP_STK_SIZE];

extern void Task_mesa(void *pdata);
extern void Task_http(void *pdata);
extern void Task_mbtcp(void *pdata);

#endif
