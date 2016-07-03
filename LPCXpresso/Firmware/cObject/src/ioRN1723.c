/**
 * @file ioRN1723.c
 * @author Mariano Mondani
 *
 * @brief Implementación de los métodos de la clase ioRN1723.
 */

#include "ioRN1723.h"
#include "ioRN1723_r.h"


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
void sendCmd (void* _this, const uint8_t* cmd, uint8_t* params);
// ********************************************************************************


// ********************************************************************************
// Definiciones de los comandos
// ********************************************************************************
const uint8_t* cmdWLANJoin = "set wlan join";
const uint8_t* cmdSysIOfunc = "set sys iofunc";
const uint8_t* cmdCmdMode = "$$$";
const uint8_t* cmdExit = "exit";
const uint8_t* cmdSave = "save";


#define CMD_WLAN_JOIN		cmdWLANJoin
#define CMD_SYS_IOFUNC		cmdSysIOfunc
#define CMD_CMD_MODE		cmdCmdMode
#define CMD_EXIT			cmdExit
#define CMD_SAVE			cmdSave

// ********************************************************************************

// ********************************************************************************
// Definiciones de las respuestas del módulo
// ********************************************************************************
#define RN_RESPONSES_COUNT		9
uint8_t* rnResponses[RN_RESPONSES_COUNT] = {
												"OK",
												"ERR",
												"WPS-SUCCESS!",
												"Associated!",
												"*OPEN*",
												"*CLOS*",
												"CMD",
												"EXIT",
												"Storing in config"
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
												17
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

// ********************************************************************************


// ********************************************************************************
// Definiciones de los eventos
// ********************************************************************************
#define EV_OK_RECEIVED					0x00000001
#define EV_ERR_RECEIVED					0x00000002
#define EV_CMD_RECEIVED						0x00000004
#define EV_RESP_RECEIVED				0x00000008
#define EV_EXIT_RECEIVED				0x00000010
#define EV_OPEN_RECEIVED				0x00000020
#define EV_CLOSE_RECEIVED				0x00000040
#define EV_WPS_SUCCESS_RECEIVED			0x00000080
#define EV_ASSOCIATED_RECEIVED			0x00000100
#define EV_SAVE_OK_RECEIVED				0x00000200

#define ev_isTriggered(v,e)		(((v & e) == 0)? 0 : 1)
#define ev_emit(v,e)			(v |= e)
#define ev_flush(v)				(v = 0)
// ********************************************************************************


// ********************************************************************************
// Definiciones de los estados
// ********************************************************************************
enum {
	FSM_CONFIG = 0,
	FSM_IDLE,
	FSM_SENDING_CMD,
	FSM_WAIT4ANSWER,
	FSM_WAIT4CMD
};

enum {
	CONFIG_WLAN_JOIN = 0,
	CONFIG_IOFUNC,
	CONFIG_SAVE,
	CONFIG_EXIT,
	CONFIG_FINAL
};
// ********************************************************************************



static void* ioRN1723_ctor  (void* _this, va_list* va)
{
	struct ioRN1723* this = _this;


	this->uart = va_arg(*va, void*);
	this->gpioNetwork = va_arg(*va, void*);
	this->gpioTCP = va_arg(*va, void*);
	this->inBuffer = va_arg(*va, void*);
	this->outBuffer = va_arg(*va, void*);

	this->cmdBuffer = cObject_new(cQueue, 40, sizeof(char));

	this->authenticated = 0;
	this->tcpConnected = 0;
	this->fsm_state = FSM_IDLE;
	this->config_state = CONFIG_WLAN_JOIN;
	this->events = 0;
	this->responseIndex = 0;
	this->cmdMode = 0;


	return this;
}


static void* ioRN1723_dtor (void* _this)
{
	struct ioRN1723* this = _this;

	cObject_delete(this->cmdBuffer);

	return _this;
}


static uint32_t ioRN1723_differ (void* _this, void* _dst)
{
	struct ioRN1723* this = _this;
	struct ioRN1723* dst = _dst;

	return ( cObject_differ(uart(this), uart(dst)) || cObject_differ(gpioNetwork(this), gpioNetwork(dst)) || cObject_differ(gpioTCP(this), gpioTCP(dst)) ||
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
	cObject_copy(gpioNetwork(this), gpioNetwork(src));
	cObject_copy(gpioTCP(this), gpioTCP(src));
	this->authenticated = src->authenticated;
	this->tcpConnected = src->tcpConnected;


	return this;
}


static uint32_t ioRN1723_init (void* _this)
{
	struct ioRN1723* this = _this;
	uint32_t res = 0;


	this->fsm_state = FSM_CONFIG;
	this->config_state = CONFIG_WLAN_JOIN;

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

	return 0;
}


static uint32_t ioRN1723_write (void* _this, uint32_t data)
{
	struct ioRN1723* this = _this;


	return 0;
}


static uint32_t ioRN1723_writeBytes (void* _this, uint32_t len, uint8_t* data)
{
	struct ioRN1723* this = _this;

	return 0;
}


static uint32_t ioRN1723_readBytes (void* _this, uint32_t len, uint8_t* data)
{
	struct ioRN1723* this = _this;

	return 0;
}


static uint32_t ioRN1723_freeSpace(void* _this)
{
	return 1;
}


static uint32_t ioRN1723_dataAvailable (void* _this)
{
	return 0;
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


	processRX(this);

	switch (this->fsm_state)
	{
		case FSM_CONFIG:
				switch(this->config_state)
				{
					case CONFIG_WLAN_JOIN:
						sendCmd(this, CMD_WLAN_JOIN, "1");
						this->config_state++;
						break;

					case CONFIG_IOFUNC:
						sendCmd(this, CMD_SYS_IOFUNC, "0x50");
						this->config_state++;
						break;

					case CONFIG_SAVE:
						sendCmd(this, cmdSave, "");
						this->config_state++;
						break;

					case CONFIG_EXIT:
						sendCmd(this, cmdExit, "");
						this->config_state++;
						break;

					default:
						this->fsm_state = FSM_IDLE;
						break;
				}
			break;

		case FSM_IDLE:
			if (this->config_state < CONFIG_FINAL && this->config_state != CONFIG_WLAN_JOIN)
			{
				this->fsm_state = FSM_CONFIG;
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
				}
				else if (ev_isTriggered(this->events, EV_CLOSE_RECEIVED))
				{
				}
				else if (ev_isTriggered(this->events, EV_WPS_SUCCESS_RECEIVED))
				{

				}
				else if (ev_isTriggered(this->events, EV_ASSOCIATED_RECEIVED))
				{

				}
				else if (ev_isTriggered(this->events, EV_ERR_RECEIVED))
				{
					// TODO: gestionar errores
				}
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

	return (this->fsm_state == FSM_IDLE);
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
			}
		}

		//  No llegó ninguna respuesta
		if (j == RN_RESPONSES_COUNT)
			this->responseIndex = 0;

		if (responseNumber != -1)
		{
			// Llegó una respuesta. Se la parsea
			switch (responseNumber)
			{
				case RESP_OK:
					ev_emit(this->events, EV_OK_RECEIVED);
					ev_emit(this->events, EV_RESP_RECEIVED);
					break;

				case RESP_ERROR:
					ev_emit(this->events, EV_ERR_RECEIVED);
					ev_emit(this->events, EV_RESP_RECEIVED);
					break;

				case RESP_CMD:
					ev_emit(this->events, EV_CMD_RECEIVED);
					ev_emit(this->events, EV_RESP_RECEIVED);
					break;

				case RESP_EXIT:
					ev_emit(this->events, EV_EXIT_RECEIVED);
					ev_emit(this->events, EV_RESP_RECEIVED);
					break;

				case RESP_OPEN:
					ev_emit(this->events, EV_OPEN_RECEIVED);
					ev_emit(this->events, EV_RESP_RECEIVED);
					this->tcpConnected = 1;
					break;

				case RESP_CLOSE:
					ev_emit(this->events, EV_CLOSE_RECEIVED);
					ev_emit(this->events, EV_RESP_RECEIVED);
					this->tcpConnected = 0;
					break;

				case RESP_WPS_SUCCESS:
					ev_emit(this->events, EV_WPS_SUCCESS_RECEIVED);
					ev_emit(this->events, EV_RESP_RECEIVED);
					break;

				case RESP_ASSOCIATED:
					ev_emit(this->events, EV_ASSOCIATED_RECEIVED);
					ev_emit(this->events, EV_RESP_RECEIVED);
					this->authenticated = 1;
					break;

				case RESP_SAVE_OK:
					ev_emit(this->events, EV_SAVE_OK_RECEIVED);
					ev_emit(this->events, EV_RESP_RECEIVED);
					break;

				default:
					break;
			}

			this->responseIndex = 0;
			responseNumber = -1;
		}


		if (this->tcpConnected == 1 && this->cmdMode == 0)
		{

		}
	}
}


void sendCmd (void* _this, const uint8_t* cmd, uint8_t* params)
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
	}

	this->fsm_state = FSM_WAIT4ANSWER;
}


