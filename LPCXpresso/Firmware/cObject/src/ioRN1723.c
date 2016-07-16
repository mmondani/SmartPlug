/**
 * @file ioRN1723.c
 * @author Mariano Mondani
 *
 * @brief Implementación de los métodos de la clase ioRN1723.
 */

#include "ioRN1723.h"
#include "ioRN1723_r.h"

#include <stdlib.h>
#include <time.h>
#include "cTimer.h"



static void* ioRN1723_ctor  (void* _this, va_list* va);
static void* ioRN1723_dtor (void* _this);
static uint32_t ioRN1723_differ (void* _this, void* _dst);
static void ioRN1723_display (void* _this);
static void* ioRN1723_copy (void* _this, void* _src);
static uint32_t ioRN1723_init (void* _this);
static uint32_t ioRN1723_deInit (void* _this);
static uint32_t ioRN1723_enable (void* _this);
static uint32_t ioRN1723_disable (void* _this);
static uint32_t ioRN1723_read (void* _this);
static uint32_t ioRN1723_write (void* _this, uint32_t data);
static uint32_t ioRN1723_writeBytes (void* _this, uint32_t len, uint8_t* data);
static uint32_t ioRN1723_readBytes (void* _this, uint32_t len, uint8_t* data);
static uint32_t ioRN1723_freeSpace (void* _this);
static uint32_t ioRN1723_dataAvailable (void* _this);
static void ioRN1723_intEnable (void* _this, uint32_t mask);
static void ioRN1723_intDisable (void* _this, uint32_t mask);
static uint32_t ioRN1723_getInt (void* _this, uint32_t intID);


// ********************************************************************************
// Declaración de la clase cStaticBuffer
// ********************************************************************************
static const struct ioComm _ioRN1723 = { { {sizeof(struct ioRN1723),
										ioRN1723_ctor,
										ioRN1723_dtor,
										ioRN1723_differ,
										ioRN1723_display,
										ioRN1723_copy },
										ioRN1723_init,
										ioRN1723_deInit,
										ioRN1723_enable,
										ioRN1723_disable,
										ioRN1723_read,
										ioRN1723_write },
										ioRN1723_writeBytes,
										ioRN1723_readBytes,
										ioRN1723_freeSpace,
										ioRN1723_dataAvailable,
										ioRN1723_intEnable,
										ioRN1723_intDisable,
										ioRN1723_getInt };

const void* ioRN1723 = &_ioRN1723;
// ********************************************************************************


// ********************************************************************************
// Funciones privadas
// ********************************************************************************
void processRX (void* _this);
void sendCmd (void* _this, const uint8_t* cmd, uint8_t* params, uint32_t answerFilter, uint32_t timeout);
// ********************************************************************************


// ********************************************************************************
// Definiciones de los comandos
// ********************************************************************************
const uint8_t* cmdWLANJoin = "set wlan join";
const uint8_t* cmdSysIOfunc = "set sys iofunc";
const uint8_t* cmdCmdMode = "$$$";
const uint8_t* cmdExit = "exit";
const uint8_t* cmdSave = "save";
const uint8_t* cmdRunWPS = "run wps";
const uint8_t* cmdLeave = "leave";
const uint8_t* cmdOpen = "open";
const uint8_t* cmdClose = "close";
const uint8_t* cmdSetSNTPIP = "set time address";
const uint8_t* cmdSetTimeZone = "set time zone";
const uint8_t* cmdTime = "time";
const uint8_t* cmdSetHeartbeatPort = "set broadcast port";
const uint8_t* cmdSetHeartbeatInterval = "set broadcast interval";
const uint8_t* cmdSetTCPServerPort = "set ip localport";
const uint8_t* cmdReboot = "reboot";
const uint8_t* cmdSetDeviceID = "set opt device_id";
const uint8_t* cmdShowTT = "show t t";
const uint8_t* cmdSetTimeEnable = "set time enable";


#define CMD_WLAN_JOIN				cmdWLANJoin
#define CMD_SYS_IOFUNC				cmdSysIOfunc
#define CMD_CMD_MODE				cmdCmdMode
#define CMD_EXIT					cmdExit
#define CMD_SAVE					cmdSave
#define CMD_RUN_WPS					cmdRunWPS
#define CMD_LEAVE					cmdLeave
#define CMD_OPEN					cmdOpen
#define CMD_CLOSE					cmdClose
#define CMD_SET_SNTP_IP				cmdSetSNTPIP
#define CMD_SET_TIME_ZONE			cmdSetTimeZone
#define CMD_TIME					cmdTime
#define CMD_SET_HEARTBEAT_PORT		cmdSetHeartbeatPort
#define CMD_SET_HEARTBEAT_INTERVAL	cmdSetHeartbeatInterval
#define CMD_SET_TCP_SERVER_PORT		cmdSetTCPServerPort
#define CMD_REBOOT					cmdReboot
#define CMD_SET_DEVICE_ID			cmdSetDeviceID
#define CMD_SHOW_T_T				cmdShowTT
#define CMD_SET_TIME_ENABLE			cmdSetTimeEnable

// ********************************************************************************

// ********************************************************************************
// Definiciones de las respuestas del módulo
// ********************************************************************************
#define RN_RESPONSES_COUNT		14
uint8_t* rnResponses[RN_RESPONSES_COUNT] = {
												"OK",
												"ERR",
												"WPS-SUCCESS!",
												"Associated!",
												"*OPEN*",
												"*CLOS*",
												"CMD",
												"EXIT",
												"Storing in config",
												"WPS-FAILED",
												"*READY*",
												"DeAuth",
												"Connect FAILED",
												"RTC="
};

uint32_t rnResponsesLength[RN_RESPONSES_COUNT] = {
												2,
												3,
												12,
												11,
												6,
												6,
												3,
												4,
												17,
												10,
												7,
												6,
												14,
												4
};

