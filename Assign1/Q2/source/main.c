// -----------------------------------main.c -------------------------------------

#include "uart5.h"

void main()
{
    // set up serial console
    uart5_init();

    // say hello
    uart5_puts("Hello uart5!\n");

    // echo everything back
    while(1) {
        //read each char
        char c = uart5_getc();
        //send back
        uart5_sendc(c);
    }
}