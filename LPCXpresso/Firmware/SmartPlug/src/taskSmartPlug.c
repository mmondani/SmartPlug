/**
 * @file taskSmartPlug.c
 * @author Mariano Mondani
 *
 * @brief Implementación de la tarea taskSmartPlug
 */



#include "taskSmartPlug.h"
#include "os.h"
#include "cObject.h"
#include "memAlloc.h"
#include "ioDigital.h"

#include "taskLeds.h"
#include "taskRTC.h"
#include "taskMeter.h"
#include "moduleLog.h"
#include "taskWiFi.h"
#include "EEPROMMap.h"


void initEEPROM (void* ee);

static void* eeprom;
static void* pinRelay;


static enum {State_Init = 0, State_Running};
static uint32_t state = State_Init;


// Horarios de encendido y apagado de la carga. Ej: onTimes[0] corresponde al domingo y onTimes[6] corresponde al sábado.
static uint8_t onTimes[7][2];
static uint8_t offTimes[7][2];
static uint8_t timesEnabled;

static uint32_t loadState = 0;
static float avgActivePower = 0.0;
static float hourEnergy;

static uint8_t activePower_ptr;
static uint8_t energy_ptr;



void taskSmartPlug_init (void* _eeprom)
{
	eeprom = _eeprom;

	ActivateTask(taskSmartPlug);
}


uint32_t taskSmartPlug_getLoadState (void)
{
	return loadState;
}


