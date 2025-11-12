/**
 * uart.c
 *
 * Contains functions to communicate with the CyBot via ADC
 * 
 * @date October 29, 2025
 * @author Thiago Bedal
 * @author Joseph Vesterby
**/

/* <----------| INCLUDES |----------> */

#include "uart.h"

/* <----------| DEFINITIONS |----------> */

// TODO: do we need these here?
extern volatile char uart_data;
extern volatile char flag;

/* <----------| IMPLEMENTATIONS |----------> */

void uart_init(int baud) {
    SYSCTL_RCGCGPIO_R |= 0b10;      // enable clock GPIOB (page 340)
    SYSCTL_RCGCUART_R |= 0b10;      // enable clock UART1 (page 344)
    GPIO_PORTB_AFSEL_R = 0b11;      // sets PB0 and PB1 as peripherals (page 671)
    GPIO_PORTB_PCTL_R  = 0x11;       // pmc0 and pmc1       (page 688)  also refer to page 650
    GPIO_PORTB_DEN_R   = 0b11;        // enables pb0 and pb1
    GPIO_PORTB_DIR_R   = 0b10;        // sets pb0 as output, pb1 as input TODO: workS??????

    // Compute baud values [UART clock= 16 MHz]
    double fbrd;
    int ibrd;

    fbrd = 16000000.0 / (16 * baud); // page 903
    ibrd = (int)fbrd;
    fbrd = fbrd - ibrd;

    UART1_CTL_R &= 0xFFFFFFFE;      // disable UART1 (page 918)
    UART1_IBRD_R |= ibrd;        // write integer portion of BRD to IBRD
    UART1_FBRD_R |= (int)(fbrd * 64 + 0.5);   // write fractional portion of BRD to FBRD
    UART1_LCRH_R = 0b0110'0000;        // write serial communication parameters (page 916) * 8bit and no parity
    UART1_CC_R   = 0x0;          // use system clock as clock source (page 939)
    UART1_CTL_R |= 0000'0001;        // enable UART1
}

void uart_sendChar(char data) {
    uint32_t original_UART_DR = UART1_DR_R;

    original_UART_DR &= 0xFFFFFF00;
    original_UART_DR |= data;

    UART1_DR_R = original_UART_DR;

    while (UART1_FR_R & 0b0000'1000) {
        // Wait
    }
}

char uart_getChar(void) {
    while (UART1_FR_R & 0b0001'0000) {
        // Wait
    }

    char recievedChar = (char)UART1_DR_R;

    return recievedChar;
}

void uart_sendStr(const char *data) {
    unsigned char cursor = 0;

    while (data[cursor]) {
        uart_sendChar(data[cursor++]);
    }
}


void uart_interruptInit() {
    // Enable interrupts for receiving bytes through UART1
    UART1_IM_R |= 0b0001'0000; //enable interrupt on receive - page 924

    // Find the NVIC enable register and bit responsible for UART1 in table 2-9
    // Note: NVIC register descriptions are found in chapter 3.4
    NVIC_EN0_R |= 0b100'0000; //enable uart1 interrupts - page 104

    // Find the vector number of UART1 in table 2-9 ! UART1 is 22 from vector number page 104
    IntRegister(INT_UART1, uart_interruptHandler); //give the microcontroller the address of our interrupt handler - page 104 22 is the vector number
}

void uart_interruptHandler() {
    // STEP 1: Check the Masked Interrupt Status
    if (!(UART1_MIS_R & 0b0001'0000)) {
        return;
    }

    // STEP 2: Copy the data
    uart_data = uart_data = uart_getChar();
    flag = 1;

    // STEP 3: Clear the interrupt
    UART1_ICR_R |= 0b0001'0000;


}
