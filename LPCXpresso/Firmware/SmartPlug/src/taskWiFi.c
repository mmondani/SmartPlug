/**
 * @file taskWiFi.c
 * @author Mariano Mondani
 *
 * @brief Implementación de la tarea taskWiFi
 */


#include "taskWiFi.h"
#include "os.h"
#include "cObject.h"
#include "memAlloc.h"
#include "ioDigital.h"
#include "cTimer.h";
#include "ioRN1723.h"
#include "ioUART.h"
#include "cQueue.h"
#include "ioEE25LCxxx.h"

#include "moduleLog.h"
#include "taskRTC.h"
#include "taskMeter.h"
#include "TCPCommands.h"
#include "EEPROMMap.h"


static void* eeprom;
static void* rn1723 = 0;
static void* gpioResetRN1723;
static void* uartRN1723;
static void* inBuffer;
static void* outBuffer;
static void* timerSynchronizeTime;
static void* timerTimeout;


enum {State_Create = 0, State_Waiting4Ready, State_Init, State_Idle, State_SynchronizeTime, State_GetNewTime,
       State_WaitFrameStart1, State_WaitFrameStart2, State_WaitFrameLength, State_WaitCommand,
       State_NodeCommand, State_SendNodeResponse, State_WaitRegister,State_WaitParam, State_SearchDate,
       State_ReadEEPROM, State_SendGetResponse, State_WaitValue, State_WriteEEPROM, State_SendSetResponse,
       State_EraseEEPROM, State_SendResetResponse, State_WaitingWPS, State_WaitingWebServer, State_ConfigureiD};
static uint32_t state = State_Create;
static uint8_t stateIn = 0, stateOut = 0;
static uint8_t executeWPS = 0;
static uint8_t executeWebServer = 0;
static uint8_t configID = 0;


static uint8_t byte;
static uint8_t length;
static uint8_t command;
static uint8_t reg;
static uint32_t counter;
static uint8_t blockPointer;		// Cuando se busca en la EEPROM un bloque de mediciones, se usa esta variable como índice.
static uint32_t EEAddress;

#define BUFFER_LEN				120
static uint8_t indexBuffer;
static uint8_t buffer[BUFFER_LEN];

static uint8_t bufferID[7];

// Cambia de estado en la FSM.
void gotoState (uint32_t newState);

uint32_t readEEPROMbyRegister (void* ee, uint8_t regEE, uint8_t bPointer, uint8_t* buff);
void writeEEPROMbyRegister (void* ee, uint8_t regEE, uint8_t count, uint8_t* buff);
void eraseEEPROMbyRegister (void* ee, uint8_t regEE, uint8_t* buff);
void float2Bytes (uint8_t* bytes, float floatVariable);



void taskWiFi_init (void* _eeprom)
{
	eeprom = _eeprom;

	// La tarea taskWiFi se va a ejecutar cada 10 ms.
	SetRelAlarm(alarmRunTaskWiFi, 5, 10);
}


void taskWiFi_initWPS (void)
{
	executeWPS = 1;
}


void taskWiFi_initWebServer (void)
{
	executeWebServer = 1;
}


void taskWiFi_isAuthenticated (void)
{
	if (ioRN1723_isAuthenticated(rn1723))
		return 1;
	else
		return 0;
}


void taskWiFi_isTPCConnected (void)
{
	if (ioRN1723_isTCPConnected(rn1723))
		return 1;
	else
		return 0;
}


void taskWiFi_configureID (uint8_t* id)
{
	uint32_t i;

	for (i = 0; i < 6; i++)
		bufferID[i] = id[i];

	// El driver del RN1723 necesita que el string del ID termine en '\0'.
	bufferID[6] = '\0';

	configID = 1;
}