#define RESP_OK						0
#define RESP_ERROR					1
#define RESP_WPS_SUCCESS			2
#define RESP_ASSOCIATED				3
#define RESP_OPEN					4
#define RESP_CLOSE					5
#define RESP_CMD					6
#define	 RESP_EXIT					7
#define	 RESP_SAVE_OK				8
#define	 RESP_WPS_FAILED			9
#define	 RESP_READY					10
#define	 RESP_DEAUTH				11
#define	 RESP_CONNECT_FAILED		12
#define	 RESP_RTC					13

#define RESP_FILTER_OK					(1 << 0)
#define RESP_FILTER_ERROR				(1 << 1)
#define RESP_FILTER_WPS_SUCCESS			(1 << 2)
#define RESP_FILTER_ASSOCIATED			(1 << 3)
#define RESP_FILTER_OPEN				(1 << 4)
#define RESP_FILTER_CLOSE				(1 << 5)
#define RESP_FILTER_CMD					(1 << 6)
#define	 RESP_FILTER_EXIT				(1 << 7)
#define	 RESP_FILTER_SAVE_OK			(1 << 8)
#define	 RESP_FILTER_WPS_FAILED			(1 << 9)
#define	 RESP_FILTER_READY				(1 << 10)
#define	 RESP_FILTER_DEAUTH				(1 << 11)
#define	 RESP_FILTER_CONNECT_FAILED		(1 << 12)
#define	 RESP_FILTER_RTC				(1 << 13)

// ********************************************************************************


// ********************************************************************************
// Definiciones de los eventos
// ********************************************************************************
#define EV_OK_RECEIVED					0x00000001
#define EV_ERR_RECEIVED					0x00000002
#define EV_CMD_RECEIVED					0x00000004
#define EV_RESP_RECEIVED				0x00000008
#define EV_EXIT_RECEIVED				0x00000010
#define EV_OPEN_RECEIVED				0x00000020
#define EV_CLOSE_RECEIVED				0x00000040
#define EV_WPS_SUCCESS_RECEIVED			0x00000080
#define EV_ASSOCIATED_RECEIVED			0x00000100
#define EV_SAVE_OK_RECEIVED				0x00000200
#define EV_WPS_FAILED_RECEIVED			0x00000400
#define EV_READY_RECEIVED				0x00000800
#define EV_DEAUTH_RECEIVED				0x00001000
#define EV_CONNECT_FAILED_RECEIVED		0x00002000
#define EV_RTC_RECEIVED					0x00004000


#define ev_isTriggered(v,e)		(((v & e) == 0)? 0 : 1)
#define ev_emit(v,e)			(v |= e)
#define ev_flush(v)				(v = 0)
// ********************************************************************************


// ********************************************************************************
// Definiciones de los estados
// ********************************************************************************
#define FSM_NO_SUBSTATE			0xFFFFFFFF

enum {
	FSM_WAIT4READY = 0x000,
	FSM_CONFIG = 0x100,
	FSM_IDLE = 0x200,
	FSM_SENDING_CMD = 0x300,
	FSM_WAIT4ANSWER = 0x400,
	FSM_WPS_WAIT4REBOOT = 0x500,
	FSM_LEAVE_NETWORK = 0x600,
	FSM_CLOSE_TCP = 0x700,
	FSM_SET_SNTP_SERVER = 0x800,
	FSM_SET_TIME_ZONE = 0x900,
	FSM_SYNC_TIME = 0xA00,
	FSM_SET_HEARTBEAT_PORT = 0xB00,
	FSM_SET_HEARTBEAT_INTERVAL = 0xC00,
	FSM_SET_TCP_SERVER_PORT = 0xD00,
	FSM_SET_DEVICE_ID = 0xE00,
	FSM_GET_TIME = 0xF00
};

enum {
	CONFIG_WLAN_JOIN = FSM_CONFIG | 1,
	CONFIG_IOFUNC = FSM_CONFIG | 2,
	CONFIG_SAVE = FSM_CONFIG | 3,
	CONFIG_TIME_ENABLE = FSM_CONFIG | 4,
	CONFIG_EXIT = FSM_CONFIG | 5,
	LEAVE_NETWORK_CMD = FSM_LEAVE_NETWORK | 1,
	LEAVE_NETWORK_EXIT = FSM_LEAVE_NETWORK | 2,
	CLOSE_TCP_CMD = FSM_CLOSE_TCP | 1,
	CLOSE_TCP_EXIT = FSM_CLOSE_TCP | 2,
	SET_SNTP_SERVER_CMD = FSM_SET_SNTP_SERVER | 1,
	SET_SNTP_SERVER_SAVE = FSM_SET_SNTP_SERVER | 2,
	SET_SNTP_SERVER_EXIT = FSM_SET_SNTP_SERVER | 3,
	SET_TIME_ZONE_CMD = FSM_SET_TIME_ZONE | 1,
	SET_TIME_ZONE_SAVE = FSM_SET_TIME_ZONE | 2,
	SET_TIME_ZONE_EXIT = FSM_SET_TIME_ZONE | 3,
	SYNC_TIME_CMD = FSM_SYNC_TIME | 1,
	SYNC_TIME_SAVE = FSM_SYNC_TIME | 2,
	SYNC_TIME_EXIT = FSM_SYNC_TIME | 3,
	SET_HEARTBEAT_PORT_CMD = FSM_SET_HEARTBEAT_PORT | 1,
	SET_HEARTBEAT_PORT_SAVE = FSM_SET_HEARTBEAT_PORT | 2,
	SET_HEARTBEAT_PORT_EXIT = FSM_SET_HEARTBEAT_PORT | 3,
	SET_HEARTBEAT_INTERVAL_CMD = FSM_SET_HEARTBEAT_INTERVAL | 1,
	SET_HEARTBEAT_INTERVAL_SAVE = FSM_SET_HEARTBEAT_INTERVAL | 2,
	SET_HEARTBEAT_INTERVAL_EXIT = FSM_SET_HEARTBEAT_INTERVAL | 3,
	SET_TCP_SERVER_PORT_CMD = FSM_SET_TCP_SERVER_PORT | 1,
	SET_TCP_SERVER_PORT_SAVE = FSM_SET_TCP_SERVER_PORT | 2,
	SET_TCP_SERVER_PORT_EXIT = FSM_SET_TCP_SERVER_PORT | 3,
	SET_DEVICE_ID_CMD = FSM_SET_DEVICE_ID | 1,
	SET_DEVICE_ID_SAVE = FSM_SET_DEVICE_ID | 2,
	SET_DEVICE_ID_EXIT = FSM_SET_DEVICE_ID | 3,
	GET_TIME_CMD = FSM_GET_TIME | 1,
	GET_TIME_EXIT = FSM_GET_TIME | 2
};
// ********************************************************************************



