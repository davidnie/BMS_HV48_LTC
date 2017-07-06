/***********************************************************************
***********************************************************************/
#include "main.h"  
#include "task_LTC.h"
#include "task_main.h"
#include "config.h"
OS_STK Stk_Task_LTC[TASK_LTC_STK_SIZE];
LINEAR_CFGREG ltcCfg;
BUFFER spi2RevBuf,spi1RevBuf;
MODULE_INFORMATION moduleInfo;
PEC_STRUCT pec;

INT16 pec15Table[256];
INT16 crc15_poly=0x4599;
extern void fillEBBattVoltage(UINT16 *pVol);
extern UNS32 temp_table_JT103[18];

void	parseLiearRes_1(UINT8);
void	parseLtcAux(UINT8 );
void SPI1WriteByte(UINT8 da);
void resetBuffer(BUFFER * buf);
static UINT8 calaPEC(UINT8 initByte, UINT8 *datain, UINT8 size);
void  initPECTbl(void);
INT16 getPEC16(UNS8 *data, int len);
UINT16 pec16;
static UINT32 statTimer;
UINT8 uiSrc[40];

void Task_LTC(void *pdata ){
static void	spiBtDly(UNS8 dlyTick);
static void ltc_cmd(UNS16 cmd);

unsigned int i = 0;
static BATTERY_TASK_ID batteryTaskId=PREPARE_QUERY;
UINT8 sendByte,OVCellNumber,packNo;
UINT16 iTemp;
static UINT64 isFirstTimeLoseComm=0;
static unsigned char u1TxCheckSum=0;
UINT16 tmp;
UINT16 bleedingFlag,Vcell_min;	//save cell bleeding information
static UINT8 CVGrp_query=0;
static UINT8 CTGrp_query=0;
	static UINT8 queryTempId=0xFF;
	static UINT8 queryLTCId=0;
	UINT8 PEC_ADD[10]={0x49,0x4E,0x47,0x40,0x55,0x52,0x5B,0x5C,0x71,0x76};
	UINT8 uiData[40];
	

	//RS232_DMA_Init();


while(1){
	OSTimeDlyHMSM(0, 0, 0, 2);// 2ms
	initPECTbl();
	//batteryTaskId=VOL_ADC_PREPARE;
switch(batteryTaskId){
	case PREPARE_QUERY:
		SPI_LTC_CS_HIGH(); 			 
		batteryTaskId=CFG_WRITE;
	SPI_LTC_CS_LOW(); 			 
		break;
	case CFG_WRITE:
		resetBuffer(&spi1RevBuf);
		SPI_LTC_CS_LOW(); 			 
		spiBtDly(1);
		ltcCfg.cfg0.CFGR0=0xFC;	//12cell mode, 13ms
		ltcCfg.cfg1.CFGR1=0;
		ltcCfg.cfg2.CFGR2 =0;
		ltcCfg.cfg3.CFGR3 =0;
		ltcCfg.CFGR4=0;
		ltcCfg.CFGR5=0;
		uiSrc[0]=0x00;
		uiSrc[1]=WRCFGA;
		uiSrc[2]=(getPEC16(uiSrc,2)>>8)& 0xFF;
		uiSrc[3]=getPEC16(uiSrc,2)& 0xFF;
		uiSrc[4]=ltcCfg.cfg0.CFGR0;
		uiSrc[5]=ltcCfg.cfg1.CFGR1;
		uiSrc[6]=ltcCfg.cfg2.CFGR2;
		uiSrc[7]=ltcCfg.cfg3.CFGR3;
		uiSrc[8]=ltcCfg.CFGR4;
		uiSrc[8]=0x0;
		uiSrc[9]=ltcCfg.CFGR5;
		uiSrc[9]=0x0;
		pec16=getPEC16(&uiSrc[4], 6);
		uiSrc[10]=(pec16>>8) & 0xFF;
		uiSrc[11]=pec16 & 0xFF;
		for(i=0;i<12;i++){
			spiBtDly(1);
			SPI1WriteByte(uiSrc[i]);
		spiBtDly(1);
		}
		SPI_LTC_CS_HIGH();
		batteryTaskId=CFG_WRITE_B;
		break;
	case CFG_WRITE_B:
		resetBuffer(&spi1RevBuf);
		SPI_LTC_CS_LOW(); 			 
		spiBtDly(1);
		ltcCfg.CFGBR0=0x0;	//12cell mode, 13ms
		ltcCfg.CFGBR1=0;
		ltcCfg.CFGBR2 =0;
		ltcCfg.CFGBR3 =0;
		ltcCfg.CFGBR4=0;
		ltcCfg.CFGBR5=0;
		uiSrc[0]=0x00;
		uiSrc[1]=WRCFGB;
		uiSrc[2]=(getPEC16(uiSrc,2)>>8)& 0xFF;
		uiSrc[3]=getPEC16(uiSrc,2)& 0xFF;
		uiSrc[4]=ltcCfg.CFGBR0;
		uiSrc[5]=ltcCfg.CFGBR1;
		uiSrc[6]=ltcCfg.CFGBR2;
		uiSrc[7]=ltcCfg.CFGBR3;
		uiSrc[8]=ltcCfg.CFGBR4;
		uiSrc[9]=ltcCfg.CFGBR5;
		pec16=getPEC16(&uiSrc[4], 6);
		uiSrc[10]=(pec16>>8) & 0xFF;
		uiSrc[11]=pec16 & 0xFF;
		for(i=0;i<12;i++){
			spiBtDly(1);
			SPI1WriteByte(uiSrc[i]);
		spiBtDly(1);
		}
		SPI_LTC_CS_HIGH();
		batteryTaskId=PWM_WRITE;
		break;
	case PWM_WRITE:
		SPI_LTC_CS_LOW(); 			 
		spiBtDly(1);
			resetBuffer(&spi1RevBuf);
			ltcCfg.CFGBR0=0xF;	//12cell mode, 13ms
			ltcCfg.CFGBR1=0;
			ltcCfg.CFGBR2 =0;
			ltcCfg.CFGBR3 =0;
			ltcCfg.CFGBR4=0;
			ltcCfg.CFGBR5=0;
			uiSrc[0]=0x00;
			uiSrc[1]=WRPWM;
			uiSrc[2]=(getPEC16(uiSrc,2)>>8)& 0xFF;
			uiSrc[3]=getPEC16(uiSrc,2)& 0xFF;
			uiSrc[4]=0xFF;
			uiSrc[5]=0x00;
			uiSrc[6]=0x0;
			uiSrc[7]=0x0;
			uiSrc[8]=0;
			uiSrc[9]=0;
			pec16=getPEC16(&uiSrc[4], 6);
			uiSrc[10]=(pec16>>8) & 0xFF;
			uiSrc[11]=pec16 & 0xFF;
			for(i=0;i<12;i++){
				spiBtDly(1);
				SPI1WriteByte(uiSrc[i]);
		spiBtDly(1);

			}
			SPI_LTC_CS_HIGH();
			batteryTaskId=CFG_READ;
			break;
		
	case CFG_READ:
			resetBuffer(&spi1RevBuf);
			SPI_LTC_CS_LOW(); 			 
				uiSrc[0]=0x00;
				uiSrc[1]=RDCFGA;
				uiSrc[2]=(getPEC16(uiSrc,2)>>8)& 0xFF;
				uiSrc[3]=getPEC16(uiSrc,2)& 0xFF;
				uiSrc[4]=0x0;
				uiSrc[5]=0;
				uiSrc[6]=0;
				uiSrc[7]=0;
				uiSrc[8]=0;
				uiSrc[9]=0;
				pec16=getPEC16(&uiSrc[4], 6);
				uiSrc[10]=(pec16>>8) & 0xFF;
				uiSrc[11]=pec16 & 0xFF;
				for(i=0;i<12;i++){
					SPI1WriteByte(uiSrc[i]);
					spiBtDly(1);
				}
				SPI_LTC_CS_HIGH();
				spiBtDly(1);
			pec16=getPEC16(&spi1RevBuf.buf[4],6);
			if((spi1RevBuf.buf[10]==((pec16>>8) & 0xFF))
				&&(spi1RevBuf.buf[11]=pec16 & 0xFF)){
					uiSrc[0]=0x00;
			}


				
		resetBuffer(&spi1RevBuf);
	
		SPI_LTC_CS_LOW(); 			 
		for(iTemp=0;iTemp<SPI_DATA_SET_INTERVAL;iTemp++){}
		resetBuffer(&spi1RevBuf);
		uiSrc[0]=0x00;
		uiSrc[1]=RDCFGB;
		uiSrc[2]=(getPEC16(uiSrc,2)>>8)& 0xFF;
		uiSrc[3]=getPEC16(uiSrc,2)& 0xFF;
		uiSrc[4]=0;	uiSrc[5]=0;	uiSrc[6]=0;	uiSrc[7]=0;	uiSrc[8]=0;	uiSrc[9]=0;	uiSrc[10]= 0xFF;uiSrc[11]= 0xFF;
		for(i=0;i<12;i++){
			SPI1WriteByte(uiSrc[i]);
			spiBtDly(1);
		}
		SPI_LTC_CS_HIGH();
		pec16=getPEC16(&spi1RevBuf.buf[4],6);
		if((spi1RevBuf.buf[10]==((pec16>>8) & 0xFF))
			&&(spi1RevBuf.buf[11]=pec16 & 0xFF)){
				uiSrc[0]=0x00;
		}
		batteryTaskId=VOL_ADC_PREPARE;
		break;
	case VOL_ADC_PREPARE:	
		ltc_cmd(CMD_MUTE);
		OSTimeDly(1);
			SPI_LTC_CS_LOW();
			for(tmp=0;tmp<2000;tmp++){}
			uiSrc[0]=(ADCV >>8) & 0xFF;
			uiSrc[1]=ADCV  & 0xFF;;
			pec16=getPEC16(uiSrc,2);
			uiSrc[2]=(pec16>>8) & 0xFF;
			uiSrc[3]=pec16 & 0xFF;
			for(i=0;i<4;i++){
				SPI1WriteByte(uiSrc[i]);
				spiBtDly(1);
			}
			//spiBtDly(1);
			// SPI2_DMA_Send(uiSrc,4);
			//	spiBtDly(1);

			
			SPI_LTC_CS_HIGH();
			OSTimeDly(1);
			batteryTaskId=VOL_ADC_WAIT;
			//batteryTaskId=VOL_ADC_PREPARE;
		break;
	case VOL_ADC_WAIT:
		if(timeDiff(OSTime,statTimer)>200){
			SPI_LTC_CS_LOW();
			uiSrc[0]=7;
			uiSrc[1]=0x14;
			pec16=getPEC16(uiSrc,2);
			uiSrc[2]=(pec16>>8) & 0xFF;
			uiSrc[3]=pec16 & 0xFF;
			for(i=0;i<14;i++){
				spiBtDly(1);
				SPI1WriteByte(uiSrc[i]);
				spiBtDly(1);
			}
			batteryTaskId=QUERY_BATTERY;
			statTimer=OSTime;
			SPI_LTC_CS_HIGH();
		}
		ltc_cmd(CMD_UNMUTE);
		break;
		
	case QUERY_BATTERY:
			for(tmp=0;tmp<5000;tmp++){}
			SPI_LTC_CS_LOW();
			resetBuffer(&spi1RevBuf);
			for(tmp=0;tmp<200;tmp++){}
			uiSrc[0]=0x0;
			if(CVGrp_query<4){
				uiSrc[1]=4+CVGrp_query*2;
			}else{
				uiSrc[1]=1+CVGrp_query*2;
			}
			uiSrc[2]=7;uiSrc[3]=0xC2;uiSrc[4]=0x0;uiSrc[5]=0x0;uiSrc[6]=0x0;uiSrc[7]=0x0;uiSrc[8]=0x0;uiSrc[9]=0x0;uiSrc[22]=0x0;
			pec16=getPEC16(uiSrc,2);
			uiSrc[2]=(pec16>>8) & 0xFF;
			uiSrc[3]=pec16 & 0xFF;
			for(i=0;i<13;i++){
				SPI1WriteByte(uiSrc[i]);
				spiBtDly(1);
			}
			//batteryTaskId=WAIT_BATTERY_RESPONSE;
//			SPI_LTC_CS_HIGH();
//			batteryTaskId=CHECK_BATTERY_RESPONSE;
//			break;
//	case	CHECK_BATTERY_RESPONSE:
			pec16=getPEC16(&spi1RevBuf.buf[4],6);
			if((spi1RevBuf.buf[10]==((pec16>>8) & 0xFF))
				&&(spi1RevBuf.buf[11]=pec16 & 0xFF)){
					parseLiearRes_1(CVGrp_query);
			}
			if(CVGrp_query++>=5){
				CVGrp_query=0;
				//batteryTaskId=CFG_WRITE;
				batteryTaskId=TEMP_ADC_PREPARE;
			}else{
				batteryTaskId=QUERY_BATTERY;
			}
			SPI_LTC_CS_HIGH();
		break;

	case TEMP_ADC_PREPARE:
			SPI_LTC_CS_LOW();
		for(tmp=0;tmp<200;tmp++){}
		uiSrc[0]=(ADAX >>8) & 0xFF;
		uiSrc[1]=ADAX  & 0xFF;;
		pec16=getPEC16(uiSrc,2);
		uiSrc[2]=(pec16>>8) & 0xFF;
		uiSrc[3]=pec16 & 0xFF;
		for(i=0;i<4;i++){
			SPI1WriteByte(uiSrc[i]);
			spiBtDly(1);
		}
		SPI_LTC_CS_HIGH();
		statTimer=OSTime;
		batteryTaskId=TEMP_POLL;
	case TEMP_ADC_WAIT:
		break;
		
	case TEMP_POLL:
			for(tmp=0;tmp<5000;tmp++){}
			SPI_LTC_CS_LOW();
			resetBuffer(&spi1RevBuf);
			for(tmp=0;tmp<200;tmp++){}
			uiSrc[0]=0x0;
			if(CTGrp_query==0) uiSrc[1]=0x0C;
			else if(CTGrp_query==1)uiSrc[1]=0x0E;
			else if(CTGrp_query==2)uiSrc[1]=0x0D;
			else if(CTGrp_query==3)uiSrc[1]=0x0F;
			pec16=getPEC16(uiSrc,2);
			uiSrc[2]=(pec16>>8) & 0xFF;
			uiSrc[3]=pec16 & 0xFF;
			uiSrc[4]=0x0;uiSrc[5]=0x0;uiSrc[6]=0x0;uiSrc[7]=0x0;uiSrc[8]=0x0;uiSrc[9]=0x0;uiSrc[22]=0x0;
			for(i=0;i<16;i++){
				SPI1WriteByte(uiSrc[i]);
				spiBtDly(1);
			}
			//batteryTaskId=WAIT_BATTERY_RESPONSE;
			SPI_LTC_CS_HIGH();
//			batteryTaskId=TEMP_CHECK;
//		break;
//	case TEMP_CHECK:
						pec16=getPEC16(&spi1RevBuf.buf[4],6);
			if((spi1RevBuf.buf[10]==((pec16>>8) & 0xFF))
				&&(spi1RevBuf.buf[11]=pec16 & 0xFF)){
					parseLtcAux(CTGrp_query);
				}
			if(CTGrp_query++>=4){
				CTGrp_query=0;
				batteryTaskId=BLEEDING_CONTROL;
			}else{
				batteryTaskId=TEMP_POLL;
			}
			SPI_LTC_CS_HIGH();
			//batteryTaskId=BLEEDING_CONTROL_PREPARE;
		break;

	case BLEEDING_CONTROL:
		SPI_LTC_CS_LOW();
			for(tmp=0;tmp<200;tmp++){}
		uiSrc[0]=( WRSCTL>>8) & 0xFF;
		uiSrc[1]=WRSCTL  & 0xFF;;
		pec16=getPEC16(uiSrc,2);
		uiSrc[2]=(pec16>>8) & 0xFF;
		uiSrc[3]=pec16 & 0xFF;
		uiSrc[4]=(UINT32)(moduleInfo.balCmd.bits.b1)*0x8;
		uiSrc[4]|=((UINT32)(moduleInfo.balCmd.bits.b2)*0x8)<<4;
		uiSrc[4]=0x1;
		uiSrc[5]=(UINT32)(moduleInfo.balCmd.bits.b3)*0x10;
		uiSrc[5]|=((UINT32)(moduleInfo.balCmd.bits.b4)*0x10)<<4;
		uiSrc[6]=0x0;
		uiSrc[7]=0x00;
		uiSrc[8]=0x0;
		uiSrc[9]=0x0;
		pec16=getPEC16(&uiSrc[4],6);
		uiSrc[10]=(pec16>>8) & 0xFF;
		uiSrc[11]=pec16 & 0xFF;
		
		for(i=0;i<12;i++){
			SPI1WriteByte(uiSrc[i]);
			spiBtDly(1);
		}
		SPI_LTC_CS_HIGH();
		statTimer=OSTime;
		batteryTaskId=BLEEDING_CONTROL_PREPARE;
		break;
	case BLEEDING_CONTROL_PREPARE:
		SPI_LTC_CS_LOW();
			resetBuffer(&spi1RevBuf);
			for(tmp=0;tmp<200;tmp++){}
		uiSrc[0]=( RDSCTL>>8) & 0xFF;
		uiSrc[1]=RDSCTL  & 0xFF;;
		pec16=getPEC16(uiSrc,2);
		uiSrc[2]=(pec16>>8) & 0xFF;
		uiSrc[3]=pec16 & 0xFF;
		uiSrc[4]=0xFF;uiSrc[5]=0xFF;uiSrc[6]=0x0;uiSrc[7]=0x0;uiSrc[8]=0x0;uiSrc[9]=0x0;uiSrc[22]=0x0;
		
		for(i=0;i<12;i++){
			SPI1WriteByte(uiSrc[i]);
			spiBtDly(1);
		}
		SPI_LTC_CS_HIGH();
		statTimer=OSTime;
		batteryTaskId=CFG_WRITE;
		break;
	default:
		batteryTaskId=0;
		break;
	}

	}
}
void SPI1WriteByte(UINT8 da){
	//Flash_ReadWriteByte(da);
	
	SPI_I2S_SendData(LTC_SPI, da);
}
void	parseLiearRes_1(UINT8 cvg_id){
	
	UINT64 lastUnderVoltageAlarms;
	UINT8 iTemp,alarmTemp;
	for(iTemp=0;iTemp<3;iTemp++){
			UINT16 im;
			im=(UINT16)spi1RevBuf.buf[4+iTemp*2];
			im+=(UINT16)spi1RevBuf.buf[4+2*iTemp+1]<<8;
			im/=10;
			bmu.cv[iTemp+cvg_id*3]=im;
	
	}
	fillEBBattVoltage(&bmu.cv[0]);
	if(moduleInfo.cellVoltage[iTemp]>5500){
			moduleInfo.cellVoltage[iTemp]=5500;
	}	


	resetBuffer(&spi1RevBuf);
	
}
void	parseLtcAux(UINT8 auxgrp_id){
	
	UINT64 lastUnderVoltageAlarms;
	UINT8 iTemp,alarmTemp;
	UINT16 im[3];
	for(iTemp=0;iTemp<3;iTemp++){
			im[iTemp]=(UINT16)spi1RevBuf.buf[4+iTemp*2];
			im[iTemp]+=(UINT16)spi1RevBuf.buf[4+2*iTemp+1]<<8;
			im[iTemp]/=10;
	}
	if(auxgrp_id==0){
		moduleInfo.boardT[0]=lookUpTable((uint32_t *)&temp_table_JT103, 18, -50,10,(UINT16)((UINT32)im[0]*4096/3300));
		moduleInfo.boardT[1]=lookUpTable((uint32_t *)&temp_table_JT103, 18, -50,10,(UINT16)((UINT32)im[1]*4096/3300));
		moduleInfo.cellTemp[0]=lookUpTable((uint32_t *)&temp_table_JT103, 18, -50,10,(UINT16)((UINT32)im[2]*4096/3300));
		bmu.ct[0]=lookUpTable((uint32_t *)&temp_table_JT103, 18, -50,10,(UINT16)((UINT32)im[2]*4096/3300));
	}else if(auxgrp_id==1){
		moduleInfo.cellTemp[1]=im[0];
		bmu.ct[1]=lookUpTable((uint32_t *)&temp_table_JT103, 18, -50,10,(UINT16)((UINT32)im[0]*4096/3300));
		bmu.ct[2]=lookUpTable((uint32_t *)&temp_table_JT103, 18, -50,10,(UINT16)((UINT32)im[1]*4096/3300));
		moduleInfo.v_ref2=im[2];
	}else if(auxgrp_id==2){
		bmu.ct[3]=lookUpTable((uint32_t *)&temp_table_JT103, 18, -50,10,(UINT16)((UINT32)im[0]*4096/3300));
		bmu.ct[4]=lookUpTable((uint32_t *)&temp_table_JT103, 18, -50,10,(UINT16)((UINT32)im[1]*4096/3300));
		bmu.ct[5]=lookUpTable((uint32_t *)&temp_table_JT103, 18, -50,10,(UINT16)((UINT32)im[2]*4096/3300));
	}else if(auxgrp_id==3){
		bmu.ct[6]=lookUpTable((uint32_t *)&temp_table_JT103, 18, -50,10,(UINT16)((UINT32)im[0]*4096/3300));
	}
	resetBuffer(&spi1RevBuf);
	
}
void resetBuffer(BUFFER * buf){
	char i;
	buf->readPos=0;
	buf->insPos=0;
	buf->dataCount=0;
	buf->cap=250;
	for(i=0;i<25;i++){buf->buf[i]=0;}
}