TASK(taskWiFi)
{
	rtc_time_t fullTime;
	uint8_t buffEE[3];
	uint8_t i;


	if (rn1723 != 0)
	ioRN1723_handler(rn1723);

	switch (state)
	{
		case State_Create:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

            }
            //**********************************************************************************************

            gpioResetRN1723 = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 2, 2);
            ioObject_init(gpioResetRN1723);


            uartRN1723 = cObject_new(ioUART, LPC_UART2, IOUART_BR_9600, IOUART_DATA_8BIT, IOUART_PAR_NONE, IOUART_STOP_1BIT, IOUART_MODE_NON_BLOCKING, 80, 80);
            ioObject_init(uartRN1723);
            ioComm_intEnable(uartRN1723, IOUART_INT_TX);
            ioComm_intEnable(uartRN1723, IOUART_INT_RX);
            ioUART_configFIFO(uartRN1723, IOUART_FIFO_LEVEL0);

            NVIC_SetPriority(UART2_IRQn, 1);
            NVIC_EnableIRQ(UART2_IRQn);


            inBuffer = cObject_new(cQueue, 50, sizeof(uint8_t));
            outBuffer = cObject_new(cQueue, 160, sizeof(uint8_t));


            rn1723 = cObject_new(ioRN1723, uartRN1723, gpioResetRN1723, inBuffer, outBuffer, IORN1723_INTERVAL_1_EVERY_4_SEC);

            timerSynchronizeTime = cObject_new(cTimer);
            timerTimeout = cObject_new(cTimer);


            gotoState(State_Waiting4Ready);
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_Waiting4Ready:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

            }
            //**********************************************************************************************
            if (ioRN1723_isIdle(rn1723) == 1)
            	gotoState(State_Init);
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_Init:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

                ioObject_init(rn1723);
            }
            //**********************************************************************************************
            if (ioRN1723_isIdle(rn1723) == 1)
            	gotoState(State_Idle);
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;

                // Al pasar un segundo se va a tratar de sincronizar con el SNTP
                cTimer_start(timerSynchronizeTime, 1000);

                if (ioRN1723_isAuthenticated(rn1723))
                {
                	SetEvent (taskSmartPlug, evAuthenticated);
                	moduleLog_log("Unido a WiFi");
                }
                else
                	SetEvent (taskSmartPlug, evDeAuthenticated);
            }
			break;

		case State_Idle:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

            }
            //**********************************************************************************************
            // Iniciar el proceso de WPS
            if (executeWPS)
            {
            	executeWPS = 0;
            	gotoState(State_WaitingWPS);
            }

            // Iniciar el Soft-Ap
            if (executeWebServer)
			{
            	executeWebServer = 0;
				gotoState(State_WaitingWebServer);
			}


            // Llegaron bytes de una nueva conexión
            if (ioComm_dataAvailable(rn1723))
            {
            	// Si en 500 ms no llega el frame start vuelve a idle y borra el buffer de recepción de WiFi.
            	cTimer_start(timerTimeout, 500);

            	gotoState(State_WaitFrameStart1);
            }

            // Expiró el timer para sincronizar el RTC
            if (cTimer_hasExpired(timerSynchronizeTime))
            {
            	if (!ioRN1723_isTimeValid(rn1723))
            	{
            		gotoState(State_SynchronizeTime);
            	}
            }

            if (configID)
            {
            	configID = 0;
            	gotoState(State_ConfigureiD);
            }
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_SynchronizeTime:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

                ioRN1723_synchronizeTime(rn1723);
            }
            //**********************************************************************************************
            if (ioRN1723_isIdle(rn1723) == 1)
            	gotoState(State_GetNewTime);
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;

        		// Espera 1,5 s para recibir la hora de internet
        		cTimer_start(timerTimeout, 1500);
            }
			break;

		case State_GetNewTime:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

                ioRN1723_refreshLocalTime(rn1723);
            }
            //**********************************************************************************************
            if (cTimer_hasExpired(timerTimeout))
            	gotoState(State_Idle);
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;

                // Si no se pudo sincronizar a los 30 segundos va a volver a intentar
                if (ioRN1723_isTimeValid(rn1723) == 0)
                	cTimer_start(timerSynchronizeTime, 20000);
                else
                {
                	ioRN1723_getTime(rn1723, &fullTime);
                	taskRTC_setTime(&fullTime);

                	moduleLog_log("RTC sincronizado");

                	//TODO: agregar que se sincronize, por ejemplo cada día, para corregir el error del RTC del micro.
                	cTimer_stop(timerSynchronizeTime);
                }
            }
			break;

		case State_WaitFrameStart1:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

            }
            //**********************************************************************************************
            if (ioComm_dataAvailable(rn1723) > 0)
            {
				byte = (uint8_t)ioObject_read(rn1723);

				if (byte == '#')
				{
	                moduleLog_log("Nueva conexion");
	                SetEvent(taskSmartPlug, evNewConn);
					gotoState(State_WaitFrameStart2);
				}
            }
            else
            {
            	if (cTimer_hasExpired(timerTimeout))
            	{
            		// No llegó el frame start, por lo que vuelve a Idle y borra el buffer de recepción de WiFi.
            		ioRN1723_flushRxData(rn1723);
            		gotoState(State_Idle);
            	}
            }
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_WaitFrameStart2:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;
            }
            //**********************************************************************************************
            if (ioComm_dataAvailable(rn1723) > 0)
            {
				byte = (uint8_t)ioObject_read(rn1723);

				if (byte == '!')
					gotoState(State_WaitFrameLength);
            }
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_WaitFrameLength:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

                length = 0;
            }
            //**********************************************************************************************
            if (ioComm_dataAvailable(rn1723) > 0)
            {
				length = (uint8_t)ioObject_read(rn1723);

				gotoState(State_WaitCommand);
            }
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_WaitCommand:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;
            }
            //**********************************************************************************************
            if (ioComm_dataAvailable(rn1723) > 0)
            {
				command = (uint8_t)ioObject_read(rn1723);

				if ( (command == CMD_NODE_ON) || (command == CMD_NODE_OFF) )
					gotoState(State_NodeCommand);
				else
					gotoState(State_WaitRegister);
            }
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_NodeCommand:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;
            }
            //**********************************************************************************************
            if (command == CMD_NODE_ON)
            	SetEvent(taskSmartPlug, evRelayOn);
            else if (command == CMD_NODE_OFF)
            	SetEvent(taskSmartPlug, evRelayOff);

            gotoState(State_SendNodeResponse);
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_SendNodeResponse:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

                ioObject_write(rn1723, '#');
                ioObject_write(rn1723, '!');
                ioObject_write(rn1723, 3);
                if (command == CMD_NODE_ON)
                	ioObject_write(rn1723, CMD_RESP_NODE_ON);
                else if (command == CMD_NODE_OFF)
                	ioObject_write(rn1723, CMD_RESP_NODE_OFF);
                ioObject_write(rn1723, '#');
                ioObject_write(rn1723, '!');
            }
            //**********************************************************************************************
            gotoState(State_Idle);
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;

                // Limpia el buffer de recepción de la conexión TCP. Se prepara para recibir una nueva trama.
                ioRN1723_flushRxData(rn1723);

                moduleLog_log("Conexion cerrada");
                SetEvent(taskSmartPlug, evCloseConn);
            }
			break;
			break;



		case State_WaitRegister:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;
            }
            //**********************************************************************************************
            if (ioComm_dataAvailable(rn1723) > 0)
            {
				reg = (uint8_t)ioObject_read(rn1723);

				if ( command == CMD_GET && (reg == REG_PER_HOUR_ACTIVE_POWER || reg == REG_PER_HOUR_ENERGY) )
					gotoState(State_WaitParam);
				else if (command == CMD_GET)
					gotoState(State_ReadEEPROM);
				else if (command == CMD_SET)
					gotoState(State_WaitValue);
				else if ( (command == CMD_RESET) )
					gotoState(State_EraseEEPROM);
            }
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_WaitParam:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

                // Van a llegar 3 bytes que indican la fecha que se debe buscar en la EEPROM.
                counter = 0;
                indexBuffer = 0;
            }
            //**********************************************************************************************
            if (ioComm_dataAvailable(rn1723) > 0)
            {
				byte = (uint8_t)ioObject_read(rn1723);

				buffer[indexBuffer] = byte;
				indexBuffer ++;
				counter ++;

				// Se recibió la fecha (DIA MES AÑO), se la busca en la EEPROM para determinar el bloque que se está buscando.
				if (counter >= 3)
					gotoState(State_SearchDate);

            }
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_SearchDate:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

                blockPointer = 0;
            }
            //**********************************************************************************************
            GetResource(resEEPROM);

            // Se busca el número del bloque de EEPROM que contiene las mediciones pedidas.
            if (reg == REG_PER_HOUR_ACTIVE_POWER)
            {
            	EEAddress = blockPointer * 128 + 64;		// Dirección del campo DIA de la fecha del bloque de mediciones
            	ioEE25LCxxx_busyPolling(eeprom);
            	ioEE25LCxxx_readData(eeprom, EEAddress, &buffEE, 3);

            	if ( (buffer[0] != buffEE[0]) || (buffer[1] != buffEE[1]) || (buffer[2] != buffEE[2]))
            	{
            		blockPointer++;
            		if (blockPointer >= 7)
            		{
            			// Se indica un valor imposible para que los siguientes estados traten este error.
            			blockPointer = 0xFF;
            			gotoState(State_ReadEEPROM);
            		}
            	}
            	else
            	{
            		// Se encontró el bloque buscado
            		gotoState(State_ReadEEPROM);
            	}
            }
            else if (reg == REG_PER_HOUR_ENERGY)
            {
            	EEAddress = blockPointer * 128 + 1088;		// Dirección del campo DIA de la fecha del bloque de mediciones
            	ioEE25LCxxx_busyPolling(eeprom);
            	ioEE25LCxxx_readData(eeprom, EEAddress, &buffEE, 3);

            	if ( (buffer[0] != buffEE[0]) || (buffer[1] != buffEE[1]) || (buffer[2] != buffEE[2]))
            	{
            		blockPointer++;
            		if (blockPointer >= 7)
            		{
            			// Se indica un valor imposible para que los siguientes estados traten este error.
            			blockPointer = 0xFF;
            			gotoState(State_ReadEEPROM);
            		}
            	}
            	else
            	{
            		// Se encontró el bloque buscado
            		gotoState(State_ReadEEPROM);
            	}
            }

            ReleaseResource(resEEPROM);
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_ReadEEPROM:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

                // Se lee la memoria EEPROM.
                counter = readEEPROMbyRegister (eeprom, reg, blockPointer, buffer);
            }
            //**********************************************************************************************
            gotoState(State_SendGetResponse);
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_SendGetResponse:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

                ioObject_write(rn1723, '#');
                ioObject_write(rn1723, '!');
                ioObject_write(rn1723, counter + 4);	// Se agrega el byte del campo COMANDO, el byte del campo REGISTRO y los #! finales.
                ioObject_write(rn1723, CMD_RESP_GET);
                ioObject_write(rn1723, reg);
                ioComm_writeBytes(rn1723, counter, buffer);
                ioObject_write(rn1723, '#');
                ioObject_write(rn1723, '!');
            }
            //**********************************************************************************************
            gotoState(State_Idle);
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;

                // Limpia el buffer de recepción de la conexión TCP. Se prepara para recibir una nueva trama.
                ioRN1723_flushRxData(rn1723);

                moduleLog_log("Conexion cerrada");
                SetEvent(taskSmartPlug, evCloseConn);
            }
			break;

		case State_WaitValue:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

                // Van a llegar (length-4) bytes que se deben guardar en la EEPROM
                counter = 0;
                indexBuffer = 0;
            }
            //**********************************************************************************************
            if (ioComm_dataAvailable(rn1723) > 0)
            {
				byte = (uint8_t)ioObject_read(rn1723);

				buffer[indexBuffer] = byte;
				indexBuffer ++;
				counter ++;

				// Se recibieron los (length - 4) bytes que se deben guardar en la EEPROM
				if ( counter >= (length -4) )
					gotoState(State_WriteEEPROM);

            }
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_WriteEEPROM:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

                // Se escribe la memoria EEPROM.
                writeEEPROMbyRegister (eeprom, reg, counter, buffer);
            }
            //**********************************************************************************************
            gotoState(State_SendSetResponse);
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_SendSetResponse:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

                ioObject_write(rn1723, '#');
                ioObject_write(rn1723, '!');
                ioObject_write(rn1723, 4);
                ioObject_write(rn1723, CMD_RESP_SET);
                ioObject_write(rn1723, reg);
                ioObject_write(rn1723, '#');
                ioObject_write(rn1723, '!');
            }
            //**********************************************************************************************
            gotoState(State_Idle);
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;

                // Limpia el buffer de recepción de la conexión TCP. Se prepara para recibir una nueva trama.
                ioRN1723_flushRxData(rn1723);

                moduleLog_log("Conexion cerrada");
                SetEvent(taskSmartPlug, evCloseConn);
            }
			break;

		case State_EraseEEPROM:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

                // Se carga con ceros buffer
                for (i = 0; i < BUFFER_LEN; i++)
                	buffer[i] = 0;

                // Se borra la EEPROM
                eraseEEPROMbyRegister (eeprom, reg, buffer);
            }
            //**********************************************************************************************
            gotoState(State_SendResetResponse);
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_SendResetResponse:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

                ioObject_write(rn1723, '#');
                ioObject_write(rn1723, '!');
                ioObject_write(rn1723, 4);
                ioObject_write(rn1723, CMD_RESP_RESET);
                ioObject_write(rn1723, reg);
                ioObject_write(rn1723, '#');
                ioObject_write(rn1723, '!');
            }
            //**********************************************************************************************
            gotoState(State_Idle);
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;

                // Limpia el buffer de recepción de la conexión TCP. Se prepara para recibir una nueva trama.
                ioRN1723_flushRxData(rn1723);

                moduleLog_log("Conexion cerrada");
                SetEvent(taskSmartPlug, evCloseConn);
            }
			break;

		case State_WaitingWPS:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

                ioRN1723_runWPS(rn1723, 3);
            }
            //**********************************************************************************************
            if (ioRN1723_isIdle(rn1723) == 1)
            	gotoState(State_Idle);
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;

                if (ioRN1723_isAuthenticated(rn1723))
                {
                	SetEvent (taskSmartPlug, evAuthenticated);
                	moduleLog_log("Unido a WiFi");
                }
                else
                	SetEvent (taskSmartPlug, evDeAuthenticated);
            }
			break;

		case State_WaitingWebServer:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

                ioRN1723_runConfigWebServer(rn1723);
            }
            //**********************************************************************************************
            if (ioRN1723_isIdle(rn1723) == 1)
            	gotoState(State_Idle);
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;

                if (ioRN1723_isAuthenticated(rn1723))
                {
                	SetEvent (taskSmartPlug, evAuthenticated);
                	moduleLog_log("Unido a WiFi");
                }
                else
                	SetEvent (taskSmartPlug, evDeAuthenticated);
            }
			break;

		case State_ConfigureiD:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

                ioRN1723_setDeviceID(rn1723, bufferID);
            }
            //**********************************************************************************************
            if (ioRN1723_isIdle(rn1723) == 1)
            	gotoState(State_Idle);
            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;
	}

	TerminateTask();
}