static void* ioRN1723_ctor  (void* _this, va_list* va)
{
	struct ioRN1723* this = _this;


	this->uart = va_arg(*va, void*);
	this->gpioReset = va_arg(*va, void*);
	this->inBuffer = va_arg(*va, void*);
	this->outBuffer = va_arg(*va, void*);

	this->cmdBuffer = cObject_new(cQueue, 40, sizeof(char));
	this->timer = cObject_new(cTimer);

	this->authenticated = 0;
	this->tcpConnected = 0;
	this->fsm_state = FSM_WAIT4READY;
	this->fsm_sub_state = FSM_NO_SUBSTATE;
	this->events = 0;
	this->responseIndex = 0;
	this->cmdMode = 0;
	this->answerFilter = 0;
	this->possibleResponses = 0xFFFFFFFF;
	this->indexSerial = 0;
	this->timeZone = -3;


	return this;
}


static void* ioRN1723_dtor (void* _this)
{
	struct ioRN1723* this = _this;

	cObject_delete(this->cmdBuffer);
	cObject_delete(this->timer);

	return _this;
}


static uint32_t ioRN1723_differ (void* _this, void* _dst)
{
	struct ioRN1723* this = _this;
	struct ioRN1723* dst = _dst;

	return ( cObject_differ(uart(this), uart(dst)) || cObject_differ(gpioReset(this), gpioReset(dst)) ||
			(this->authenticated != dst->authenticated) || (this->tcpConnected != dst->tcpConnected) );
}


static void ioRN1723_display (void* _this)
{

}


static void* ioRN1723_copy (void* _this, void* _src)
{
	struct ioRN1723* this = _this;
	struct ioRN1723* src = src;

	cObject_copy(uart(this), uart(src));
	cObject_copy(gpioReset(this), gpioReset(src));
	this->authenticated = src->authenticated;
	this->tcpConnected = src->tcpConnected;


	return this;
}


static uint32_t ioRN1723_init (void* _this)
{
	struct ioRN1723* this = _this;
	uint32_t res = 0;


	this->fsm_state = FSM_CONFIG;
	this->fsm_sub_state = CONFIG_WLAN_JOIN;

	return res;
}


static uint32_t ioRN1723_deInit (void* _this)
{

	return 0;
}


static uint32_t ioRN1723_enable (void* _this)
{

	return 0;
}


static uint32_t ioRN1723_disable (void* _this)
{

	return 0;
}


static uint32_t ioRN1723_read (void* _this)
{
	struct ioRN1723* this = _this;
	uint8_t data = 0;

	if (cBuffer_getPending(inBuffer(this)) > 0)
	{
		cBuffer_remove(inBuffer(this), &data);
	}

	return data;
}


static uint32_t ioRN1723_write (void* _this, uint32_t data)
{
	struct ioRN1723* this = _this;

	if (cBuffer_getFreeSpace(outBuffer(this)) > 0)
	{
		cBuffer_put(outBuffer(this), &data);
		return 0;
	}
	else
		return 1;
}


static uint32_t ioRN1723_writeBytes (void* _this, uint32_t len, uint8_t* data)
{
	struct ioRN1723* this = _this;
	uint32_t i;
	uint8_t c;

	for (i = 0; i < len; i ++)
	{
		c = *data;
		if (ioRN1723_write(this, c) == 0)
			data ++;
		else
			break;
	}


	this->fsm_state = FSM_IDLE;

	return i;
}


static uint32_t ioRN1723_readBytes (void* _this, uint32_t len, uint8_t* data)
{
	struct ioRN1723* this = _this;
	uint32_t bytes2read;
	uint32_t i;


	bytes2read = cBuffer_getPending(inBuffer(this));
	if (bytes2read > len)
		bytes2read = len;

	for (i = 0; i < bytes2read; i ++)
	{
		*data = (uint8_t)ioRN1723_read(this);
		data++;
	}


	return bytes2read;
}


static uint32_t ioRN1723_freeSpace(void* _this)
{
	struct ioRN1723* this = _this;

	return cBuffer_getFreeSpace(outBuffer(this));
}


static uint32_t ioRN1723_dataAvailable (void* _this)
{
	struct ioRN1723* this = _this;

	return cBuffer_getPending(inBuffer(this));
}


static void ioRN1723_intEnable (void* _this, uint32_t mask)
{

}


static void ioRN1723_intDisable (void* _this, uint32_t mask)
{

}


static uint32_t ioRN1723_getInt (void* _this, uint32_t intID)
{
	return 0;
}




