/**
 * @file taskLeds.c
 * @author Mariano Mondani
 *
 * @brief Implementación de la tarea taskLeds
 */


#include "taskLeds.h"
#include "os.h"
#include "cObject.h"
#include "memAlloc.h"
#include "ioDigital.h"
#include "cTimer.h"


static void* pinLedGreen;
static void* pinLedRed;
static void* timerOn_green;
static void* timerOff_green;
static void* timerOn_red;
static void* timerOff_red;

static uint8_t blinkingGreen = 0;
static uint8_t blinkingRed = 0;


enum {
	State_OFF = 0,
	State_ON
};

static uint8_t stateGreen = State_OFF;
static uint8_t stateRed = State_OFF;



void taskLeds_init (void)
{
	// Se instancias los pines que van a manejar los leds.
	pinLedGreen = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 0, 21);
    ioObject_init(pinLedGreen);

    pinLedRed = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 2, 13);
    ioObject_init(pinLedRed);


    // Se instancias los timers que van a controlar los tiempos de encendido y de apagado de los leds.
    timerOn_green = cObject_new(cTimer);
    timerOff_green = cObject_new(cTimer);
    timerOn_red = cObject_new(cTimer);
    timerOff_red = cObject_new(cTimer);


    // Se va a llamar a taskLeds cada 10 ms
    SetRelAlarm(alarmRunTaskLeds, 10, 5);
}


void taskLeds_blinkLed (uint32_t ledID, uint32_t tOn, uint32_t tOff)
{
	if (tOn == 0)
	{
		// Si tOn es cero, el led va a permanecer apagado.

		if (ledID == LED_ID_GREEN)
		{
			blinkingGreen = 0;
			stateGreen = State_OFF;
		}
		else
		{
			blinkingRed = 0;
			stateRed = State_OFF;
		}
	}
	else if (tOff == 0)
	{
		// Si tOff es cero, el led va a permanecer encendido

		if (ledID == LED_ID_GREEN)
		{
			blinkingGreen = 0;
			stateGreen = State_ON;
		}
		else
		{
			blinkingRed = 0;
			stateRed = State_ON;
		}
	}
	else
	{
		// Si tOn y tOff son distintos de cero, el led va a destellar.
		// Se inician los dos timers para que queden cargadas las temporizaciones de cada uno.

		if (ledID == LED_ID_GREEN)
		{
			blinkingGreen = 1;
			stateGreen = State_ON;
			cTimer_start(timerOn_green, tOn);
			cTimer_start (timerOff_green, tOff);
		}
		else
		{
			blinkingRed = 1;
			stateRed = State_ON;
			cTimer_start(timerOn_red, tOn);
			cTimer_start (timerOff_red, tOff);
		}
	}
}


TASK(taskLeds)
{
	// *****************************************************************************************
	// FSM led verde
	switch (stateGreen)
	{
		case State_OFF:
			ioObject_write(pinLedGreen, 0);

			if ( (blinkingGreen == 1) && cTimer_hasExpired(timerOff_green) )
			{
				cTimer_restart(timerOn_green);
				stateGreen = State_ON;
			}
			break;


		case State_ON:
			ioObject_write(pinLedGreen, 1);

			if ( (blinkingGreen == 1) && cTimer_hasExpired(timerOn_green) )
			{
				cTimer_restart(timerOff_green);
				stateGreen = State_OFF;
			}
			break;

		default:
			ioObject_write(pinLedGreen, 0);
			break;
	}
	// *****************************************************************************************


	// *****************************************************************************************
	// FSM led rojo
	switch (stateRed)
	{
		case State_OFF:
			ioObject_write(pinLedRed, 0);

			if ( (blinkingRed == 1) && cTimer_hasExpired(timerOff_red) )
			{
				cTimer_restart(timerOn_red);
				stateRed = State_ON;
			}
			break;


		case State_ON:
			ioObject_write(pinLedRed, 1);

			if ( (blinkingRed == 1) && cTimer_hasExpired(timerOn_red) )
			{
				cTimer_restart(timerOff_red);
				stateRed = State_OFF;
			}
			break;

		default:
			ioObject_write(pinLedRed, 0);
			break;
	}
	// *****************************************************************************************


	TerminateTask();
}
