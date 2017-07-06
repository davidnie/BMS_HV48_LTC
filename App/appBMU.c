/***********************************************************************
***********************************************************************/
#include "bmu.h"
#include "main.h"
#include "can.h"
#include "sysTick.h"
#include "Task_sysCheck.h"
#include "Task_main.h"
#include "Task_adc.h"
#include "appBMU.h"


UNS32 abs(UNS32 a, UNS32 b);
UNS32 abso(INT32 a);
BMU_INFO bmu;

BOOLEAN isHWShutdownExist();
BOOLEAN strAlarmClr();
BOOLEAN strFaultClr();
void strTCCheck(UNS8 strid);
void TCTableInit(void);

UNS8 getTCStat(FAW_ID_T tcid);
int parseBroadMsg(Message *m, UNS8 nodeId);
UINT16 SOC_OCV_Cal(BMU_INFO *bm,UINT16 savedSOC);
UINT16 SOC_OCV_Lookup(INT8 temp,UINT16 Vcell);
void strPlimSet(BATT_STR_INFO *bm);
static void setBalance(UNS8 sid);
void bmuInit(void);
	
void bmuInit(){
	uint8_t bid;
	memset(&bmu,0,sizeof(BMU_INFO));
}
void updateBMSoc(UNS16 interval_ms){
	
	static UNS32 getTCVal(FAW_ID_T tcid);
	
	//interval: 100ms
//	UINT16 updateSOC(UINT8 interval_ms){
		UINT16 totalSOC=0;
		UINT8 packNo,mid,cid;
		UINT8 bleedingCellNum;
		UINT16 bleedingEnergyLose;	//0.001VAS
		INT16 I_last_loop_5min,I_last_loop_1min,I_last_loop_30sec,I_last_loop_2sec; //current at 5min, 1min,30sec ago
		//average current at last 5min,1min,30sec. flow out:>0, flow in:<0
	//	static TICK currCbeginTick[STR_NUM_IN_CAB];
		UINT8 interval;
		static UINT8 SOC_OCV_AjuestAllowed=1;
		interval=interval_ms/100;		//interVal_ms unit is ms, so all calculation based on 0.1S (100ms)
	
	//temperatary	updateBatteryFullCapcity();
	

			
			/*battery in discharge status. or idle status(in this situation, maybe have current between two packs*/
			if(1
				&& (abso(bmu.m_curr)>10) ////flow out and I>1A (for test mistake)
				&&(isDelayExp(SECONDS(5),sysInfo.sysStartTime))
				
			){ 
				UINT32 capaDiff=0;	// pack capacity change 
				if(bmu.m_curr_dir==1){		//discharge
					capaDiff=(UINT32)(bmu.m_curr/100)*interval;
					bmu.capa_lose += capaDiff;
					bmu.capa_lose_life+= capaDiff;
					bmu.capa_rt-=capaDiff;
					if(bmu.capa_rt >bmu.capa_full)bmu.capa_rt=0; //capacity < 0
				//	eeprom_ram_para.sstrCapacity[packNo]=bm->capacity; //save a copy in eeprom
					
					// for record Energy lost to balance and power
					//nie bleedingEnergyLose=(UINT32)bleedingCellNum*38*38*interval/10;	//0.001VAS
					
				//nie	bs->BalancePower=bleedingEnergyLose/interval;//0.001watt
				//nie	bs->EngLostToBalance+=bleedingEnergyLose/10;//0.01VAS
	
					bmu.pwr_lose += (UINT32)(bmu.m_curr/100)*bmu.V_bmu/100*interval;
					//nie bs->powerLose +=bleedingEnergyLose;	//0.001VAS
	
					bmu.pwr_lose_life+= (UINT32)(bmu.m_curr/100)*bmu.V_bmu/100*interval;
					bmu.pwr_lose_life+= (UINT32)bleedingEnergyLose; //0.001VAS
	
				}else{		//charge
						capaDiff=(UINT32)(bmu.m_curr/100)*interval;
						bmu.capa_gain+= capaDiff;
						bmu.capa_gain_life+= capaDiff;
						bmu.capa_rt+=capaDiff;
						if(bmu.capa_rt >bmu.capa_full)bmu.capa_rt=bmu.capa_full; //capacity>0
						bmu.pwr_gain += (UINT32)(abso(bmu.m_curr)/100)*bmu.V_bmu/100*interval;
						//nie bs->powerLose +=bleedingEnergyLose;	//0.001VAS
					
						bmu.pwr_gain_life+= (UINT32)(abso(bmu.m_curr)/100)*bmu.V_bmu/100*interval;
				}
				bmu.bm_soc=(UNS16)((UINT64)bmu.capa_rt*1000/bmu.capa_full);
				#if 0 //nie
				//save in eeprom
				eeprom_ram_para.capacityGainTotal[STR_NUM_IN_CAB]=bm->capacityGainTotal;
				eeprom_ram_para.capacityLoseTotal[STR_NUM_IN_CAB]=bm->capacityLoseTotal;
				//kwallt_dischg
				eeprom_ram_para.KWattHrs_Chg[STR_NUM_IN_CAB]=bm->powerGainTotal/360000000;
				//DisChgAH
				eeprom_ram_para.KWattHrs_DisChg[STR_NUM_IN_CAB]=bm->powerLoseTotal/360000000;
				//DisChgAH
				eeprom_ram_para.DisChgAH[STR_NUM_IN_CAB]=bm->capacityLoseTotal/36000;
				//chgAH 
				eeprom_ram_para.ChgAH[STR_NUM_IN_CAB]=(UINT64)bm->capacityGainTotal/36000;
				#endif
			}
	

				//SOC-OCV calibrate
				if(1
				//&&(strInfo.bmu_alive_num==strInfo.bmu_total)
		//		&&(systemInfo.CVErrCnt==0)
		//		&&(systemInfo.CTErrCnt==0)
		//		&&(systemStatus!=SYS_WAIT)
		//		&&(systemStatus != SYS_ALL_PACK_CHG)
				 &&(isDelayExp(SECONDS(4),sysInfo.sysStartTime))
				 &&(!isDelayExp(SECONDS(10),sysInfo.sysStartTime))
				 //&&(strInfo.SOC_OCV_adjusted==0)
				 &&(abso(bmu.m_curr)<10)
				 &&(bmu.cv_min >0) 
				 &&(bmu.cv_min<5000)
				 &&(bmu.cv_max<5000)
				){
						UINT16 soc;
					//nie	systemInfo.HVBBattSOCAdjCriMetOCV=1;
						soc=SOC_OCV_Cal(&bmu,bmu.bm_soc);
						bmu.bm_soc=soc;
						SOC_OCV_AjuestAllowed=0;
						bmu.SOC_OCV_adjusted=1;
						bmu.capa_rt=((UINT64)(bmu.bm_soc)*bmu.capa_full)/1000;
				}
	
	
	#if 0
		//update SYSTEMINFO
		UINT32 cg=0,cl=0,cgl=0,cll=0;
		UINT64 pg=0,pl=0,pgl=0,pll=0;
				cg+=bm->capacityGain;
				cl+=bm->capacityLose;
				cgl+=bm->capacityGainTotal;
				cll+=bm->capacityLoseTotal;
				pg+=bm->powerGain;
				pgl+=bm->powerGainTotal;
				pl+=bm->powerLose;
				pll+=bm->powerLoseTotal;
				bm->SOC=((UINT64)(bm->capacity)*1000/bm->fullCapacity);
			systemInfo.powerGain=pg;
			systemInfo.powerGainTotal=pgl;
			systemInfo.powerLose=pl;
			systemInfo.powerLoseTotal=pll;
			systemInfo.capacityGain=cg;
			systemInfo.capacityLose=cl;
			systemInfo.capacityGainTotal=cgl;
			systemInfo.capacityLoseTotal=cll;
			
			eeprom_ram_para.syscapacityGainTotal=systemInfo.capacityGainTotal;
			eeprom_ram_para.syscapacityLoseTotal=systemInfo.capacityLoseTotal;
			//kwallt_dischg
			eeprom_ram_para.sysKWattHrs_Chg=systemInfo.powerGainTotal/360000000;
			//DisChgAH
			eeprom_ram_para.sysKWattHrs_DisChg=systemInfo.powerLoseTotal/360000000;
	
			//DisChgAH
			eeprom_ram_para.sysDisChgAH=systemInfo.capacityLoseTotal/36000;
			//chgAH 
			eeprom_ram_para.sysChgAH=(UINT64)systemInfo.capacityGainTotal/36000;
	
	
			if(systemInfo.currentSensorValidity==V_VALID){
				int currentDirection;
				if(systemInfo.currentDirection) currentDirection=1;
				else currentDirection=-1;
	
				if(systemInfo.voltBatteryValidity==V_VALID) systemInfo.power=(INT64)currentDirection*systemInfo.voltBattery*systemInfo.current/100;
				else if(HVBMsg.HVBBusVoltageValidity==V_VALID) systemInfo.power=(INT64)currentDirection*systemInfo.voltBus*systemInfo.current/100;
				else systemInfo.power=0;
			}else{
				systemInfo.power=0;
			}
	
	
		 
	
	
	
		totalSOC=0;
		for(packNo=0;packNo<STR_NUM_IN_CAB;packNo++){
			if(isACPresent(0)){
				totalSOC += bm->SOC;
			}else{
				totalSOC += bm->SOC;
			}
			eeprom_ram_para.sstrSOC[packNo]=bm->SOC;
		}
		systemInfo.SOC =totalSOC/STR_NUM_IN_CAB;
		uiBattTaskStage=206;
	#endif
	//	return 1;
	
	}
	
	