void ioRN1723_handler (void* _this)
{
	struct ioRN1723* this = _this;
	uint32_t i, count;
	uint8_t data;

	ioObject_write(gpioReset(this), 1);

	processRX(this);

	switch (this->fsm_state)
	{
		case FSM_WAIT4READY:
			if (ev_isTriggered(this->events, EV_READY_RECEIVED))
			{
				this->fsm_state = FSM_IDLE;
			}

			break;

		case FSM_CONFIG:
				switch(this->fsm_sub_state)
				{
					case CONFIG_WLAN_JOIN:
						sendCmd(this, CMD_WLAN_JOIN, "1", RESP_FILTER_OK | RESP_FILTER_ERROR, 2000);
						this->fsm_sub_state = CONFIG_IOFUNC;
						break;

					case CONFIG_IOFUNC:
						sendCmd(this, CMD_SYS_IOFUNC, "0x50", RESP_FILTER_OK | RESP_FILTER_ERROR, 2000);
						this->fsm_sub_state = CONFIG_TIME_ENABLE;
						break;

					case CONFIG_TIME_ENABLE:
						sendCmd(this, CMD_SET_TIME_ENABLE, "1", RESP_FILTER_OK | RESP_FILTER_ERROR, 2000);
						this->fsm_sub_state = CONFIG_SAVE;
						break;

					case CONFIG_SAVE:
						sendCmd(this, CMD_SAVE, "", RESP_FILTER_SAVE_OK, 2000);
						this->fsm_sub_state = CONFIG_EXIT;
						break;

					case CONFIG_EXIT:
						sendCmd(this, cmdExit, "", RESP_FILTER_EXIT, 2000);
						this->fsm_sub_state = FSM_NO_SUBSTATE;
						break;

					default:
						this->fsm_state = FSM_IDLE;
						this->fsm_sub_state = FSM_NO_SUBSTATE;
						break;
				}
			break;

		case FSM_IDLE:

			if (this->fsm_sub_state != FSM_NO_SUBSTATE)
			{
				// El subestado tiene información del estado al que pertenece.
				this->fsm_state = this->fsm_sub_state & 0xFF00;
			}
			else
			{
				// Si está conectado por TCP y hay datos para enviar se los envía
				if (ioRN1723_isTCPConnected(this) && (this->cmdMode == 0) )
				{
					if (cBuffer_getPending(outBuffer(this)) > 0)
					{
						if (ioComm_freeSpace(uart(this)) > 0)
						{
							cBuffer_remove(outBuffer(this), &data);
							ioObject_write(uart(this), data);
						}
					}
				}
			}

			break;


		case FSM_WAIT4ANSWER:
			if (ev_isTriggered(this->events, EV_RESP_RECEIVED))
			{
				if (ev_isTriggered(this->events, EV_OK_RECEIVED))
				{
					this->fsm_state = FSM_IDLE;
				}
				else if (ev_isTriggered(this->events, EV_CMD_RECEIVED))
				{
					this->cmdMode = 1;

					// Se entró en el modo de comando, se envía el comando que está en cmdBuffer
					count = cBuffer_getPending(cmdBuffer(this));
					for (i = 0; i < count; i++)
					{
						if(ioComm_freeSpace(uart(this)) > 0)
						{
							cBuffer_remove(cmdBuffer(this), &data);
							ioObject_write(uart(this), data);
						}
					}

					this->fsm_state = FSM_WAIT4ANSWER;
				}
				else if (ev_isTriggered(this->events, EV_EXIT_RECEIVED))
				{
					this->cmdMode = 0;
					this->fsm_state = FSM_IDLE;
				}
				else if (ev_isTriggered(this->events, EV_SAVE_OK_RECEIVED))
				{
					this->fsm_state = FSM_IDLE;
				}
				else if (ev_isTriggered(this->events, EV_OPEN_RECEIVED))
				{
					this->cmdMode = 0;		// Al abrir una conexión sale del modo comando.
					this->fsm_state = FSM_IDLE;
				}
				else if (ev_isTriggered(this->events, EV_CLOSE_RECEIVED))
				{
					this->fsm_state = FSM_IDLE;
				}
				else if (ev_isTriggered(this->events, EV_ASSOCIATED_RECEIVED))
				{
					this->fsm_state = FSM_IDLE;
				}
				else if (ev_isTriggered(this->events, EV_WPS_SUCCESS_RECEIVED))
				{
					this->cmdMode = 0;
					this->fsm_state = FSM_IDLE;
				}
				else if (ev_isTriggered(this->events, EV_WPS_FAILED_RECEIVED))
				{
					this->retries --;
					this->cmdMode = 0;
					if (this->retries > 0)
					{
						this->fsm_state = FSM_WPS_WAIT4REBOOT;
					}
					else
					{
						this->fsm_state = FSM_IDLE;
					}
				}
				else if (ev_isTriggered(this->events, EV_DEAUTH_RECEIVED))
				{
					this->fsm_state = FSM_IDLE;
				}
				else if (ev_isTriggered(this->events, EV_CONNECT_FAILED_RECEIVED))
				{
					this->cmdMode = 1;			// Al no poder abrir una conexión, se queda en modo comando
					this->fsm_state = FSM_IDLE;
				}
				else if (ev_isTriggered(this->events, EV_RTC_RECEIVED))
				{
					this->fsm_state = FSM_IDLE;
				}
				else if (ev_isTriggered(this->events, EV_ERR_RECEIVED))
				{
					// TODO: gestionar errores
				}
			}
			else
			{
				if (cTimer_hasExpired(timer(this)))
				{
					// Se produjo un timeout esperando una respueta.
					// Se resetea el módulo para tratar de salvarlo.
					ioObject_write(gpioReset(this), 0);
					this->fsm_state = FSM_WAIT4READY;
					this->cmdMode = 0;
				}
			}

			break;

		case FSM_WPS_WAIT4REBOOT:
			if (ev_isTriggered(this->events, EV_READY_RECEIVED))
			{
				sendCmd(this, CMD_RUN_WPS, "", RESP_FILTER_WPS_SUCCESS | RESP_FILTER_WPS_FAILED, 8000);
			}
			break;

		case FSM_LEAVE_NETWORK:
			switch (this->fsm_sub_state)
			{
				case LEAVE_NETWORK_CMD:
					sendCmd(this, CMD_LEAVE, "", RESP_FILTER_DEAUTH, 2000);
					this->fsm_sub_state = LEAVE_NETWORK_EXIT;
					break;

				case LEAVE_NETWORK_EXIT:
					sendCmd(this, CMD_EXIT, "", RESP_FILTER_EXIT, 2000);
					this->fsm_sub_state = FSM_NO_SUBSTATE;
					break;

				default:
					this->fsm_state = FSM_IDLE;
					this->fsm_sub_state = FSM_NO_SUBSTATE;
					break;
			}
			break;

		case FSM_CLOSE_TCP:
			switch (this->fsm_sub_state)
			{
				case CLOSE_TCP_CMD:
					sendCmd(this, CMD_CLOSE, "", RESP_FILTER_CLOSE, 2000);
					this->fsm_sub_state = CLOSE_TCP_EXIT;
					break;

				case CLOSE_TCP_EXIT:
					sendCmd(this, CMD_EXIT, "", RESP_FILTER_EXIT, 2000);
					this->fsm_sub_state = FSM_NO_SUBSTATE;
					break;

				default:
					this->fsm_state = FSM_IDLE;
					this->fsm_sub_state = FSM_NO_SUBSTATE;
					break;
			}
			break;

		case FSM_SET_SNTP_SERVER:
			switch (this->fsm_sub_state)
			{
				case SET_SNTP_SERVER_CMD:
					sendCmd(this, CMD_SET_SNTP_IP, this->param, RESP_FILTER_OK | RESP_FILTER_ERROR, 2000);
					this->fsm_sub_state = SET_SNTP_SERVER_SAVE;
					break;

				case SET_SNTP_SERVER_SAVE:
					sendCmd(this, CMD_SAVE, "", RESP_FILTER_SAVE_OK, 2000);
					this->fsm_sub_state = SET_SNTP_SERVER_EXIT;
					break;

				case SET_SNTP_SERVER_EXIT:
					sendCmd(this, CMD_EXIT, "", RESP_FILTER_EXIT, 2000);
					this->fsm_sub_state = FSM_NO_SUBSTATE;
					break;

				default:
					this->fsm_state = FSM_IDLE;
					this->fsm_sub_state = FSM_NO_SUBSTATE;
					break;
			}
			break;

		case FSM_SET_TIME_ZONE:
			switch (this->fsm_sub_state)
			{
				case SET_TIME_ZONE_CMD:
					sendCmd(this, CMD_SET_TIME_ZONE, this->param, RESP_FILTER_OK | RESP_FILTER_ERROR, 2000);
					this->fsm_sub_state = SET_TIME_ZONE_SAVE;
					break;

				case SET_TIME_ZONE_SAVE:
					sendCmd(this, CMD_SAVE, "", RESP_FILTER_SAVE_OK, 2000);
					this->fsm_sub_state = SET_TIME_ZONE_EXIT;
					break;

				case SET_TIME_ZONE_EXIT:
					sendCmd(this, CMD_EXIT, "", RESP_FILTER_EXIT, 2000);
					this->fsm_sub_state = FSM_NO_SUBSTATE;
					break;

				default:
					this->fsm_state = FSM_IDLE;
					this->fsm_sub_state = FSM_NO_SUBSTATE;
					break;
			}
			break;

		case FSM_SYNC_TIME:
			switch (this->fsm_sub_state)
			{
				case SYNC_TIME_CMD:
					sendCmd(this, CMD_TIME, "", RESP_FILTER_OK | RESP_FILTER_ERROR, 2000);
					this->fsm_sub_state = SYNC_TIME_SAVE;
					break;

				case SYNC_TIME_SAVE:
					sendCmd(this, CMD_SAVE, "", RESP_FILTER_SAVE_OK, 2000);
					this->fsm_sub_state = SYNC_TIME_EXIT;
					break;

				case SYNC_TIME_EXIT:
					sendCmd(this, CMD_EXIT, "", RESP_FILTER_EXIT, 2000);
					this->fsm_sub_state = FSM_NO_SUBSTATE;
					break;

				default:
					this->fsm_state = FSM_IDLE;
					this->fsm_sub_state = FSM_NO_SUBSTATE;
					break;
			}
			break;

		case FSM_SET_HEARTBEAT_PORT:
			switch (this->fsm_sub_state)
			{
				case SET_HEARTBEAT_PORT_CMD:
					sendCmd(this, CMD_SET_HEARTBEAT_PORT, this->param, RESP_FILTER_OK | RESP_FILTER_ERROR, 2000);
					this->fsm_sub_state = SET_HEARTBEAT_PORT_SAVE;
					break;

				case SET_HEARTBEAT_PORT_SAVE:
					sendCmd(this, CMD_SAVE, "", RESP_FILTER_SAVE_OK, 2000);
					this->fsm_sub_state = SET_HEARTBEAT_PORT_EXIT;
					break;

				case SET_HEARTBEAT_PORT_EXIT:
					sendCmd(this, CMD_EXIT, "", RESP_FILTER_EXIT, 2000);
					this->fsm_sub_state = FSM_NO_SUBSTATE;
					break;

				default:
					this->fsm_state = FSM_IDLE;
					this->fsm_sub_state = FSM_NO_SUBSTATE;
					break;
			}
			break;

		case FSM_SET_HEARTBEAT_INTERVAL:
			switch (this->fsm_sub_state)
			{
				case SET_HEARTBEAT_INTERVAL_CMD:
					sendCmd(this, CMD_SET_HEARTBEAT_INTERVAL, this->param, RESP_FILTER_OK | RESP_FILTER_ERROR, 2000);
					this->fsm_sub_state = SET_HEARTBEAT_INTERVAL_SAVE;
					break;

				case SET_HEARTBEAT_INTERVAL_SAVE:
					sendCmd(this, CMD_SAVE, "", RESP_FILTER_SAVE_OK, 2000);
					this->fsm_sub_state = SET_HEARTBEAT_INTERVAL_EXIT;
					break;

				case SET_HEARTBEAT_INTERVAL_EXIT:
					sendCmd(this, CMD_EXIT, "", RESP_FILTER_EXIT, 2000);
					this->fsm_sub_state = FSM_NO_SUBSTATE;
					break;

				default:
					this->fsm_state = FSM_IDLE;
					this->fsm_sub_state = FSM_NO_SUBSTATE;
					break;
			}
			break;

		case FSM_SET_TCP_SERVER_PORT:
			switch (this->fsm_sub_state)
			{
				case SET_TCP_SERVER_PORT_CMD:
					sendCmd(this, CMD_SET_TCP_SERVER_PORT, this->param, RESP_FILTER_OK | RESP_FILTER_ERROR, 2000);
					this->fsm_sub_state = SET_TCP_SERVER_PORT_SAVE;
					break;

				case SET_TCP_SERVER_PORT_SAVE:
					sendCmd(this, CMD_SAVE, "", RESP_FILTER_SAVE_OK, 2000);
					this->fsm_sub_state = SET_TCP_SERVER_PORT_EXIT;
					break;

				case SET_TCP_SERVER_PORT_EXIT:
					sendCmd(this, CMD_EXIT, "", RESP_FILTER_EXIT, 2000);
					this->fsm_sub_state = FSM_NO_SUBSTATE;
					break;

				default:
					this->fsm_state = FSM_IDLE;
					this->fsm_sub_state = FSM_NO_SUBSTATE;
					break;
			}
			break;

		case FSM_SET_DEVICE_ID:
			switch (this->fsm_sub_state)
			{
				case SET_DEVICE_ID_CMD:
					sendCmd(this, CMD_SET_DEVICE_ID, this->param, RESP_FILTER_OK | RESP_FILTER_ERROR, 2000);
					this->fsm_sub_state = SET_DEVICE_ID_SAVE;
					break;

				case SET_DEVICE_ID_SAVE:
					sendCmd(this, CMD_SAVE, "", RESP_FILTER_SAVE_OK, 2000);
					this->fsm_sub_state = SET_DEVICE_ID_EXIT;
					break;

				case SET_DEVICE_ID_EXIT:
					sendCmd(this, CMD_EXIT, "", RESP_FILTER_EXIT, 2000);
					this->fsm_sub_state = FSM_NO_SUBSTATE;
					break;

				default:
					this->fsm_state = FSM_IDLE;
					this->fsm_sub_state = FSM_NO_SUBSTATE;
					break;
			}
			break;

		case FSM_GET_TIME:
			switch (this->fsm_sub_state)
			{
				case GET_TIME_CMD:
					sendCmd(this, CMD_SHOW_T_T, "", RESP_FILTER_RTC, 2000);
					this->fsm_sub_state = GET_TIME_EXIT;
					break;

				case GET_TIME_EXIT:
					sendCmd(this, CMD_EXIT, "", RESP_FILTER_EXIT, 2000);
					this->fsm_sub_state = FSM_NO_SUBSTATE;
					break;

				default:
					this->fsm_state = FSM_IDLE;
					this->fsm_sub_state = FSM_NO_SUBSTATE;
					break;
			}
			break;

		default:
			this->fsm_state = FSM_IDLE;
			break;
	}

	ev_flush(this->events);
}


