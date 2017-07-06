/***********************************************************************
***********************************************************************/
#include "bmu.h"
#include "main.h"
#include "can.h"
#include "appBMU.h"
#include "Task_main.h"
#include "sysTick.h"

/***********************************************************************
***********************************************************************/
	BOOLEAN isAlarmExist();
BOOLEAN isCriticalFaultExist();

void stask_bmu(void)
{		 
		static UNS32 tm_state=0;
		unsigned  char	os_err,i,isCmdIn;
		Message RxMSG;
		UNS32 mTsk_rt;
		
		//UNS32 static enterTime;
		UNS32 nowTime;
		unsigned char sdData[10]={1,2,3,4,6,7,8,9};
		SC_POWER_ON;
		PWR_12V_ON;		TCTableInit();
		while(1){
		OSTimeDlyHMSM(0, 0, 0, 10);//suspend at here for 10ms
		wdg_feed();		//feed dog

		
		mTsk_rt=OSTimeGet();	//get real time counter

	
		if(isHWShutdownExist()){
			gBOpMd=BP_FINAL;
		}else if((isCriticalFaultExist()||isAlarmExist())&&(gBOpMd!=BP_ERROR)&&(gBOpMd!=BP_FINAL)){
			tm_state=mTsk_rt;
			gBOpMd=BP_ERROR;
		}
		switch(gBOpMd){
			case BP_INIT:   //wakeup bmu and make sure 2 BMU alive
				//initilize battery BSCInit();
				bpInfo.bmuWakeupCmd=0;
				bpInfo.cmd_strCtrl=CMD_BMU_FET_OFF;
				//bmu_pwer_ctrl(1);	//enable 12V output for wake up BMU
				if(timeDiff(tm_state,mTsk_rt)>100000){ //
					gBOpMd=BP_FAULT;
				}else if(timeDiff(tm_state,mTsk_rt)>15000){
					tm_state=mTsk_rt;
					gBOpMd=BP_SYNC_OFF;
				}else if(timeDiff(tm_state,mTsk_rt)>5000){
					//if(bpInfo.str_alive_num==bpInfo.num_str){
					if(bpInfo.str_alive_num==2){
						tm_state=mTsk_rt;
						gBOpMd=BP_DO_NOT_ATTACH;
					}
					BMU_WAKE_LOW;
				}else if(timeDiff(tm_state,mTsk_rt)>4000){
					if(bpInfo.str_alive_num==0){
						BMU_WAKE_HIGH;
					}
				}else if(timeDiff(tm_state,mTsk_rt)>3000){
					BMU_WAKE_LOW;
				}
				break;
			case BP_SYNC_OFF:
				bpInfo.cmd_strCtrl=CMD_BMU_SHUTDOWN;
				if(timeDiff(tm_state,mTsk_rt)>2000){
					if(bpInfo.str_alive_num==0){
						gBOpMd=BP_INIT;			
						tm_state=mTsk_rt;
						
						BMU_WAKE_LOW;
					}
				}
				break;
			case BP_DO_NOT_ATTACH:  //no BMU has fault and FET is open
				bpInfo.bmuWakeupCmd=1;
				bpInfo.cmd_strCtrl=CMD_BMU_FET_OFF;
				BMU_WAKE_LOW;
				//nie if((bmInfo.bmu_ready_num>0)&&(bmInfo.bmu_alive_num==2)){
				if((bpInfo.str_ready_num>0)&&(bpInfo.str_alive_num>0)){
					gBOpMd=BP_READY_TO_ATTACH;
				}else if((timeDiff(tm_state,mTsk_rt)>10000)&&(bpInfo.str_alive_num<2)){
				
					bpInfo.cmd_strCtrl=CMD_BMU_SHUTDOWN;		
					BMU_WAKE_LOW;
					gBOpMd=BP_INIT;
					tm_state=mTsk_rt;
				}
				break;
			case BP_READY_TO_ATTACH: 	//at least one BMU ready to work. wait for controller command
				LS3_OFF;
				//nie if(vcuInfo.vcuCmd==VCMD_CLOSE){
					bpInfo.masterCmd=MCMD_CLOSE;
					gBOpMd=BP_NOMOR_OPERATION;
					if(bpInfo.str_on_num>0){
						gBOpMd=BP_NOMOR_OPERATION;
					}
				//}
				break;
			case BP_NOMOR_OPERATION:
				bpInfo.masterCmd=MCMD_CLOSE;
				bpInfo.cmd_strCtrl=CMD_BMU_FET_ON;
				if(vcuInfo.vcuCmd==VCMD_OPEN){
					gBOpMd=BP_DO_NOT_ATTACH;
				}else if(isAlarmExist()){
					gBOpMd=BP_REQUEST_DETACH;
				}
				break;
			case BP_REQUEST_DETACH:
				if(vcuInfo.vcuCmd==VCMD_OPEN){
					gBOpMd=BP_DO_NOT_ATTACH;
				}else if(isAlarmExist()==0){
					gBOpMd=BP_NOMOR_OPERATION;
				}
				break;
			case BP_ERROR:
				bpInfo.cmd_strCtrl=VCMD_OPEN;
				if((isCriticalFaultExist()==0)&&(isAlarmExist()==0)){
					gBOpMd=BP_DO_NOT_ATTACH;
				}
				if(timeDiff(tm_state,mTsk_rt)>600000){
					gBOpMd=BP_FINAL;
				}
				break;
			case BP_FINAL:  //turn off MBU power, and SC power
				bpInfo.cmd_strCtrl=CMD_BMU_SHUTDOWN;
				if(bpInfo.str_alive_num==0){
					SC_POWER_OFF;
					
				}
				
				break;
			case BP_WARMUP:		//in low temperature. turn on heater by extenal power
				break;
				
			default:
				break;
		}
		if(sysInfo.f_vcu_lost){
			gBOpMd=BP_FINAL;
		}
	}		
}
static void bmu_pwer_ctrl(UNS8 chg){
	static UNS8 lastCmd;
	static UNS8 pulse_stat=0;
	static UNS16 lp;
	static UNS32 tskTm=0;
	UNS8 bt[8];
	UNS32 mTsk_rt;
	mTsk_rt=OSTime;
	switch(pulse_stat){
		case 0:
			if(timeDiff(tskTm,mTsk_rt)>1000){
				pulse_stat=	1;
				tskTm=mTsk_rt;
			}
			BMU_WAKE_LOW;
			break;
		case 1:
			if(((bpInfo.bmuWakeupCmd && (bpInfo.str_alive_num<1))
				||((bpInfo.bmuWakeupCmd==0) && (bpInfo.str_alive_num==2)))
				&&(timeDiff(tskTm,mTsk_rt)>1000))
			{
 					pulse_stat=2;
					tskTm=mTsk_rt;
					lp=0;
				
			}
			break;
		case 2:  //send a pulse
			if(timeDiff(tskTm,mTsk_rt)>2000){
				pulse_stat=3;
				tskTm=mTsk_rt;
			}
			BMU_WAKE_HIGH;
			break;
		case 3:
			if(timeDiff(tskTm,mTsk_rt)>3000){
				pulse_stat=0;
				tskTm=mTsk_rt;
			}
			BMU_WAKE_LOW;
			break;
		case 4:
			break;
	}
}

BOOLEAN isAlarmExist(){
	BOOLEAN ret=0;
	if(isDelayExp(5000,sysInfo.sysStartTime)==0){
		return 0;
	}
	if(0
		){
			ret=1;
		}else{
			ret=0;
		}

	return ret;
}
BOOLEAN isCriticalFaultExist(){
	BOOLEAN ret=0;
	if(isDelayExp(5000,sysInfo.sysStartTime)==0){
		return 0;
	}
	if(0
		){
		ret=1;
	}
		
	return ret;
}

