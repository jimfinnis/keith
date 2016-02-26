/**
 * \file
 * Code generated automatically by regparse.hs - DO NOT MODIFY.
 */

#include "regs.h"
extern MAYBEPROGMEM Register registerTable_DRIVE[]={
	{65, -1.0, -1.0}, 	// RESET: (0)  set to 1 to reset
	{65, -1.0, -1.0}, 	// PING: (1)  debugging
	{66, -1.0, -1.0}, 	// DEBUG: (2)  debugging
	{65, -1.0, -1.0}, 	// DEBUGLED: (3)  debug LED
	{2, -1.0, -1.0}, 	// TIMER: (4)  millis since start
	{2, -1.0, -1.0}, 	// INTERVALI2C: (5)  interval between I2C ticks
	{2, -1.0, -1.0}, 	// STATUS: (6)  see status flags in regs.h
	{1, -1.0, -1.0}, 	// EXCEPTIONDATA: (7)  error state
	{66, -4000.0, 4000.0}, 	// M1_REQSPEED: (8)  required speed
	{66, 0.0, 10.0}, 	// M1_PGAIN: (9)  P-gain
	{66, 0.0, 10.0}, 	// M1_IGAIN: (10)  I-gain
	{66, -10.0, 10.0}, 	// M1_DGAIN: (11)  D-gain
	{66, 0.0, 1000.0}, 	// M1_INTEGRALCAP: (12)  integral error cap
	{66, 0.0, 1.0}, 	// M1_INTEGRALDECAY: (13)  integral decay
	{66, 0.0, 50.0}, 	// M1_DEADZONE: (14)  errors less than this are disregarded
	{2, -4000.0, 4000.0}, 	// M1_ACTUALSPEED: (15)  actual speed from encoder
	{2, -1000.0, 1000.0}, 	// M1_ERROR: (16)  required minus actual speed
	{2, -1000.0, 1000.0}, 	// M1_ERRORINTEGRAL: (17)  error integral magnitude
	{2, -200.0, 200.0}, 	// M1_ERRORDERIV: (18)  error derivative
	{2, -255.0, 255.0}, 	// M1_CONTROL: (19)  value being sent to motor
	{2, 0.0, 1000.0}, 	// M1_INTERVALCTRL: (20)  time between control runs (ms)
	{2, -1.0, -1.0}, 	// M1_CURRENT: (21)  raw current reading
	{2, -1.0, -1.0}, 	// M1_ODO: (22)  encoder ticks
	{66, -4000.0, 4000.0}, 	// M2_REQSPEED: (23)  required speed
	{66, 0.0, 10.0}, 	// M2_PGAIN: (24)  P-gain
	{66, 0.0, 10.0}, 	// M2_IGAIN: (25)  I-gain
	{66, -10.0, 10.0}, 	// M2_DGAIN: (26)  D-gain
	{66, 0.0, 1000.0}, 	// M2_INTEGRALCAP: (27)  integral error cap
	{66, 0.0, 1.0}, 	// M2_INTEGRALDECAY: (28)  integral decay
	{66, 0.0, 50.0}, 	// M2_DEADZONE: (29)  errors less than this are disregarded
	{2, -4000.0, 4000.0}, 	// M2_ACTUALSPEED: (30)  actual speed from encoder
	{2, -1000.0, 1000.0}, 	// M2_ERROR: (31)  required minus actual speed
	{2, -1000.0, 1000.0}, 	// M2_ERRORINTEGRAL: (32)  error integral magnitude
	{2, -200.0, 200.0}, 	// M2_ERRORDERIV: (33)  error derivative
	{2, -255.0, 255.0}, 	// M2_CONTROL: (34)  value being sent to motor
	{2, 0.0, 1000.0}, 	// M2_INTERVALCTRL: (35)  time between control runs (ms)
	{2, -1.0, -1.0}, 	// M2_CURRENT: (36)  raw current reading
	{2, -1.0, -1.0}, 	// M2_ODO: (37)  encoder ticks
	{32, -1.0, -1.0}, 	// TERMINATOR: (38)  
};

extern MAYBEPROGMEM Register registerTable_MASTER[]={
	{65, -1.0, -1.0}, 	// RESET: (0)  set to clear exception state
	{2, -1.0, -1.0}, 	// EXCEPTIONDATA: (1)  data of exception
	{66, -1.0, -1.0}, 	// DEBUG: (2)  master test
	{32, -1.0, -1.0}, 	// TERMINATOR: (3)  
};