uint32_t ioRN1723_isIdle (void* _this)
{
	struct ioRN1723* this = _this;

	return ( (this->fsm_state == FSM_IDLE) && (this->fsm_sub_state = FSM_NO_SUBSTATE) );
}


void ioRN1723_runWPS (void* _this, uint32_t retries)
{
	struct ioRN1723* this = _this;

	sendCmd(this, CMD_RUN_WPS, "", RESP_FILTER_WPS_SUCCESS | RESP_FILTER_WPS_FAILED, 8000);

	this->retries = retries;
}


void ioRN1723_runConfigWebServer (void* _this)
{

}


uint32_t ioRN1723_isAuthenticated (void* _this)
{
	struct ioRN1723* this = _this;

	return (this->authenticated == 1);
}


void ioRN1723_leaveNetwork (void* _this)
{
	struct ioRN1723* this = _this;

	this->fsm_state = FSM_LEAVE_NETWORK;
	this->fsm_sub_state = LEAVE_NETWORK_CMD;
}


void ioRN1723_connectTCP (void* _this, uint8_t* ip, uint8_t* port)
{
	struct ioRN1723* this = _this;
	uint8_t socket[30];
	uint8_t i;

	// Se arma el socket a partir de la dirección IP y el puerto TCP
	for (i = 0; *ip != '\0'; i++, ip++)
		socket[i] = *ip;

	socket[i] = ' ';

	for (i = i+1; *port != '\0'; i++, port++)
		socket[i] = *port;

	socket[i] = '\0';


	sendCmd(this, CMD_OPEN, socket, RESP_FILTER_OPEN | RESP_FILTER_CONNECT_FAILED, 8000);

	//this->retries = retries;
}


