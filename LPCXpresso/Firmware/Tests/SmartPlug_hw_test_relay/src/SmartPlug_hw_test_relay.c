/**
 * @file SmartPlug_hw_test_relay.c
 *
 * @brief Prueba de hardware sobre la placa del Smart Plug en su versión 1.
 *
 * Cada vez que se presiona el pulsador, se togglea el pin que controla al
 * relay. El relay arranca desenegizado.
 */


#include "chip.h"

#include <cr_section_macros.h>
#include "memAlloc.h"
#include "cObject.h"
#include "ioDigital.h"
#include "ioDebounce.h"

void* pinRelay;
void* pinSwitch;
void* debounceSwitch;



void SysTick_Handler(void)
{
	ioDebounce_handler(debounceSwitch);
}



int main(void) {


    SystemCoreClockUpdate();

    initMemHeap();

    pinRelay = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 2, 7);
    ioObject_init(pinRelay);

    pinSwitch = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 0, 3);
    ioObject_init(pinSwitch);
    debounceSwitch = cObject_new(ioDebounce, pinSwitch, IODIGITAL_LEVEL_HIGH, 4);


    SysTick_Config(SystemCoreClock / 100);


    /**
     *El relay arranca desenergizado.
     */
    ioObject_write(pinRelay, 0);

    while(1)
    {
    	/**
    	 * Se produjo un flanco positivo?
    	 */
    	if (ioDebounce_getActiveEdge(debounceSwitch))
    	{
    		ioDigital_toggle(pinRelay);
    	}
    }


    return 0 ;
}
