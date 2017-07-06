#ifndef _APP_TASK_MAIN_H_
#define _APP_TASK_MAIN_H_
#include "HAL.h";

#define TASK_MAIN_STK_SIZE	2000

#ifdef _TASK_MAIN_DEFINE
	#define EXT_VAR 
#else
	#define EXT_VAR extern
#endif
typedef union{
   UNS16 wd[4];
   UNS8 bt[8];
}CAN_DATA;
#define CMD_BMU_FET_ON 1
#define CMD_BMU_FET_OFF 2
#define CMD_BMU_SHUTDOWN 3
#define TOTAL_BMU_NUM 2
typedef enum{
	STR_INIT,
	STR_READY,
	STR_RUN,
	STR_FAULT
}STR_STAGE_T;


typedef union mod_err{
	UINT32 word_m_err;
	UINT32 err_m_ov,
		err_m_uv,
		err_m_ot,
		err_m_ut;
		
}MOD_ERR_T;


typedef enum{
   B_BOOT,
   B_ARBITRATION,
   B_PCHG_WAIT,
   B_ENGIZE_DC,
   B_WAIT_VCU,	 //8
   B_RUN,
   B_RUN_SLAVE,
   B_WAIT_SLAVE,
   B_SHUTDOWN_PRE,
   B_SHUTDOWN,
   B_IDLE,
   B_FAULT,
   B_CHARGE,
   B_CHARGE_DONE,
   B_DISCHARGE,
   B_STORAGE,
   B_STORAGE_CHARGE
}B_OP_MODE;
typedef enum{
   BP_INIT,
   BP_STANDBY,
   
   BP_DO_NOT_ATTACH,
   BP_READY_TO_ATTACH,
   BP_NOMOR_OPERATION,
   BP_REQUEST_DETACH,
   BP_ERROR,
   BP_FINAL,
   BP_SYNC_OFF,
   
   BP_PCHG,
   BP_ENGIZE_DC,
   BP_RUNING,
   BP_WARMUP,
   BP_FAULT,
   BP_SHUTDOWN,
   BP_CHARGE,
   BP_DISCHARGE
}B_TKS_OP_MODE;
typedef enum{
	WK_MACHINE,
	WK_CHARGE,
	WK_PUSH_BUTTON,
	WK_EXT_HV
}WK_SOURCE;
typedef enum{
	ARTRIBITION,
	MASTER,
	SLAVE,
}BM_POSISTION;
typedef struct{
	UNS8 co_addr;
	UNS8 ser_no[20]; //serial number ASCII

	//index 0x6E80 sub_index:3
	uint16_t cv[CVS_NUM]; // UINT16	mV		R
	int16_t ct[CTS_NUM]; //0.1C
	uint16_t Balancing; //	bit0: cell1, bit15:cell16	Balancing status	R

	UNS16 mod_soc;	//0.1 %
	UNS16 mod_soh;	//0.1 %
	INT32 mod_curr; //0.001A charge:+   discharge:-
	BOOLEAN currentDirection; // 1 charge   0- discharge

	REAL32	t_fet; //Temp[3] FLOAT	?	T-FET	R
	REAL32	t_pfet; //[4] FLOAT ?	T-Pre_R R
	REAL32	t_ambient; //[5] FLOAT	?	T-Ambient	R
	UNS8 bmuOnCmd; // 1 : fet on, 2:fet off  3:shutdown open/close FET in BMU
	UNS32 mod_sn;	//serial number
	UNS16 rand_num;
	BM_POSISTION Inter_Role;  // 2: slave 1:master 0:artribition
	UNS8  bmu_fw_ver[3]; //ver[2]: major, ver[1]:minor, ver[0]:patch
	UNS8  bmu_hw_ver[3];

	UNS64 mod_life_ahr_c; 	// mA.Sec 
	UNS64 mod_life_ahr_d; 	// mA.Sec 
	UNS64 mod_life_whr_c; 	// mw.sec 
	UNS64 mod_life_whr_d;	//mv.sec
	UNS32 mod_volt; //0.001V
	UNS16 mod_clc;	//0.1A
	UNS16 mod_cld;	//0.1A
	UNS16 mod_plc;	//w
	UNS16 mod_pld;	//w
	UNS32 modSafeFlag_1;
	UNS32 modSafeFlag_2;
	UNS32 modStatus;
	UNS8 modFetFlag;
	UNS8 parllFlag;
	BMU_INFO *ptBMU;

	UINT16 s_cv_avg;	//mv
	UINT16 s_cv_min;	//mv
	UINT16 s_cv_max;	//mv
	INT16 s_ct_min;	// 0.1C
	INT16 s_ct_max; //0.1C
	INT16 s_ct_avg; //0.1C
	UNS8 s_cvmin_cid;
	UNS8 s_cvmin_mid;
	UNS8 s_cvmax_cid;
	UNS8 s_cvmax_mid;
	UNS8 s_ctmin_cid;
	UNS8 s_ctmin_mid;
	UNS8 s_ctmax_cid;
	UNS8 s_ctmax_mid;
	MOD_ERR_T m_err;
	UNS8 SOC_OCV_adjusted;
	INT32 AHrAdjTgt;
	INT32 AHrAdjLeft;
	UINT32 capacity;		//unit 0.01A.S
	UINT32 capacityLose;		//32 bits, I*Time uint: 0.01A.S valid in Discharge Precess
	UINT32 capacityGain;		//32 bits, I*Time uint: 0.01 A.S . valid in Charge precess
	UINT32 capacityLoseTotal;		//32 bits, I*Time uint: 0.01A.S valid in Discharge Precess
	UINT32 capacityGainTotal;		//32 bits, I*Time uint: 0.01 A.S . valid in Charge precess
	UINT64 powerLose;		//uint 0.001V.A.S
	UINT64 powerGain;		// uint 0.001 V.A.S
	UINT64 powerLoseTotal;		//uint 0.001V.A.S
	UINT64 powerGainTotal;		// uint 0.001 V.A.S
	UINT32 fullCapacity;	//unit: 0.01A.S
	UINT16 fullEnergy;	//unit wh
	UINT16 cycleCount;

	//index 6E80 sub_index 1
	uint32_t gBatteryStatus; // UINT32	-	Battery Status
	uint8_t gParallelFlags; //	UINT8	-	Parallel Status
	BMU_FLAG_6D07_t gFlags1; //	UINT32	-	Warning Flags1
	BMU_FLAG_6D08_t gFlags2; // UINT8	-	Warning Flags2
	uint16_t gBmuStatus; //UINT8 
	uint8_t gFetStatus; //	UINT8	-	(CFET/DFET/ON/OFF)


	UNS32	TotalChgCap; 	//mAH 	R
	UNS32	TotalDsgCap; // UNS32	mAH 	R
	BOOLEAN f_bmu_lost;
	UINT32 sv_sum;
	UINT32 sv_meas;
	UINT32 sv;
	STR_STAGE_T strStat;

}BATT_STR_INFO;
#define MCMD_CLOSE	1
#define MCMD_OPEN	2
#define MCMD_SHUTDOWN	3
#define MCMD_NA	0
typedef struct{
	INT8 tempture;
	UINT8 actionCmd:1,		//0:open 1:close
		status:1,	//0:open 1: close
		overTempture:1,
		weld:1,
		broken:1,
		coilShort:1,
		coilOpenLoad:1;
	uint32_t closeTick;
	uint32_t actionTick;
	uint32_t openTick;
}RELAY_INFO;

