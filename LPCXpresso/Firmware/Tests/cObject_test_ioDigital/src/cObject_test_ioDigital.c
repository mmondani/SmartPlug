/*
===============================================================================
 Name        : cObject_test_ioDigital.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>
#include "memAlloc.h"
#include "cObject.h"
#include "ioDigital.h"

void* pinLed1;
void* pinLed2;
void* pinSwitch1;


void SysTick_Handler(void)
{
	ioDigital_toggle(pinLed1);
}

int main(void) {

#if defined (__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
    // Set the LED to the state of "On"
    Board_LED_Set(0, true);
#endif
#endif

    initMemHeap();

    pinLed1 = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 0, 22);
    ioObject_init(pinLed1);

    pinLed2 = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 0, 21);
    ioObject_init(pinLed2);

    pinSwitch1 = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 0, 3);
    ioObject_init(pinSwitch1);

    SysTick_Config(SystemCoreClock / 20);


    while(1)
    {
    	if (ioObject_read(pinSwitch1) == 1)
    		ioObject_write(pinLed2, 1);
    	else
    		ioObject_write(pinLed2, 0);
    }


    return 0 ;
}
