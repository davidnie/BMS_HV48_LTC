#include "main.h"
#include "Task_PUD.h"
#include "Task_Main.h"
#include "Task_CAN1.h"
OS_STK Stk_Task_PUD[TASK_PUD_STK_SIZE];

OS_EVENT  *sem_addr_acquired;			//get vilid address
void Task_PUD(void *pdata);

/***********************************************************************
note: this task detect bms power up and down codition and relevent operation
***********************************************************************/
void Task_PUD(void *pdata)
{	
	static UNS8 tid_pud=0;
	UNS16 sem_ccu_num=0;
	INT8 err;
	while(1){
		OSTimeDly(20);//1000ms
		switch(tid_pud){
			case 0:
				/* wait 100ms, make sure got all node in the network */
				OSTimeDly(100);
				sem_ccu_num=OSSemAccept(sem_addr_claim_rev_flag);
				if(sem_ccu_num>0){
					bmu.co_addr=sysInfo.co_node_total+1;
					//start CO task. the bmu.co_addr will become CANOPEN Node id in serviceInitVals.nodeId[0]=bmu.co_addr;
					OSTaskCreate(Task_CO_EB_Slave, (void *)0, &Stk_Task_CO_EB[TASK_CO_EB_STK_SIZE-1], OS_USER_PRIO_GET(8));
					tid_pud++;
				}else{
					bmu.co_addr=1;
					OSTaskCreate(Task_CO_EB_Slave, (void *)0, &Stk_Task_CO_EB[TASK_CO_EB_STK_SIZE-1], OS_USER_PRIO_GET(8));
					tid_pud++;
				}	
				/* reset the sempaphere */
				//OSSemSet(sem_addr_claim_rev_flag,1000,&err);
				break;
			case 1:	//transfer wakeup signal to next bmu
				WAKEUP_TRANS_ON;
				tid_pud++;
				break;
			case 2: //normal operation
				sem_ccu_num=OSSemAccept(sem_addr_claim_rev_flag);
				if(sem_ccu_num==0){
					tid_pud++;
				}
			
				break;
			case 3:
				NVM_SysinfoWrite();
				NVM_sysCfgWrite();
				tid_pud++;
				break;
			case 4:
				SC_POWER_OFF;
				break;
			default:
				tid_pud=0;
			break;
		}
		
	}
}