void insertByteToBuffer(BUFFER * buf,UINT8 ch){
	buf->buf[buf->insPos]=ch;

	buf->insPos++;
	if(buf->insPos>=buf->cap) buf->insPos=0;

	buf->dataCount++;
	if(buf->dataCount>=buf->cap){
		buf->dataCount=buf->cap;
		buf->readPos++;
		if(buf->readPos>=buf->cap) buf->readPos=0;
	}
}
static UINT8 calaPEC(UINT8 initByte, UINT8 *datain, UINT8 size){
     PEC_STRUCT data;
	UINT8 in0,in1,in2;
	UINT8 i,j;
	
       data.bytes=*datain;
	initByte=0x41;
	pec.bytes=initByte;
	for(j=0;j<size;j++){
	      for(i=0;i<8;i++){
      			in0=pec.bits.bit7 ^ ((*datain >>(7-i)) & 0x01);
      			in1=pec.bits.bit0 ^ in0;
      			in2=pec.bits.bit1 ^ in0;	

      			pec.bytes=pec.bytes<<1;
      			pec.bits.bit0=in0;
      			pec.bits.bit1=in1;
      			pec.bits.bit2=in2;
	      	}
	      datain++;
      	}
	
	return pec.bytes;

	
}
void  initPECTbl(void){
	int i,bit;
	INT16 remainder;
for(i=0;i<256;i++){
	remainder=i<<7;
	for(bit=8;bit>0;--bit){
		if(remainder & 0x4000){
			remainder=((remainder<<1));
			remainder=(remainder ^ crc15_poly);
		}else{
			remainder=((remainder<<1));
		}
	}
	pec15Table[i]=remainder&0xFFFF;
}
}

