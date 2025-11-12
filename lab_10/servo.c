/**
 * uart.c
 *
 * Contains functions to operate the Servo motor on the CyBot
 *
 * @date November 5, 2025
 * @author Thiago Bedal
 * @author Joseph Vesterby
**/

/* <----------| INCLUDES |----------> */

#include "servo.h"
#include "button.h"
#include "lcd.h"

/* <----------| DEFINES |----------> */

#define BUTTON_DELAY_MICROS 500 // Magic value for faux "button debouncing" implemented in servo_callibrate()
#define SERVO_DELAY_MILLIS 100

volatile int button_num;

/* <----------| IMPLEMENTATIONS |----------> */

void servo_init(void) {
    // TOOD: comment me!

    /* <----------| INIT GPIO |----------> */

    // ENABLE CLOCK TO CORRESPONDING GPIO MODULES
    SYSCTL_RCGCGPIO_R |= 0b0000'0000'0000'0000'0000'0000'0000'0010; // Enable Port B

    // SET DIRECTION
    GPIO_PORTB_DIR_R |= 0b0000'0000'0000'0000'0000'0000'0010'0000; // Set Pin 5 to Output

    // SET TO TIMER MODE
    GPIO_PORTB_AFSEL_R |= 0b0000'0000'0000'0000'0000'0000'0010'0000; // Enable AFSEL for Pin 3

    // CONFIG GPIO FOR SENDING PWM
    GPIO_PORTB_PCTL_R &= ~0x00F00000;
    GPIO_PORTB_PCTL_R |= 0x00700000; // Select TCCP1 function for PB5

    // SET TO DIGITAL MODE
    GPIO_PORTB_DEN_R |= 0b0000'0000'0000'0000'0000'0000'0010'0000;


    /* <----------| INIT COUNTER |----------> */

    // ENABLE CLOCK TIMERS
    SYSCTL_RCGCTIMER_R |= 0b0010; // Timer 1

    // DISABLE TIMERS FOR CONFIG
    TIMER1_CTL_R &= ~0b0001'0000'0000;

    // CONFIG OUTPUT STATE OF OF PWM SIGNAL
    TIMER1_CFG_R &= ~0x3;
    TIMER1_CFG_R |=  0x4; // Set Timer 1 to 16-bit mode

    // CONFIG TIMER TO PWM MODE
    TIMER1_TBMR_R &= ~0b0000'0001'0101; 
    TIMER1_TBMR_R |=  0b0000'0000'1010;

    // DISABLE INVERSION
    TIMER1_CTL_R &= ~0b0100'0000'0000'0000;

    // LOAD INTERVAL VALUES
    TIMER1_TBPR_R &= ~0xFF;
    TIMER1_TBPR_R |=  0x04; // Extend to 24 bit timer
    TIMER1_TBILR_R &= ~0xFFFF;
    TIMER1_TBILR_R |=  0xE200;

    // LOAD MATCH VALUES
    // FIXME: Servo keeps breaking its back because match value is wrong
    TIMER1_TBMATCHR_R &= ~0xFFFF;
    TIMER1_TBMATCHR_R |=  0xA1E6;
    TIMER1_TBPMR_R &= ~0xFF;
    TIMER1_TBPMR_R |=  0x04;

    // RE-ENABLE TIMER POST-CONFIG
    TIMER1_CTL_R |= 0b0001'0000'0000; // Timer 1
}

void servo_callibrate() {
    uint16_t callibrationValue = TIMER1_TBMATCHR_R;

    // Keep reading button state until user presses SW2
    while (button_num != 2) {
        if (button_num == 1) {
            TIMER1_TBMATCHR_R++ ;
            callibrationValue = TIMER1_TBMATCHR_R;
        }
        else if (button_num == 4) {
            TIMER1_TBMATCHR_R--;
            callibrationValue = TIMER1_TBMATCHR_R;
        }

        timer_waitMicros(BUTTON_DELAY_MICROS); // This feels like bad practice, but the efficiency loss here is minimal and it works as good as debouncing
    }

    // Update selected match value to LCD
    lcd_printf("CAL_VAL: %u", callibrationValue);
}

void servo_move(float degrees) {
    uint16_t requestedMatchValue = (int)(((servo_rightBound - servo_leftBound) * degrees) / 180 + servo_leftBound);
    TIMER1_TBMATCHR_R |= requestedMatchValue;
    TIMER1_TBMATCHR_R &= 0xFFFF0000 + requestedMatchValue;

    timer_waitMillis(SERVO_DELAY_MILLIS);
}

void servo_demo(void) {
    int8_t userWantsClockwise = 1;
    uint8_t degrees;

    // Keep reading button state until user presses SW2
    while (1) {
        degrees = (TIMER1_TBMATCHR_R - servo_leftBound) * 180 / (servo_rightBound - servo_leftBound);

        switch (button_num) {
            case 1: servo_move(degrees + (userWantsClockwise)); break;
            case 2: servo_move(degrees + (5 * userWantsClockwise)); break;
            case 3: userWantsClockwise *= -1; break;
            case 4: servo_move((userWantsClockwise == 1 ? 0 : 180)); break;
        }

        timer_waitMicros(BUTTON_DELAY_MICROS);

        // Update selected match value to LCD
        lcd_printf("CAL_VAL: %u\nDEG: %u\n%d", TIMER1_TBMATCHR_R, degrees, userWantsClockwise);
    }

}
