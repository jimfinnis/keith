/**
 * \file
 * Code generated automatically by regparse.hs - DO NOT MODIFY.
 */

#include "regs.h"
#define REG_RESET	0 	//set to 1 to reset
#define REG_PING	1 	//debugging
#define REG_DEBUG	2 	//debugging
#define REG_DEBUGLED	3 	//debug LED
#define REG_TIMER	4 	//millis since start
#define REG_INTERVALI2C	5 	//interval between I2C ticks
#define REG_STATUS	6 	//see status flags in regs.h
#define REG_EXCEPTIONDATA	7 	//error state





#define REGDRIVE_M1_REQSPEED	8 	//required speed
#define REGDRIVE_M1_PGAIN	9 	//P-gain
#define REGDRIVE_M1_IGAIN	10 	//I-gain
#define REGDRIVE_M1_DGAIN	11 	//D-gain
#define REGDRIVE_M1_INTEGRALCAP	12 	//integral error cap
#define REGDRIVE_M1_INTEGRALDECAY	13 	//integral decay
#define REGDRIVE_M1_DEADZONE	14 	//errors less than this are disregarded
#define REGDRIVE_M1_ACTUALSPEED	15 	//actual speed from encoder
#define REGDRIVE_M1_ERROR	16 	//required minus actual speed
#define REGDRIVE_M1_ERRORINTEGRAL	17 	//error integral magnitude
#define REGDRIVE_M1_ERRORDERIV	18 	//error derivative
#define REGDRIVE_M1_CONTROL	19 	//value being sent to motor
#define REGDRIVE_M1_INTERVALCTRL	20 	//time between control runs (ms)
#define REGDRIVE_M1_CURRENT	21 	//raw current reading
#define REGDRIVE_M1_ODO	22 	//encoder ticks
#define REGDRIVE_M2_REQSPEED	23 	//required speed
#define REGDRIVE_M2_PGAIN	24 	//P-gain
#define REGDRIVE_M2_IGAIN	25 	//I-gain
#define REGDRIVE_M2_DGAIN	26 	//D-gain
#define REGDRIVE_M2_INTEGRALCAP	27 	//integral error cap
#define REGDRIVE_M2_INTEGRALDECAY	28 	//integral decay
#define REGDRIVE_M2_DEADZONE	29 	//errors less than this are disregarded
#define REGDRIVE_M2_ACTUALSPEED	30 	//actual speed from encoder
#define REGDRIVE_M2_ERROR	31 	//required minus actual speed
#define REGDRIVE_M2_ERRORINTEGRAL	32 	//error integral magnitude
#define REGDRIVE_M2_ERRORDERIV	33 	//error derivative
#define REGDRIVE_M2_CONTROL	34 	//value being sent to motor
#define REGDRIVE_M2_INTERVALCTRL	35 	//time between control runs (ms)
#define REGDRIVE_M2_CURRENT	36 	//raw current reading
#define REGDRIVE_M2_ODO	37 	//encoder ticks
#define NUMREGS_DRIVE 38

extern MAYBEPROGMEM Register registerTable_DRIVE[];


#define REGMASTER_RESET	0 	//set to clear exception state
#define REGMASTER_EXCEPTIONDATA	1 	//data of exception
#define REGMASTER_DEBUG	2 	//master test
#define NUMREGS_MASTER 3

extern MAYBEPROGMEM Register registerTable_MASTER[];