UINT16 SOC_OCV_Cal(BMU_INFO *bm,UINT16 savedSOC){
	UINT16 minSOC, maxSOC, avgSOC,deltaMinSOC, deltaMaxSOC, blendedSOC,retSOC,ampSOC;
	UINT8 socadj;

	
	minSOC=SOC_OCV_Lookup(bm->ct_min/10,bm->cv_min);
	maxSOC=SOC_OCV_Lookup(bm->ct_min/10,bm->cv_max);
	avgSOC=SOC_OCV_Lookup(bm->ct_min/10,bm->cv_max);
	
	//cal SOCamp
		deltaMinSOC=avgSOC-minSOC;
		deltaMaxSOC=maxSOC-avgSOC;
		ampSOC=avgSOC;
	
	//cal SOCblended
	if(ampSOC<SOC_CAL_LOW){
		if(ampSOC>deltaMinSOC) blendedSOC=ampSOC-deltaMinSOC;
		else blendedSOC=0;
	
	}else if(ampSOC>SOC_CAL_HIGH){
		blendedSOC=ampSOC+deltaMaxSOC;
		if(blendedSOC>1000) blendedSOC=1000;
	}else{
		blendedSOC=(UINT32)(ampSOC+deltaMaxSOC)*(ampSOC-SOC_CAL_LOW)/(SOC_CAL_HIGH-SOC_CAL_LOW);
		blendedSOC+=(UINT32)(ampSOC-deltaMinSOC)*(SOC_CAL_HIGH - ampSOC)/(SOC_CAL_HIGH-SOC_CAL_LOW);
	}

	retSOC=savedSOC;
	socadj=1;
	if(blendedSOC>=(savedSOC+250)){
		retSOC=blendedSOC;
//nie		bm->AHrAdjTgt=((INT32)(systemInfo.fullCapacity/1000)*(blendedSOC - savedSOC));	//AHrAdj is 0.01A.S, fullCapacity is A.S
//nie		bm->AHrAdjLeft=0;
//		sstrinfo[0].HVBBattSOCAdj=1;
	}else if(blendedSOC>=(savedSOC+100)){
		retSOC=blendedSOC;
//nie		bm->AHrAdjTgt=((INT32)(systemInfo.fullCapacity/1000)*(blendedSOC - savedSOC));	//AHrAdj is 0.01A.S, fullCapacity is A.S
//nie		bm->AHrAdjLeft=0;
/* from 11.8.2    not adjust SOC if target SOC big than exist SOC and difference lower than 25%
		systemInfo.AHrAdjTgt=((INT32)(systemInfo.fullCapacity/1000)*(blendedSOC - savedSOC));	//AHrAdj is 0.01A.S, fullCapacity is A.S
		systemInfo.AHrAdjLeft=systemInfo.AHrAdjTgt;
		systemInfo.HVBBattSOCAdj=1;
*/
//10.51.5	}else if(blendedSOC>=(savedSOC+50)){
	}else if(blendedSOC>=(savedSOC+80)){
/* from 11.8.2    not adjust SOC if target SOC big than exist SOC and difference lower than 25%
		systemInfo.AHrAdjTgt=((INT32)(systemInfo.fullCapacity/1000)*(blendedSOC - savedSOC));	//AHrAdj is 0.01A.S, fullCapacity is A.S
		systemInfo.AHrAdjLeft=systemInfo.AHrAdjTgt;
		systemInfo.HVBBattSOCAdj=0;
*/
	}else if(savedSOC >= (blendedSOC+250)){
//nie		bm->AHrAdjTgt=((INT32)(systemInfo.fullCapacity/1000)*(savedSOC- blendedSOC));	//AHrAdj is 0.01A.S, fullCapacity is A.S
//nie		bm->AHrAdjLeft=0;
		retSOC=blendedSOC;
//nie		systemInfo.HVBBattSOCAdj=1;
//10.51.5	}else if(savedSOC>=(blendedSOC+50)){
	}else if(savedSOC>=(blendedSOC+80)){
		retSOC=blendedSOC;
	//nei	bm->AHrAdjTgt=((INT32)(systemInfo.fullCapacity/1000)*(savedSOC - blendedSOC));	//AHrAdj is 0.01A.S, fullCapacity is A.S
	//nie	bm->AHrAdjLeft=0;

/*		systemInfo.AHrAdjTgt=-((INT32)(systemInfo.fullCapacity/1000)*(savedSOC - blendedSOC));	//AHrAdj is 0.01A.S, fullCapacity is A.S
		systemInfo.AHrAdjLeft=systemInfo.AHrAdjTgt;
		systemInfo.HVBBattSOCAdj=0;
*/
	}else{
		socadj=0;
		bm->AHrAdjTgt=0;
		bm->AHrAdjLeft=0;
//		bm->HVBBattSOCAdj=0;
	}



	retSOC=minSOC;	// to be sample
	
	return retSOC;
	
}

