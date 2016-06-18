#include "tskSender1.h"

static void* uart;


void tskSender1_init(void* _uart)
{
	uart = _uart;

	ActivateTask(Sender1);
}


/*
 * Espera a que se presione el switch 1 o 2  y envía un mensaje por la UART.
 */
TASK(Sender1)
{
	EventMaskType events;

	while(1)
	{
		WaitEvent(evSw1 | evSw2);
		GetEvent(Sender1, &events);
		ClearEvent(events);

		if (events & evSw1)
		{
			// Se presionó el switch 1
			GetResource(resUART3);
			ioComm_writeBytes(uart, sizeof ("Switch 1 presionado. Sender1\n\r"), "Switch 1 presionado. Sender1\n\r");
			ReleaseResource(resUART3);
		}
		else if(events & evSw2)
		{
			// Se presionó el switch 2
			GetResource(resUART3);
			ioComm_writeBytes(uart, sizeof ("Switch 2 presionado. Sender1\n\r"), "Switch 2 presionado. Sender1\n\r");
			ReleaseResource(resUART3);
		}

	}

	TerminateTask();
}