void gotoState (uint32_t newState)
{
    state = newState;
    stateOut = 1;
}


uint32_t readEEPROMbyRegister (void* ee, uint8_t regEE, uint8_t bPointer, uint8_t* buff)
{
	uint32_t bytesRead = 0;
	float floatValue;
	rtc_time_t fullTIme;


	if (regEE == REG_V_RMS)
	{
		floatValue = taskMeter_getMeterValue(ID_VRMS);
		float2Bytes(buff, floatValue);
		bytesRead = 4;
	}
	else if (regEE == REG_I_RMS)
	{
		floatValue = taskMeter_getMeterValue(ID_IRMS);
		float2Bytes(buff, floatValue);
		bytesRead = 4;
	}
	else if (regEE == REG_POWER_FACTOR)
	{
		floatValue = taskMeter_getMeterValue(ID_POWER_FACTOR);
		float2Bytes(buff, floatValue);
		bytesRead = 4;
	}
	else if (regEE == REG_FREQUENCY)
	{
		floatValue = taskMeter_getMeterValue(ID_FREQUENCY);
		float2Bytes(buff, floatValue);
		bytesRead = 4;
	}
	else if (regEE == REG_ACTIVE_POWER)
	{
		floatValue = taskMeter_getMeterValue(ID_ACTIVE_POWER);
		float2Bytes(buff, floatValue);
		bytesRead = 4;
	}
	else if (regEE == REG_CURRENT_HOUR_ENERGY)
	{
		floatValue = taskMeter_getMeterValue(ID_IRMS);
		float2Bytes(buff, floatValue);
		bytesRead = 4;
	}
	else if (regEE == REG_LOAD_STATE)
	{
		buff[0] = taskSmartPlug_getLoadState();
		bytesRead = 1;
	}
	else
	{
		GetResource(resEEPROM);


		if (regEE == REG_TOTAL_ENERGY)
		{
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ACUM_ENERGY, buff, 4);
			bytesRead = 4;
		}
		else if (regEE == REG_DEVICE_ID)
		{
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_DEVICE_ID, buff, 33);
			bytesRead = 33;
		}
		else if (regEE == REG_DEVICE_ID)
		{
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_DEVICE_ID, buff, 33);
			bytesRead = 33;
		}
		else if (regEE == REG_MONDAY_LOAD_ON_TIME)
		{
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_MONDAY_LOAD_ON_TIME, buff, 2);
			bytesRead = 2;
		}
		else if (regEE == REG_MONDAY_LOAD_OFF_TIME)
		{
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_MONDAY_LOAD_OFF_TIME, buff, 2);
			bytesRead = 2;
		}
		else if (regEE == REG_TUESDAY_LOAD_ON_TIME)
		{
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_TUESDAY_LOAD_ON_TIME, buff, 2);
			bytesRead = 2;
		}
		else if (regEE == REG_TUESDAY_LOAD_OFF_TIME)
		{
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_TUESDAY_LOAD_OFF_TIME, buff, 2);
			bytesRead = 2;
		}
		else if (regEE == REG_WEDNESDAY_LOAD_ON_TIME)
		{
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_WEDNESDAY_LOAD_ON_TIME, buff, 2);
			bytesRead = 2;
		}
		else if (regEE == REG_WEDNESDAY_LOAD_OFF_TIME)
		{
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_WEDNESDAY_LOAD_OFF_TIME, buff, 2);
			bytesRead = 2;
		}
		else if (regEE == REG_THURSDAY_LOAD_ON_TIME)
		{
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_THURSDAY_LOAD_ON_TIME, buff, 2);
			bytesRead = 2;
		}
		else if (regEE == REG_THURSDAY_LOAD_OFF_TIME)
		{
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_THURSDAY_LOAD_OFF_TIME, buff, 2);
			bytesRead = 2;
		}
		else if (regEE == REG_FRIDAY_LOAD_ON_TIME)
		{
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_FRIDAY_LOAD_ON_TIME, buff, 2);
			bytesRead = 2;
		}
		else if (regEE == REG_FRIDAY_LOAD_OFF_TIME)
		{
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_FRIDAY_LOAD_OFF_TIME, buff, 2);
			bytesRead = 2;
		}
		else if (regEE == REG_SATURDAY_LOAD_ON_TIME)
		{
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_SATURDAY_LOAD_ON_TIME, buff, 2);
			bytesRead = 2;
		}
		else if (regEE == REG_SATURDAY_LOAD_OFF_TIME)
		{
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_SATURDAY_LOAD_OFF_TIME, buff, 2);
			bytesRead = 2;
		}
		else if (regEE == REG_SUNDAY_LOAD_ON_TIME)
		{
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_SUNDAY_LOAD_ON_TIME, buff, 2);
			bytesRead = 2;
		}
		else if (regEE == REG_SUNDAY_LOAD_OFF_TIME)
		{
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_SUNDAY_LOAD_OFF_TIME, buff, 2);
			bytesRead = 2;
		}
		else if (regEE == REG_ENABLE_ONOFF_TIME)
		{
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, buff, 1);
			bytesRead = 1;
		}
		else
		{
			if (blockPointer != 0xFF)
			{
				// Se encontró el bloque correspondiente a la fecha buscada.
				if (regEE == REG_PER_HOUR_ACTIVE_POWER)
				{
					ioEE25LCxxx_busyPolling(ee);
					ioEE25LCxxx_readData(ee, blockPointer * 128 + EE_ACTIVE_POWER_HOUR_00, buff, 48);			// Primeras 12 horas del día
					ioEE25LCxxx_busyPolling(ee);
					ioEE25LCxxx_readData(ee, blockPointer * 128 + EE_ACTIVE_POWER_HOUR_12, &buff[48], 48);		// Últimas 12 horas del día
					bytesRead = 96;
				}
				else if (regEE == REG_PER_HOUR_ENERGY)
				{
					ioEE25LCxxx_busyPolling(ee);
					ioEE25LCxxx_readData(ee, blockPointer * 128 + EE_ENERGY_HOUR_00, buff, 48);			// Primeras 12 horas del día
					ioEE25LCxxx_busyPolling(ee);
					ioEE25LCxxx_readData(ee, blockPointer * 128 + EE_ENERGY_HOUR_12, &buff[48], 48);		// Últimas 12 horas del día
					bytesRead = 96;
				}
			}
			else
			{
				// No se encontró el bloque. Se devuelve vacío
				bytesRead = 0;
			}

		}

		ReleaseResource(resEEPROM);
	}



	return bytesRead;
}

