#include "taskSmartPlug.h"
#include "chip.h"
#include "os.h"
#include "cObject.h"
#include "memAlloc.h"
#include "ioDigital.h"


static void* eeprom;
static void* ledVerde;
static void* ledRojo;


void taskSmartPlug_init (void* _eeprom)
{
	eeprom = _eeprom;

	ledVerde = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 0, 21);
    ioObject_init(ledVerde);

    ledRojo = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 2, 13);
    ioObject_init(ledRojo);


	ActivateTask(taskSmartPlug);
}

TASK(taskSmartPlug)
{
	EventMaskType events;

	while(1)
	{
		WaitEvent(evSwitch | evSwitch_5sec);
		GetEvent(taskSmartPlug, &events);
		ClearEvent(events);

		if (events & evSwitch)
		{
			ioDigital_toggle(ledVerde);
		}

		if (events & evSwitch_5sec)
		{
			ioDigital_toggle(ledRojo);
		}
	}

	TerminateTask();
}
