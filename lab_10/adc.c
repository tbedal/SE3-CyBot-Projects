/**
 * adc.c
 *
 * Contains functions to help operate the ADC on the IR sensor of the Cybot
 * 
 * @date October 29, 2025
 * @author Thiago Bedal
 * @author Joseph Vesterby
**/

/* <----------| INCLUDES |----------> */

#include "adc.h"

/* <----------| FUNCTIONS |----------> */

// Private method to continuously poll busywait until ADC completes sampling conversions. Isn't that terrible practice? Yes. Do I care? No!
static void adc_waitForSample(void);

/* <----------| IMPLEMENTATIONS |----------> */

void adc_init(void) {
    /* <----------| MODULE INITIALIZATON |----------> */

    // 1. ENABLE ADC CLOCK
    SYSCTL_RCGCADC_R |= 0b0000'0000'0000'0000'0000'0000'0000'0001; // Enable ADC Module 0

    // 2. ENABLE CLOCK TO CORRESPONDING GPIO MODULES
    SYSCTL_RCGCGPIO_R |= 0b0000'0000'0000'0000'0000'0000'0000'0010; // Enable Port B

    // 3. CONFIGURE GPIO AFSEL FOR ADC INPUT
     GPIO_PORTB_AFSEL_R |= 0b0000'0000'0000'0000'0000'0000'0001'0000; // Force PB4 to use AFSEL

    // 4. CONFIG AIN SIGNALS AS ANALOG INPUTS
    GPIO_PORTB_DEN_R &= 0b1111'1111'1111'1111'1111'1111'1110'1111; // Disable digital functions for PB4

    // 5. DISABLE ANALOG ISOLATION FOR ALL ADC INPUTS
    GPIO_PORTB_AMSEL_R |= 0b0000'0000'0000'0000'0000'0000'0001'0000; // Enable PB4 to use analog function AIN10 and disable isolation

    // 6. CONFIG SAMPLE SEQUENCER PRIORITY
    ADC0_SSPRI_R &= 0b1111'1111'1111'1111'1111'1111'1111'1111; // Keep default values

    /* <----------| SAMPLE SEQUENCER INITIALIZATON |----------> */

    // 1. DISABLE SAMPLE SEQUENCER FOR INITIALIZATON
    ADC0_ACTSS_R &= 0b1111'1111'1111'1111'1111'1111'1111'1110; // Disable Sample Sequencers 0-3

    // 2. CONFIG TRIGGER EVENT FOR SAMPLE SEQUENCER
    ADC0_EMUX_R &= 0b1111'1111'1111'1111'0000'1111'1111'1111; // Set SS0-SS3 to processor-triggered sampling
    ADC0_PSSI_R &= 0b0111'0111'1111'1111'1111'1111'1111'1110; // Disable GSYNC, SYNCWAIT; clear initiation bits for SS0-SS3

    // Skip step 3

    // 4. CONFIG INPUT SOURCE FOR EACH SAMPLE IN THE SAMPLE SEQUENCE
    ADC0_SSMUX3_R &= 0b1111'1111'1111'1111'1111'1111'1111'0000; // Clear first sample input
    ADC0_SSMUX3_R |= 0b0000'0000'0000'0000'0000'0000'0000'1010; // Set first sample input to AIN10

    // 5. CONFIG SAMPLE CONTROL REGISTERS
    ADC0_SSCTL3_R &= 0b1111'1111'1111'1111'1111'1111'1111'0000; // Clear register
    ADC0_SSCTL3_R |= 0b0000'0000'0000'0000'0000'0000'0000'0110; // Set first (and only) sample as END sample, enable interrupts

    // 6. CONFIG INTERRUPTS
    ADC0_IM_R &= 0b1111'1111'1111'0000'1111'1111'1111'0000; // Disable interrupt masks (prevent interrupts from being sent to controller)

    // EXTRA: Enable hardware oversampling (averaging)
    ADC0_SAC_R &= 0b1111'1111'1111'1111'1111'1111'1111'1000; // Clear register
    ADC0_SAC_R |= 0b0000'0000'0000'0000'0000'0000'0000'0100; // Enable 16x hardware oversampling

    // 7. RE-ENABLE SAMPLE SEQUENCERS
    ADC0_ACTSS_R |= 0b0000'0000'0000'0000'0000'0000'0000'1000; // Enable SS3
}

uint16_t adc_read(void) {
    // Inititate sampling on SS3
    ADC0_PSSI_R |= 0b0000'0000'0000'0000'0000'0000'0000'1000;

    // Wait until ADC completes sample conversion
    adc_waitForSample();

    // Return converted value as integer
    return ADC0_SSFIFO3_R & 0b0000'0000'0000'0000'0000'1111'1111'1111;
}

uint8_t adc_calculateIRDistance(uint16_t millivolts) {
    //                DISTANCE_VALUES   =  10    12    14    16    18    20    22    24    26    28    30    32    34    36    38    40    42    44    46    48    50
    const uint16_t IR_DISTANCE_LOOKUP[] = {2784, 2456, 2185, 1983, 1867, 1756, 1663, 1546, 1469, 1414, 1376, 1324, 1281, 1236, 1206, 1169, 1148, 1143, 1098, 1077, 1059};

    uint8_t smallestIndex = 0;
    uint8_t largestIndex = 20;
    uint8_t closestIndex;

    // Edge case for values closer than 9cm or further than 50cm
    if      (millivolts > IR_DISTANCE_LOOKUP[smallestIndex]) { return 10; }
    else if (millivolts < IR_DISTANCE_LOOKUP[largestIndex ]) { return 50; }

    // This gets to the closest larger index of the millivolts
    while (smallestIndex < largestIndex) {
        closestIndex = (largestIndex - smallestIndex) / 2 + smallestIndex;

        if (IR_DISTANCE_LOOKUP[closestIndex] > millivolts) {
            smallestIndex = closestIndex + 1;
        } else {
            largestIndex = closestIndex - 1;
        }
    }

    int distBetween = (IR_DISTANCE_LOOKUP[closestIndex] - IR_DISTANCE_LOOKUP[closestIndex + 1]) / 2;

    // TODO: According to Joe, this is technically not needed. But function works as of right now
    // Forces the closestIndex down if it normally would've been too big
    if (IR_DISTANCE_LOOKUP[closestIndex] < millivolts) {
        closestIndex--;
    }

    //Check if millivolts is closer to closestIndex + 1
    if (IR_DISTANCE_LOOKUP[closestIndex] - distBetween > millivolts) {
        closestIndex++;

    }

    return 10 + (closestIndex * 2);
}

static void adc_waitForSample(void) {
    while (!(ADC0_RIS_R & 0b0000'0000'0000'0000'0000'0000'0000'1000)) {
        // Wait until ADC completes conversion
    }

    // Clear interrupt status
    ADC0_ISC_R |= 0b0000'0000'0000'0000'0000'0000'0000'1000;
}
