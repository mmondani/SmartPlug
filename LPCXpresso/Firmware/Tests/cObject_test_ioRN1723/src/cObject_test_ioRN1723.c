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
#include "ioDebounce.h"
#include "cTimer.h"

void* uart2;
void* rn1723;
void* gpioReset;
void* outBuffer;
void* inBuffer;
void* gpioConfig;
void* swConfig;
void* gpioWPS;
void* swWPS;
void* gpioLeave;
void* swLeave;
void* gpioOpenTcp;
void* swOpenTcp;
void* gpioCloseTcp;
void* swCloseTcp;
void* gpioSend1Tcp;
void* swSend1Tcp;
void* gpioRefreshTime;
void* swRefreshTime;
void* gpioGetTime;
void* swGetTime;
void* led1;
void* led2;


void SysTick_Handler(void)
{
	ioRN1723_handler(rn1723);

	cTimer_handler();

	ioDebounce_handler(swConfig);
	ioDebounce_handler(swWPS);
	ioDebounce_handler(swLeave);
	ioDebounce_handler(swOpenTcp);
	ioDebounce_handler(swCloseTcp);
	ioDebounce_handler(swSend1Tcp);
	ioDebounce_handler(swRefreshTime);
	ioDebounce_handler(swGetTime);
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
	uint8_t c, index = 0;
	uint8_t buff[50];
	uint8_t nameComing = 0;
	uint32_t horas, minutos, segundos, dia, mes, anio, dSemana;

    // Read clock settings and update SystemCoreClock variable
	SystemCoreClockUpdate();


    initMemHeap();

    gpioReset = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 2, 4);
    ioObject_init(gpioReset);


    uart2 = cObject_new(ioUART, LPC_UART2, IOUART_BR_9600, IOUART_DATA_8BIT, IOUART_PAR_NONE, IOUART_STOP_1BIT, IOUART_MODE_NON_BLOCKING, 50, 50);
    ioObject_init(uart2);
    ioComm_intEnable(uart2, IOUART_INT_TX);
    ioComm_intEnable(uart2, IOUART_INT_RX);
    ioUART_configFIFO(uart2, IOUART_FIFO_LEVEL0);

    NVIC_SetPriority(UART2_IRQn, 1);
    NVIC_EnableIRQ(UART2_IRQn);


    inBuffer = cObject_new(cQueue, 10, sizeof(uint8_t));
    outBuffer = cObject_new(cQueue, 10, sizeof(uint8_t));


    rn1723 = cObject_new(ioRN1723, uart2, gpioReset, inBuffer, outBuffer);


    // Al presionarlo se configura el RN1723
    gpioConfig = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 0,3);
    ioObject_init(gpioConfig);
    swConfig = cObject_new(ioDebounce, gpioConfig, IODIGITAL_LEVEL_HIGH, 300);

    // Al presionarlo se dispara el proceso de WPS en el RN1723
    gpioWPS = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 0,27);
    ioObject_init(gpioConfig);
    swWPS = cObject_new(ioDebounce, gpioWPS, IODIGITAL_LEVEL_HIGH, 300);

    // Al presionarlo se abandona la red a la que está unido el módulo
    gpioLeave = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 0,28);
    ioObject_init(gpioConfig);
    swLeave = cObject_new(ioDebounce, gpioLeave, IODIGITAL_LEVEL_HIGH, 300);

    // Al presionarlo se conecta al socket 192.168.0.163 : 9871
    gpioOpenTcp = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 2,12);
    ioObject_init(gpioOpenTcp);
    swOpenTcp = cObject_new(ioDebounce, gpioOpenTcp, IODIGITAL_LEVEL_HIGH, 300);

    // Al presionarlo cierra la conexión TCP
    gpioCloseTcp = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 2,11);
    ioObject_init(gpioCloseTcp);
    swCloseTcp = cObject_new(ioDebounce, gpioCloseTcp, IODIGITAL_LEVEL_HIGH, 300);

    // Al presionarlo se envía un mensaje por TCP
    gpioSend1Tcp = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 2,6);
    ioObject_init(gpioSend1Tcp);
    swSend1Tcp = cObject_new(ioDebounce, gpioSend1Tcp, IODIGITAL_LEVEL_HIGH, 300);

    // Al presionarlo se obtiene el tiempo actual
    gpioRefreshTime = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 2,7);
    ioObject_init(gpioRefreshTime);
    swRefreshTime = cObject_new(ioDebounce, gpioRefreshTime, IODIGITAL_LEVEL_HIGH, 300);

    // Al presionarlo se parsea el tiempo y la fecha actual
    gpioGetTime = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 2,8);
    ioObject_init(gpioGetTime);
    swGetTime = cObject_new(ioDebounce, gpioGetTime, IODIGITAL_LEVEL_HIGH, 300);


    led1 = cObject_new (ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 0, 21);
    ioObject_init(led1);

    led2 = cObject_new (ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 2, 13);
    ioObject_init(led2);


    // Cada 1 ms
    SysTick_Config(SystemCoreClock / 1000);


    while(1)
    {
    	if (ioDebounce_getActiveEdge(swConfig))
    	{
    		// El módulo debe estar desocupado.
    		if (ioRN1723_isIdle(rn1723))
    			ioObject_init(rn1723);
    	}
    	if (ioDebounce_getActiveEdge(swWPS))
    	{
    		// El módulo debe estar desocupado.
    		if (ioRN1723_isIdle(rn1723))
    			ioRN1723_runWPS(rn1723, 3);
    	}
    	if (ioDebounce_getActiveEdge(swLeave))
		{
			// El módulo debe estar desocupado.
			if (ioRN1723_isIdle(rn1723))
			{
				// El módulo debe estar unido a una red
				if (ioRN1723_isAuthenticated(rn1723))
					ioRN1723_leaveNetwork(rn1723);
			}
		}
    	if (ioDebounce_getActiveEdge(swOpenTcp))
		{
			// El módulo debe estar desconectado de un socket
			if (ioRN1723_isTCPConnected(rn1723) == 0)
				ioRN1723_connectTCP(rn1723, "192.168.0.103", "9871");
		}
    	if (ioDebounce_getActiveEdge(swCloseTcp))
		{
			// El módulo debe estar conectado a un socket
			if (ioRN1723_isTCPConnected(rn1723))
				ioRN1723_disconnectTCP(rn1723);
		}
    	if (ioDebounce_getActiveEdge(swSend1Tcp))
		{
			// El mensaje se va a enviar cuando esté conectado al socket.
    		ioComm_writeBytes(rn1723, 10, "Hola mundo");
		}
    	if (ioDebounce_getActiveEdge(swRefreshTime))
		{
    		ioRN1723_refreshLocalTime(rn1723);
		}
    	if (ioDebounce_getActiveEdge(swGetTime))
		{
    		if (ioRN1723_isTimeValid(rn1723))
			{
				ioRN1723_getTime(rn1723, &horas, &minutos, &segundos);
				ioRN1723_getDate(rn1723, &dia, &mes, &anio, &dSemana);
				horas = horas;
			}
		}


    	if (ioComm_dataAvailable(rn1723))
    	{
    		c = ioObject_read(rn1723);

    		if (c == '1' && nameComing == 0)
    			ioDigital_toggle(led1);
    		else if (c == '2' && nameComing == 0)
    			ioDigital_toggle(led2);

    		if (c == '#')
    		{
      			if (nameComing == 0)
      			{
    				nameComing = 1;
    				index = 0;
      			}
    			else
    			{
    				nameComing = 0;
    				buff[index] = '\0';
    				index = 0;
    				ioRN1723_setDeviceID(rn1723, buff);
    			}
    		}

    		if (c != '#' && nameComing == 1)
    		{
    			buff[index] = c;
    			index ++;
    		}
    	}
    }


    return 0 ;
}
