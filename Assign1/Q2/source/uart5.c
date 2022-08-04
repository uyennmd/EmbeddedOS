#include "uart5.h"
#include "gpio.h"

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart5_init()
{
    register unsigned int r;

	/* Turn off UART5 */
	*UART5_CR = 0x0;

	/* Setup GPIO 12 and 13
	   Set GPIO12 and GPIO13 (in GPFSEL1) to be UART5 which is ALT4 */
	r = *GPFSEL1;
	r &= ~((7 << 6) | (7 << 9));		// Clear the gpio 12 and 13
	r |=    (3 << 6)|(3 << 9);   //Set value 3 (select ALT4: UART5)
	*GPFSEL1 = r;

	/* Enable GPIO 12, 13 */
#ifdef RBP3 //RBP3
	*GPPUD = 0;            //No pull up/down control
	//Toogle clock to flush GPIO setup
	r = 150; while(r--) { asm volatile("nop"); } //waiting 150 cycles
	*GPPUDCLK0 = (1 << 12)|(1 << 13); //enable clock for GPIO 12, 13
	r = 150; while(r--) { asm volatile("nop"); } //waiting 150 cycles
	*GPPUDCLK0 = 0;        // flush GPIO setup

#else //RPB4
	r = *GPIO_PUP_PDN_CNTRL_REG0;
	r &= ~((3 << 24) | (3 << 26)); // Clear Resistor Select for GPIO 12, 13
	*GPIO_PUP_PDN_CNTRL_REG0 = r;
#endif

	/* Mask all interrupts. Clear all the interrupts */
	*UART5_IMSC = 0;

	/* Clear pending interrupts. Clear all the interrupts */
	*UART5_ICR = 0x7FF;

	/* Set integer & fractional part of Baud rate
	Divider = UART_CLOCK/(16 * Baud)            
	Default UART_CLOCK = 48MHz (old firmware it was 3MHz); 
	Integer part register UART5_IBRD  = integer part of Divider 
	Fraction part register UART5_FBRD = (Fractional part * 64) + 0.5 */

	//57600 baud
	*UART5_IBRD = 52;       
	*UART5_FBRD = 5;

	/* Set up the Line Control Register 
	- Enable FIFO 
	- Set length to 8 bit 
	- Defaults for other bits. No parity, 1 stop bit */
	*UART5_LCRH = UART5_LCRH_FEN | UART5_LCRH_WLEN_8BIT;

	/* Enable UART5, receive, and transmit */
	*UART5_CR = 0x301;     // enable Tx, Rx, FIFO 0b0011 0000 0001
}



/**
 * Send a character
 */
void uart5_sendc(unsigned char c) {

    /* Check Flags Register */
	/* And wait until transmitter is not full */
	do {
		asm volatile("nop");
	} while (*UART5_FR & UART5_FR_TXFF);

	/* Write our data byte out to the data register */
	*UART5_DR = c ;
}


/**
 * Receive a character
 */
unsigned char uart5_getc() {
    char c = 0;

    /* Check Flags Register */
    /* Wait until Receiver is not empty
     * (at least one byte data in receive fifo)*/
	do {
		asm volatile("nop");
    } while ( *UART5_FR & UART5_FR_RXFE );

    /* read it and return */
    c = (unsigned char) (*UART5_DR);

    /* convert carriage return to newline */
    return (c == '\r' ? '\n' : c);
}


/**
 * Display a string
 */
void uart5_puts(char *s) {
    while (*s) {
        /* convert newline to carriage return + newline */
        if (*s == '\n')
            uart5_sendc('\r');
        uart5_sendc(*s++);
    }
}