TASK(taskSmartPlug)
{
	rtc_time_t fullTime;
	EventMaskType events;
	float valueFloat;
	uint8_t valueChar;
	uint8_t i;
	uint32_t dayOfWeek;
	uint8_t buffDate[3];



	switch (state)
	{
		case State_Init:

			GetResource(resEEPROM);
			ioEE25LCxxx_busyPolling(eeprom);
			ioEE25LCxxx_readData(eeprom, EE_INIT_FLAG, &valueChar, 1);
			ReleaseResource(resEEPROM);

			if (valueChar != 33)
			{
				// La memoria no está inicializada. Se la inicializa
				initEEPROM(eeprom);
			}

			// La memoria está inicializada. Se recuperan los valores.
			GetResource(resEEPROM);

			ioEE25LCxxx_busyPolling(eeprom);
			ioEE25LCxxx_readData(eeprom, EE_MONDAY_LOAD_ON_TIME, onTimes[1], 2);
			ioEE25LCxxx_busyPolling(eeprom);
			ioEE25LCxxx_readData(eeprom, EE_TUESDAY_LOAD_ON_TIME, onTimes[2], 2);
			ioEE25LCxxx_busyPolling(eeprom);
			ioEE25LCxxx_readData(eeprom, EE_WEDNESDAY_LOAD_ON_TIME, onTimes[3], 2);
			ioEE25LCxxx_busyPolling(eeprom);
			ioEE25LCxxx_readData(eeprom, EE_THURSDAY_LOAD_ON_TIME, onTimes[4], 2);
			ioEE25LCxxx_busyPolling(eeprom);
			ioEE25LCxxx_readData(eeprom, EE_FRIDAY_LOAD_ON_TIME, onTimes[5], 2);
			ioEE25LCxxx_busyPolling(eeprom);
			ioEE25LCxxx_readData(eeprom, EE_SATURDAY_LOAD_ON_TIME, onTimes[6], 2);
			ioEE25LCxxx_busyPolling(eeprom);
			ioEE25LCxxx_readData(eeprom, EE_SUNDAY_LOAD_ON_TIME, onTimes[0], 2);
			ioEE25LCxxx_busyPolling(eeprom);
			ioEE25LCxxx_readData(eeprom, EE_MONDAY_LOAD_OFF_TIME, offTimes[1], 2);
			ioEE25LCxxx_busyPolling(eeprom);
			ioEE25LCxxx_readData(eeprom, EE_TUESDAY_LOAD_OFF_TIME, offTimes[2], 2);
			ioEE25LCxxx_busyPolling(eeprom);
			ioEE25LCxxx_readData(eeprom, EE_WEDNESDAY_LOAD_OFF_TIME, offTimes[3], 2);
			ioEE25LCxxx_busyPolling(eeprom);
			ioEE25LCxxx_readData(eeprom, EE_THURSDAY_LOAD_OFF_TIME, offTimes[4], 2);
			ioEE25LCxxx_busyPolling(eeprom);
			ioEE25LCxxx_readData(eeprom, EE_FRIDAY_LOAD_OFF_TIME, offTimes[5], 2);
			ioEE25LCxxx_busyPolling(eeprom);
			ioEE25LCxxx_readData(eeprom, EE_SATURDAY_LOAD_OFF_TIME, offTimes[6], 2);
			ioEE25LCxxx_busyPolling(eeprom);
			ioEE25LCxxx_readData(eeprom, EE_SUNDAY_LOAD_OFF_TIME, offTimes[0], 2);

			ioEE25LCxxx_busyPolling(eeprom);
			ioEE25LCxxx_readData(eeprom, EE_ENABLE_ONOFF_TIME, &timesEnabled, 1);


			// Se recuperan los punteros de las mediciones históricas de potencia activa y energía
			ioEE25LCxxx_busyPolling(eeprom);
			ioEE25LCxxx_readData(eeprom, EE_ACTIVE_POWER_PTR, &activePower_ptr, 1);
			ioEE25LCxxx_busyPolling(eeprom);
			ioEE25LCxxx_readData(eeprom, EE_ENERGY_PTR, &energy_ptr, 1);


			// TODO recuperar los valores de calibración


			ReleaseResource(resEEPROM);


			// Pin que maneja el relay
			pinRelay = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 2, 7);
			ioObject_init(pinRelay);

			state = State_Running;

			ChainTask(taskSmartPlug);

			break;

		case State_Running:

			while(1)
			{
				WaitEvent(evSwitch | evSwitch_5sec | evRelayOn | evRelayOff | evRTC_1min | evRTC_1hour | evAuthenticated |
							evAuthenticated | evResetEnergy | evChangeOnOffTime | evFactoryReset);
				GetEvent(taskSmartPlug, &events);
				ClearEvent(events);

				if (events & evSwitch)
				{
					taskLeds_blinkLed(LED_ID_GREEN, 500, 500);
					taskLeds_blinkLed(LED_ID_RED, 0, 1);

					taskWiFi_initWPS();

					moduleLog_log("Inicio WPS");
				}

				if (events & evSwitch_5sec)
				{
					taskLeds_blinkLed(LED_ID_GREEN, 250, 250);
					taskLeds_blinkLed(LED_ID_RED, 0, 1);

					taskWiFi_initWPS();

					moduleLog_log("Inicio Soft-AP");
				}

				if (events & evRelayOn)
				{
					ioObject_write(pinRelay, 1);
					loadState = 1;

					moduleLog_log("Carga encendida");
				}

				if (events & evRelayOff)
				{
					ioObject_write(pinRelay, 0);
					loadState = 0;

					moduleLog_log("Carga apagada");
				}

				if (events & evRTC_1min)
				{
					// Se anailiza si hay que prender o apagar la carga en función de la hora
					taskRTC_getTime(&fullTime);

					if (fullTime.dayOfWeek < 7)
					{
						// Está habilitada la programación horaria para el día de la semana?
						if (timesEnabled & (1 << fullTime.dayOfWeek))
						{
							if ( (onTimes[fullTime.dayOfWeek][0] == fullTime.hour) && (onTimes[fullTime.dayOfWeek][1] == fullTime.minute) )
							{
								// La hora actual coincide con la hora de encendido
								ioObject_write(pinRelay, 1);
								loadState = 1;

								moduleLog_log("Carga encendida");
							}

							if ( (offTimes[fullTime.dayOfWeek][0] == fullTime.hour) && (offTimes[fullTime.dayOfWeek][1] == fullTime.minute) )
							{
								// La hora actual coincide con la hora de apagado
								ioObject_write(pinRelay, 0);
								loadState = 0;

								moduleLog_log("Carga apagada");
							}
						}
					}


					// Se acumula la potencia activa de la hora actual. Se la va a dividir cuando pase 1 hora
					avgActivePower += taskMeter_getMeterValue(ID_ACTIVE_POWER);
				}

				if (events & evRTC_1hour)
				{
					taskRTC_getTime(&fullTime);


					avgActivePower = avgActivePower / 60.0;
					hourEnergy = taskMeter_getMeterValue(ID_ENERGY);

					GetResource(resEEPROM);

					// Se guarda la potencia activa promediada de la hora actual
					if (fullTime.hour <= 11)
					{
						ioEE25LCxxx_busyPolling(eeprom);
						ioEE25LCxxx_setWriteEnable(eeprom);
						ioEE25LCxxx_writeData(eeprom, activePower_ptr * 128 + EE_ACTIVE_POWER_HOUR_00 + fullTime.hour * 4, &avgActivePower, 4);
					}
					else
					{
						ioEE25LCxxx_busyPolling(eeprom);
						ioEE25LCxxx_setWriteEnable(eeprom);
						ioEE25LCxxx_writeData(eeprom, activePower_ptr * 128 + EE_ACTIVE_POWER_HOUR_12 + (fullTime.hour-12) * 4, &avgActivePower, 4);
					}

					// Se guarda la energía acumulada en la hora actual
					if (fullTime.hour <= 11)
					{
						ioEE25LCxxx_busyPolling(eeprom);
						ioEE25LCxxx_setWriteEnable(eeprom);
						ioEE25LCxxx_writeData(eeprom, energy_ptr * 128 + EE_ENERGY_HOUR_00 + fullTime.hour * 4, &hourEnergy, 4);
					}
					else
					{

						ioEE25LCxxx_setWriteEnable(eeprom);
						ioEE25LCxxx_writeData(eeprom, energy_ptr * 128 + EE_ENERGY_HOUR_12 + (fullTime.hour-12) * 4, &hourEnergy, 4);
					}

					// Se acumula la energía de la hora a la energía total
					ioEE25LCxxx_busyPolling(eeprom);
					ioEE25LCxxx_readData(eeprom, EE_ACUM_ENERGY, &valueFloat, 4);

					valueFloat += hourEnergy;

					ioEE25LCxxx_busyPolling(eeprom);
					ioEE25LCxxx_setWriteEnable(eeprom);
					ioEE25LCxxx_writeData(eeprom,EE_ACUM_ENERGY, &valueFloat, 4);

					ReleaseResource(resEEPROM);

					// Se resetea la potencia promediada de una hora y la energía acumulada de una hora
					avgActivePower = 0.0;
					taskMeter_resetEnergyPulses();
				}

				if (events & evRTC_1day)
				{
					taskRTC_getTime(&fullTime);

					// Se actualizan los punteros de mediciones históricas
					activePower_ptr ++;
					if (activePower_ptr >= 7)
						activePower_ptr = 0;

					energy_ptr ++;
					if (energy_ptr >= 7)
						energy_ptr = 0;

					// Se va a cargar la fecha al inicio del nuevo bloque
					buffDate[0] = fullTime.dayOfMonth;
					buffDate[1] = fullTime.month;
					buffDate[2] = fullTime.year - 2000;

					GetResource(resEEPROM);

					ioEE25LCxxx_busyPolling(eeprom);
					ioEE25LCxxx_setWriteEnable(eeprom);
					ioEE25LCxxx_writeData(eeprom, EE_ACTIVE_POWER_PTR, &activePower_ptr, 1);
					ioEE25LCxxx_busyPolling(eeprom);
					ioEE25LCxxx_setWriteEnable(eeprom);
					ioEE25LCxxx_writeData(eeprom, EE_ENERGY_PTR, &energy_ptr, 1);

					ioEE25LCxxx_busyPolling(eeprom);
					ioEE25LCxxx_setWriteEnable(eeprom);
					ioEE25LCxxx_writeData(eeprom, activePower_ptr * 128 + EE_ACTIVE_POWER_DATE, buffDate, 3);
					ioEE25LCxxx_busyPolling(eeprom);
					ioEE25LCxxx_setWriteEnable(eeprom);
					ioEE25LCxxx_writeData(eeprom, energy_ptr * 128 + EE_ENERGY_PTR, buffDate, 3);

					ReleaseResource(resEEPROM);
				}

				if (events & evAuthenticated)
				{
					taskLeds_blinkLed(LED_ID_GREEN, 1, 0);
					taskLeds_blinkLed(LED_ID_RED, 0, 1);
				}

				if (events & evDeAuthenticated)
				{
					taskLeds_blinkLed(LED_ID_GREEN, 0, 1);
					taskLeds_blinkLed(LED_ID_RED, 500, 500);
				}

				if (events & evResetEnergy)
				{
					taskMeter_resetEnergyPulses();
				}

				if (events & evChangeOnOffTime)
				{
					// Se vuelven a leer los horarios de encendido y apagado ya que alguno se modificó

					GetResource(resEEPROM);

					ioEE25LCxxx_busyPolling(eeprom);
					ioEE25LCxxx_readData(eeprom, EE_MONDAY_LOAD_ON_TIME, onTimes[1], 2);
					ioEE25LCxxx_busyPolling(eeprom);
					ioEE25LCxxx_readData(eeprom, EE_TUESDAY_LOAD_ON_TIME, onTimes[2], 2);
					ioEE25LCxxx_busyPolling(eeprom);
					ioEE25LCxxx_readData(eeprom, EE_WEDNESDAY_LOAD_ON_TIME, onTimes[3], 2);
					ioEE25LCxxx_busyPolling(eeprom);
					ioEE25LCxxx_readData(eeprom, EE_THURSDAY_LOAD_ON_TIME, onTimes[4], 2);
					ioEE25LCxxx_busyPolling(eeprom);
					ioEE25LCxxx_readData(eeprom, EE_FRIDAY_LOAD_ON_TIME, onTimes[5], 2);
					ioEE25LCxxx_busyPolling(eeprom);
					ioEE25LCxxx_readData(eeprom, EE_SATURDAY_LOAD_ON_TIME, onTimes[6], 2);
					ioEE25LCxxx_busyPolling(eeprom);
					ioEE25LCxxx_readData(eeprom, EE_SUNDAY_LOAD_ON_TIME, onTimes[0], 2);
					ioEE25LCxxx_busyPolling(eeprom);
					ioEE25LCxxx_readData(eeprom, EE_MONDAY_LOAD_OFF_TIME, offTimes[1], 2);
					ioEE25LCxxx_busyPolling(eeprom);
					ioEE25LCxxx_readData(eeprom, EE_TUESDAY_LOAD_OFF_TIME, offTimes[2], 2);
					ioEE25LCxxx_busyPolling(eeprom);
					ioEE25LCxxx_readData(eeprom, EE_WEDNESDAY_LOAD_OFF_TIME, offTimes[3], 2);
					ioEE25LCxxx_busyPolling(eeprom);
					ioEE25LCxxx_readData(eeprom, EE_THURSDAY_LOAD_OFF_TIME, offTimes[4], 2);
					ioEE25LCxxx_busyPolling(eeprom);
					ioEE25LCxxx_readData(eeprom, EE_FRIDAY_LOAD_OFF_TIME, offTimes[5], 2);
					ioEE25LCxxx_busyPolling(eeprom);
					ioEE25LCxxx_readData(eeprom, EE_SATURDAY_LOAD_OFF_TIME, offTimes[6], 2);
					ioEE25LCxxx_busyPolling(eeprom);
					ioEE25LCxxx_readData(eeprom, EE_SUNDAY_LOAD_OFF_TIME, offTimes[0], 2);

					ioEE25LCxxx_busyPolling(eeprom);
					ioEE25LCxxx_readData(eeprom, EE_ENABLE_ONOFF_TIME, &timesEnabled, 1);

					ReleaseResource(resEEPROM);
				}

				if (events & evFactoryReset)
				{
					initEEPROM(eeprom);

					timesEnabled = 0;

					moduleLog_log("Reset a valores de fabrica");
				}
			}

			break;
	}

	TerminateTask();
}


