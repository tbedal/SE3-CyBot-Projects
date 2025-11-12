/**
 * main.c
 *
 * The main file.
 *
 * @date November 5, 2025
 * @author Thiago Bedal
 * @author Joseph Vesterby
**/

/* <----------| INCLUDES |----------> */

#include "servo.h"
#include "timer.h"
#include "lcd.h"
#include "button.h"

/* <----------| DECLARATIONS |----------> */

volatile int button_num; // Current value of LCD pushbuttons

uint16_t servo_rightBound = 50128;
uint16_t servo_leftBound = 23781;

/* <----------| IMPLEMENTATIONS |----------> */

int main(void)
{
    // Initialize components
    servo_init();
    button_init();
    init_button_interrupts();
    lcd_init();

    // Initiate servo callibration
//    servo_callibrate();

//    servo_move(90.0);
//    timer_waitMillis(1000);

//    float angle = 0.0;
//    for (angle = 0.0; angle < 180.0; angle += 2.0) {
//        servo_move(angle);
//    }

//    while (1) {
//        servo_move(0.0);
//        servo_move(180.0);
//    }



	return 0;
}
