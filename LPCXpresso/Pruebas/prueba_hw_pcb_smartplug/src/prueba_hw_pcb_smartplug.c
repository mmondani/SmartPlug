/*
===============================================================================
 Name        : prueba_hw_pcb_smartplug.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/


#include "chip.h"


#include <cr_section_macros.h>
#include <stdio.h>

#include "memAlloc.h"
#include "cObject.h"
#include "ioDigital.h"
#include "ioDebounce.h"
#include "ioUART.h"
#include "ioInternalRTC.h"


void* pinLedVerde;
void* pinLedRojo;
void* pinSwitch;
void* debounceSwitch;
void* uartDebug;
void* rtc;

uint32_t tickCount;


void SysTick_Handler(void)
{
	ioDebounce_handler(debounceSwitch);

	tickCount ++;
	if (tickCount >= 10)
	{
		// Cada 1 seg
		tickCount = 0;

		ioDigital_toggle(pinLedVerde);
		ioDigital_toggle(pinLedRojo);
	}
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
	rtc_time_t fullTime;
	uint8_t buff[50];


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

    rtc = cObject_new(ioInternalRTC, LPC_RTC);
    ioObject_init(rtc);


    fullTime.second = 0;
    fullTime.minute = 50;
    fullTime.hour = 12;
    fullTime.dayOfMonth = 7;
    fullTime.dayOfWeek = 0;			// Domingo
    fullTime.month = 8;
    fullTime.year = 2016;

    ioRTC_setFullTime(rtc, &fullTime);

    ioObject_enable(rtc);


    SysTick_Config(SystemCoreClock / 100);

    ioObject_write(pinLedVerde, 1);
    ioObject_write(pinLedRojo, 0);

    while(1)
    {
    	if (ioDebounce_getActiveEdge(debounceSwitch))
    	{
    		ioRTC_getFullTime(rtc, &fullTime);

    		sprintf(buff, "%.2d:%.2d:%.2d  -  %.2d/%.2d/%.4d  - %.1d\n\r", fullTime.hour, fullTime.minute, fullTime.second,
    																		fullTime.dayOfMonth, fullTime.month, fullTime.year,
																			fullTime.dayOfWeek);
    		ioUART_writeString(uartDebug, buff);
    	}
    }


    return 0 ;
}
