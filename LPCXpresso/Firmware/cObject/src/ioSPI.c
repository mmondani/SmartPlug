/**
 * @file ioSPI.c
 * @author Mariano Mondani
 *
 * @brief Implementación de los métodos de la clase ioSPI.
 */

#include "ioSPI.h"
#include "ioSPI_r.h"


static void* ioSPI_ctor  (void* _this, va_list* va);
static void* ioSPI_dtor (void* _this);
static uint32_t ioSPI_differ (void* _this, void* _dst);
static void ioSPI_display (void* _this);
static void* ioSPI_copy (void* _this, void* _src);
static uint32_t ioSPI_init (void* _this);
static uint32_t ioSPI_deInit (void* _this);
static uint32_t ioSPI_enable (void* _this);
static uint32_t ioSPI_disable (void* _this);
static uint32_t ioSPI_read (void* _this);
static uint32_t ioSPI_write (void* _this, uint32_t data);
static uint32_t ioSPI_writeBytes (void* _this, uint32_t len, uint8_t* data);
static uint32_t ioSPI_readBytes (void* _this, uint32_t len, uint8_t* data);
static uint32_t ioSPI_freeSpace (void* _this);
static uint32_t ioSPI_dataAvailable (void* _this);
static void ioSPI_intEnable (void* _this, uint32_t mask);
static void ioSPI_intDisable (void* _this, uint32_t mask);
static uint32_t ioSPI_getInt (void* _this, uint32_t intID);


// ********************************************************************************
// Declaración de la clase cStaticBuffer
// ********************************************************************************
static const struct ioComm _ioSPI = { { {sizeof(struct ioSPI),
										ioSPI_ctor,
										ioSPI_dtor,
										ioSPI_differ,
										ioSPI_display,
										ioSPI_copy },
										ioSPI_init,
										ioSPI_deInit,
										ioSPI_enable,
										ioSPI_disable,
										ioSPI_read,
										ioSPI_write },
										ioSPI_writeBytes,
										ioSPI_readBytes,
										ioSPI_freeSpace,
										ioSPI_dataAvailable,
										ioSPI_intEnable,
										ioSPI_intDisable,
										ioSPI_getInt };

const void* ioSPI = &_ioSPI;
// ********************************************************************************



static void* ioSPI_ctor  (void* _this, va_list* va)
{
	struct ioSPI* this = _this;


	this->periphMem = va_arg(*va, void*);
	this->clockRate = va_arg(*va, uint32_t);
	this->dataLen = va_arg(*va, uint32_t);
	this->clockMode = va_arg(*va, uint32_t);
	this->spiMode = va_arg(*va, uint32_t);


	return this;
}


static void* ioSPI_dtor (void* _this)
{

	return _this;
}


static uint32_t ioSPI_differ (void* _this, void* _dst)
{
	struct ioSPI* this = _this;
	struct ioSPI* dst = _dst;

	return ( (this->periphMem != dst->periphMem) || (this->clockRate != dst->clockRate) ||
			(this->dataLen != dst->dataLen) || (this->clockMode != dst->clockMode) || (this->spiMode != dst->spiMode) );
}


static void ioSPI_display (void* _this)
{

}


static void* ioSPI_copy (void* _this, void* _src)
{
	struct ioSPI* this = _this;
	struct ioSPI* src = src;

	this->periphMem = src->periphMem;
	this->clockRate = src->clockRate;
	this->dataLen = src->dataLen;
	this->clockMode = src->clockMode;
	this->spiMode = src->spiMode;

	return this;
}