void writeEEPROMbyRegister (void* ee, uint8_t regEE, uint8_t count, uint8_t* buff)
{
	uint8_t enabledTimes;

	if (regEE == REG_LOAD_STATE)
	{
		// Llega un único byte que indica si se debe encender o apagar la carga.
		if (buff[0] == 0)
			SetEvent(taskSmartPlug, evRelayOff);
		else if (buff[0] == 1)
			SetEvent(taskSmartPlug, evRelayOn);
	}
	else
	{
		GetResource(resEEPROM);


		if (regEE == REG_DEVICE_ID)
		{
			// Siempre llegan 33 bytes, aunque el nombre sea más corto.
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_DEVICE_ID, buff, 33);
		}
		else if(regEE == REG_MONDAY_LOAD_ON_TIME)
		{
			// Llegan 2 bytes: horas y minutos. Se debe habilitar la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes |= (1 << 1);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_MONDAY_LOAD_ON_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_MONDAY_LOAD_OFF_TIME)
		{
			// Llegan 2 bytes: horas y minutos. Se debe habilitar la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes |= (1 << 1);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_MONDAY_LOAD_OFF_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_TUESDAY_LOAD_ON_TIME)
		{
			// Llegan 2 bytes: horas y minutos. Se debe habilitar la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes |= (1 << 2);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_TUESDAY_LOAD_ON_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_TUESDAY_LOAD_OFF_TIME)
		{
			// Llegan 2 bytes: horas y minutos. Se debe habilitar la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes |= (1 << 2);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_TUESDAY_LOAD_OFF_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_WEDNESDAY_LOAD_ON_TIME)
		{
			// Llegan 2 bytes: horas y minutos. Se debe habilitar la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes |= (1 << 3);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_WEDNESDAY_LOAD_ON_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_WEDNESDAY_LOAD_OFF_TIME)
		{
			// Llegan 2 bytes: horas y minutos. Se debe habilitar la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes |= (1 << 3);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_WEDNESDAY_LOAD_OFF_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_THURSDAY_LOAD_ON_TIME)
		{
			// Llegan 2 bytes: horas y minutos. Se debe habilitar la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes |= (1 << 4);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_THURSDAY_LOAD_ON_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_THURSDAY_LOAD_OFF_TIME)
		{
			// Llegan 2 bytes: horas y minutos. Se debe habilitar la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes |= (1 << 4);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_THURSDAY_LOAD_OFF_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_FRIDAY_LOAD_ON_TIME)
		{
			// Llegan 2 bytes: horas y minutos. Se debe habilitar la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes |= (1 << 5);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_FRIDAY_LOAD_ON_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_FRIDAY_LOAD_OFF_TIME)
		{
			// Llegan 2 bytes: horas y minutos. Se debe habilitar la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes |= (1 << 5);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_FRIDAY_LOAD_OFF_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_SATURDAY_LOAD_ON_TIME)
		{
			// Llegan 2 bytes: horas y minutos. Se debe habilitar la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes |= (1 << 6);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_SATURDAY_LOAD_ON_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_SATURDAY_LOAD_OFF_TIME)
		{
			// Llegan 2 bytes: horas y minutos. Se debe habilitar la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes |= (1 << 6);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_SATURDAY_LOAD_OFF_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_SUNDAY_LOAD_ON_TIME)
		{
			// Llegan 2 bytes: horas y minutos. Se debe habilitar la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes |= (1 << 0);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_SUNDAY_LOAD_ON_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_SUNDAY_LOAD_OFF_TIME)
		{
			// Llegan 2 bytes: horas y minutos. Se debe habilitar la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes |= (1 << 0);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_SUNDAY_LOAD_OFF_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}

		ReleaseResource(resEEPROM);
	}
}


void eraseEEPROMbyRegister (void* ee, uint8_t regEE, uint8_t* buff)
{
	uint8_t i;
	uint8_t enabledTimes;


	if (regEE == REG_ALL_REGISTERS)
	{
		// Se genera el evento para que taskSmartPlug vuelva a valores de fábrica.
		SetEvent(taskSmartPlug, evFactoryReset);
	}
	else
	{
		GetResource(resEEPROM);


		if (regEE == REG_DEVICE_ID)
		{
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_DEVICE_ID, buff, 33);
		}
		else if (regEE == REG_TOTAL_ENERGY)
		{
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ACUM_ENERGY, buff, 4);
		}
		else if(regEE == REG_MONDAY_LOAD_ON_TIME)
		{
			// Se deshabilita la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes &= ~(1 << 1);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_MONDAY_LOAD_ON_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_MONDAY_LOAD_OFF_TIME)
		{
			// Se deshabilita la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes &= ~(1 << 1);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_MONDAY_LOAD_OFF_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_TUESDAY_LOAD_ON_TIME)
		{
			// Se deshabilita la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes &= ~(1 << 2);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_TUESDAY_LOAD_ON_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_TUESDAY_LOAD_OFF_TIME)
		{
			// Se deshabilita la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes &= ~(1 << 2);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_TUESDAY_LOAD_OFF_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_WEDNESDAY_LOAD_ON_TIME)
		{		// Se deshabilita la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes &= ~(1 << 3);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_WEDNESDAY_LOAD_ON_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_WEDNESDAY_LOAD_OFF_TIME)
		{
			// Se deshabilita la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes &= ~(1 << 3);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_WEDNESDAY_LOAD_OFF_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_THURSDAY_LOAD_ON_TIME)
		{
			// Se deshabilita la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes &= ~(1 << 4);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_THURSDAY_LOAD_ON_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_THURSDAY_LOAD_OFF_TIME)
		{
			// Se deshabilita la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes &= ~(1 << 4);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_THURSDAY_LOAD_OFF_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_FRIDAY_LOAD_ON_TIME)
		{
			// Se deshabilita la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes &= ~(1 << 5);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_FRIDAY_LOAD_ON_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_FRIDAY_LOAD_OFF_TIME)
		{
			// Se deshabilita la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes &= ~(1 << 5);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_FRIDAY_LOAD_OFF_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_SATURDAY_LOAD_ON_TIME)
		{
			// Se deshabilita la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes &= ~(1 << 6);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_SATURDAY_LOAD_ON_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_SATURDAY_LOAD_OFF_TIME)
		{
			// Se deshabilita la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes &= ~(1 << 6);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_SATURDAY_LOAD_OFF_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_SUNDAY_LOAD_ON_TIME)
		{
			// Se deshabilita la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes &= ~(1 << 0);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_SUNDAY_LOAD_ON_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_SUNDAY_LOAD_OFF_TIME)
		{
			// Se deshabilita la programación horaria
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_readData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);
			enabledTimes &= ~(1 << 0);
			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_ENABLE_ONOFF_TIME, &enabledTimes, 1);

			ioEE25LCxxx_busyPolling(ee);
			ioEE25LCxxx_setWriteEnable(ee);
			ioEE25LCxxx_writeData(ee, EE_SUNDAY_LOAD_OFF_TIME, buff, 2);
			SetEvent(taskSmartPlug, evChangeOnOffTime);
		}
		else if(regEE == REG_PER_HOUR_ENERGY)
		{
			for (i = 0; i < 7; i ++)
			{
				ioEE25LCxxx_busyPolling(ee);
				ioEE25LCxxx_setWriteEnable(ee);
				ioEE25LCxxx_writeData(ee, i * 128 + EE_ENERGY_HOUR_00, buff, 48);			// Primeras 12 horas del día
				ioEE25LCxxx_busyPolling(ee);
				ioEE25LCxxx_setWriteEnable(ee);
				ioEE25LCxxx_writeData(ee, i * 128 + EE_ENERGY_HOUR_12, &buff[48], 48);	// Últimas 12 horas del día
			}
		}
		else if(regEE == REG_PER_HOUR_ACTIVE_POWER)
		{
			for (i = 0; i < 7; i ++)
			{
				ioEE25LCxxx_busyPolling(ee);
				ioEE25LCxxx_setWriteEnable(ee);
				ioEE25LCxxx_writeData(ee, i * 128 + EE_ACTIVE_POWER_HOUR_00, buff, 48);			// Primeras 12 horas del día
				ioEE25LCxxx_busyPolling(ee);
				ioEE25LCxxx_setWriteEnable(ee);
				ioEE25LCxxx_writeData(ee, i * 128 + EE_ACTIVE_POWER_HOUR_12, &buff[48], 48);		// Últimas 12 horas del día
			}
		}


		ReleaseResource(resEEPROM);
	}
}


void float2Bytes (uint8_t* bytes, float floatVariable)
{
	uint8_t i;

	for (i = 0; i < 4; i ++)
		bytes[i] = ((uint8_t*)&floatVariable)[3-i];
}




ISR(UART2_handler)
{
	if (ioComm_getInt(uartRN1723, IOUART_INT_ID_TX))
	{
		ioUART_txHandler(uartRN1723);
	}
	else if(ioComm_getInt(uartRN1723, IOUART_INT_ID_RX))
	{
		ioUART_rxHandler(uartRN1723);
	}
}
