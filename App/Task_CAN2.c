/***********************************************************************
***********************************************************************/
#include "main.h"
#include "bsp_can.h"
#include "Task_ADC.h"
#include "Task_Main.h"
#include "Task_sysCheck.h"
#include "shed_ctrl.h"
#include "battery_gt.h"
OS_STK Stk_Task_CAN2_Rev[TASK_CAN2_REV_STK_SIZE];
OS_STK Stk_Task_CAN2_Snd2Buf[TASK_CAN2_SEND_TO_BUF_STK_SIZE];
OS_STK Stk_Task_CAN_Send[TASK_CAN_SEND_STK_SIZE];

OS_EVENT  *sem_CAN2_rec_flag;			//CAN2 receive semphere
OS_EVENT  *sem_CAN2_send_flag;			//CAN2 send semphere
CAN_DATA c2_d;
void sdIntAddrClaim(CAN_TypeDef * CANPort);
void can2Dispatch(CanRxMsg * RxMSG);
void j1939_send(CAN_TypeDef *CANx,J1939_MESSAGE *jmsg);

void		parseIntAddrClaim(CanRxMsg * RxMSG);
void		parseDebugCommand_0x131(CanRxMsg * RxMSG);
static void CAN_Bp2Vcu(void);
static void CAN_Bm2Bp(UNS8 id);
static void CAN_Bp2Relay(UNS8 rl_stat);

static void sendRLCtrl(void);
static void insCellInfo(UNS8 scrAddr,UNS8 * da);
static void insModInfo_1(UNS8 scrAddr,UNS8 * da);
static void insModInfo_2(UNS8 scrAddr,UNS8 * da);
static void CAN_GT_ReadInfo(UNS8 id,GT_OP_CMD op);
static void CAN_GT_BalCtrl(UNS8 id);
static UNS8 isAddrSent=0;
extern 	UNS32  main_task_timer;
extern UNS32 mTsk_rt;

void Task_CAN2MsgBuf_GT(void *pdata){

}

/***********************************************************************
note: STM32F407 only has 3 send mail box
***********************************************************************/
void Task_CAN2_Rev(void *pdata)
{		 
}

