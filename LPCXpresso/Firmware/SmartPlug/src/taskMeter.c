/**
 * @file taskMeter.c
 * @author Mariano Mondani
 *
 * @brief Implementación de la tarea taskMeter
 */


#include "taskMeter.h"
#include "os.h"
#include "cObject.h"
#include "memAlloc.h"
#include "ioDigital.h"
#include "ioUART.h"
#include "ioCS5490.h"

#include "moduleLog.h"


static void* eeprom;
static void* gpioDO;
static void* gpioReset;
static void* uartCS5490;
static void* cs5490;

static uint32_t irms, vrms, activePower, powerFactor, epsilon;
static uint32_t energyPulses = 0;

static enum {STATE_INIT = 0, STATE_RUNNING_IRMS, STATE_RUNNING_VRMS, STATE_RUNNING_ACTIVE_POWER, STATE_RUNNING_POWER_FACTOR, STATE_RUNNING_EPSILON};
static uint32_t state = STATE_INIT;



void taskMeter_init (void* _eeprom)
{
	eeprom = _eeprom;

	// La tarea taskMeter se va a ejecutar cada 1 s
	SetRelAlarm(alarmRunTaskMeter, 150, 1500);
	//ActivateTask(taskMeter);
}


float taskMeter_getMeterValue (uint32_t variableID)
{
	float conversion;
	float result = 0.0;


	switch(variableID)
	{
		case ID_VRMS:
			conversion = ioCS5490_unsignedFract2Float(vrms, 0, 24);
			result = conversion * ioCS5490_getVmax(cs5490) / 0.6;
			break;

		case ID_IRMS:
			conversion = ioCS5490_unsignedFract2Float(irms, 0, 24);
			result = conversion * ioCS5490_getIcalibration(cs5490) / ioCS5490_getMeterScale(cs5490);
			break;

		case ID_FREQUENCY:
			conversion = ioCS5490_signedFract2Float(epsilon, 0, 23);
			result = conversion * ioCS5490_getWordRate(cs5490);
			break;

		case ID_ENERGY:
			result = energyPulses / ioCS5490_getMeterConstant(cs5490);
			break;

		case ID_POWER_FACTOR:
			result = ioCS5490_signedFract2Float(powerFactor, 0, 23);
			break;

		case ID_ACTIVE_POWER:
			conversion = ioCS5490_signedFract2Float(activePower, 0, 23);
			result = conversion * ioCS5490_getMaxPower(cs5490) / ioCS5490_getPowerScale(cs5490);
			break;
	}

	return result;
}


void taskMeter_resetEnergyPulses (void)
{
	energyPulses = 0;
}


void taskMeter_initCalibrationProcess (uint32_t processID)
{
	// TODO
}


TASK(taskMeter)
{
	switch (state)
	{
		case STATE_INIT:

			// Pin encargado de contar los pulsos de energía generados por el CS5490
			gpioDO = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 0, 27);
			ioObject_init(gpioDO);
			Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, GPIOINT_PORT0, 1 << 27);
			NVIC_ClearPendingIRQ(EINT3_IRQn);
			NVIC_EnableIRQ(EINT3_IRQn);

			// Pin para el reset del CS5490.
			gpioReset = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 1, 30);
			ioObject_init(gpioReset);


			uartCS5490 = cObject_new(ioUART, LPC_UART3, IOUART_BR_600, IOUART_DATA_8BIT, IOUART_PAR_NONE, IOUART_STOP_1BIT, IOUART_MODE_BLOCKING, 2, 20);
			ioObject_init(uartCS5490);
			ioComm_intEnable(uartCS5490, IOUART_INT_TX);
			ioComm_intEnable(uartCS5490, IOUART_INT_RX);
			ioUART_configFIFO(uartCS5490, IOUART_FIFO_LEVEL0);

			NVIC_SetPriority(UART3_IRQn, 1);
			NVIC_EnableIRQ(UART3_IRQn);

			// Instancia del driver del CS5490
			cs5490 = cObject_new (ioCS5490, uartCS5490, gpioReset);
			ioCS5490_init(cs5490, 0xFE5657, 0xFFE604, 0x3A62AA, 0x73AC71, 4000.0, 220.0, 5.0, 1.75, 5000.0, 8.0);

			// Comienza el proceso de conversión continua en el CS5490
			ioCS5490_instructionWrite(cs5490, IOCS5490_INS_CONTINUOUS_CONV);


			ioCS5490_pageSelect(cs5490, IOCS5490_PAGE_16);

			moduleLog_log("CS5490 Inicializado");

			state = STATE_RUNNING_IRMS;


			break;

		case STATE_RUNNING_IRMS:

			//ioCS5490_pageSelect(cs5490, IOCS5490_PAGE_16);
			irms = ioCS5490_registerRead(cs5490, IOCS5490_REG_I_RMS);
			state = STATE_RUNNING_VRMS;
			break;

		case STATE_RUNNING_VRMS:

			//ioCS5490_pageSelect(cs5490, IOCS5490_PAGE_16);
			vrms = ioCS5490_registerRead(cs5490, IOCS5490_REG_V_RMS);
			state = STATE_RUNNING_ACTIVE_POWER;
			break;

		case STATE_RUNNING_ACTIVE_POWER:

			//oCS5490_pageSelect(cs5490, IOCS5490_PAGE_16);
			activePower = ioCS5490_registerRead(cs5490, IOCS5490_REG_P_AVG);
			state = STATE_RUNNING_POWER_FACTOR;
			break;

		case STATE_RUNNING_POWER_FACTOR:

			//ioCS5490_pageSelect(cs5490, IOCS5490_PAGE_16);
			powerFactor = ioCS5490_registerRead(cs5490, IOCS5490_REG_PF);
			state = STATE_RUNNING_EPSILON;
			break;

		case STATE_RUNNING_EPSILON:

			//ioCS5490_pageSelect(cs5490, IOCS5490_PAGE_16);
			epsilon = ioCS5490_registerRead(cs5490, IOCS5490_REG_EPSILON);
			moduleLog_log("Nuevas mediciones");
			state = STATE_RUNNING_IRMS;
			break;
	}

	TerminateTask();
}


ISR(UART3_handler)
{
	if (ioComm_getInt(uartCS5490, IOUART_INT_ID_TX))
	{
		ioUART_txHandler(uartCS5490);
	}
	else if(ioComm_getInt(uartCS5490, IOUART_INT_ID_RX))
	{
		ioUART_rxHandler(uartCS5490);
	}
}


ISR(EINT3_handler)
{
	Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, GPIOINT_PORT0, 1 << 27);
	energyPulses++;
}
