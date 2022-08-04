#include "uart4.h"

void main()
{
    // set up serial console
    uart4_init();
    
    // say hello
    uart4_puts("Hello World! This is uart4 !!!\n");

    // echo everything back
    while(1) {
    	//read each char
    	char c = uart4_getc();

    	//send back twice
    	uart4_sendc(c);
    	uart4_sendc(c);
    }
}