/*
	UINT16 current : 0.1A
	UINT8 currentDirection: 0 - in, 1- out
	INT8 temp: -40 to 85
	unsigned int Vmin: mv
*/
UINT16 SOC_OCV_Lookup(INT8 temp,UINT16 Vcell)
{
	int intSOC;
	#define TEMP_N40_LOW	0
	#define TEMP_N40_N30	1
	#define TEMP_N30_N20	2
	

	UINT16 SOCTable[11]={3500,3670,3740,3810,3880,3930,4000,4050,4100,4140,4200};
 	int i,tem,j,k,index_table_i,index_table_j,index_table_k;
 	int count=0;
	float fSOC;
	UINT16 I;
	UINT8   Idir;
	UINT16 * socTbl_1, *socTbl_2, *socTbl;

	if(temp<-40) {
		SOCTable[0]=3320;SOCTable[1]=3470;SOCTable[2]=3570;SOCTable[3]=3620;SOCTable[4]=3650;
		SOCTable[5]=3690;SOCTable[6]=3730;SOCTable[7]=3790;SOCTable[8]=3860;SOCTable[9]=3940;SOCTable[10]=4050;
	}else if(temp<-30){
		SOCTable[0]=3320;SOCTable[1]=3470;SOCTable[2]=3570;SOCTable[3]=3620;SOCTable[4]=3650;
		SOCTable[5]=3690;SOCTable[6]=3730;SOCTable[7]=3790;SOCTable[8]=3860;SOCTable[9]=3940;SOCTable[10]=4050;
	}else if(temp<5){
		SOCTable[0]=3320;SOCTable[1]=3470;SOCTable[2]=3570;SOCTable[3]=3620;SOCTable[4]=3650;
		SOCTable[5]=3690;SOCTable[6]=3730;SOCTable[7]=3790;SOCTable[8]=3860;SOCTable[9]=3940;SOCTable[10]=4050;
	}else if(temp<40){
		SOCTable[0]=3320;SOCTable[1]=3470;SOCTable[2]=3570;SOCTable[3]=3620;SOCTable[4]=3650;
		SOCTable[5]=3690;SOCTable[6]=3730;SOCTable[7]=3790;SOCTable[8]=3860;SOCTable[9]=3940;SOCTable[10]=4050;
	}else if(temp<50){
		SOCTable[0]=3320;SOCTable[1]=3470;SOCTable[2]=3570;SOCTable[3]=3620;SOCTable[4]=3650;
		SOCTable[5]=3690;SOCTable[6]=3730;SOCTable[7]=3790;SOCTable[8]=3860;SOCTable[9]=3940;SOCTable[10]=4050;
	}else if(temp<60){
		SOCTable[0]=3320;SOCTable[1]=3470;SOCTable[2]=3570;SOCTable[3]=3620;SOCTable[4]=3650;
		SOCTable[5]=3690;SOCTable[6]=3730;SOCTable[7]=3790;SOCTable[8]=3860;SOCTable[9]=3940;SOCTable[10]=4050;
	}
		

  i=0;  j=10;
  if (Vcell>SOCTable[10]) {
     	intSOC=100;
  }else  if (Vcell<SOCTable[0]) {
  	intSOC=0;
  }else{
	  for (;;){
		    count++;
		    index_table_i=i;
		    index_table_j=j;
		    index_table_k=(index_table_i+index_table_j)/2;
		    k=index_table_k;
		    if (j-i<=1) {
		      	tem=i*10+(float)(Vcell-SOCTable[index_table_i])/(float)(SOCTable[index_table_j]-SOCTable[index_table_i])*10;
				intSOC=tem;
				break;
	          }else{
			    if  (Vcell>SOCTable[index_table_k])      i=k;
			    else if (Vcell<SOCTable[index_table_k])      j=k;
			    else{
					intSOC=index_table_k*10;
					break;
				}

		    }  
	    }
   }
	return intSOC*10;
		
}
//new version power limit

