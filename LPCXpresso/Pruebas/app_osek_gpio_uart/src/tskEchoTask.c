#include "tskEchoTask.h"

static void* uart;


void tskEchoTask_init(void* _uart)
{
	uart = _uart;

	ActivateTask(EchoTask);
}


/*
 * Espera a recibir el evento de un nuevo mensaje en la UART.
 */
TASK(EchoTask)
{
	uint8_t* buff[10];
	uint32_t* bytesToRead;

	while(1)
	{
		WaitEvent(evRxMsg);
		ClearEvent(evRxMsg);

		// Se recibió un mensaje por la UART. Se lo retransmite.
		GetResource(resUART3);
		bytesToRead = ioComm_dataAvailable(uart);
		ioComm_readBytes(uart, bytesToRead, buff);
		ioComm_writeBytes(uart, bytesToRead, buff);
		ReleaseResource(resUART3);

	}

	TerminateTask();
}
