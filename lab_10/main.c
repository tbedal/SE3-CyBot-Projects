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

/* <----------| IMPLEMENTATIONS |----------> */

int main(void)
{
    // Initialize components
    servo_init();
    button_init();
    init_button_interrupts();
    lcd_init();

    // Initiate servo callibration
    servo_callibrate();

	return 0;
}
