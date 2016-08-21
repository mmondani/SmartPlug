#include "taskSmartPlug.h"
#include "chip.h"
#include "os.h"
#include "cObject.h"
#include "memAlloc.h"
#include "ioDigital.h"

#include "taskLeds.h"


static void* eeprom;


void taskSmartPlug_init (void* _eeprom)
{
	eeprom = _eeprom;


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
			taskLeds_blinkLed(LED_ID_GREEN, 250, 500);
			taskLeds_blinkLed(LED_ID_RED, 0, 1);
		}

		if (events & evSwitch_5sec)
		{
			taskLeds_blinkLed(LED_ID_RED, 500, 250);
			taskLeds_blinkLed(LED_ID_GREEN, 0, 1);
		}
	}

	TerminateTask();
}
