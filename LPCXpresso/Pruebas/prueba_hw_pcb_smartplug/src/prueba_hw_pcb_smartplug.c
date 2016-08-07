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
#include "ioUART.h"

void* pinLedVerde;
void* pinLedRojo;
void* pinSwitch;
void* debounceSwitch;
void* uartDebug;


void SysTick_Handler(void)
{
	ioDebounce_handler(debounceSwitch);
}


void UART1_IRQHandler(void)
{
	if (ioComm_getInt(uartDebug, IOUART_INT_ID_TX))
	{
		ioUART_txHandler(uartDebug);
	}
	else if(ioComm_getInt(uartDebug, IOUART_INT_ID_RX))
	{
		ioUART_rxHandler(uartDebug);
	}
}


int main(void) {
	uint32_t edges = 0;


    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();


    initMemHeap();

    pinLedVerde = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 0, 21);
    ioObject_init(pinLedVerde);

    pinLedRojo = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 2, 13);
    ioObject_init(pinLedRojo);

    pinSwitch = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 0, 3);
    ioObject_init(pinSwitch);

    debounceSwitch = cObject_new(ioDebounce, pinSwitch, IODIGITAL_LEVEL_HIGH, 4);


    uartDebug = cObject_new(ioUART, LPC_UART1, IOUART_BR_9600, IOUART_DATA_8BIT, IOUART_PAR_NONE, IOUART_STOP_1BIT, IOUART_MODE_NON_BLOCKING, 50, 50);
    ioObject_init(uartDebug);
    ioComm_intEnable(uartDebug, IOUART_INT_TX);
    ioComm_intEnable(uartDebug, IOUART_INT_RX);
    ioUART_configFIFO(uartDebug, IOUART_FIFO_LEVEL0);

    NVIC_SetPriority(UART1_IRQn, 1);
    NVIC_EnableIRQ(UART1_IRQn);


    SysTick_Config(SystemCoreClock / 100);

    ioObject_write(pinLedVerde, 1);
    ioObject_write(pinLedRojo, 0);

    while(1)
    {
    	if (ioDebounce_getActiveEdge(debounceSwitch))
    	{
    		ioDigital_toggle(pinLedVerde);
    		ioDigital_toggle(pinLedRojo);

    		if (ioObject_read(pinLedRojo) == 1)
    			ioUART_writeString(uartDebug, "Led Rojo Encendido\n\r");

    		if (ioObject_read(pinLedVerde) == 1)
    		    ioUART_writeString(uartDebug, "Led Verde Encendido\n\r");
    	}
    }


    return 0 ;
}
