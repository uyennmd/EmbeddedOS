#include "uart4.h"
#include "gpio.h"

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart4_init()
{
    register unsigned int r;

	/* Turn off UART4 */
	*UART4_CR = 0x0;

	/* Setup GPIO 8 and 9
	   Set GPIO8 and GPIO9 (in GPFSEL0) to be UART4 which is ALT4 */
	r = *GPFSEL0;
	r &= ~((7 << 24) | (7 << 27));		// Clear the gpio 14 and 15
	r |=    (3 << 24)|(3 << 27);   //Set value 3 (select ALT4: UART4)
	*GPFSEL0 = r;

	/* Enable GPIO 8, 9 */
#ifdef RBP3 //RBP3
	*GPPUD = 0;            //No pull up/down control
	//Toogle clock to flush GPIO setup
	r = 150; while(r--) { asm volatile("nop"); } //waiting 150 cycles
	*GPPUDCLK0 = (1 << 24)|(1 << 27); //enable clock for GPIO 14, 15
	r = 150; while(r--) { asm volatile("nop"); } //waiting 150 cycles
	*GPPUDCLK0 = 0;        // flush GPIO setup

#else //RPB4
	r = *GPIO_PUP_PDN_CNTRL_REG0;
	r &= ~((3 << 16) | (3 << 19)); // Clear Resistor Select for GPIO 8, 9
	*GPIO_PUP_PDN_CNTRL_REG0 = r;
#endif

	/* Mask all interrupts. Clear all the interrupts */
	*UART4_IMSC = 0;

	/* Clear pending interrupts. Clear all the interrupts */
	*UART4_ICR = 0x7FF;

	/* Set integer & fractional part of Baud rate
	Divider = UART_CLOCK/(16 * Baud)            
	Default UART_CLOCK = 48MHz (old firmware it was 3MHz); 
	Integer part register UART4_IBRD  = integer part of Divider 
	Fraction part register UART4_FBRD = (Fractional part * 64) + 0.5 */

	//115200 baud
	*UART4_IBRD = 26;       
	*UART4_FBRD = 3;

	/* Set up the Line Control Register 
	- Enable FIFO 
	- Set length to 8 bit 
	- Defaults for other bits. No parity, 1 stop bit */
	*UART4_LCRH = UART4_LCRH_FEN | UART4_LCRH_WLEN_8BIT;

	/* Enable UART4, receive, and transmit */
	*UART4_CR = 0x301;     // enable Tx, Rx, FIFO 0b0011 0000 0001
}



/**
 * Send a character
 */
void uart4_sendc(unsigned char c) {

    /* Check Flags Register */
	/* And wait until transmitter is not full */
	do {
		asm volatile("nop");
	} while (*UART4_FR & UART4_FR_TXFF);

	/* Write our data byte out to the data register */
	*UART4_DR = c ;
}


/**
 * Receive a character
 */
unsigned char uart4_getc() {
    char c = 0;

    /* Check Flags Register */
    /* Wait until Receiver is not empty
     * (at least one byte data in receive fifo)*/
	do {
		asm volatile("nop");
    } while ( *UART4_FR & UART4_FR_RXFE );

    /* read it and return */
    c = (unsigned char) (*UART4_DR);

    /* convert carriage return to newline */
    return (c == '\r' ? '\n' : c);
}


/**
 * Display a string
 */
void uart4_puts(char *s) {
    while (*s) {
        /* convert newline to carriage return + newline */
        if (*s == '\n')
            uart4_sendc('\r');
        uart4_sendc(*s++);
    }
}
