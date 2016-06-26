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



static void* ioRN1723_ctor  (void* _this, va_list* va)
{
	struct ioRN1723* this = _this;


	this->uart = va_arg(*va, void*);
	this->gpioNetwork = va_arg(*va, void*);
	this->gpioTCP = va_arg(*va, void*);

	this->authenticated = 0;
	this->tcpConnected = 0;


	return this;
}


static void* ioRN1723_dtor (void* _this)
{

	return _this;
}


static uint32_t ioRN1723_differ (void* _this, void* _dst)
{
	struct ioRN1723* this = _this;
	struct ioRN1723* dst = _dst;

	return ( cObject_differ(uart(this), uart(dst)) || cObject_differ(gpioNetwork(this), gpioNetwork(dst)) || cObject_differ(gpioTCP(this), gpioTCP(dst)) ||
			(authenticated(this) != authenticated(dst)) || (tcpConnected(this) != tcpConnected(dst)) );
}


static void ioRN1723_display (void* _this)
{

}


static void* ioRN1723_copy (void* _this, void* _src)
{
	struct ioRN1723* this = _this;
	struct ioRN1723* src = src;

	cObject_copy(uart(this), uart(srt));
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


static uint32_t ioRN1723_deInit (void* _this)
{
	Chip_SSP_DeInit(periphMem(_this));

	return 0;
}


static uint32_t ioRN1723_enable (void* _this)
{
	Chip_SSP_Enable(periphMem(_this));

	return 0;
}


static uint32_t ioRN1723_disable (void* _this)
{
	Chip_SSP_Disable(periphMem(_this));

	return 0;
}


static uint32_t ioRN1723_read (void* _this)
{
	struct ioRN1723* this = _this;

	// Se limpia el Status del SSP
	Chip_SSP_ClearIntPending(periphMem(this), SSP_INT_CLEAR_BITMASK);

	// Se envía un dato dummy para recibir.
	Chip_SSP_SendFrame(periphMem(this), 0xFFFF);

	// Espera hasta recibir el dato.
	while (Chip_SSP_GetStatus(periphMem(this), SSP_STAT_RNE) == RESET);

	// Se lee el dato recibido
	return ( Chip_SSP_ReceiveFrame(periphMem(this)) );
}


static uint32_t ioRN1723_write (void* _this, uint32_t data)
{
	struct ioRN1723* this = _this;


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


static uint32_t ioRN1723_writeBytes (void* _this, uint32_t len, uint8_t* data)
{
	struct ioRN1723* this = _this;
	uint32_t i;
	uint16_t data16;

	for (i = 0; i < len; i++)
	{
		data16 = *(data+i);
		ioRN1723_write(this, data16);
	}

	return len;
}


static uint32_t ioRN1723_readBytes (void* _this, uint32_t len, uint8_t* data)
{
	struct ioRN1723* this = _this;
	uint32_t i;
	uint8_t data8;

	for (i = 0; i < len; i++)
	{
		data8 = ioRN1723_read(this);
		*(data+i) = data8;
	}

	return len;
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
	Chip_SSP_Int_Enable(periphMem(_this));
}


static void ioRN1723_intDisable (void* _this, uint32_t mask)
{
	Chip_SSP_Int_Disable(periphMem(_this));
}


static uint32_t ioRN1723_getInt (void* _this, uint32_t intID)
{
	return ( Chip_SSP_GetRawIntStatus(periphMem(_this), intID) == SET );
}





void ioRN1723_handler (void* _this, uint32_t retries)
{

}






