/*
===============================================================================
 Name        : cObject_test_cTimer.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/


#include "chip.h"

#include <cr_section_macros.h>
#include "memAlloc.h"
#include "cObject.h"
#include "ioDigital.h"
#include "cTimer.h"

void* pinLed;
void* timer1;


void SysTick_Handler(void)
{
	cTimer_handler();
}

int main(void) {

    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();


    initMemHeap();

    pinLed = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 0, 22);
    ioObject_init(pinLed);

    timer1 = cObject_new (cTimer);
    cTimer_start (timer1, 1000);


    SysTick_Config(SystemCoreClock / 1000);


    while(1)
    {
    	if (cTimer_hasExpired (timer1))
    	{
    		cTimer_restart (timer1);
    		ioDigital_toggle(pinLed);
    	}
    }


    return 0 ;
}
