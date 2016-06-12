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
#include "ioUART.h"

void* uart3;
const uint8_t* str1 = "Hola mundo!\r\n";


void SysTick_Handler(void)
{
	//ioComm_writeBytes(uart3, sizeof ("Hola mundo\n\r"), str1);
}


void UART3_IRQHandler(void)
{
	if (ioComm_getInt(uart3, IOUART_INT_ID_TX))
	{
		ioUART_txHandler(uart3);
	}
	else if(ioComm_getInt(uart3, IOUART_INT_ID_RX))
	{
		ioUART_rxHandler(uart3);
	}
}

int main(void)
{
	uint8_t bytes, bytesRead;
	uint8_t buff[50];

    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();


    initMemHeap();

    uart3 = cObject_new(ioUART, LPC_UART3, IOUART_BR_9600, IOUART_DATA_8BIT, IOUART_PAR_NONE, IOUART_STOP_1BIT, IOUART_MODE_NON_BLOCKING, 50, 50);
    ioObject_init(uart3);
    ioComm_intEnable(uart3, IOUART_INT_TX);
    ioComm_intEnable(uart3, IOUART_INT_RX);
    ioUART_configFIFO(uart3, IOUART_FIFO_LEVEL0);

    NVIC_SetPriority(UART3_IRQn, 1);
    NVIC_EnableIRQ(UART3_IRQn);


    SysTick_Config(SystemCoreClock / 10);


    while(1)
    {
    	bytes = ioComm_dataAvailable(uart3);
    	if(bytes)
    	{
    		bytesRead = ioComm_readBytes(uart3, bytes, buff);
    		ioComm_writeBytes(uart3, bytesRead, buff);
    	}
    }


    return 0 ;
}
