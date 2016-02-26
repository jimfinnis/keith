/**
 * @file config.h
 * @brief  Brief description of file.
 *
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include "hwconfig.h"

// pins in here!

#define PWM1 3
#define DIR1 4

#define PWM2 5
#define DIR2 6

// of course the code must be customised for these to get
// the interrupts. Pin A is the interrupt pin
#define ENC1A 7 
#define ENC1B 8

#define ENC2A 9
#define ENC2B 10

#define LED1  12
#define LED2  13

#endif /* __CONFIG_H */
