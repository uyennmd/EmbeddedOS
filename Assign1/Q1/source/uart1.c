// -----------------------------------uart.c -------------------------------------

#include "uart1.h"
#include "gpio.h"

/**
* Set baud rate and characteristics (115200 8N1) and map to GPIO
*/
void uart_init()
{
    register unsigned int r;

    /* initialize UART */
    *AUX_ENABLE |= 1; //enable UART1, AUX mini uart (AUXENB)
    *AUX_MU_CNTL = 0; //stop transmitter and receiver
    *AUX_MU_LCR = 3; //8-bit mode (also enable bit 1 to be used for RBP3)
    *AUX_MU_MCR = 0; //RTS (request to send)
    *AUX_MU_IER = 0; //disable interrupts
    *AUX_MU_IIR = 0xc6; //clear FIFOs
    *AUX_MU_BAUD = 541; // 57600 baud (system_clk_freq/(baud_rate*8) - 1)
    
    /* map UART1 to GPIO pins */
    r = *GPFSEL1;
    r &= ~( (7 << 12)|(7 << 15) ); //Clear bits 12-17 (gpio14, gpio15)
    r |= (2 << 12)|(2 << 15); //Set value 2 (select ALT5: UART1)
    *GPFSEL1 = r;
    
    /* enable GPIO 14, 15 */
    *GPPUD = 0; //No pull up/down control
    r = 150; while(r--) { asm volatile("nop"); } //waiting 150 cycles
    *GPPUDCLK0 = (1 << 14)|(1 << 15); //enable clock for GPIO 14, 15
    r = 150; while(r--) { asm volatile("nop"); } //waiting 150 cycles
    *GPPUDCLK0 = 0; // flush GPIO setup
    *AUX_MU_CNTL = 3; //Enable transmitter and receiver (Tx, Rx)
}
/**
* Send a character
*/
void uart_sendc(unsigned char c) {
    /* wait until transmitter is empty */
    do {
        asm volatile("nop");
    } while ( !(*AUX_MU_LSR & 0x20) );
    
    /* write the character to the buffer */
    *AUX_MU_IO = c;
}

/**
* Receive a character
*/

char uart_getc() {
    char c;

    /* wait until data is ready (one symbol) */
    do {
        asm volatile("nop");
    } while ( !(*AUX_MU_LSR & 0x01) );
    
    /* read it and return */
    c = (char)(*AUX_MU_IO);
    
    /* convert carriage return to newline */
    return (c == '\r' ? '\n' : c);
}
/**
* Display a string
*/
void uart_puts(char *s) {
    while (*s) {
        /* convert newline to carriage return + newline */
        if (*s == '\n')
            uart_sendc('\r');
        uart_sendc(*s++);
    }
}