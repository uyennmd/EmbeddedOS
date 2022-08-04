#include "uart3.h"

void main()
{
    // set up serial console
    uart3_init();
    
    // say hello
    uart3_puts("Hello World! This is UART3 !!!\n");

    // echo everything back
    while(1) {
    	//read each char
    	char c = uart3_getc();

    	//send back twice
    	uart3_sendc(c);
    	uart3_sendc(c);
    }
}
