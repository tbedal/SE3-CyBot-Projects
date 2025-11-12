/**
 * uart.c
 *
 * Contains functions to operate the PING)) ultrasonic sensory on the CyBot.
 * 
 * @date November 5, 2025
 * @author Thiago Bedal
 * @author Joseph Vesterby
**/

/* <----------| INCLUDES |----------> */

#include "ping.h"

/* <----------| DEFINES |----------> */

volatile uint32_t positiveEdgeTime; // When sensor gives out pulse
volatile uint32_t negativeEdgeTime; // When sensor recieves pulse
volatile uint8_t doneFlag = 0; // 1 after end of pulse
volatile uint8_t firstFlag = 0; // Stores which pulse we're on so timer ISR updates proper time (ONLY FOR ISR)

// ISR for caputring PING)) pulse uisng Timer 3B 
static void ping_timerHandler(void);

/* <----------| IMPLEMENTATIONS |----------> */

void ping_init(void) {
    // TOOD: comment me!

    /* <----------| INIT GPIO |----------> */

    // ENABLE CLOCK TO CORRESPONDING GPIO MODULES
    SYSCTL_RCGCGPIO_R |= 0b0000'0000'0000'0000'0000'0000'0000'0010; // Enable Port B

    // SET DIRECTION
    GPIO_PORTB_DIR_R |= 0b0000'0000'0000'0000'0000'0000'0000'1000; // Set Pin 3 to Output

    // SET TO GPIO MODE
    GPIO_PORTB_AFSEL_R &= 0b1111'1111'1111'1111'1111'1111'1111'0111; // Disable AFSEL for Pin 3

    // CONFIGURE GPIO FOR RECEIVING AFTER SWITCH
    GPIO_PORTB_PCTL_R &= ~0x0000F000;
    GPIO_PORTB_PCTL_R |= 0x00007000;

    // SET TO DIGITAL MODE
    GPIO_PORTB_DEN_R |= 0b0000'0000'0000'0000'0000'0000'0000'1000;


    /* <----------| INIT COUNTER |----------> */

    SYSCTL_RCGCTIMER_R |= 0b00'1000;

    TIMER3_CTL_R &= ~0b1'0000'0000;

    TIMER3_CFG_R &= ~0x3;
    TIMER3_CFG_R |= 0x4;

    TIMER3_TBMR_R |= 0b0000'0000'0111;
    TIMER3_TBMR_R &= ~0b0000'0001'0000;

    TIMER3_CTL_R |= 0b000'1100'0000'0000;

    TIMER3_TBPR_R = 0xFF; // Extends it to 24 bit timer
    TIMER3_TBILR_R = 0xFFFF;

    TIMER3_IMR_R &= ~0b1'0000'1111'0001'1111;

    /* <----------| CONFIG INTERRUPTS |----------> */

    IntMasterEnable();
    NVIC_EN1_R |= (0b1 << (36 - 32));
    IntRegister(INT_TIMER3B, ping_timerHandler);

    TIMER3_CTL_R |= 0b000'0001'0000'0000;
}

double ping_read(void) {
    double distanceCM = 0;

    /* <----------| PULSE OUT |----------> */

    // MASK TIMER INTERRUPT
    TIMER3_IMR_R &= ~0b0100'0000'0000;

    // SET TO OUTPUT
    GPIO_PORTB_DIR_R |= 0b0000'0000'0000'0000'0000'0000'0000'1000;
    // SET TO GPIO MODE
    GPIO_PORTB_AFSEL_R &= 0b1111'1111'1111'1111'1111'1111'1111'0111;


    // MAKE THE PULSE
    GPIO_PORTB_DATA_R |= 0b0000'0000'0000'0000'0000'0000'0000'1000;
    timer_waitMicros(5);
    GPIO_PORTB_DATA_R &= ~0b0000'0000'0000'0000'0000'0000'0000'1000;
    
    /* <----------| PULSE IN |----------> */

    // SET TO INPUT
    GPIO_PORTB_DIR_R &= ~0b0000'1000;

    // SET TO TIMER MODE
    GPIO_PORTB_AFSEL_R |= 0b0000'1000;

    // CLEAR INTERRUPTS
    TIMER3_ICR_R |= 0b1'0000'1111'0001'1111;

    // UNMASK TIMER INTERRUPT
    TIMER3_IMR_R |= 0b0100'0000'0000;

    // WAIT UNTIL SIGNAL DONE READING
    while (!doneFlag) {}

    /* <----------| CALCULATE DISTANCE |----------> */

    // Multiply by periods to calculate time
    if (positiveEdgeTime < negativeEdgeTime) {
        positiveEdgeTime += 0xFFFFFF;
    }

    // Calclates distance there and back using the speed of sound in cm/s ( /2 because we only care about the distance there)
    distanceCM = (positiveEdgeTime - negativeEdgeTime) * 0.0000000625;
    distanceCM *= (34300.0 / 2.0);

    // Reset flags
    doneFlag = 0;

    return distanceCM;
}

static void ping_timerHandler(void) {
    // TODO: comment me!

    if (!(TIMER3_MIS_R & 0b0'0100'0000'0000)) {
        TIMER3_ICR_R |= 0b1'0000'1111'0001'1111;
        return;
    }

    if (!firstFlag) {
        positiveEdgeTime = TIMER3_TBR_R;
        firstFlag = 1;
    } else {
        negativeEdgeTime = TIMER3_TBR_R;
        firstFlag = 0;
        doneFlag = 1;
    }

    TIMER3_ICR_R |= 0b0'0100'0000'0000;
}