void strPlimSet(BATT_STR_INFO *bm){

	#define PL_DERATE_MODE 1
	#define PL_NORMAL_MODE 0
// Minimum Charge Power at low SOC
#define MPchg 15000 /* minimum charge power at low SOC (W) */
static UINT8 plimcMode=PL_NORMAL_MODE;
static UINT8 plimdMode=PL_NORMAL_MODE;
	UINT16  PowLimC, PowLimD, contPowD, contPowC ;
	UINT16 plimcT = (UINT16)FPchg ;
	UINT16 plimcV =(UINT16) FPchg ;
	UINT16 plimcSOC = (UINT16)FPchg ;
	UINT16 plimdT =(UINT16) FPdis ;
	UINT16 plimdV = (UINT16)FPdis ;
	UINT16 plimdSOC = (UINT16)FPdis ;
	UINT16 CA,CB,CV,CP,CQ;	//some constant
	//static TICK cDerateBeginTick,cNomrolBeginTick;
	//static TICK dDerateBeginTick,dNomrolBeginTick;
	UINT16 NPL4,NPL3,NPL2,NPL1,NPL;
	UINT32 avgPow30sec,avgPow1_8sec;

//UINT16 plim[101];
INT8 i,j,tempSensorFailCount,voltSensorFailCount;
//nie	if((Iavg_d_5min[0]>600) ||(Iavg_d_1min[0]>750) ||(Iavg_d_30sec[0]>900)){	// Iavg_5min>60A or Iavg_1min>75A or Iavg_30sec>90A
if(1){
		CA=48; CB=25;
	}else{
		CA=60; CB=28;
	}
/*
for(i=0;i<=100;i++){plim[i]=0;}
systemInfo.cellVltMin=3000;
systemInfo.cellVltMax=3000;

for(i=0;i<=100;i++){
	plimcT = (UINT16)FPchg ;
	plimcV =(UINT16) FPchg ;
	plimcSOC = (UINT16)FPchg ;
	plimdT =(UINT16) FPdis ;
	plimdV = (UINT16)FPdis ;
	plimdSOC = (UINT16)FPdis ;

	systemInfo.cellVltMin+=i*10;
	systemInfo.cellVltMax+=i*10;
	systemInfo.SOC=(UINT16)i*10;
	systemInfo.current=100;
	systemInfo.cellTempMax=40;
	systemInfo.voltBattery=3700;

*/

/*power limit calibrated by cell voltage */
	//discharge power limit by cell voltage
	NPL4=FPdis;
		if((bm->s_cv_min) >3360){ 
			NPL4=FPdis;
		}else if(bm->s_cv_min>3200){
//			NPL4=FPdis -(UINT32)FPdis*(100-CA)*(3360-bm->s_cv_min)/16000;
			NPL4=FPdis -(UINT32)FPdis*(3360-bm->s_cv_min)/250;	//when Vcell_min is 3.2V, power limit change to 36% of full pl
		}else if(bm->s_cv_min>3150){
			NPL4=(UINT32)FPdis*(bm->s_cv_min - 3150)/150;
		}else NPL4=0;
	NPL1=FPdis;
	NPL2=FPdis;
#if 1

/* SOC calication */
	
	//discharge power limit
		if (bm->mod_soc  <=30){
			NPL3=0;
		}else if (bm->mod_soc  <=300){
			NPL3  =(UINT32) 35000-(UINT32)(30 - bm->mod_soc/10)*(30 - bm->mod_soc/10)*464/10; 
		}else if (bm->mod_soc  <=550){
			NPL3  =(UINT32) 35000+(UINT32)(bm->mod_soc/10-30)*(bm->mod_soc/10-30)*464/10; 
		}else if (bm->mod_soc  <= 1000) {
			NPL3=FPdis;
		}
#endif	
/* temperature calibration */		
NPL=NPL1;
if(NPL>NPL2) NPL=NPL2;
if(NPL>NPL3) NPL=NPL3;
if(NPL>NPL4) NPL=NPL4;

	if(bm->s_ct_max>ThighH){
		NPL=0;
	}else if(bm->s_ct_max>ThighS){
		NPL=(UINT32)NPL*(ThighH-bm->s_ct_max)/(ThighH-ThighS);
	}else if(bm->s_ct_max<25){
		if(bm->s_ct_min<-40){
		 	NPL=0;
		}else if(bm->s_ct_min<15){
	 		NPL=(UINT32)NPL*(bm->s_ct_min+40)*(bm->s_ct_min+40)/3025;
		}
	}


	PowLimD = NPL;


//charge power limit

/*power limit calibrated by cell voltage */
	plimcV=FPchg;

			if (bm->s_cv_max>4100) plimcV = 0 ;
			else if (bm->s_cv_max > 3950) {
//				plimcV=FPchg - (UINT32)(bm->s_cv_max-V40)/4; //*250/1000
#ifdef PRO_HATCH
				plimcV=FPchg - (UINT32)(bm->s_cv_max-3950)*120; //*250/1000
#else
				plimcV=FPchg - (UINT32)(bm->s_cv_max-3950)*100; //*250/1000
#endif
			}else if(bm->s_cv_max>3250){
//				plimcV=(UINT32)sstrinfo[strid].strVolt*13;
				//plimcV=(UINT32)bm->mod_volt*4;
				if(plimcV>FPchg) plimcV=FPchg;
					
			}else if(bm->s_cv_max<=3250){
			
				plimcV =FPchg - (UINT32)FPchg*(3250-bm->s_cv_max)/250;
				//nie if(plimcV>(UINT32)(bm->mod_volt)*13) plimcV=(UINT32)bm->mod_volt*13;
			}
#if 1
/* SOC calication */
	if (bm->mod_soc >900) plimcSOC = (UINT32)FPchg*(1000-bm->mod_soc)*3/550;
	
	else if (bm->mod_soc > 600) plimcSOC = (UINT32)FPchg*(1260-bm->mod_soc)/660;
	else if (bm->mod_soc  > 200) {
		plimcSOC = FPchg ;
	}else{
		plimcSOC=(UINT32)FPchg*(900 + bm->mod_soc)/1100;
	}
#endif

    plimcT=plimcV;
   if(plimcT>plimcSOC) plimcT=plimcSOC;
    
/* temperature calication */
	if(bm->s_ct_max>ThighH) PowLimC=0;
	else if(bm->s_ct_max>ThighS){
		PowLimC=(UINT32)plimcT*(ThighH-bm->s_ct_max)/(ThighH-ThighS); 
	}else if(bm->s_ct_max>15){
		PowLimC=plimcT;
	}else if(bm->s_ct_min>(-20)){
		PowLimC=(UINT32)plimcT*(bm->s_ct_max+20)*(bm->s_ct_max+20)/35/35;
	}else{
		PowLimC=0;
	}
		
	

//get temperature sensor fail count
tempSensorFailCount=0;
for(i=0;i<4;i++){
	for(j=0;j<24;j++){
//		if(!((sstrinfo[0].cellTmpValidity[i]>>j) & 0x01)) tempSensorFailCount++;
	}
}
voltSensorFailCount++;
for(i=0;i<4;i++){
	for(j=0;j<24;j++){
//		if(!((sstrinfo[0].cellVolValidity[i]>>j) & 0x01)) voltSensorFailCount++;
	}
}

#if 0
	if(systemInfo.bmsDebugMode==1){
		if((PowLimD>PowLimD_Last[strid]) && (sstrinfo[strid].current>20) && (sstrinfo[strid].currentDirection ==1)){
			PowLimD=PowLimD_Last[strid];
		}else{
			PowLimD_Last[strid]=PowLimD;
		}
		if((PowLimC>PowLimC_Last[strid]) && (sstrinfo[strid].current<(-20))  && (sstrinfo[strid].currentDirection ==0)){
			PowLimC=PowLimC_Last[strid];
		}else{
			PowLimC_Last[strid]=PowLimC;
		}

	}
#endif
	
	bm->mod_pld=PowLimD;
	bm->mod_plc=PowLimC;



	
}
UNS32 abs(UNS32 a, UNS32 b){
	if(a>b) return (a-b);
	else return (b-a);
}
UNS32 abso(INT32 a){
	if(a>=0) return (a);
	else return (-a);
}
BOOLEAN isSysFaultExist(){
	BOOLEAN ret=0;
	if(0
		//||(bpInfo.str_on_num<NUM_BMU_TOTAL)

	){
		ret=0;
	}
	return ret;
}