INT16 getPEC16(UNS8 *data, int len){
	INT16 remainder, address;
	int i;
	remainder=16; //PEC seed
	for(i=0;i<len;i++){
		address=((remainder>>7)^data[i]) & 0xFF; //calculate PEC table address
		remainder=(remainder<<8)^pec15Table[address];
		}
	return (remainder*2); //the crc15 has a 0 in the LSB so the final value must be multiplied by 2
}
static void spiBtDly(UNS8 tickDly){
	UNS16 iTemp;
	//OSTimeDly(tickDly);
	for(iTemp=0;iTemp<SPI_DATA_SET_INTERVAL;iTemp++){}
}
static void ltc_cmd(UNS16 cmd){
	UNS8 i;
	OSTimeDly(1);
	SPI_LTC_CS_LOW();
	spiBtDly(1);
	uiSrc[0]=(cmd >>8) & 0xFF;
	uiSrc[1]=cmd  & 0xFF;;
	pec16=getPEC16(uiSrc,2);
	uiSrc[2]=(pec16>>8) & 0xFF;
	uiSrc[3]=pec16 & 0xFF;
	for(i=0;i<4;i++){
		SPI1WriteByte(uiSrc[i]);
		spiBtDly(1);
	}
	OSTimeDly(1);
	SPI_LTC_CS_HIGH();
	OSTimeDly(1);
}
