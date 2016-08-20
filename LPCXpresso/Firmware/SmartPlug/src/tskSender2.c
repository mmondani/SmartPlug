#include "tskSender2.h"

static void* uart;


void tskSender2_init(void* _uart)
{
	uart = _uart;

	ActivateTask(Sender2);
}


/*
 * Espera a que se presione el switch 2 o 3  y envía un mensaje por la UART.
 */
TASK(Sender2)
{
	EventMaskType events;

	while(1)
	{
		WaitEvent(evSw2 | evSw3);
		GetEvent(Sender2, &events);
		ClearEvent(events);

		if (events & evSw2)
		{
			// Se presionó el switch 2
			GetResource(resUART3);
			ioComm_writeBytes(uart, sizeof ("Switch 2 presionado. Sender2\n\r"), "Switch 2 presionado. Sender2\n\r");
			ReleaseResource(resUART3);
		}
		else if(events & evSw3)
		{
			// Se presionó el switch 3
			GetResource(resUART3);
			ioComm_writeBytes(uart, sizeof ("Switch 3 presionado. Sender2\n\r"), "Switch 3 presionado. Sender2\n\r");
			ReleaseResource(resUART3);
		}

	}

	TerminateTask();
}
