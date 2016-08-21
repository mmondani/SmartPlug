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
#include "ioRN1723.h"
#include "ioUART.h"
#include "cQueue.h"


void* eeprom;
void* rn1723;
void* gpioResetRN1723;
void* uartRN1723;
void* inBuffer;
void* outBuffer;

enum {State_Create = 0, State_Waiting4Ready, State_Init, State_Idle, State_SynchronizeTime,
       State_WaitFrameStart1, State_WaitFrameStart2, State_WaitFrameLength, State_WaitCommand,
       State_NodeCommand, State_WaitFrameEnd1, State_WaitFrameEnd2, State_WaitRegister,
       State_ReadEEPROM, State_SendGetResponse, State_WaitValue, State_WriteEEPROM,
       State_EraseEEPROM, State_WaitingWPS, State_WaitingWebServer};
uint32_t state = State_Create;
uint8_t stateIn = 0, stateOut = 0;
uint8_t executeWPS = 0;
uint8_t executeWebServer = 0;

// Cambia de estado en la FSM.
void gotoState (uin32_t newState);





void taskWiFi_init (void* _eeprom)
{
	eeprom = _eeprom;

	// La tarea taskWiFi se va a ejecutar cada 10 ms.
	SetRelAlarm(alarmRunTaskWiFi, 5, 10);
}


void taskLeds_initWPS (void)
{
	executeWPS = 1;
}


void taskLeds_initWebServer (void)
{
	executeWebServer = 1;
}


void taskLeds_isAuthenticated (void)
{
	if (ioRN1723_isAuthenticated(rn1723))
		return 1;
	else
		return 0;
}


void taskLeds_isTPCConnected (void)
{
	if (ioRN1723_isTCPConnected(rn1723))
		return 1;
	else
		return 0;
}


TASK(taskWiFi)
{
	cTimer_handler();

	switch (state)
	{
		case State_Create:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

            }
            //**********************************************************************************************


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

            }
            //**********************************************************************************************


            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_Idle:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

            }
            //**********************************************************************************************


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

            }
            //**********************************************************************************************


            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_WaitFrameStart1:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

            }
            //**********************************************************************************************


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

            }
            //**********************************************************************************************


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


            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_WaitFrameEnd1:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

            }
            //**********************************************************************************************


            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_WaitFrameEnd2:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

            }
            //**********************************************************************************************


            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_WaitRegister:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

            }
            //**********************************************************************************************


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

            }
            //**********************************************************************************************


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

            }
            //**********************************************************************************************


            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_WaitValue:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

            }
            //**********************************************************************************************


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

            }
            //**********************************************************************************************


            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_EraseEEPROM:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

            }
            //**********************************************************************************************


            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_WaitingWPS:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

            }
            //**********************************************************************************************


            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;

		case State_WaitingWebServer:
            if (stateIn)
            {
                stateIn = 0;
                stateOut = 0;

            }
            //**********************************************************************************************


            //**********************************************************************************************
            if (stateOut)
            {
                stateOut = 0;
                stateIn = 1;
            }
			break;
	}
}

void gotoState (uin32_t newState)
{
    state = newState;
    stateOut = 1;
}



ISR(UART2_handler)
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
