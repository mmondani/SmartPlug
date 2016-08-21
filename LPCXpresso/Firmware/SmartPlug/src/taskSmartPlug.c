#include "taskSmartPlug.h"
#include "chip.h"
#include "os.h"
#include "cObject.h"
#include "memAlloc.h"
#include "ioDigital.h"

#include "taskLeds.h"
#include "taskRTC.h"
#include "moduleLog.h"


static void* eeprom;


void taskSmartPlug_init (void* _eeprom)
{


	eeprom = _eeprom;



	ActivateTask(taskSmartPlug);
}

TASK(taskSmartPlug)
{
	rtc_time_t fullTime;
	EventMaskType events;

    fullTime.second = 0;
    fullTime.minute = 0;
    fullTime.hour = 13;
    fullTime.dayOfWeek = 0;			// Domingo
    fullTime.dayOfMonth = 21;
    fullTime.month = 8;
    fullTime.year = 2016;
	taskRTC_setTime(&fullTime);

	while(1)
	{
		WaitEvent(evSwitch | evSwitch_5sec | evRTC_1min | evRTC_1hour | evRTC_1day);
		GetEvent(taskSmartPlug, &events);
		ClearEvent(events);

		if (events & evSwitch)
		{
			taskLeds_blinkLed(LED_ID_GREEN, 250, 500);
			taskLeds_blinkLed(LED_ID_RED, 0, 1);
			moduleLog_log("evSwitch");
		}

		if (events & evSwitch_5sec)
		{
			taskLeds_blinkLed(LED_ID_RED, 500, 250);
			taskLeds_blinkLed(LED_ID_GREEN, 0, 1);
			moduleLog_log("evSwitch_5sec");
		}

		if (events & evRTC_1min)
		{
			taskLeds_blinkLed(LED_ID_RED, 500, 250);
			taskLeds_blinkLed(LED_ID_GREEN, 0, 1);
			moduleLog_log("evRTC_1min");
		}

		if (events & evRTC_1hour)
		{
			moduleLog_log("evRTC_1hour");
		}

		if (events & evRTC_1day)
		{
			moduleLog_log("evRTC_1day");
		}
	}

	TerminateTask();
}
