/*
===============================================================================
 Name        : osek_gpio_uart.c
 Author      : Mariano Mondani
 Version     :
 Copyright   : (c)2016, Mariano Mondani
 Description : Existen 3 tareas:
 	 - InputManager: se encarga de muestrear 3 switches y generar los eventos
 	 	 	 	 	 cuando se los presiona.
 	 - Sender1: cuando se presiona el switch 1 o 2 se envía un mensaje por
 	 	 	 	la UART informándolo.
 	 - Sender2: cuando se presiona el switch 2 o 3 se envía un mensaje por
 	 	 	 	la UART informándolo.
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <stdio.h>
#include <cr_section_macros.h>

#include "os.h"               /* <= operating system header */

#include "memAlloc.h"
#include "cObject.h"
#include "ioDebounce.h"
#include "ioUART.h"

#include "tskInputManager.h"
#include "tskSender1.h"
#include "tskSender2.h"



void* uart;



/*
 * See the OIL configuration file in
 * etc/config.oil
 */

/* Comment this line in order to use LPC-Link based printf()
 * If uncommented, all printf() calls will be disabled
 */
//#define printf(...)

int main(void)
{
	// Lee las configuraciones del clock y actualiza la variable SystemCoreClock
	SystemCoreClockUpdate();
	initMemHeap();


	// Comienza el RTOS en el modo AppMode1
    StartOS(AppMode1);

    // No debe llegar hasta acá
    while(1);

    return 0 ;
}


void ErrorHook(void)
{
	/* kernel panic :( */
	ShutdownOS(0);
}

TASK(TaskInit)
{
	uart = cObject_new(ioUART, LPC_UART3, IOUART_BR_9600, IOUART_DATA_8BIT, IOUART_PAR_NONE, IOUART_STOP_1BIT, IOUART_MODE_NON_BLOCKING, 100, 10);
    ioObject_init(uart);
    ioComm_intEnable(uart, IOUART_INT_TX);
    //ioComm_intEnable(uart3, IOUART_INT_RX);
    ioUART_configFIFO(uart, IOUART_FIFO_LEVEL0);

    NVIC_SetPriority(UART3_IRQn, 1);
    NVIC_EnableIRQ(UART3_IRQn);



	tskInputManager_init();
	tskSender1_init(uart);
	tskSender2_init(uart);


	// Cada 10 ms se va llamar a InpurManager para que muestree los switches.
	SetRelAlarm(ActivateInputManager, 100, 10);


	TerminateTask();
}


ISR(UART3_handler)
{
	if (ioComm_getInt(uart, IOUART_INT_ID_TX))
	{
		ioUART_txHandler(uart);
	}
	/*else if(ioComm_getInt(uart3, IOUART_INT_ID_RX))
	{
		ioUART_rxHandler(uart3);
	}*/
}

