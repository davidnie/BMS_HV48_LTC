#ifndef __HAL_H
#define __HAL_H

#define CONT_ON 1
#define CONT_OFF 0


#define RL_MAIN_POS 		1
#define RL_MAIN_NEG		2
#define RL_PCHG	3
#define RELAY_SSTR_1	4
#define RELAY_SSTR_2	5
#define RELAY_SSTR_3	6
#define RELAY_SSTR_4	7
#define RELAY_SSTR_5	8
#define RELAY_SSTR_6	9
#define RELAY_SSTR_7	10
#define RELAY_SSTR_8	11
#define RELAY_SSTR_9	12
#define RELAY_SSTR_10	13
#define CHG_RELAY_NEG	14
#define RL_CHG_POS	15
#define RELAY_COLLING_1	16
#define RELAY_COLLING_2	17
#define RELAY_COLLING_3	18
#define RELAY_COLLING_4	19
#define RELAY_MOTOR_ENABLE 20
#define RELAY_CHG_DISABLE 		21
#define RELAY_FAN_PWR	22
#define RELAY_BAL_1	23
#define RELAY_BAL_2	24
#define RELAY_BAL_3	25
#define RELAY_BAL_4	26
#define RELAY_CAB_FAN 27
#define RL_DCDC 28
#define RL_HEATER_1	29
#define RL_HEATER_2	30
#define RL_HEATER_3	31
#define RL_HEATER_4	32
#define RL_HEATER_5	33
#define RL_HEATER_6	34
#define RL_HEATER_7	35
#define RL_HEATER_8	36
#define RL_HEATER_9	37
#define RL_HEATER_10	38



extern void relayControl(uint8_t rl, uint8_t status);

#endif