void ioRN1723_disconnectTCP (void* _this)
{
	struct ioRN1723* this = _this;

	if (ioRN1723_isTCPConnected(this))
	{
		this->fsm_state = FSM_CLOSE_TCP;
		this->fsm_sub_state = CLOSE_TCP_CMD;
	}
}


uint32_t ioRN1723_isTCPConnected (void* _this)
{
	struct ioRN1723* this = _this;

	return (this->tcpConnected == 1);
}


void ioRN1723_setSNTPServer (void* _this, uint8_t* ip)
{
	struct ioRN1723* this = _this;
	uint32_t i;

	for (i = 0; *ip != '\0'; i++, ip++)
		this->param[i] = *ip;
	this->param[i] = '\0';


	this->fsm_state = FSM_SET_SNTP_SERVER;
	this->fsm_sub_state = SET_SNTP_SERVER_CMD;
}


void ioRN1723_setTimeZone (void* _this, int8_t zone)
{
	struct ioRN1723* this = _this;

	this->timeZone = zone;
}


void ioRN1723_synchronizeTime (void* _this)
{
	struct ioRN1723* this = _this;

	this->fsm_state = FSM_SYNC_TIME;
	this->fsm_sub_state = SYNC_TIME_CMD;
}


void ioRN1723_refreshLocalTime (void* _this)
{
	struct ioRN1723* this = _this;

	this->fsm_state = FSM_GET_TIME;
	this->fsm_sub_state = GET_TIME_CMD;
}

