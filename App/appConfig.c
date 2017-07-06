/***********************************************************************
***********************************************************************/
#include "main.h"  
#include "config.h"
#include "task_main.h"
void SCInit(void );
void SysVarInit(void);

void sysConfigInit(void);


/***********************************************************************
 ***********************************************************************/
void SCInit(void){
	SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_RXNE,DISABLE);
	SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_TXE,DISABLE);

	sysConfigInit();
	bmuInit();
	
	//BMinit must after BPInit
	SysVarInit();
	vcuInfo.keep_alive_req=VCMD_NA;
	sysInfo.sysStartTime=OSTime;
	
	SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_RXNE,ENABLE);
}
void SysVarInit(void){
	UNS16 bsyssize;
	bsyssize=sizeof(sysInfo);
	SPI_FLASH_CS_HIGH();
	df_read_open(ADDR_SYS_IN_NVM);
	df_read((UNS8 *)&sysInfo,sizeof(sysInfo));
	SPI_FLASH_CS_HIGH();
	sysInfo.f_alarm=0;
	sysInfo.f_vcu_lost=0;
	sysInfo.f_fault=0;
	sysInfo.sysResetCnt+=1;
	sysInfo.co_node_total=0;

}
void sysConfigInit(void){
	
	uint16_t const_size;
	const_size=sizeof(gSysCfg);
	df_read_open(ADDR_CONFIG_IN_NVM);
	SPI_FLASH_CS_HIGH();
	df_read((UNS8 *)&gSysCfg,sizeof(gSysCfg));
	
	gSysCfg.ucNUM_CV_IN_BMU=NUM_CV_IN_BMU;
	gSysCfg.ucNUM_CT_IN_BMU=NUM_CT_IN_BMU;
	gSysCfg.uiCAP_CELL_FULL=CAP_CELL_FULL;	//AHr
	gSysCfg.uiCAL_CS_OFFSET=CAL_CS_OFFSET;
	gSysCfg.uiCAL_CS_AMP=CAL_CS_AMP;
	
}