BOOLEAN isStrFaultExist(UNS8 sid){
	BOOLEAN ret=0;
	if(isDelayExp(5000,sysInfo.sysStartTime)==0){
		return 0;
	}
	if(0
		//||(LID_SW_STAT==0)
		//||(MANUAL_SW_STAT==0)
		||(bmu.tc[F_STR_OV].tc_stat)
		||(bmu.tc[F_STR_UV].tc_stat)
		||(bmu.tc[F_STR_OT].tc_stat)
		||(bmu.tc[F_STR_UT].tc_stat)
		||(bmu.tc[F_STR_DOC].tc_stat)
		||(bmu.tc[F_STR_COC].tc_stat)
		){
		ret=1;
	}
		
	return ret;
}
BOOLEAN strFaultClr(UNS8 sid){
	BOOLEAN ret=0;
	bmu.tc[F_STR_OV].tc_stat=0;
	bmu.tc[F_STR_UV].tc_stat=0;
	bmu.tc[F_STR_OT].tc_stat=0;
	bmu.tc[F_STR_UT].tc_stat=0;
	bmu.tc[F_STR_DOC].tc_stat=0;
	bmu.tc[F_STR_COC].tc_stat=0;
	strAlarmClr();
	ret=1;
	return ret;
}
BOOLEAN isStrAlarmExist(UNS8 sid){
	BOOLEAN ret=0;
	if(isDelayExp(5000,sysInfo.sysStartTime)==0){
		return 0;
	}
	if(0
		//||(LID_SW_STAT==0)
		//||(MANUAL_SW_STAT==0)
		||(bmu.tc[F_STR_OV].tc_stat)
		||(bmu.tc[A_STR_OV].tc_stat)
		||(bmu.tc[F_STR_UV].tc_stat)
		||(bmu.tc[A_STR_UV].tc_stat)
		||(bmu.tc[F_STR_OT].tc_stat)
		||(bmu.tc[A_STR_OT].tc_stat)
		||(bmu.tc[F_STR_UT].tc_stat)
		||(bmu.tc[A_STR_UT].tc_stat)
		||(bmu.tc[F_STR_DOC].tc_stat)
		||(bmu.tc[A_STR_DOC].tc_stat)
		||(bmu.tc[F_STR_COC].tc_stat)
		||(bmu.tc[A_STR_COC].tc_stat)
		){
			ret=1;
		}else{
			ret=0;
		}

	return ret;
}
BOOLEAN strAlarmClr(UNS8 sid){
	BOOLEAN ret=0;
		bmu.tc[F_STR_OV].tc_stat=0;
		 bmu.tc[A_STR_OV].tc_stat=0;
		 bmu.tc[F_STR_UV].tc_stat=0;
		 bmu.tc[A_STR_UV].tc_stat=0;
		 bmu.tc[F_STR_OT].tc_stat=0;
		 bmu.tc[A_STR_OT].tc_stat=0;
		 bmu.tc[F_STR_UT].tc_stat=0;
		 bmu.tc[A_STR_UT].tc_stat=0;
		 bmu.tc[F_STR_DOC].tc_stat=0;
		 bmu.tc[A_STR_DOC].tc_stat=0;
		 bmu.tc[F_STR_COC].tc_stat=0;
		 bmu.tc[A_STR_COC].tc_stat=0;
		ret=1;
	return ret;
}

