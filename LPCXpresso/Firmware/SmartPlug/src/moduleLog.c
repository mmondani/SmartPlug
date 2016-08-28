/**
 * @file moduleLog.c
 * @author Mariano Mondani
 *
 * @brief Implementación del módulo moduleLog
 */

#include <stdio.h>

#include "moduleLog.h"
#include "os.h"
#include "cObject.h"
#include "memAlloc.h"
#include "ioUART.h"
#include "rtc_time_t_r.h"

#include "taskRTC.h"


static void* uartLog;
static char strLog[50];



void moduleLog_init (void)
{
	uartLog = cObject_new(ioUART, LPC_UART1, IOUART_BR_115200, IOUART_DATA_8BIT, IOUART_PAR_NONE, IOUART_STOP_1BIT, IOUART_MODE_BLOCKING, 2, 10);
    ioObject_init(uartLog);
    ioComm_intEnable(uartLog, IOUART_INT_TX);
    ioComm_intEnable(uartLog, IOUART_INT_RX);
    ioUART_configFIFO(uartLog, IOUART_FIFO_LEVEL0);

    NVIC_SetPriority(UART1_IRQn, 1);
    NVIC_EnableIRQ(UART1_IRQn);
}


void moduleLog_log (uint8_t* str)
{
	rtc_time_t fullTime;

	taskRTC_getTime(&fullTime);

	sprintf(strLog, "[%.2d:%.2d:%.2d-%.2d/%.2d/%.4d] ", fullTime.hour, fullTime.minute, fullTime.second,
														  fullTime.dayOfMonth, fullTime.month, fullTime.year);

	GetResource(resLog);

	ioUART_writeString(uartLog, strLog);

	ioUART_writeString(uartLog, str);

	ioUART_writeString(uartLog, "\n\r");

	ReleaseResource(resLog);
}


ISR(UART1_handler)
{
	if (ioComm_getInt(uartLog, IOUART_INT_ID_TX))
	{
		ioUART_txHandler(uartLog);
	}
	else if(ioComm_getInt(uartLog, IOUART_INT_ID_RX))
	{
		ioUART_rxHandler(uartLog);
	}
}


