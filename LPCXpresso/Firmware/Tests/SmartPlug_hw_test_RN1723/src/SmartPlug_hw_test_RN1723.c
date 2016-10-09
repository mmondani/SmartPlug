/**
 * @file SmartPlug_hw_test_CS5490.c
 *
 * @brief Prueba de hardware sobre la placa del Smart Plug en su versión 1.
 *
 * Prueba el funcionamiento del módulo WiFi RN1723. Inicializa el módulo
 * y determina si recibe un timeout en alguno de los comandos enviados
 * al módulo. Si no lo recibe, informa por la UART1 "OK".
 */


#include "chip.h"


#include <cr_section_macros.h>
#include <stdio.h>

#include "memAlloc.h"
#include "cObject.h"
#include "ioDigital.h"
#include "ioUART.h"
#include "cTimer.h"
#include "ioRN1723.h"
#include "cQueue.h"

void* uartDebug;
void* refreshTimer;
void* rn1723;
void* gpioResetRN1723;
void* uartRN1723;
void* inBuffer;
void* outBuffer;
void* rn1723InitTimer;




void term_clear (void* uart);
void term_home (void* uart);




void SysTick_Handler(void)
{
	cTimer_handler();

	ioRN1723_handler(rn1723);
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


void UART2_IRQHandler(void)
{
	if (ioComm_getInt(uartRN1723, IOUART_INT_ID_TX))
	{
		ioUART_txHandler(uartRN1723);
	}
	else if(ioComm_getInt(uartRN1723, IOUART_INT_ID_RX))
	{
		ioUART_rxHandler(uartRN1723);
	}
}



int main(void) {
	uint8_t initializing = 0;
	uint8_t timeout = 0;


	SystemCoreClockUpdate();


    initMemHeap();


    // ===================== [UART DEBUG] =====================
    uartDebug = cObject_new(ioUART, LPC_UART1, IOUART_BR_115200, IOUART_DATA_8BIT, IOUART_PAR_NONE, IOUART_STOP_1BIT, IOUART_MODE_BLOCKING, 2, 10);
    ioObject_init(uartDebug);
    ioComm_intEnable(uartDebug, IOUART_INT_TX);
    ioComm_intEnable(uartDebug, IOUART_INT_RX);
    ioUART_configFIFO(uartDebug, IOUART_FIFO_LEVEL0);

    NVIC_SetPriority(UART1_IRQn, 1);
    NVIC_EnableIRQ(UART1_IRQn);
    // =====================================================


    // ===================== [RN1723] =====================
    gpioResetRN1723 = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 2, 2);
    ioObject_init(gpioResetRN1723);


    uartRN1723 = cObject_new(ioUART, LPC_UART2, IOUART_BR_9600, IOUART_DATA_8BIT, IOUART_PAR_NONE, IOUART_STOP_1BIT, IOUART_MODE_NON_BLOCKING, 50, 50);
    ioObject_init(uartRN1723);
    ioComm_intEnable(uartRN1723, IOUART_INT_TX);
    ioComm_intEnable(uartRN1723, IOUART_INT_RX);
    ioUART_configFIFO(uartRN1723, IOUART_FIFO_LEVEL0);

    NVIC_SetPriority(UART2_IRQn, 1);
    NVIC_EnableIRQ(UART2_IRQn);


    inBuffer = cObject_new(cQueue, 10, sizeof(uint8_t));
    outBuffer = cObject_new(cQueue, 10, sizeof(uint8_t));


    rn1723 = cObject_new(ioRN1723, uartRN1723, gpioResetRN1723, inBuffer, outBuffer, IORN1723_INTERVAL_1_EVERY_4_SEC);
    // =====================================================


    // ===================== [SYS TICK] =====================
    SysTick_Config(SystemCoreClock / 1000);
    // =====================================================


    rn1723InitTimer = cObject_new(cTimer);
    cTimer_start(rn1723InitTimer, 1000);




    while(1)
    {
    	/**
    	 * Inicializa el módulo luego de vencido el timer rn1723InitTimer (1 segundo).
    	 * Si no está en estado IDLE, espera otro segundo.
    	 */
    	if (cTimer_hasExpired(rn1723InitTimer))
		{
    		// Intenta inicializar el módulo RN1723
    		if (ioRN1723_isIdle(rn1723))
    		{
    		    ioObject_init(rn1723);
    		    cTimer_stop(rn1723InitTimer);

    		    initializing = 1;
    		}
    		else
    			cTimer_start(rn1723InitTimer, 1000);
		}


    	if (initializing == 1)
    	{
    		if (!ioRN1723_isIdle(rn1723))
    		{
    			if (ioRN1723_timeoutOcurred(rn1723))
    				timeout = 1;
    		}
    		else
    		{
    			/**
    			 * Si llegó al estado IDLE despues de la inicialización sin que
    			 * Ocurra el timeout significa que la comunicación funciona.
    			 */
    			if (timeout == 0)
    			{
    				ioUART_writeString(uartDebug, "OK");
    				initializing = 0;
    			}
    		}

    	}
    }


    return 0 ;
}


void term_clear (void* uart)
{
	ioObject_write(uart, 0x1B);
	ioComm_writeBytes(uart, 3, "[2J");
}


void term_home (void* uart)
{
	ioObject_write(uart, 0x1B);
	ioComm_writeBytes(uart, 2, "[H");
}