BOOLEAN isWarningExist(UNS8 sid){
	BOOLEAN ret=0;
	if(0
		||(bmu.tc[W_STR_OV].tc_stat)
		||(bmu.tc[W_STR_UV].tc_stat)
		||(bmu.tc[W_STR_OT].tc_stat)
		||(bmu.tc[W_STR_UT].tc_stat)
		||(bmu.tc[W_STR_DOC].tc_stat)
		||(bmu.tc[W_STR_COC].tc_stat))
		{
		ret=1;
	}else{
		ret=0;
	}
	return ret;
}
BOOLEAN isHWShutdownExist(){
	BOOLEAN ret=0;
	if(isDelayExp(5000,sysInfo.sysStartTime)==0){
		return 0;
	}
	if((0
		||(vcuInfo.vcuCmd==VCMD_SHUTDOWN)
		//||(LID_SW_STAT==0)
		//||(MANUAL_SW_STAT==0)
		)){
			ret=1;
		}else{
			ret=0;
	}
	return ret;
}
void strTCCheck(UNS8 sid){
	FAW_ID_T tcid;
	TCODE_T *ptc;
	TCODE_T *pStart;
	TCODE_T tc;
	UINT32 rtVal;
	static uint16_t tm_rem_s[TC_NUM]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	static uint16_t tm_rem_r[TC_NUM]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	pStart=(TCODE_T *)&bmu.tc[0];
	for(tcid=0;tcid<20;tcid++){
		rtVal=getTCVal(tcid);
		ptc=pStart+tcid;
		if (ptc->tc_stat==0){  //no fault
			if(ptc->dir>0){	/* upside threshold */
				if(rtVal > ptc->thd_s){
					tm_rem_s[tcid]++;
					if (tm_rem_s[tcid] > ptc->tm_s){
						ptc->tc_stat=1;
						tm_rem_r[tcid]=0;
					}
				}else{
					tm_rem_s[tcid]=0;
				}
			}else{
				if(rtVal < ptc->thd_s){
					tm_rem_s[tcid] ++;
					if (tm_rem_s[tcid] > ptc->tm_s){
						ptc->tc_stat=1;
						tm_rem_r[tcid]=0;
					}
				}else{
					tm_rem_s[tcid]=0;
				}
			}
		}else{	//fault exist
			if(ptc->tm_r !=0xFFFF){	//fault recoverable
			if(ptc->dir>0){	/* upside threshold reset when val<threshold */
				if(rtVal < ptc->thd_r){
					tm_rem_r[tcid] ++;
					if (tm_rem_r[tcid] > ptc->tm_r){
						ptc->tc_stat=0;
						tm_rem_s[tcid]=0;
					}
				}else{
					tm_rem_r[tcid]=0;
				}
			}else{
				if(rtVal > ptc->thd_r){	//lowside. reset when val>threshold
					tm_rem_r[tcid] ++;
					if (tm_rem_r[tcid] > ptc->tm_r){
						ptc->tc_stat=0;
						tm_rem_s[tcid]=0;
					}
				}else{
					tm_rem_r[tcid]=0;
				}
			}
			}
	}
	}
}