void Task_CANSend(void *pdata){
		static UNS8 C1RdPoint=0;
	static UNS8 C2RdPoint=0;
	UNS8 transmit_mailbox;
    unsigned  char  os_err;
	sem_CAN2_send_flag = OSSemCreate(10); //create a semphere,
	while(1){
	//OSSemPend(sem_CAN2_send_flag, 0,&os_err);//suspend at here wait for CAN send command
	OSTimeDly(1);//suspend at here for 5ms
	if(C2_wait_num>0){
			transmit_mailbox = CAN_Transmit(CAN2,&CAN2TxBuf[C2RdPoint]);  /*  */	
			if(transmit_mailbox<CAN_TxStatus_NoMailBox){
				if(++C2RdPoint>=CAN_MSG_SEND_BUFFER_SIZE) C2RdPoint=0;
				C2_wait_num--;
			}
		}
		if(C2_wait_num>0){
			transmit_mailbox = CAN_Transmit(CAN2,&CAN2TxBuf[C2RdPoint]);  /*  */	
			if(transmit_mailbox<CAN_TxStatus_NoMailBox){
				if(++C2RdPoint>=CAN_MSG_SEND_BUFFER_SIZE) C2RdPoint=0;
				C2_wait_num--;
			}
		}
		if(C2_wait_num>0){
			transmit_mailbox = CAN_Transmit(CAN2,&CAN2TxBuf[C2RdPoint]);  /*  */	
			if(transmit_mailbox<CAN_TxStatus_NoMailBox){
				if(++C2RdPoint>=CAN_MSG_SEND_BUFFER_SIZE) C2RdPoint=0;
				C2_wait_num--;
			}
		}
			
		if(C1_wait_num>0){
			transmit_mailbox = CAN_Transmit(CAN1,&CAN1TxBuf[C1RdPoint]);  /*  */	
			if(transmit_mailbox<CAN_TxStatus_NoMailBox){
				if(++C1RdPoint>=CAN_MSG_SEND_BUFFER_SIZE) C1RdPoint=0;
				C1_wait_num--;
			}
		}
		if(C1_wait_num>0){
			transmit_mailbox = CAN_Transmit(CAN1,&CAN1TxBuf[C1RdPoint]);  /*  */	
			if(transmit_mailbox<CAN_TxStatus_NoMailBox){
				if(++C1RdPoint>=CAN_MSG_SEND_BUFFER_SIZE) C1RdPoint=0;
				C1_wait_num--;
			}
		}
		if(C1_wait_num>0){
			transmit_mailbox = CAN_Transmit(CAN1,&CAN1TxBuf[C1RdPoint]);  /*  */	
			if(transmit_mailbox<CAN_TxStatus_NoMailBox){
				if(++C1RdPoint>=CAN_MSG_SEND_BUFFER_SIZE) C1RdPoint=0;
				C1_wait_num--;
			}
		}
	}
}
void sdIntAddrClaim(CAN_TypeDef * CANPort){
	UNS8 bt[8];

	
//	CAN2_WriteData(0x18FF8900+strInfo.intAddr,bt,8,CAN_Id_Extended);
}
void parseIntAddrClaim(CanRxMsg * RxMSG){
	UNS16 ser_no;
	UNS8 i,j,k, bm_num,isNewMod=0,rand_num,addr;
	addr=(RxMSG->ExtId)&0xFF;

}
void parseDebugCommand_0x131(CanRxMsg * RxMSG){
	//nie if(RxMSG->Data[0]==strInfo.modId){
		switch(RxMSG->Data[1]){
			case 1:
				
				break;
			case 2:
				bmu.bm_soc=RxMSG->Data[2]*10;
				break;
			case 3:
				bmu.bm_soh=RxMSG->Data[2]*10;
				break;
			case 4:
				//strInfo[0].mod_sn=((UNS16)(RxMSG->Data[2])<<8)+RxMSG->Data[3];
				break;
			case 5:
				sysInfo.f_fault=0;
				OSSemPost(sem_fault_clr);
				
				break;
			case 6:
				//NVM_BMInfoWrite();
				break;
			case 100:
				sysInfo.boot_option=0;
				NVM_SysinfoWrite();
				if(sysInfo.boot_option==0){
					NVIC_SystemReset();
					}
				break;
			default:
				break;
		}
	//nie}		
	
}
/*below message will send to VCH */
static void CAN_Bm2Bp(UNS8 cid){
	UNS8 bt[8];
	INT32 whr;

}
/*below message will send to VCH */
static void CAN_Bp2Vcu(void){
	CanTxMsg txMsg;
	UNS8 bt[8];

	
}
 void J1939IdAssb_pgn(unsigned int pgn,J1939_MESSAGE *JMsg){
	unsigned char i;
	unsigned char pd,ps;
	JMsg->jid.j1939_id.DataPage=(pgn>>16) && 0x1;
	pd=(pgn>>8)&& 0xFF;
	ps=pgn&&0xFF;
	if(pd>=0xF0){	//broad cast
		JMsg->jid.eid&=0x1C0000FF;
		JMsg->jid.j1939_id.PDUFormat=pd;
		JMsg->jid.j1939_id.PDUSpecific=ps;
	}else{   //to specisic node
		JMsg->jid.j1939_id.PDUFormat=pd;
		JMsg->jid.eid &= 0x1C00FFFF;
	}
	JMsg->jid.eid|=pgn<<8;

}

void j1939_send(CAN_TypeDef *CANx,J1939_MESSAGE *jmsg){
	J1939_MESSAGE msg;
	msg=*jmsg;
	//CAN2_WriteData(msg.jid.eid,msg.data,msg.dlc,CAN_Id_Extended);
}

static void insCellInfo(UNS8 scrAddr,UNS8 * da){

	
}


