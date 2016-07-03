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
#include "ioRN1723.h"

void* uart2;
void* rn1723;
void* gpioIP;
void* gpioTCP;
void* outBuffer;
void* inBuffer;


void SysTick_Handler(void)
{
	//ioComm_writeBytes(uart3, sizeof ("Hola mundo\n\r"), str1);
	ioRN1723_handler(rn1723);
}


void UART2_IRQHandler(void)
{
	if (ioComm_getInt(uart2, IOUART_INT_ID_TX))
	{
		ioUART_txHandler(uart2);
	}
	else if(ioComm_getInt(uart2, IOUART_INT_ID_RX))
	{
		ioUART_rxHandler(uart2);
	}
}

int main(void)
{
	uint8_t bytes, bytesRead;
	uint8_t buff[50];

    // Read clock settings and update SystemCoreClock variable
	SystemCoreClockUpdate();


    initMemHeap();

    gpioIP = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 2, 4);
    ioObject_init(gpioIP);
    gpioTCP = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 2, 5);
    ioObject_init(gpioTCP);


    uart2 = cObject_new(ioUART, LPC_UART2, IOUART_BR_9600, IOUART_DATA_8BIT, IOUART_PAR_NONE, IOUART_STOP_1BIT, IOUART_MODE_NON_BLOCKING, 50, 50);
    ioObject_init(uart2);
    ioComm_intEnable(uart2, IOUART_INT_TX);
    ioComm_intEnable(uart2, IOUART_INT_RX);
    ioUART_configFIFO(uart2, IOUART_FIFO_LEVEL0);

    NVIC_SetPriority(UART2_IRQn, 1);
    NVIC_EnableIRQ(UART2_IRQn);


    inBuffer = cObject_new(cQueue, 10, sizeof(uint8_t));
    outBuffer = cObject_new(cQueue, 10, sizeof(uint8_t));


    rn1723 = cObject_new(ioRN1723, uart2, gpioIP, gpioTCP, inBuffer, outBuffer);
    ioObject_init(rn1723);

    SysTick_Config(SystemCoreClock / 10);


    while(1)
    {

    }


    return 0 ;
}
