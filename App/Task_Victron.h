/***********************************************************************
***********************************************************************/
#ifndef _APP_VICTRON_H_
#define _APP_VICTRON_H_


/*assign stack for task */
#define TASK_VICTRON_STK_SIZE  400
typedef struct{
	uint32_t as_general:1,	//B0
		ac_general:1,
		as_ov:1,
		ac_ov:1,
		as_uv:1,
		ac_uv:1,
		as_ot:1,
		ac_ot:1,
		as_ut:1,	//B1
		ac_ut:1,
		as_cot:1,
		ac_cot:1,
		as_cut:1,
		ac_cut:1,
		as_oc:1,	
		ac_oc:1,
		as_coc:1,	//B2
		ac_coc:1,
		as_cntr:1,
		ac_cntr:1,
		as_sc:1,
		ac_sc:1,	
		as_bms:1,	
		ac_bms:1,
		as_cim:1,	//B3
		ac_cim:1,
		aunused_1:6;
	uint32_t ws_general:1,	//B0
		wc_general:1,
		ws_ov:1,
		wc_ov:1,
		ws_uv:1,
		wc_uv:1,
		ws_ot:1,
		wc_ot:1,
		ws_ut:1,	//B1
		wc_ut:1,
		ws_cot:1,
		wc_cot:1,
		ws_cut:1,
		wc_cut:1,
		ws_oc:1,	
		wc_oc:1,
		ws_coc:1,	//B2
		wc_coc:1,
		ws_cntr:1,
		wc_cntr:1,
		ws_sc:1,
		wc_sc:1,	
		ws_bms:1,	
		wc_bms:1,
		ws_cim:1,	//B3
		wc_cim:1,
		wunused_1:6;
}VICTRON_FAULT_T;


extern OS_STK Stk_Task_Victron[TASK_VICTRON_STK_SIZE];
extern OS_EVENT *sem_vcu_can_rev;
extern OS_EVENT *sem_bmu_can_rev;
extern OS_EVENT *sem_fault_clr;
/**/
extern void Task_Victron(void *pdata);

#endif