void initEEPROM (void* ee)
{
	uint8_t i;
	uint32_t value;
	uint8_t valueChar;


	GetResource(resEEPROM);

	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_setWriteEnable(ee);
	ioEE25LCxxx_writeString(ee, EE_DEVICE_ID, "Smart Plug");

	// Borra todos los horarios de encendido y apagado
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_setWriteEnable(ee);
	ioEE25LCxxx_erase(ee, EE_MONDAY_LOAD_ON_TIME, 28);

	// Se deshabilita la programación horaria para todos los días
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_setWriteEnable(ee);
	ioEE25LCxxx_erase(ee, EE_ENABLE_ONOFF_TIME, 1);


	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_setWriteEnable(ee);
	ioEE25LCxxx_erase(ee, EE_ACTIVE_POWER_PTR, 1);
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_setWriteEnable(ee);
	ioEE25LCxxx_erase(ee, EE_ENERGY_PTR, 1);


	for (i = 0; i < 7; i++)
	{
		ioEE25LCxxx_busyPolling(ee);
		ioEE25LCxxx_setWriteEnable(ee);
		ioEE25LCxxx_erase(ee, i * 128 + EE_ACTIVE_POWER_DATE, 3);			// Fecha
		ioEE25LCxxx_busyPolling(ee);
		ioEE25LCxxx_setWriteEnable(ee);
		ioEE25LCxxx_erase(ee, i * 128 + EE_ACTIVE_POWER_HOUR_00, 48);		// Primeras 12 horas del día
		ioEE25LCxxx_busyPolling(ee);
		ioEE25LCxxx_setWriteEnable(ee);
		ioEE25LCxxx_erase(ee, i * 128 + EE_ACTIVE_POWER_HOUR_12, 48);		// Últimas 12 horas del día
	}


	for (i = 0; i < 7; i++)
	{
		ioEE25LCxxx_busyPolling(ee);
		ioEE25LCxxx_setWriteEnable(ee);
		ioEE25LCxxx_erase(ee, i * 128 + EE_ENERGY_DATE, 3);			// Fecha
		ioEE25LCxxx_busyPolling(ee);
		ioEE25LCxxx_setWriteEnable(ee);
		ioEE25LCxxx_erase(ee, i * 128 + EE_ENERGY_HOUR_00, 48);		// Primeras 12 horas del día
		ioEE25LCxxx_busyPolling(ee);
		ioEE25LCxxx_setWriteEnable(ee);
		ioEE25LCxxx_erase(ee, i * 128 + EE_ENERGY_HOUR_12, 48);		// Últimas 12 horas del día
	}


	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_setWriteEnable(ee);
	ioEE25LCxxx_erase(ee, EE_ACUM_ENERGY, 4);

	value = 0;
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_setWriteEnable(ee);
	ioEE25LCxxx_writeData(ee, EE_VDC_OFFSET, &value, 4);
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_setWriteEnable(ee);
	ioEE25LCxxx_writeData(ee, EE_IDC_OFFSET, &value, 4);

	value = 0x400000;
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_setWriteEnable(ee);
	ioEE25LCxxx_writeData(ee, EE_VAC_GAIN, &value, 4);
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_setWriteEnable(ee);
	ioEE25LCxxx_writeData(ee, EE_IAC_GAIN, &value, 4);

	valueChar = 33;
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_setWriteEnable(ee);
	ioEE25LCxxx_writeData(ee, EE_INIT_FLAG, &valueChar, 1);

	ReleaseResource(resEEPROM);
}