void ioRN1723_getTime (void* _this, uint32_t *hours, uint32_t *minutes, uint32_t *seconds)
{
	struct ioRN1723* this = _this;
	struct tm* time;
	uint32_t modifiedTime;

	// Se cambia el valor del RTC a la zona horaria configurada.
	modifiedTime = this->lastRTC + 3600 * this->timeZone;
	time = gmtime(&modifiedTime);

	*hours = time->tm_hour;
	*minutes = time->tm_min;
	*seconds = time->tm_sec;
}

void ioRN1723_getDate (void* _this, uint32_t *day, uint32_t *month, uint32_t *year, uint32_t* dayOfWeek)
{
	struct ioRN1723* this = _this;
	struct tm* time;
	uint32_t modifiedTime;

	// Se cambia el valor del RTC a la zona horaria configurada.
	modifiedTime = this->lastRTC + 3600 * this->timeZone;
	time = gmtime(&modifiedTime);

	*day = time->tm_mday;
	*month= time->tm_mon + 1;
	*year = time->tm_year + 1900;
	*dayOfWeek = time->tm_wday;
}


void ioRN1723_setHeartbeatPort (void* _this, uint8_t* port)
{
	struct ioRN1723* this = _this;
	uint32_t i;

	for (i = 0; *port != '\0'; i++, port++)
		this->param[i] = *port;
	this->param[i] = '\0';


	this->fsm_state = FSM_SET_HEARTBEAT_PORT;
	this->fsm_sub_state = SET_HEARTBEAT_PORT_CMD;
}


void ioRN1723_setHeartbeatInterval (void* _this, uint8_t* interval)
{
	struct ioRN1723* this = _this;
	uint32_t i;

	for (i = 0; *interval != '\0'; i++, interval++)
		this->param[i] = *interval;
	this->param[i] = '\0';


	this->fsm_state = FSM_SET_HEARTBEAT_INTERVAL;
	this->fsm_sub_state = SET_HEARTBEAT_INTERVAL_CMD;
}


void ioRN1723_setTCPServerPort (void* _this, uint8_t* port)
{
	struct ioRN1723* this = _this;
	uint32_t i;

	for (i = 0; *port != '\0'; i++, port++)
		this->param[i] = *port;
	this->param[i] = '\0';


	this->fsm_state = FSM_SET_TCP_SERVER_PORT;
	this->fsm_sub_state = SET_TCP_SERVER_PORT_CMD;
}

void ioRN1723_reboot (void* _this)
{
	struct ioRN1723* this = _this;

	sendCmd(this, CMD_REBOOT, "", RESP_FILTER_OK | RESP_FILTER_ERROR, 0);
}


void ioRN1723_setDeviceID (void* _this, uint8_t* id)
{
	struct ioRN1723* this = _this;
	uint32_t i;

	for (i = 0; *id != '\0'; i++, id++)
		this->param[i] = *id;
	this->param[i] = '\0';


	this->fsm_state = FSM_SET_DEVICE_ID;
	this->fsm_sub_state = SET_DEVICE_ID_CMD;
}


// ********************************************************************************
// ********************************************************************************

