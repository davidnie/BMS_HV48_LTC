/***********************************************************************
***********************************************************************/
#define _TASK_MAIN_DEFINE
#include "main.h"
#include "def.h"
#include "Task_main.h"
#include "Task_sysCheck.h"
#include "Task_ADC.h"
#include "wifi.h"
//#include "eb_battery.h"

BATT_PACK_INFO bpInfo;
UNS32  main_task_timer;
UNS32 mTsk_rt;

WK_SOURCE WakeSrcCheck(void);
void wakeupBmu(void);
void setWhoAmI(void);
static void bmu_pwer_ctrl(UNS8 chg);
void Task_Main_Toro(void *pdata);
static void updateEBData(void);

/***********************************************************************
***********************************************************************/
	void Task_Main(void *pdata)
	{		 
		unsigned  char	os_err,i,isCmdIn;
		Message RxMSG;
		static UNS32 tskmain_lpcnt=0;
		static UNS8 tc_chk_id=0;
		UNS32 nowTime;
		unsigned char sdData[10]={1,2,3,4,6,7,8,9};

		TCTableInit();

		while(1){
			if((tskmain_lpcnt++)>=0xFFFFFFFE) tskmain_lpcnt=0;
			OSTimeDlyHMSM(0, 0, 0, 10);//suspend at here for 10ms

			updateBMSoc(&bmu,100);

			wdg_feed();		//feed dog

			mTsk_rt=OSTimeGet();	//get real time counter

			fillEBData(&bmu);
			
		}
	}
WK_SOURCE WakeSrcCheck(void){
	int a;
	WK_SOURCE src;
	if((!DI_WAKE_BY_BUTTON)&&(sysInfo.f_vcu_lost)){
		src=WK_CHARGE;
	}else{
		src=WK_MACHINE;
	}
	
	
	return src;
}
void wakeupBmu(void){
	if (DI_WAKE_BY_BUTTON==1){
				SC_POWER_ON;
	}
}
void setWhoAmI(void){
	UNS16 mySn,sn;
	BM_POSISTION whoAmI=MASTER;
	UNS8 mid,rank=0;
	bpInfo.str[0].Inter_Role=whoAmI;	
	
}
void masterRun(void){
}
static void bmu_pwer_ctrl(UNS8 chg){
	static UNS8 lastCmd;
	static UNS8 pulse_stat=0;
	static UNS16 lp;
	static UNS32 tskTm=0;
	UNS8 bt[8];

}

