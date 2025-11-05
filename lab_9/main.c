/**
 * main.c
 */

#include "ping.h"
#include "Timer.h"
#include "lcd.h"
#include <stdio.h>

int main(void)
{
    ping_init();

    while (1) {
        printf("%f\n", ping_read());
        timer_waitMillis(500);
    }
}