void TCTableInit(void){
	
	FAW_ID_T fawId;
	UNS8 sid;
	int i;
	fawId=F_STR_OV;
	bmu.tc[fawId].tc_stat=0;
	bmu.tc[fawId].dir=1;
	bmu.tc[fawId].thd_s=4150;
	bmu.tc[fawId].tm_s=300;
	bmu.tc[fawId].thd_r=3900;
	bmu.tc[fawId].tm_r=300;
	fawId=A_STR_OV;
	bmu.tc[fawId].tc_stat=0;
	bmu.tc[fawId].dir=1;
	bmu.tc[fawId].thd_s=4050;
	bmu.tc[fawId].tm_s=1000;
	bmu.tc[fawId].thd_r=3900;
	bmu.tc[fawId].tm_r=300;
	fawId=W_STR_OV;
	bmu.tc[fawId].tc_stat=0;
	bmu.tc[fawId].dir=1;
	bmu.tc[fawId].thd_s=4000;
	bmu.tc[fawId].tm_s=300;
	bmu.tc[fawId].thd_r=3900;
	bmu.tc[fawId].tm_r=100;
	fawId=F_STR_UV;
	bmu.tc[fawId].tc_stat=0;
	bmu.tc[fawId].dir=0;
	bmu.tc[fawId].thd_s=2950;
	bmu.tc[fawId].tm_s=300;
	bmu.tc[fawId].thd_r=3200;
	bmu.tc[fawId].tm_r=300;
	fawId=A_STR_UV;
	bmu.tc[fawId].tc_stat=0;
	bmu.tc[fawId].dir=0;
	bmu.tc[fawId].thd_s=3000;
	bmu.tc[fawId].tm_s=300;
	bmu.tc[fawId].thd_r=3150;
	bmu.tc[fawId].tm_r=300;
	fawId=W_STR_UV;
	bmu.tc[fawId].tc_stat=0;
	bmu.tc[fawId].dir=0;
	bmu.tc[fawId].thd_s=3050;
	bmu.tc[fawId].tm_s=300;
	bmu.tc[fawId].thd_r=3100;
	bmu.tc[fawId].tm_r=100;
	fawId=F_STR_OT;
	bmu.tc[fawId].tc_stat=0;
	bmu.tc[fawId].dir=1;
	bmu.tc[fawId].thd_s=550; // 0.1C
	bmu.tc[fawId].tm_s=100;
	bmu.tc[fawId].thd_r=400; // 0.1C
	bmu.tc[fawId].tm_r=500;
	fawId=A_STR_OT;
	bmu.tc[fawId].tc_stat=0;
	bmu.tc[fawId].dir=1;
	bmu.tc[fawId].thd_s=500; // 0.1C
	bmu.tc[fawId].tm_s=300;
	bmu.tc[fawId].thd_r=450; // 0.1C
	bmu.tc[fawId].tm_r=100;
	fawId=W_STR_OT;
	bmu.tc[fawId].tc_stat=0;
	bmu.tc[fawId].dir=1;
	bmu.tc[fawId].thd_s=480; // 0.1C
	bmu.tc[fawId].tm_s=46;
	bmu.tc[fawId].thd_r=460; // 0.1C
	bmu.tc[fawId].tm_r=100;
	fawId=F_STR_UT;
	bmu.tc[fawId].tc_stat=0;
	bmu.tc[fawId].dir=0;
	bmu.tc[fawId].thd_s=(UINT32)-350; // 0.1C
	bmu.tc[fawId].tm_s=300;
	bmu.tc[fawId].thd_r=(UINT32)-100; // 0.1C
	bmu.tc[fawId].tm_r=100;
	fawId=A_STR_UT;
	bmu.tc[fawId].tc_stat=0;
	bmu.tc[fawId].dir=0;
	bmu.tc[fawId].thd_s=(UINT32)-300; // 0.1C
	bmu.tc[fawId].tm_s=300;
	bmu.tc[fawId].thd_r=(UINT32)-100; // 0.1C
	bmu.tc[fawId].tm_s=100;
	fawId=W_STR_UT;
	bmu.tc[fawId].tc_stat=0;
	bmu.tc[fawId].dir=0;
	bmu.tc[fawId].thd_s=(UINT32)-100; // 0.1C
	bmu.tc[fawId].tm_s=300;
	bmu.tc[fawId].thd_r=(UINT32)-80; // 0.1C
	bmu.tc[fawId].tm_r=100;
	
	fawId=F_STR_DOC;
	bmu.tc[fawId].tc_stat=0;
	bmu.tc[fawId].dir=1;
	bmu.tc[fawId].thd_s=50000; //mA
	bmu.tc[fawId].tm_s=300;
	bmu.tc[fawId].thd_r=2000; //mA
	bmu.tc[fawId].tm_r=100;
	fawId=A_STR_DOC;
	bmu.tc[fawId].tc_stat=0;
	bmu.tc[fawId].dir=1;
	bmu.tc[fawId].thd_s=40000; //mA
	bmu.tc[fawId].tm_s=300;
	bmu.tc[fawId].thd_r=35000; //mA
	bmu.tc[fawId].tm_r=0;
	fawId=W_STR_DOC;
	bmu.tc[fawId].tc_stat=0;
	bmu.tc[fawId].dir=1;
	bmu.tc[fawId].thd_s=30000; //mA
	bmu.tc[fawId].tm_s=300;
	bmu.tc[fawId].thd_r=30000; //mA
	bmu.tc[fawId].tm_r=100;
	fawId=F_STR_COC;
	bmu.tc[fawId].tc_stat=0;
	bmu.tc[fawId].dir=1;
	bmu.tc[fawId].thd_s=35000; //mA
	bmu.tc[fawId].tm_s=300;	//10ms 
	bmu.tc[fawId].thd_r=30000; //mA
	bmu.tc[fawId].tm_r=100;
	fawId=A_STR_COC;
	bmu.tc[fawId].tc_stat=0;
	bmu.tc[fawId].dir=1;
	bmu.tc[fawId].thd_s=30000; //mA
	bmu.tc[fawId].tm_s=300;
	bmu.tc[fawId].thd_r=25000; //mA
	bmu.tc[fawId].tm_r=100;
	fawId=W_STR_COC;
	bmu.tc[fawId].tc_stat=0;
	bmu.tc[fawId].dir=1;
	bmu.tc[fawId].thd_s=25000; //mA
	bmu.tc[fawId].tm_s=300;
	bmu.tc[fawId].thd_r=25000; //mA
	bmu.tc[fawId].tm_r=0;


	fawId=F_HEATER_OT;
	bmu.tc[fawId].tc_stat=0;
	bmu.tc[fawId].dir=1;
	bmu.tc[fawId].thd_s=100; //C
	bmu.tc[fawId].tm_s=300;
	bmu.tc[fawId].thd_r=100; //C
	bmu.tc[fawId].tm_r=0;
		fawId=A_HEATER_OT;
		bmu.tc[fawId].tc_stat=0;
		bmu.tc[fawId].dir=1;
		bmu.tc[fawId].thd_s=100; //C
		bmu.tc[fawId].tm_s=300;
		bmu.tc[fawId].thd_r=100; //C
		bmu.tc[fawId].tm_r=0;
		fawId=F_PK_DUT;
		bmu.tc[fawId].tc_stat=0;
		bmu.tc[fawId].dir=0;
		bmu.tc[fawId].thd_s=(UINT32)-350; //0.1C
		bmu.tc[fawId].tm_s=300;
		bmu.tc[fawId].thd_r=(UINT32)-100; //0.1C
		bmu.tc[fawId].tm_r=0;
		fawId=F_PK_CUT;
		bmu.tc[fawId].tc_stat=0;
		bmu.tc[fawId].dir=0;
		bmu.tc[fawId].thd_s=(UINT32)-300; //0.1C
		bmu.tc[fawId].tm_s=300;
		bmu.tc[fawId].thd_r=(UINT32)-100; //0.1C
		bmu.tc[fawId].tm_r=0;
	
	fawId=F_PK_DOC;
	bmu.tc[fawId].tc_stat=0;
	bmu.tc[fawId].dir=1;
	bmu.tc[fawId].thd_s=500000; //mA
	bmu.tc[fawId].tm_s=300;
	bmu.tc[fawId].thd_r=200000; //mA
	bmu.tc[fawId].tm_r=0;
	fawId=F_PK_COC;
	bmu.tc[fawId].tc_stat=0;
	bmu.tc[fawId].dir=1;
	bmu.tc[fawId].thd_s=500000; //mA
	bmu.tc[fawId].tm_s=300;
	bmu.tc[fawId].thd_r=200000; //mA
	bmu.tc[fawId].tm_r=0;

	
}

