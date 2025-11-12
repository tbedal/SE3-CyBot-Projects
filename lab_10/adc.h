/**
 * adc.h
 *
 * Contains functions to help operate the ADC on the IR sensor of the Cybot
 * 
 * @date October 29, 2025
 * @author Thiago Bedal
 * @author Joseph Vesterby
**/

#ifndef ADC_H_
#define ADC_H_

#include <inc/tm4c123gh6pm.h>
#include <stdbool.h>
#include <stdint.h>
#include "driverlib/interrupt.h"

// Sets the registers necessary for reading raw IR data through the ADC
void adc_init(void);

// Returns average of 16x hardware oversample ADC conversion from raw IR data
uint16_t adc_read(void);

// Uses hardcoded lookup table to "calculate" object distance from raw IR data
uint8_t adc_calculateIRDistance(uint16_t millivolts);

#endif /* ADC_H_ */
