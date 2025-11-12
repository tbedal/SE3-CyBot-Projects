/**
 * uart.c
 *
 * Contains functions to operate the PING)) ultrasonic sensory on the CyBot.
 * 
 * @date November 5, 2025
 * @author Thiago Bedal
 * @author Joseph Vesterby
**/

#ifndef PING_H_
#define PING_H_

#include <inc/tm4c123gh6pm.h>
#include <stdbool.h>
#include <stdint.h>
#include "driverlib/interrupt.h"
#include "Timer.h"

// Sets registers necessary for operating ultrasonic sensor
void ping_init_OURS(void);

// Sends out 5 us pulse and times length of pulse in to calculate distance from sensor in cm
double ping_read(void);

#endif /* PING_H_ */
