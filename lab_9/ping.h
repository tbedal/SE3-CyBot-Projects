/*
 * ping.h
 *
 *  Created on: Oct 29, 2025
 *      Author: thbedal
 */

#ifndef PING_H_
#define PING_H_

#include <inc/tm4c123gh6pm.h>
#include <stdbool.h>
#include <stdint.h>
#include "driverlib/interrupt.h"
#include "Timer.h"

// Sets registers necessary for operating ultrasonic sensor
void ping_init(void);

// TODO: comment me!
double ping_read(void);

#endif /* PING_H_ */