static uint32_t ioSPI_init (void* _this)
{
	struct ioSPI* this = _this;
	uint32_t res = 0;

	// Para configurar los pines del SSP
	Chip_IOCON_Init(LPC_IOCON);

	if (periphMem(this) == LPC_SSP0)
	{
		Chip_IOCON_PinMux(LPC_IOCON, 0, 15, IOCON_MODE_INACT, IOCON_FUNC2);		// P0.15: SCK
		Chip_IOCON_PinMux(LPC_IOCON, 0, 16, IOCON_MODE_INACT, IOCON_FUNC2);		// P0.16: SSEL
		Chip_IOCON_PinMux(LPC_IOCON, 0, 17, IOCON_MODE_INACT, IOCON_FUNC2);		// P0.17: MISO
		Chip_IOCON_PinMux(LPC_IOCON, 0, 18, IOCON_MODE_INACT, IOCON_FUNC2);		// P0.18: MOSI
	}
	else if (periphMem(this) == LPC_SSP1)
	{
		Chip_IOCON_PinMux(LPC_IOCON, 0, 7, IOCON_MODE_INACT, IOCON_FUNC2);		// P0.7: SCK
		Chip_IOCON_PinMux(LPC_IOCON, 0, 6, IOCON_MODE_INACT, IOCON_FUNC2);		// P0.6: SSEL
		Chip_IOCON_PinMux(LPC_IOCON, 0, 8, IOCON_MODE_INACT, IOCON_FUNC2);		// P0.8: MISO
		Chip_IOCON_PinMux(LPC_IOCON, 0, 9, IOCON_MODE_INACT, IOCON_FUNC2);		// P0.9: MOSI
	}
	else
		res = 1;



	Chip_SSP_Init(periphMem(this));

	Chip_SSP_SetBitRate(periphMem(this), clockRate(this));

	Chip_SSP_SetFormat(periphMem(this), dataLen(this), SSP_FRAMEFORMAT_SPI, clockMode(this));

	Chip_SSP_Enable(periphMem(this));


	return res;
}


static uint32_t ioSPI_deInit (void* _this)
{
	Chip_SSP_DeInit(periphMem(_this));

	return 0;
}


static uint32_t ioSPI_enable (void* _this)
{
	Chip_SSP_Enable(periphMem(_this));

	return 0;
}


static uint32_t ioSPI_disable (void* _this)
{
	Chip_SSP_Disable(periphMem(_this));

	return 0;
}


static uint32_t ioSPI_read (void* _this)
{
	struct ioSPI* this = _this;

	// Se limpia el Status del SSP
	Chip_SSP_ClearIntPending(periphMem(this), SSP_INT_CLEAR_BITMASK);

	// Se envía un dato dummy para recibir.
	Chip_SSP_SendFrame(periphMem(this), 0xFFFF);

	// Espera hasta recibir el dato.
	while (Chip_SSP_GetStatus(periphMem(this), SSP_STAT_RNE) == RESET);

	// Se lee el dato recibido
	return ( Chip_SSP_ReceiveFrame(periphMem(this)) );
}


static uint32_t ioSPI_write (void* _this, uint32_t data)
{
	struct ioSPI* this = _this;


	// Se limpia el Status del SSP
	Chip_SSP_ClearIntPending(periphMem(this), SSP_INT_CLEAR_BITMASK);

	// Se envía un dato
	Chip_SSP_SendFrame(periphMem(this), data);

	// Espera hasta recibir el dato.
	while (Chip_SSP_GetStatus(periphMem(this), SSP_STAT_RNE) == RESET);

	// Limpia el buffer de Rx
	Chip_SSP_ReceiveFrame(periphMem(this));

	return 0;
}


static uint32_t ioSPI_writeBytes (void* _this, uint32_t len, uint8_t* data)
{
	struct ioSPI* this = _this;
	uint32_t i;
	uint16_t data16;

	for (i = 0; i < len; i++)
	{
		data16 = *(data+i);
		ioSPI_write(this, data16);
	}

	return len;
}


static uint32_t ioSPI_readBytes (void* _this, uint32_t len, uint8_t* data)
{
	struct ioSPI* this = _this;
	uint32_t i;
	uint8_t data8;

	for (i = 0; i < len; i++)
	{
		data8 = ioSPI_read(this);
		*(data+i) = data8;
	}

	return len;
}


static uint32_t ioSPI_freeSpace(void* _this)
{
	return 1;
}


static uint32_t ioSPI_dataAvailable (void* _this)
{
	return 0;
}


static void ioSPI_intEnable (void* _this, uint32_t mask)
{
	Chip_SSP_Int_Enable(periphMem(_this));
}


static void ioSPI_intDisable (void* _this, uint32_t mask)
{
	Chip_SSP_Int_Disable(periphMem(_this));
}


static uint32_t ioSPI_getInt (void* _this, uint32_t intID)
{
	return ( Chip_SSP_GetRawIntStatus(periphMem(_this), intID) == SET );
}





void ioSPI_setClock (void* _this, uint32_t clock)
{
	Chip_SSP_SetBitRate(periphMem(_this), clockRate(_this));
}


void ioSPI_sendReceive (void* _this, uint32_t lenTx, uint8_t* buffTx, uint32_t lenRx, uint8_t* buffRx)
{
	ioSPI_writeBytes(_this, lenTx, buffTx);

	ioSPI_readBytes(_this, lenRx, buffRx);
}

