/**
 * @file ioUART.c
 * @author Mariano Mondani
 *
 * @brief Implementación de los métodos de la clase ioUART.
 */

#include "ioUART.h"
#include "ioUART_r.h"


static void* ioUART_ctor  (void* _this, va_list* va);
static void* ioUART_dtor (void* _this);
static uint32_t ioUART_differ (void* _this, void* _dst);
static void ioUART_display (void* _this);
static void* ioUART_copy (void* _this, void* _src);
static uint32_t ioUART_init (void* _this);
static uint32_t ioUART_deInit (void* _this);
static uint32_t ioUART_enable (void* _this);
static uint32_t ioUART_disable (void* _this);
static uint32_t ioUART_read (void* _this);
static uint32_t ioUART_write (void* _this, uint32_t data);
static uint32_t ioUART_writeBytes (void* _this, uint32_t len, uint8_t* data);
static uint32_t ioUART_readBytes (void* _this, uint32_t len, uint8_t* data);
static uint32_t ioUART_freeSpace (void* _this);
static uint32_t ioUART_dataAvailable (void* _this);
static void ioUART_intEnable (void* _this, uint32_t mask);
static void ioUART_intDisable (void* _this, uint32_t mask);
static uint32_t ioUART_getInt (void* _this, uint32_t intID);


// ********************************************************************************
// Declaración de la clase cStaticBuffer
// ********************************************************************************
static const struct ioComm _ioUART = { { {sizeof(struct ioUART),
										ioUART_ctor,
										ioUART_dtor,
										ioUART_differ,
										ioUART_display,
										ioUART_copy },
										ioUART_init,
										ioUART_deInit,
										ioUART_enable,
										ioUART_disable,
										ioUART_read,
										ioUART_write },
										ioUART_writeBytes,
										ioUART_readBytes,
										ioUART_freeSpace,
										ioUART_dataAvailable,
										ioUART_intEnable,
										ioUART_intDisable,
										ioUART_getInt };

const void* ioUART = &_ioUART;
// ********************************************************************************



static void* ioUART_ctor  (void* _this, va_list* va)
{
	struct ioUART* this = _this;
	void* queue;
	uint32_t txLen, rxLen;

	this->periphMem = va_arg(*va, void*);
	this->baudRate = va_arg(*va, uint32_t);
	this->dataLen = va_arg(*va, uint32_t);
	this->parity = va_arg(*va, uint32_t);
	this->stopBit = va_arg(*va, uint32_t);
	this->mode = va_arg(*va, uint32_t);
	txLen = va_arg(*va, uint32_t);
	rxLen = va_arg(*va, uint32_t);


	// Reserva memoria para los queues. Si se usa modo BLOCKING, solo hay queue de Rx.
	if (mode(this) == IOUART_MODE_NON_BLOCKING)
	{
		queue = cObject_new(cQueue, txLen, sizeof(uint8_t));
		assert(queue != (void*)0);
		set_txQueue(this, queue);
	}

	queue = cObject_new(cQueue, rxLen, sizeof(uint8_t));
	assert(queue != (void*)0);
	set_rxQueue(this, queue);


	return this;
}


static void* ioUART_dtor (void* _this)
{
	// Se liberan las queues de Tx (si existe) y Rx

	if (mode(_this) == IOUART_MODE_NON_BLOCKING)
		cObject_delete(txQueue(_this));

	cObject_delete(rxQueue(_this));

	return _this;
}


static uint32_t ioUART_differ (void* _this, void* _dst)
{
	struct ioUART* this = _this;
	struct ioUART* dst = _dst;

	return ( (this->periphMem != dst->periphMem) || (this->baudRate != dst->baudRate) ||
			(this->dataLen != dst->dataLen) || (this->parity != dst->parity) || (this->stopBit != dst->stopBit) ||
			(this->mode != dst->mode) || cObject_differ(txQueue(this), txQueue(dst)) || cObject_differ(rxQueue(this), rxQueue(dst)) );
}


static void ioUART_display (void* _this)
{

}


static void* ioUART_copy (void* _this, void* _src)
{
	struct ioUART* this = _this;
	struct ioUART* src = src;

	this->periphMem = src->periphMem;
	this->baudRate = src->baudRate;
	this->dataLen = src->dataLen;
	this->parity = src->parity;
	this->stopBit = src->stopBit;
	this->mode = src->mode;
	cObject_copy(txQueue(this), txQueue(src));
	cObject_copy(rxQueue(this), rxQueue(src));

	return this;
}


