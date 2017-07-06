#ifndef _APP_TASK_LTC_H_
#define _APP_TASK_LTC_H_
#define TASK_LTC_STK_SIZE	2000
#define  WRCFGA    0x01 
#define  WRCFGB   0x24
#define	RDCFGA    0x02 
#define	RDCFGB    0x26 
#define	RDSTATA     0x10 //read status register group A
#define	RDSTATB     0x12 //read status register group B
#define	RDFLG    0x0c 
#define	STCVAD   0x10 
#define	STOWAD   0x20 
#define	PLADC    0x40 
#define	PLINT    0x50 
#define	DAGN     0x52 
#define	RDDGNR   0x54 
#define	STCVDC   0x60 
#define	STOWDC   0x70
//#define ADCV     0x0360
#define ADCV     0x0370
#define RDCVA    0x0004  //1-3 cell
#define RDCVB    0x0006  //4-6 cell
#define RDCVC    0x0008  //7-9 cell
#define RDCVD    0x000a  //10-12cell
#define RDCVE    0x0009  //13-15cell
#define RDCVF    0x000b  //16-18cell for 6813
#define ADAX     0x0460  // start GPIO adc conversion
#define RDAUXA   0x000c  //GPIO1-3
#define RDAUXB   0x000e  //GPIO4-5
#define RDAUXC   0x000d  //GPIO6-7
#define RDAUXD   0x000f  //GPIO8-9
#define WRSCTL   0x0014  //balance  1-12 .  write S register group
#define WRPS2    0x001c  //balance  13-18   write S/PWM register group B
#define RDSCTL    0x0016  //read balance  1-12
#define WRPWM    0x0020  //read balance  1-12
#define WRPWM2    0x001C  //read balance  1-12
#define CMD_MUTE 0x0028		//stop balance
#define CMD_UNMUTE	0x0029	//start balance
#define SPI_DATA_SET_INTERVAL	2000
typedef union{
 UINT8 bytes;
 struct{
 UINT8 bit7:1,
	  bit6:1,
	  bit5:1,
	  bit4:1,
	  bit3:1,
	  bit2:1,
	  bit1:1,
	  bit0:1; 
	 }bits;
}PEC_STRUCT;

typedef enum{
	PREPARE_QUERY,
	QUERY_BATTERY,
	VOL_ADC_WAIT,
	VOL_ADC_PREPARE,
	VOL_ADC,
	VOL_POLL,
	TEMP_ADC_PREPARE,
	TEMP_ADC,
	TEMP_ADC_WAIT,
	TEMP_POLL,
	TEMP_CHECK,
	SEND_BBS_BYTE_1,
	SEND_BBS_BYTE_2,
	SEND_BBS_BYTE_3,
	SEND_BBS_BYTE_4,
	SEND_BBS_BYTE_5,
	SEND_CHECK_SUM,
	WAIT_BATTERY_RESPONSE,
	CHECK_BATTERY_RESPONSE,
	BLEEDING_CONTROL_PREPARE,
	BLEEDING_CONTROL,
	CFG_WRITE,
	CFG_WRITE_B,
	CFG_WRITE_WAIT,
	CFG_READ,
	PWM_WRITE,
	PWM_WRITE_B
}BATTERY_TASK_ID;
typedef struct{
	UINT8 CDC0:1,
		CDC1:1,
		CDC2:1,
		CELL10:1,
		LVLPL:1,
		GPIO1:1,
		GPIO2:1,
		WDTEN:1;
	}CFG0_BIT;
typedef struct{
	union{
		CFG0_BIT cfg0_bits;

		UINT8 CFGR0;
			
	}cfg0;
	union{
		UINT8 DCC8:1,
			DCC7:1,
			DCC6:1,
			DCC5:1,
			DCC4:1,
			DCC3:1,
			DCC2:1,
			DCC1:1;
		UINT8 CFGR1;
		}cfg1;
	union{
		UINT8 MC4I:1,
			MC3I:1,
			MC2I:1,
			MC1I:1,
			DCC12:1,
			DCC11:1,
			DCC10:1,
			DCC9:1;
		UINT8 CFGR2;
		}cfg2;
	union{
		UINT8 MC12I:1,
			MC11I:1,
			MC10I:1,
			MC9I:1,
			MC8I:1,
			MC7I:1,
			MC6I:1,
			MC5I:1;
		UINT8 CFGR3;
		}cfg3;
	UINT8 CFGR4;
	UINT8 CFGR5;
	UINT8 CFGBR0;
	UINT8 CFGBR1;
	UINT8 CFGBR2;
	UINT8 CFGBR3;
	UINT8 CFGBR4;
	UINT8 CFGBR5;
}LINEAR_CFGREG;
extern LINEAR_CFGREG ltcCfg;

typedef struct{
	UINT8 address;
	UINT8 errors;
	UINT8 lowerCellNum;	//cell number connect to lower linear chip
	UINT8 higherCellNum;	// cell number connect to higher linear chip
	UINT16 cellVoltage[18];	//unit mv
	UINT16 v_ref2;
	CELL_BITSTAT bleedingFlag;
	CELL_BITSTAT balCmd;
	INT16 cellTemp[18]; //unit C
	INT16 boardT[2];
	
	//UINT8 tempature[CELL_NUM_PER_MODULE]; //unit C
	UINT8 isMBUExist;	// 1: MB alive, 0: MB dead
	UINT32 bleedingBeginTick[14];
	UINT32 bleedingStopTick[14];
	UINT8 bleedingCellNum;
}MODULE_INFORMATION;
extern MODULE_INFORMATION moduleInfo;
typedef struct{
   UINT8 revFlag:1,
   		 ovFlag:1,
		 sendFlag:1;
   
   UINT8 unused;
}SPI_FLAGS;
typedef struct{
	UINT8 cap;
	UINT8 insPos;
	UINT8 readPos;
	UINT8 dataCount;
	UINT8 buf[256];
	
}BUFFER;

extern OS_STK Stk_Task_LTC[TASK_LTC_STK_SIZE];
extern 	BUFFER spi2RevBuf,spi1RevBuf;
extern void Task_LTC(void *pdata );



#endif
