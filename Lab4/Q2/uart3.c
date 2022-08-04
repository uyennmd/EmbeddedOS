#include "uart3.h"
#include "gpio.h"

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart3_init()
{
    register unsigned int r;

	/* Turn off UART3 */
	*UART3_CR = 0x0;

	/* Setup GPIO 4 and 5 
	   Set GPIO4 and GPIO5 (in GPFSEL0) to be UART3 which is ALT4 */
	r = *GPFSEL0;
	r &= ~((7 << 12) | (7 << 15));		// Clear the gpio 4 and 5
	r |=    (3 << 12)|(3 << 15);   //Set value 3 (select ALT4: UART3)
	*GPFSEL0 = r;

	/* Enable GPIO 4, 5 */
#ifdef RBP3 //RBP3
	*GPPUD = 0;            //No pull up/down control
	//Toogle clock to flush GPIO setup
	r = 150; while(r--) { asm volatile("nop"); } //waiting 150 cycles
	*GPPUDCLK0 = (1 << 14)|(1 << 15); //enable clock for GPIO 14, 15
	r = 150; while(r--) { asm volatile("nop"); } //waiting 150 cycles
	*GPPUDCLK0 = 0;        // flush GPIO setup

#else //RPB4
	r = *GPIO_PUP_PDN_CNTRL_REG0;
	r &= ~((3 << 8) | (3 << 10)); // Clear Resistor Select for GPIO 04, 05
	*GPIO_PUP_PDN_CNTRL_REG0 = r;
#endif

	/* Mask all interrupts. Clear all the interrupts */
	*UART3_IMSC = 0;

	/* Clear pending interrupts. Clear all the interrupts */
	*UART3_ICR = 0x7FF;

	/* Set integer & fractional part of Baud rate
	Divider = UART_CLOCK/(16 * Baud)            
	Default UART_CLOCK = 48MHz (old firmware it was 3MHz); 
	Integer part register UART3_IBRD  = integer part of Divider 
	Fraction part register UART3_FBRD = (Fractional part * 64) + 0.5 */

	//115200 baud
	*UART3_IBRD = 26;       
	*UART3_FBRD = 3;

	/* Set up the Line Control Register 
	- Enable FIFO 
	- Set length to 8 bit 
	- Defaults for other bits. No parity, 1 stop bit */
	*UART3_LCRH = UART3_LCRH_FEN | UART3_LCRH_WLEN_8BIT;

	/* Enable UART3, receive, and transmit */
	*UART3_CR = 0x301;     // enable Tx, Rx, FIFO 0b0011 0000 0001
}



/**
 * Send a character
 */
void uart3_sendc(unsigned char c) {

    /* Check Flags Register */
	/* And wait until transmitter is not full */
	do {
		asm volatile("nop");
	} while (*UART3_FR & UART3_FR_TXFF);

	/* Write our data byte out to the data register */
	*UART3_DR = c ;
}


/**
 * Receive a character
 */
unsigned char uart3_getc() {
    char c = 0;

    /* Check Flags Register */
    /* Wait until Receiver is not empty
     * (at least one byte data in receive fifo)*/
	do {
		asm volatile("nop");
    } while ( *UART3_FR & UART3_FR_RXFE );

    /* read it and return */
    c = (unsigned char) (*UART3_DR);

    /* convert carriage return to newline */
    return (c == '\r' ? '\n' : c);
}


/**
 * Display a string
 */
void uart3_puts(char *s) {
    while (*s) {
        /* convert newline to carriage return + newline */
        if (*s == '\n')
            uart3_sendc('\r');
        uart3_sendc(*s++);
    }
}