typedef struct{
	UNS16 bp_sn;
	UNS8 hb;
	UNS8 cmd_strCtrl; // 1 : fet on, 2:fet off  3:shutdown open/close FET in BMU
	UNS32 VBus; //measured. after main output relay
	UNS32 VBat;	//measured. before main output relay
	UNS8 extAddr;
	UNS8 masterCmd; // 1-close FET, 2-open fets 3-shutdown  is command to all BMUs
	UNS8 bmuWakeupCmd; // 1: wakeup, 0: sleep   set hardware signal for BMU
	UNS8 num_str;
	UNS8 sc_fw_ver_major; 
	UNS8 sc_fw_ver_minor; 
	UNS8 sc_fw_ver_patch; 
	UNS16 sc_hw_version[3];
	BATT_STR_INFO *str;
	INT32 bp_curr; // 0.001A  chg:>0 dchg:<0
	UNS32 bp_VBus; //0.001V
	UNS32 bp_clc; //0.1A
	UNS32 bp_cld; //0.1A
	UNS16 bp_soc; //0.1%
	UNS16 bp_soh; //0.1%
	UNS32 bp_lifw_wh; //wh
	BOOLEAN w_idle; // 10min I<200mA
	
	UNS8 str_alive_num;
	UNS8 str_ready_num;
	UNS8 str_on_num;
	UNS8 str_off_num;
	CELL_BITSTAT strAlive;
	UNS32 strVolt_min;
	UNS32 strVolt_max;
	UNS8 sid_vmin;
	UNS8 sid_vmax;
	UNS8 sid_cvmin;
	UNS8 sid_cvmax;
	UNS8 sid_ctmin;
	UNS8 sid_ctmax;
	INT16 ct_min; //0.1C
	
	
}BATT_PACK_INFO;
typedef enum{
	VCMD_NA,
	VCMD_CLOSE,
	VCMD_OPEN,
	VCMD_SHUTDOWN,
	VCMD_SD_PRE
}VCMD_ENUM;
typedef enum{
	VREQ_NA,
	VREQ_CLEAR_ALARM,
	VREQ_CLEAR_FAULT,
}VREQ_ENUM;
typedef struct{
	UNS8 msgId;
	VCMD_ENUM keep_alive_req; // 0: open, 1: operate,FET close 2: shuting down, FET in close status
	VCMD_ENUM vcuCmd; // 0: open, 1: operate,FET close 2: shuting down, FET in close status
	VREQ_ENUM vcuMtnReq;	// 0: n/a 1: clear alarm 2: clear fault
}VCU_INFO;
typedef struct{
	UNS8 boot_option; // 0: reflash 1: jump to APP  2:flash sucessed FF:stay at boot
	WK_SOURCE wkSrc; 	// 0:N/A 1:Machine, 2:charger
	BOOLEAN f_vcu_lost;
	UNS8 co_node_total;
	
	BOOLEAN f_warning;
	BOOLEAN f_alarm;
	BOOLEAN f_fault;
	UNS32 sysStartTime;
	uint16_t sysResetCnt;
	uint8_t da[248];
	
	RELAY_INFO mainRelayPosInfo;
	RELAY_INFO mainRelayNegInfo;
	RELAY_INFO pchgRlInfo;
	RELAY_INFO chgRelayPosInfo;
	RELAY_INFO chgRelayNegInfo;
	RELAY_INFO BrakerRelayInfo;
	RELAY_INFO fanPwrRelayInfo;
	
	RELAY_INFO fanCABRLInfo;
	RELAY_INFO pumpRLInfo;
}SYS_INFO;
EXT_VAR SYS_INFO sysInfo;
EXT_VAR VCU_INFO vcuInfo;

EXT_VAR WK_SOURCE gWkSrc;
EXT_VAR OS_STK Stk_Task_Main[TASK_MAIN_STK_SIZE];

void Task_Main(void *pdata);
#endif

