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
#include "ioDebounce.h"

void* pinLed1;
void* pinLed2;
void* pinSwitch1;
void* debounceSwitch1;


void SysTick_Handler(void)
{
	ioDebounce_handler(debounceSwitch1);
}

int main(void) {
	uint32_t edges = 0;


    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();


    initMemHeap();

    pinLed1 = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 0, 22);
    ioObject_init(pinLed1);

    pinLed2 = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 0, 21);
    ioObject_init(pinLed2);

    pinSwitch1 = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 0, 3);
    ioObject_init(pinSwitch1);

    debounceSwitch1 = cObject_new(ioDebounce, pinSwitch1, IODIGITAL_LEVEL_HIGH, 4);



    SysTick_Config(SystemCoreClock / 100);


    while(1)
    {
    	if (ioDebounce_isActive(debounceSwitch1))
    		ioObject_write(pinLed1, 1);
    	else
    		ioObject_write(pinLed1, 0);



    	if (ioDebounce_getActiveEdge(debounceSwitch1))
    		edges++;

    	// Cuando la cantidad de flancos positivos es múltiplo de 4, prende el led 2
    	if (edges % 4 == 0)
    		ioObject_write(pinLed2, 1);
    	else
    		ioObject_write(pinLed2, 0);
    }


    return 0 ;
}
