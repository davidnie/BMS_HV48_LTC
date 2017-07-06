#include "main.h"
#include "task_main.h"
#include "HAL.h"
#define ON 1
#define OFF 0
void relayControl(uint8_t rl, uint8_t status);
UINT8 getRelayStatus( UINT8 relayName);

void relayControl(UINT8 relayName, UINT8 status){
	#define RELAY_DELAY_MILLISECONDS	500
	int i;

}


UINT8 getSStrRelayStatus( UINT8 sid){

return 1;
}
UINT8 getSStrInterlockStatus( UINT8 sid){

	
}
		

void battPowerControl(UINT8 status){
	
	//if((status==ON)&&(BATTERY_BOARD_POWER_STATUS==OFF)){
	if(0){
		//batteryPowerTick=TickGet();
	}
	//if((status==OFF)&&(BATTERY_BOARD_POWER_STATUS==ON)){
		//batteryPowerOffTick=TickGet();
	//}
	
//	BATTERY_BOARD_POWER_SET(status);


}