void processRX (void* _this)
{
	struct ioRN1723* this = _this;
	uint32_t bytesPending;
	uint32_t i, j;
	uint8_t c;
	int32_t responseNumber = -1;


	bytesPending = ioComm_dataAvailable(uart(this));

	for (i = 0; i < bytesPending; i++)
	{
		c = ioObject_read(uart(this));

		// Se determina si se está recibiendo alguna de las respuestas esperadas
		for (j = 0; j < RN_RESPONSES_COUNT; j++)
		{
			if ( (this->possibleResponses & (1 << j)) != 0)
			{
				// Todavía es una respuesta que puede llegar
				if (this->responseIndex < rnResponsesLength[j])
				{
					if (c == rnResponses[j][this->responseIndex])
					{
						this->responseIndex ++;

						if (this->responseIndex == rnResponsesLength[j])
						{
							// Llegó una respuesta
							responseNumber = j;
						}

						break;
					}
					else
					{
						// La respuesta j-esima ya no puede llegar
						this->possibleResponses &= ~(1 << j);
					}
				}
			}
		}

		//  No está llegando ninguna respuesta.
		if (j == RN_RESPONSES_COUNT)
		{
			this->responseIndex = 0;
			this->possibleResponses = 0xFFFFFFFF;
		}

		if (responseNumber != -1)
		{
			// Llegó una respuesta. Se la parsea.
			switch (responseNumber)
			{
				case RESP_OK:
					if (this->answerFilter & RESP_FILTER_OK)
					{
						ev_emit(this->events, EV_OK_RECEIVED);
						ev_emit(this->events, EV_RESP_RECEIVED);
					}
					break;

				case RESP_ERROR:
					if (this->answerFilter & RESP_FILTER_ERROR)
					{
						ev_emit(this->events, EV_ERR_RECEIVED);
						ev_emit(this->events, EV_RESP_RECEIVED);
					}
					break;

				case RESP_CMD:
					if (this->answerFilter & RESP_FILTER_CMD)
					{
						ev_emit(this->events, EV_CMD_RECEIVED);
						ev_emit(this->events, EV_RESP_RECEIVED);
					}
					break;

				case RESP_EXIT:
					if (this->answerFilter & RESP_FILTER_EXIT)
					{
						ev_emit(this->events, EV_EXIT_RECEIVED);
						ev_emit(this->events, EV_RESP_RECEIVED);
					}
					break;

				case RESP_OPEN:
					ev_emit(this->events, EV_OPEN_RECEIVED);
					ev_emit(this->events, EV_RESP_RECEIVED);
					this->tcpConnected = 1;
					this->cmdMode = 0;			// Si se abre una conexión se sale del modo comando
					break;

				case RESP_CLOSE:
					ev_emit(this->events, EV_CLOSE_RECEIVED);
					ev_emit(this->events, EV_RESP_RECEIVED);
					this->tcpConnected = 0;
					break;

				case RESP_WPS_SUCCESS:
					if (this->answerFilter & RESP_FILTER_WPS_SUCCESS)
					{
						ev_emit(this->events, EV_WPS_SUCCESS_RECEIVED);
						ev_emit(this->events, EV_RESP_RECEIVED);
					}
					break;

				case RESP_ASSOCIATED:
					ev_emit(this->events, EV_ASSOCIATED_RECEIVED);
					ev_emit(this->events, EV_RESP_RECEIVED);
					this->authenticated = 1;
					break;

				case RESP_SAVE_OK:
					if (this->answerFilter & RESP_FILTER_SAVE_OK)
					{
						ev_emit(this->events, EV_SAVE_OK_RECEIVED);
						ev_emit(this->events, EV_RESP_RECEIVED);
					}
					break;

				case RESP_WPS_FAILED:
					if (this->answerFilter & RESP_FILTER_WPS_FAILED)
					{
						ev_emit(this->events, EV_WPS_FAILED_RECEIVED);
						ev_emit(this->events, EV_RESP_RECEIVED);
					}
					break;

				case RESP_READY:
					ev_emit(this->events, EV_READY_RECEIVED);
					ev_emit(this->events, EV_RESP_RECEIVED);
					this->cmdMode = 0;
					break;

				case RESP_DEAUTH:
					ev_emit(this->events, EV_DEAUTH_RECEIVED);
					ev_emit(this->events, EV_RESP_RECEIVED);
					this->authenticated = 0;
					break;

				case RESP_CONNECT_FAILED:
					if (this->answerFilter & RESP_FILTER_CONNECT_FAILED)
					{
						ev_emit(this->events, EV_CONNECT_FAILED_RECEIVED);
						ev_emit(this->events, EV_RESP_RECEIVED);
					}
					break;

				case RESP_RTC:
					if (this->answerFilter & RESP_FILTER_RTC)
					{
						// No se genera el evento de respuesta recibida porque todavía falta que llegue el valor del RTC.
						this->readingRTCValue = 1;
						this->indexSerial = 0;
					}
					break;

				default:
					break;
			}

			this->responseIndex = 0;
			this->possibleResponses = 0xFFFFFFFF;
			responseNumber = -1;
		}

		if (this->readingRTCValue == 1)
		{
			if (c != '=')
			{
				if ( (c >= '0') && (c <= '9') && (this->indexSerial < 24) )
				{
					this->param[this->indexSerial] = c;
					this->indexSerial ++;
				}
				else
				{
					// Se terminó de recibir el RTC
					this->param[this->indexSerial] = '\0';
					this->readingRTCValue = 0;

					this->lastRTC = atoi(this->param);

					ev_emit(this->events, EV_RTC_RECEIVED);
					ev_emit(this->events, EV_RESP_RECEIVED);
				}
			}
		}

		if (this->tcpConnected == 1 && this->cmdMode == 0)
		{
			if (cBuffer_getFreeSpace(inBuffer(this)) > 0)
				cBuffer_put(inBuffer(this), &c);
		}
	}
}


void sendCmd (void* _this, const uint8_t* cmd, uint8_t* params, uint32_t answerFilter, uint32_t timeout)
{
	struct ioRN1723* this = _this;
	uint8_t i;
	uint8_t c;


	// Si no está en modo comando, primero se debe entrar en este modo y luego enviar el comando.
	// Si está en modo comando, directamente se envía por la UART.
	if (this->cmdMode == 1)
	{
		// Comando
		for (i = 0; *cmd != '\0'; i++, cmd++)
		{
			c = *cmd;
			if(ioComm_freeSpace(uart(this)) > 0)
				ioObject_write(uart(this), c);
		}

		// Espacio entre el comando y los parámetros
		ioObject_write(uart(this), ' ');

		// Parámetros
		for (i = 0; *params != '\0'; i++, params++)
		{
			c = *params;
			if(ioComm_freeSpace(uart(this)) > 0)
				ioObject_write(uart(this), c);
		}

		// CR final
		ioObject_write(uart(this), 13);

		// Se configura el filtro de respuestas
		this->answerFilter = answerFilter;
	}
	else
	{
		// Comando
		for (i = 0; *cmd != '\0'; i++, cmd++)
		{
			c = *cmd;
			if(cBuffer_getFreeSpace(cmdBuffer(this)) > 0)
				cBuffer_put(cmdBuffer(this), (void*)&c);
		}

		// Espacio entre el comando y los parámetros
		c = ' ';
		cBuffer_put(cmdBuffer(this), (void*)&c);

		// Parámetros
		for (i = 0; *params != '\0'; i++, params++)
		{
			c = *params;
			if(cBuffer_getFreeSpace(cmdBuffer(this)) > 0)
				cBuffer_put(cmdBuffer(this), (void*)&c);
		}

		// CR final
		c = 13;
		cBuffer_put(cmdBuffer(this), (void*)&c);


		// Se envía por la UART el comando para entrar en modo comando
		if(ioComm_freeSpace(uart(this)) > 3)
		{
			ioComm_writeBytes(uart(this), 3, "$$$");
		}

		// Al filtro de respuestas se le agrega el de CMD
		this->answerFilter = answerFilter | RESP_FILTER_CMD;
	}

	if (timeout > 0)
		cTimer_start(timer(this), timeout);

	this->fsm_state = FSM_WAIT4ANSWER;
}


