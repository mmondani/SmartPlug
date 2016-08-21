/**
 * @file taskLeds.c
 * @author Mariano Mondani
 *
 * @brief Implementación de la tarea taskTimer
 */


#include "taskTimer.h"
#include "os.h"
#include "chip.h"
#include "cObject.h"
#include "memAlloc.h"
#include "ioInternalRTC.h"


static void* rtc;

static uint32_t synchronized = 0;			// Se pone en uno cuando se carga un tiempo con setTime.
static uint32_t timeChanged = 0;			// Se pone en uno cuando se carga un tiempo con setTime.
static uint32_t previousMinutes = 0;
static uint32_t previousHours = 0;
static uint32_t previousDay = 0;



void taskRTC_init (void)
{
	rtc_time_t fullTime;

	// Se crea la instancia del driver del RTC interno del LPC176x
    rtc = cObject_new(ioInternalRTC, LPC_RTC);

    // Se configura una fecha default. Va a ser modificada cuando el Smart Plug obtenga la fecha de un SNTP.
    fullTime.second = 0;
    fullTime.minute = 0;
    fullTime.hour = 12;
    fullTime.dayOfWeek = 0;			// Domingo
    fullTime.dayOfMonth = 21;
    fullTime.month = 8;
    fullTime.year = 2016;

    ioRTC_setFullTime(rtc, &fullTime);

    // Se habilita el periférico
    ioObject_init(rtc);
    ioObject_enable(rtc);


    timeChanged = 1;		// Se fuerza a cargar los valores actuales de minutos, horas y día en las variables *previous.
    synchronized = 0;


    // La tarea taskRTC se va a ejecutar cada 200 ms.
    SetRelAlarm(alarmRunTaskRTC, 200, 100);
}


void taskRTC_setTime (rtc_time_t* fullTime)
{
	GetResource(resRTC);

	ioRTC_setFullTime(rtc, fullTime);
	synchronized = 1;
	timeChanged = 1;

	ReleaseResource(resRTC);
}


void taskRTC_getTime (rtc_time_t* fullTime)
{
	if (synchronized == 1)
	{
		GetResource(resRTC);

		ioRTC_getFullTime(rtc, fullTime);

		ReleaseResource(resRTC);
	}
	else
	{
		// Si no está sincronizado se devuelve una fecha imposible.
	    fullTime->second = 0;
	    fullTime->minute = 0;
	    fullTime->hour = 0;
	    fullTime->dayOfWeek = 0;
	    fullTime->dayOfMonth = 0;
	    fullTime->month = 0;
	    fullTime->year = 0;
	}
}


uint32_t taskRTC_isTimeSynchronized (void)
{
	return synchronized;
}



TASK(taskRTC)
{
	rtc_time_t fullTime;


	// Solamente se generan los eventos si el RTC está sincronizado
	if (synchronized == 1)
	{
		// Se obtiene la fecha y hora actual
		GetResource(resRTC);
		ioRTC_getFullTime(rtc, &fullTime);
		ReleaseResource(resRTC);


		if (timeChanged == 1)
		{
			timeChanged = 0;

			previousMinutes = fullTime.minute;
			previousHours = fullTime.hour;
			previousDay = fullTime.dayOfMonth;
		}
		else
		{
			if (previousMinutes != fullTime.minute)
			{
				previousMinutes = fullTime.minute;
				SetEvent(taskSmartPlug, evRTC_1min);
			}

			if (previousHours != fullTime.hour)
			{
				previousHours = fullTime.hour;
				SetEvent(taskSmartPlug, evRTC_1hour);
			}

			if (previousDay != fullTime.dayOfMonth)
			{
				previousDay = fullTime.dayOfMonth;
				SetEvent(taskSmartPlug, evRTC_1day);
			}
		}
	}


	TerminateTask();
}