UNS8 getTCStat(FAW_ID_T tcid){
	return bmu.tc[tcid].tc_stat;
}
		
static UNS32 getTCVal(FAW_ID_T tcid){
		UNS32 val;
		switch(tcid){
		case F_STR_OV:
		case A_STR_OV:
		case W_STR_OV:
			val=bmu.cv_max;
			break;
		case F_STR_UV:
		case A_STR_UV:
		case W_STR_UV:
			val=bmu.cv_min;
			break;
		case F_STR_OT:
		case A_STR_OT:
		case W_STR_OT:
			if(bmu.ct_max<=0) val=0;
			else val=(UINT32)bmu.ct_max;
			break;
		case F_STR_UT:
		case A_STR_UT:
		case W_STR_UT:
			if(bmu.ct_min>=0) val=0xFFFFFFFF;
			else val=(UINT32)bmu.ct_min;
			break;
		case F_STR_DOC:
		case A_STR_DOC:
		case W_STR_DOC:
			if(bmu.m_curr<=0) val=0;	//charging
			else val=(UINT32)(bmu.m_curr);
			break;
		case F_STR_COC:
		case A_STR_COC:
		case W_STR_COC:
			if(bmu.m_curr<=0) val=-bmu.m_curr;	//charging
			else val=(UINT32)(0);
			break;
		case F_CV_DIFF:
		case A_CV_DIFF:
		case W_CV_DIFF:
			val=(bmu.cv_max>bmu.cv_min)?(bmu.cv_max-bmu.cv_min):0;
			break;
		case F_HEATER_OT:
		case A_HEATER_OT:
		case W_HEATER_OT:
			//bpInfo.
			break;
		case F_PK_DUT:
		case F_PK_CUT:
				//if(strInfo[sid].s_ct_min>=0) val=0xFFFFFFFF;
				//else val=(UINT32)bpInfo.ct_min;
			break;
		case F_PK_DOC:
			if(bmu.m_curr<=0) val=0; //charging
			else val=(UINT32)(bmu.m_curr);
			break;
		case F_PK_COC:
			if(bmu.m_curr<=0) val=-bmu.m_curr; //charging
			else val=(UINT32)(0);
			break;
		default:
			break;
	}
	return val;
}

static void setBalance(UNS8 sid){
	uint8_t mid,cid;
	for(mid=sid*gSysCfg.ucNUM_BUM_PER_STR;mid<(sid+1)*gSysCfg.ucNUM_BUM_PER_STR;mid++){
		for(cid=0;cid<gSysCfg.ucNUM_CV_IN_BMU;cid++){
			if((bmu.cv[cid]-bmu.cv_min)>10){
				bmu.cbCtrl.wd |= (UINT32)1<<cid;
			}else{
				bmu.cbCtrl.wd &= ~((UINT32)1<<cid);
			}
		}
	}
		
}
