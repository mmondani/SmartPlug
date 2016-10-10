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
void loadEEPROM (void* ee);
void updateLeds (uint8_t status);


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

static uint8_t block_ptr;

// La variable smartPlug_status contiene una serie de bits indicando el estado actual
// del Smart Plug. Se la usa para generar las señalizaciones con los dos leds.
static uint8_t smartPlug_status;
#define SMART_PLUG_STATUS_WPS			0x01
#define SMART_PLUG_STATUS_SOFT_AP		0x02
#define SMART_PLUG_STATUS_RTC_SYNCH		0x04
#define SMART_PLUG_STATUS_AUTH			0x08



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
	uint32_t hour;




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
			loadEEPROM(eeprom);


			// Pin que maneja el relay
			pinRelay = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 2, 7);
			ioObject_init(pinRelay);

			// Arranca apagada la carga
			ioObject_write(pinRelay, 1);

			state = State_Running;

			smartPlug_status = 0;

			ChainTask(taskSmartPlug);

			break;

		case State_Running:

			while(1)
			{
				WaitEvent(evSwitch | evSwitch_5sec | evRelayOn | evRelayOff | evRTC_1min | evRTC_1hour | evAuthenticated |
							evAuthenticated | evResetEnergy | evChangeOnOffTime | evFactoryReset | evRTCNoSynch | evRTCSynch);
				GetEvent(taskSmartPlug, &events);
				ClearEvent(events);

				if (events & evAuthenticated)
				{
					smartPlug_status |= SMART_PLUG_STATUS_AUTH;
					smartPlug_status &= (~SMART_PLUG_STATUS_WPS);
					smartPlug_status &= (~SMART_PLUG_STATUS_SOFT_AP);

					updateLeds (smartPlug_status);
				}

				if (events & evDeAuthenticated)
				{
					smartPlug_status &= (~SMART_PLUG_STATUS_AUTH);
					smartPlug_status &= (~SMART_PLUG_STATUS_WPS);
					smartPlug_status &= (~SMART_PLUG_STATUS_SOFT_AP);

					updateLeds (smartPlug_status);
				}

				if (events & evRTCNoSynch)
				{
					moduleLog_log("RTC No sincronizado");

					smartPlug_status &= (~SMART_PLUG_STATUS_RTC_SYNCH);

					updateLeds (smartPlug_status);
				}

				if (events & evRTCSynch)
				{
					moduleLog_log("RTC sincronizado");

					smartPlug_status |= SMART_PLUG_STATUS_RTC_SYNCH;

					updateLeds (smartPlug_status);
				}

				if (events & evSwitch)
				{
					taskWiFi_initWPS();

					moduleLog_log("Inicio WPS");

					smartPlug_status |= SMART_PLUG_STATUS_WPS;

					updateLeds (smartPlug_status);
				}

				if (events & evSwitch_5sec)
				{
					taskWiFi_initWebServer();

					moduleLog_log("Inicio Soft-AP");

					smartPlug_status |= SMART_PLUG_STATUS_SOFT_AP;

					updateLeds (smartPlug_status);
				}

				if (events & evRelayOn)
				{
					ioObject_write(pinRelay, 0);
					loadState = 1;

					moduleLog_log("Carga encendida");
				}

				if (events & evRelayOff)
				{
					ioObject_write(pinRelay, 1);
					loadState = 0;

					moduleLog_log("Carga apagada");
				}

				if (events & evRTC_1min)
				{
					// Se anailiza si hay que prender o apagar la carga en función de la hora
					taskRTC_getTime(&fullTime);

					if (taskRTC_isTimeSynchronized())
					{
						if (fullTime.dayOfWeek < 7)
						{
							// Está habilitada la programación horaria para el día de la semana?
							if (timesEnabled & (1 << fullTime.dayOfWeek))
							{
								if ( (onTimes[fullTime.dayOfWeek][0] == fullTime.hour) && (onTimes[fullTime.dayOfWeek][1] == fullTime.minute) )
								{
									// La hora actual coincide con la hora de encendido
									ioObject_write(pinRelay, 0);
									loadState = 1;

									moduleLog_log("Carga encendida");
								}

								if ( (offTimes[fullTime.dayOfWeek][0] == fullTime.hour) && (offTimes[fullTime.dayOfWeek][1] == fullTime.minute) )
								{
									// La hora actual coincide con la hora de apagado
									ioObject_write(pinRelay, 1);
									loadState = 0;

									moduleLog_log("Carga apagada");
								}
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

					if (taskRTC_isTimeSynchronized())
					{
						// Cada vez que se carga las mediciones de una hora, se carga la fecha de la medición al inicio del bloque.
						// Esto permite que se inicie el proceso de medición en cualquier hora del día y el bloque siempre tenga
						// la fecha del día.
						if (fullTime.hour > 0)
						{
							hour = fullTime.hour - 1;

							buffDate[0] = fullTime.dayOfMonth;
							buffDate[1] = fullTime.month;
							buffDate[2] = fullTime.year - 2000;

							ioEE25LCxxx_busyPolling(eeprom);
							ioEE25LCxxx_setWriteEnable(eeprom);
							ioEE25LCxxx_writeData(eeprom, block_ptr * 128 + EE_ACTIVE_POWER_DATE, buffDate, 3);
							ioEE25LCxxx_busyPolling(eeprom);
							ioEE25LCxxx_setWriteEnable(eeprom);
							ioEE25LCxxx_writeData(eeprom, block_ptr * 128 + EE_ENERGY_DATE, buffDate, 3);
						}
						else
						{
							// Si la hora actual es la 0, los valores que se deben guardar corresponden
							// a lo acumulado durante la hora 23 del día anterior. Por lo tanto no se carga
							// fecha en el bloque actual ya que todavía no se actualiza block_ptr y se está
							// apuntando todavía al bloque del día anterior.
							hour = 23;
						}



						// Se guarda la potencia activa promediada de la hora actual
						if (hour <= 11)
						{
							ioEE25LCxxx_busyPolling(eeprom);
							ioEE25LCxxx_setWriteEnable(eeprom);
							ioEE25LCxxx_writeData(eeprom, block_ptr * 128 + EE_ACTIVE_POWER_HOUR_00 + (hour * 4), &avgActivePower, 4);
						}
						else
						{
							ioEE25LCxxx_busyPolling(eeprom);
							ioEE25LCxxx_setWriteEnable(eeprom);
							ioEE25LCxxx_writeData(eeprom, block_ptr * 128 + EE_ACTIVE_POWER_HOUR_12 + ((hour - 12) * 4), &avgActivePower, 4);
						}

						// Se guarda la energía acumulada en la hora actual
						if (hour <= 11)
						{
							ioEE25LCxxx_busyPolling(eeprom);
							ioEE25LCxxx_setWriteEnable(eeprom);
							ioEE25LCxxx_writeData(eeprom, block_ptr * 128 + EE_ENERGY_HOUR_00 + (hour * 4), &hourEnergy, 4);
						}
						else
						{
							ioEE25LCxxx_busyPolling(eeprom);
							ioEE25LCxxx_setWriteEnable(eeprom);
							ioEE25LCxxx_writeData(eeprom, block_ptr * 128 + EE_ENERGY_HOUR_12 + ((hour - 12) * 4), &hourEnergy, 4);
						}
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

					if (taskRTC_isTimeSynchronized())
					{
						// Se actualiza el puntero de mediciones históricas
						block_ptr ++;
						if (block_ptr >= 7)
							block_ptr = 0;


						// Se va a cargar la fecha al inicio del nuevo bloque
						buffDate[0] = fullTime.dayOfMonth;
						buffDate[1] = fullTime.month;
						buffDate[2] = fullTime.year - 2000;

						GetResource(resEEPROM);

						// Se guarda el block_ptr actual
						ioEE25LCxxx_busyPolling(eeprom);
						ioEE25LCxxx_setWriteEnable(eeprom);
						ioEE25LCxxx_writeData(eeprom, EE_BLOCK_PTR, &block_ptr, 1);

						// Se guarda la fecha al inicio del nuevo bloque
						ioEE25LCxxx_busyPolling(eeprom);
						ioEE25LCxxx_setWriteEnable(eeprom);
						ioEE25LCxxx_writeData(eeprom, block_ptr * 128 + EE_ACTIVE_POWER_DATE, buffDate, 3);
						ioEE25LCxxx_busyPolling(eeprom);
						ioEE25LCxxx_setWriteEnable(eeprom);
						ioEE25LCxxx_writeData(eeprom, block_ptr * 128 + EE_ENERGY_DATE, buffDate, 3);


						// Se borran las mediciones del nuevo bloque
						ioEE25LCxxx_busyPolling(eeprom);
						ioEE25LCxxx_setWriteEnable(eeprom);
						ioEE25LCxxx_erase(eeprom, block_ptr * 128 + EE_ENERGY_HOUR_00, 48);			// Primeras 12 horas del día
						ioEE25LCxxx_busyPolling(eeprom);
						ioEE25LCxxx_setWriteEnable(eeprom);
						ioEE25LCxxx_erase(eeprom, block_ptr * 128 + EE_ENERGY_HOUR_12, 48);			// Últimas 12 horas del día

						ioEE25LCxxx_busyPolling(eeprom);
						ioEE25LCxxx_setWriteEnable(eeprom);
						ioEE25LCxxx_erase(eeprom, block_ptr * 128 + EE_ACTIVE_POWER_HOUR_00, 48);			// Primeras 12 horas del día
						ioEE25LCxxx_busyPolling(eeprom);
						ioEE25LCxxx_setWriteEnable(eeprom);
						ioEE25LCxxx_erase(eeprom, block_ptr * 128 + EE_ACTIVE_POWER_HOUR_12, 48);			// Últimas 12 horas del día

						ReleaseResource(resEEPROM);
					}
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
					loadEEPROM (eeprom);

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
	uint8_t buffer[6];


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
	ioEE25LCxxx_erase(ee, EE_BLOCK_PTR, 1);


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


	// TODO: este ID se carga en la rutina de prueba, en fábrica
	buffer[0] = '1';
	buffer[1] = '2';
	buffer[2] = '3';
	buffer[3] = '4';
	buffer[4] = '5';
	buffer[5] = '6';

	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_setWriteEnable(ee);
	ioEE25LCxxx_writeData(ee, EE_DEVICE_NUMBER, buffer, 6);


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


void loadEEPROM (void* ee)
{
	uint8_t deviceNumber[6];


	GetResource(resEEPROM);

	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_readData(ee, EE_MONDAY_LOAD_ON_TIME, onTimes[1], 2);
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_readData(ee, EE_TUESDAY_LOAD_ON_TIME, onTimes[2], 2);
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_readData(ee, EE_WEDNESDAY_LOAD_ON_TIME, onTimes[3], 2);
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_readData(ee, EE_THURSDAY_LOAD_ON_TIME, onTimes[4], 2);
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_readData(ee, EE_FRIDAY_LOAD_ON_TIME, onTimes[5], 2);
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_readData(ee, EE_SATURDAY_LOAD_ON_TIME, onTimes[6], 2);
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_readData(ee, EE_SUNDAY_LOAD_ON_TIME, onTimes[0], 2);
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_readData(ee, EE_MONDAY_LOAD_OFF_TIME, offTimes[1], 2);
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_readData(ee, EE_TUESDAY_LOAD_OFF_TIME, offTimes[2], 2);
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_readData(ee, EE_WEDNESDAY_LOAD_OFF_TIME, offTimes[3], 2);
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_readData(ee, EE_THURSDAY_LOAD_OFF_TIME, offTimes[4], 2);
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_readData(ee, EE_FRIDAY_LOAD_OFF_TIME, offTimes[5], 2);
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_readData(ee, EE_SATURDAY_LOAD_OFF_TIME, offTimes[6], 2);
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_readData(ee, EE_SUNDAY_LOAD_OFF_TIME, offTimes[0], 2);

	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &timesEnabled, 1);


	// Se recupera el puntero de las mediciones históricas de potencia activa y energía
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_readData(ee, EE_BLOCK_PTR, &block_ptr, 1);


	// Se recupera el número de dispositivo
	ioEE25LCxxx_busyPolling(ee);
	ioEE25LCxxx_readData(ee, EE_DEVICE_NUMBER, deviceNumber, 6);
	taskWiFi_configureID(deviceNumber);


	// TODO recuperar los valores de calibración


	ReleaseResource(resEEPROM);
}


/*
 * Las señalizaciones siguen la siguiente tabla:
 *
 * Senalización					Soft-AP			WPS			Autenticado			RTC Synch
 * Verde = 2Hz						1			X				X					X
 * Verde = 1Hz						0			1				X					X
 * Rojo = 1Hz						0			0				0					X
 * Verde y rojo destellan			0			0				1					0
 * Verde encendido					0			0				1					1
 */
void updateLeds (uint8_t status)
{
	if (status & SMART_PLUG_STATUS_SOFT_AP)
	{
		taskLeds_blinkLed(LED_ID_GREEN, 250, 250);
		taskLeds_blinkLed(LED_ID_RED, 0, 1);
	}
	else if (status & SMART_PLUG_STATUS_WPS)
	{
		taskLeds_blinkLed(LED_ID_GREEN, 500, 500);
		taskLeds_blinkLed(LED_ID_RED, 0, 1);
	}
	else if ( (status & SMART_PLUG_STATUS_AUTH) == 0 )
	{
		taskLeds_blinkLed(LED_ID_GREEN, 0, 1);
		taskLeds_blinkLed(LED_ID_RED, 500, 500);
	}
	else
	{
		if (status & SMART_PLUG_STATUS_RTC_SYNCH)
		{
			taskLeds_blinkLed(LED_ID_GREEN, 1, 0);
			taskLeds_blinkLed(LED_ID_RED, 0, 1);
		}
		else
		{
			taskLeds_blinkLed(LED_ID_GREEN, 500, 250);
			taskLeds_blinkLed(LED_ID_RED, 250, 500);
		}
	}
}
