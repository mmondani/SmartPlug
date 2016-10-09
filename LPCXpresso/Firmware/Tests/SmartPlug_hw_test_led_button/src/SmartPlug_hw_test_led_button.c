/**
 * @file SmartPlug_hw_test_led_button.c
 *
 * @brief Prueba de hardware sobre la placa del Smart Plug en su versión 1.
 *
 * Cada vez que se presiona el pulsador se togglean los pines que controlan
 * los leds rojo y verde.
 */


#include "chip.h"

#include <cr_section_macros.h>
#include "memAlloc.h"
#include "cObject.h"
#include "ioDigital.h"
#include "ioDebounce.h"

void* pinGreenLed;
void* pinRedLed;
void* pinSwitch;
void* debounceSwitch;



void SysTick_Handler(void)
{
	ioDebounce_handler(debounceSwitch);
}



int main(void) {


    SystemCoreClockUpdate();

    initMemHeap();

    pinGreenLed = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 0, 21);
    ioObject_init(pinGreenLed);

    pinRedLed = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 2, 13);
    ioObject_init(pinRedLed);

    pinSwitch = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 0, 3);
    ioObject_init(pinSwitch);
    debounceSwitch = cObject_new(ioDebounce, pinSwitch, IODIGITAL_LEVEL_HIGH, 4);


    SysTick_Config(SystemCoreClock / 100);


    /**
     * Los leds arrancan invertidos.
     */
    ioObject_write(pinGreenLed, 1);
    ioObject_write(pinRedLed, 0);

    while(1)
    {
    	/**
    	 * Se produjo un flanco positivo?
    	 */
    	if (ioDebounce_getActiveEdge(debounceSwitch))
    	{
    		ioDigital_toggle(pinGreenLed);
    		ioDigital_toggle(pinRedLed);
    	}
    }


    return 0 ;
}
