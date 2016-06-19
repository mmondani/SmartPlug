#include "tskInputManager.h"


static void* sw1;
static void* sw2;
static void* sw3;
static void* debounceSw1;
static void* debounceSw2;
static void* debounceSw3;
static void* uart;


void tskInputManager_init (void* _uart)
{
	uart = _uart;

	sw1 = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 0, 3);
	ioObject_init(sw1);

	sw2 = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 0, 27);
	ioObject_init(sw2);

	sw3 = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 0, 28);
	ioObject_init(sw3);


	debounceSw1 = cObject_new(ioDebounce, sw1, IODIGITAL_LEVEL_HIGH, 4);
	debounceSw2 = cObject_new(ioDebounce, sw2, IODIGITAL_LEVEL_HIGH, 4);
	debounceSw3 = cObject_new(ioDebounce, sw3, IODIGITAL_LEVEL_HIGH, 4);

}




/*
 * Este task se ejecuta periódicamente cada 10 mseg y genera
 * los eventos cuando se presiona un pulsador o llega un mensaje por la UART
 */
TASK(InputManager)
{
	// Se ejecutan los handler de la lógica de anti-rebote
	ioDebounce_handler(debounceSw1);
	ioDebounce_handler(debounceSw2);
	ioDebounce_handler(debounceSw3);

	if (ioDebounce_getActiveEdge(debounceSw1))
		SetEvent(Sender1, evSw1);

	if (ioDebounce_getActiveEdge(debounceSw2))
	{
		SetEvent(Sender1, evSw2);
		SetEvent(Sender2, evSw2);
	}

	if (ioDebounce_getActiveEdge(debounceSw3))
		SetEvent(Sender2, evSw3);

	if (ioComm_dataAvailable(uart))
		SetEvent(EchoTask, evRxMsg);

	TerminateTask();
}
