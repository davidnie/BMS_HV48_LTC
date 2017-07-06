/***********************************************************************
***********************************************************************/
#include "main.h"
#include "def.h"
#include "bsp_can.h"
#include "Task_ADC.h"
#include "task_main.h"
#include  "cof_data.h"
#include "GlobalVar.h"

extern	AD_RESULT ad_res;
extern	CO_Data* CANOpenMasterObject;
extern 	OS_EVENT *sem_vcu_can_rev;
OS_STK Stk_Task_CAN1[TASK_TEST_CAN1_STK_SIZE];
	OS_STK Stk_Task_CO_EB[TASK_CO_EB_STK_SIZE];
#ifdef CO_FESTVAL_MASTER
	OS_STK Stk_Task_CO_FESTVAL[TASK_CO_FESTVAL_STK_SIZE];
#endif	
OS_EVENT  *sem_CAN1_rec_flag;			//CAN1接收完一桢数据信号量定义
OS_EVENT  *sem_addr_claim_rev_flag;			//CAN1接收完一桢数据信号量定义
/***********************************************************************
***********************************************************************/
	void Task_CAN1(void *pdata)
	{		 
		unsigned  char	os_err,i,err;
		unsigned char node_max;
		Message RxMSG;
		sem_CAN1_rec_flag = OSSemCreate(1); //create a semphere. 
		sem_addr_claim_rev_flag = OSSemCreate(1); //create a semphere. 
sem_addr_claim_rev_flag->OSEventCnt=0;
		while(1)
		{  
			OSSemPend(sem_CAN1_rec_flag,0,&os_err); 		//wait sem_CAN1_rec_flag being set in ISR

		if(can1_rec_flag == 1)							//check if received valid message
		{
			if(timeDiff(sysInfo.sysStartTime,OSTime)>50){
		
				if((CAN1RxMsg.IDE==0)&&(CAN1RxMsg.StdId>=0x580)){
					RxMSG.cob_id=CAN1RxMsg.StdId;
					RxMSG.len=CAN1RxMsg.DLC;
					RxMSG.rtr=CAN1RxMsg.RTR;
					for (i=0;i<8;i++){
						RxMSG.data[i]=CAN1RxMsg.Data[i];
					}
					
				}
					if(CAN1RxMsg.StdId==0x305){
						OSSemPost(sem_vcu_can_rev);
					}else if(CAN1RxMsg.StdId>0x700){
						 if(CAN1RxMsg.StdId==0x7FE){
							  node_max=CAN1RxMsg.Data[0];
								sysInfo.f_vcu_lost=0;
						 }else{
							  node_max=CAN1RxMsg.StdId & 0xFF;
						 }
						 if(node_max>sysInfo.co_node_total){
							 sysInfo.co_node_total=node_max+1;
						 }
						OSSemPost(sem_addr_claim_rev_flag);
						OSSemSet(sem_addr_claim_rev_flag,1000,&err);
					}else if((CAN1RxMsg.IDE==CAN_Id_Standard)&&(CAN1RxMsg.StdId==0x132)){
						if((CAN1RxMsg.Data[2] & 0x01)>0) {LEDR_ON;} else {LEDR_OFF;}
						if((CAN1RxMsg.Data[2] & 0x02)>0) {LEDG_ON;} else {LEDG_OFF;}
						if((CAN1RxMsg.Data[2] & 0x04)>0) {WAKEUP_TRANS_ON;} else {WAKEUP_TRANS_OFF;}
						if((CAN1RxMsg.Data[2] & 0x08)>0) {DRY_SIG_ON;} else {DRY_SIG_OFF;}
						if((CAN1RxMsg.Data[2] & 0x10)>0) {DRY_SIG_B_ON;} else {DRY_SIG_B_OFF;}
						if((CAN1RxMsg.Data[2] & 0x20)>0) {LTC_BLC_TEN_EN;} else {LTC_BLC_TEN_DIS;}
						if((CAN1RxMsg.Data[2] & 0x40)>0) {SC_POWER_ON;} else {SC_POWER_OFF;}
						if((CAN1RxMsg.Data[2] & 0x80)>0) {;} else {;}
						if((CAN1RxMsg.Data[3] & 0x01)>0) {ISODO_ON_1;} else {ISODO_OFF_1;}
						if((CAN1RxMsg.Data[3] & 0x02)>0) {ISODO_ON_2;} else {ISODO_OFF_2;}
						if((CAN1RxMsg.Data[3] & 0x04)>0) {ISODO_ON_3;} else {ISODO_OFF_3;}
					}

			}
		}
		}
	}
void Task_CO_EB_Slave(void *pdata){
	unsigned  char	os_err;
	Message RxMSG;
	static uint32_t t_co;
	static uint8_t CO_tid=0;
	static uint32_t t_stat;
	uint32_t tnow;
	static uint32_t loop_cnt=0;
	uint8_t sndData[8];
	uint8_t *pData,*pt;
	uint8_t i,j,k;
	static uint8_t pollBmuNum=1;
	static uint8_t svrNodeId=0;
	UNS16 sdo_id;
	UNS8 bt[8];
	UNS16 wd[4];
//	sem_CAN1_rec_flag = OSSemCreate(1); //create a semphere. 

	/* init framework and canopen */
	ebFrameworkInit(250, 8200, NULL); /* bitrate, timer *///TMB orig
	/* init application */	
	batApplInit();
	while(1)
	{  
		OSTimeDlyHMSM(0, 0, 0, 10);// 10ms
		coCommTask();
		codrvTimerISR();
	}
}
void parseCOLong(CO_Data * d,uint8_t nodeid){
	uint32_t idx;
	uint8_t sidx;
	idx=d->transfers[0].index;
	sidx=d->transfers[0].subIndex;
}