static uint32_t ioUART_init (void* _this)
{
	struct ioUART* this = _this;
	uint32_t res = 0;

	// Para configurar los pines de la UART
	Chip_IOCON_Init(LPC_IOCON);

	if (periphMem(this) == LPC_UART0)
	{
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 2, IOCON_MODE_INACT | IOCON_FUNC1);		// TX0
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 3, IOCON_MODE_INACT | IOCON_FUNC1);		// RX0
	}
	else if (periphMem(this) == LPC_UART1)
	{
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 15, IOCON_MODE_INACT | IOCON_FUNC1);		// TX1
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 16, IOCON_MODE_INACT | IOCON_FUNC1);		// RX1
	}
	else if (periphMem(this) == LPC_UART2)
	{
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 10, IOCON_MODE_INACT | IOCON_FUNC1);		// TX2
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 11, IOCON_MODE_INACT | IOCON_FUNC1);		// RX2
	}
	else if (periphMem(this) == LPC_UART3)
	{
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 0, IOCON_MODE_INACT | IOCON_FUNC2);		// TX3
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 1, IOCON_MODE_INACT | IOCON_FUNC2);		// RX3
	}


	Chip_UART_Init(periphMem(this));

	if (Chip_UART_SetBaudFDR(periphMem(this), baudRate(this)) == 0)
			res = 1;

	Chip_UART_ConfigData(periphMem(this), dataLen(this) | parity(this) | stopBit(this));

	Chip_UART_TXEnable(periphMem(this));

	return res;
}


static uint32_t ioUART_deInit (void* _this)
{
	Chip_UART_DeInit(periphMem(_this));

	return 0;
}


static uint32_t ioUART_enable (void* _this)
{
	return 0;
}


static uint32_t ioUART_disable (void* _this)
{
	return 0;
}


static uint32_t ioUART_read (void* _this)
{
	struct ioUART* this = _this;
	uint8_t data;

	cBuffer_remove(this->rxQueue, &data);

	return data;
}


static uint32_t ioUART_write (void* _this, uint32_t data)
{
	struct ioUART* this = _this;
	uint8_t data8 = data;
	uint32_t res = 0;

	if (this->mode == IOUART_MODE_NON_BLOCKING)
	{
		if (cBuffer_getFreeSpace(this->txQueue) > 0)
			cBuffer_put(this->txQueue, (void*)&data8);
		else
			res = 1;
	}
	else
		Chip_UART_SendByte(periphMem(this), data8);


	return res;
}


static uint32_t ioUART_writeBytes (void* _this, uint32_t len, uint8_t* data)
{
	struct ioUART* this = _this;
	uint32_t freeSpace = 0;
	uint32_t writtenBytes = 0;
	uint32_t i;

	if (this->mode == IOUART_MODE_NON_BLOCKING)
	{
		freeSpace = cBuffer_getFreeSpace(this->txQueue);

		if (len <= freeSpace)
		{
			// Hay espacio en la queue
			for (i = 0; i < len; i ++)
			{
				cBuffer_put(this->txQueue, (void*)(data+i));
			}

			// Se envía el primer caracter
			ioUART_txHandler(this);

			writtenBytes = len;
		}
		else
		{
			// No hay suficiente espacio por lo que se trunca data
			for (i = 0; i < freeSpace; i ++)
			{
				cBuffer_put(this->txQueue, (void*)(data+i));
			}

			writtenBytes = freeSpace;

		}
	}
	else
	{
		Chip_UART_SendBlocking(periphMem(this), data, len);

		writtenBytes = len;
	}


	return writtenBytes;
}


static uint32_t ioUART_readBytes (void* _this, uint32_t len, uint8_t* data)
{
	struct ioUART* this = _this;
	uint8_t ch;
	uint32_t bytesRead = 0;

	if (cBuffer_getPending(this->rxQueue))
	{
		cBuffer_remove(this->rxQueue, &ch);
		*(data+bytesRead) = ch;
		bytesRead ++;
	}

	return bytesRead;
}


static uint32_t ioUART_freeSpace(void* _this)
{
	struct ioUART* this = _this;

	return (cBuffer_getFreeSpace(this->txQueue));
}


static uint32_t ioUART_dataAvailable (void* _this)
{
	struct ioUART* this = _this;

	return (cBuffer_getPending(this->rxQueue));
}


static void ioUART_intEnable (void* _this, uint32_t mask)
{
	Chip_UART_IntEnable(periphMem(_this), mask);
}


static void ioUART_intDisable (void* _this, uint32_t mask)
{
	Chip_UART_IntDisable(periphMem(_this), mask);
}


static uint32_t ioUART_getInt (void* _this, uint32_t intID)
{
	return Chip_UART_ReadIntIDReg(periphMem(_this)) & intID;
}





void ioUART_configFIFO (void* _this, uint32_t config)
{
	Chip_UART_SetupFIFOS(periphMem(_this), config);
}


void ioUART_rxHandler (void* _this)
{
	struct ioUART* this = _this;
	uint8_t ch;

	// Se lee toda la data que haya en el FIFO de Rx de la UART
	while (Chip_UART_ReadLineStatus(periphMem(this)) & UART_LSR_RDR)
	{
		ch = Chip_UART_ReadByte(periphMem(this));
		cBuffer_put(this->rxQueue, &ch);
	}
}


void ioUART_txHandler (void* _this)
{
	struct ioUART* this = _this;
	uint8_t data;

	if (cBuffer_getPending(this->txQueue))
	{
		cBuffer_remove(this->txQueue, &data);
		Chip_UART_SendByte(periphMem(this), data);
	}
}


void ioUART_disableTx (void* _this)
{
	Chip_UART_TXDisable(periphMem(_this));
}


void ioUART_enableTx (void* _this)
{
	Chip_UART_TXEnable(periphMem(_this));
}


void ioUART_disableRx (void* _this)
{

}


void ioUART_enableRx (void* _this)
{

}
