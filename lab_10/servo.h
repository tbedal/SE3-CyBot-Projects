/**
 * uart.c
 *
 *  Contains functions to operate the Servo motor on the CyBot
 *
 * @date November 5, 2025
 * @author Thiago Bedal
 * @author Joseph Vesterby
**/

#ifndef SERVO_H_
#define SERVO_H_

#include <inc/tm4c123gh6pm.h>
#include <stdbool.h>
#include <stdint.h>
#include "driverlib/interrupt.h"
#include "Timer.h"

// Sets registers necessary for operating ultrasonic sensor
void servo_init(void);

// Sends out 5 us pulse and times length of pulse in to calculate distance from sensor in cm
void servo_move(float degrees);

#endif /* SERVO_H_ */
