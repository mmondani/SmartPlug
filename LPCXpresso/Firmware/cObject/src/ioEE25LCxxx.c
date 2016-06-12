/**
 * @file ioEE25LCxxx.c
 * @author Mariano Mondani
 *
 * @brief Implementación de los métodos de la clase ioEE25LCxxx.
 */

#include "ioEE25LCxxx.h"
#include "ioEE25LCxxx_r.h"


static void* ioEE25LCxxx_ctor  (void* _this, va_list* va);
static void* ioEE25LCxxx_dtor (void* _this);
static uint32_t ioEE25LCxxx_differ (void* _this, void* _dst);
static void ioEE25LCxxx_display (void* _this);
static void* ioEE25LCxxx_copy (void* _this, void* _src);




// ********************************************************************************
// Declaración de la clase ioEE25LCxxx
// ********************************************************************************
static const struct cObject _ioEE25LCxxx = {sizeof(struct ioEE25LCxxx),
												ioEE25LCxxx_ctor,
												ioEE25LCxxx_dtor,
												ioEE25LCxxx_differ,
												ioEE25LCxxx_display,
												ioEE25LCxxx_copy};

const void* ioEE25LCxxx = &_ioEE25LCxxx;
// ********************************************************************************



// ********************************************************************************
// Comandos para las memorias EEPROM 25LCxxx
// ********************************************************************************
#define EE25LCXXX_WREN			0b00000110			// Write enable
#define EE25LCXXX_WRITE			0b00000010			// Write EEPROM
#define EE25LCXXX_READ			0b00000011			// Read EEPROM
#define EE25LCXXX_RDSR			0b00000101			// Read status register
#define EE25LCXXX_WRSR			0b00000001			// Write status register
// ********************************************************************************





static void* ioEE25LCxxx_ctor  (void* _this, va_list* va)
{
	struct ioEE25LCxxx* this = _this;

	this->spiPort = va_arg(*va, void*);
	this->sselPin = va_arg(*va, void*);
	this->eeSize = va_arg(*va, uint32_t);

	// Se pone en 1 el pin SSEL para deshabilitar la comunicación con la EEPROM.
	ioObject_write(sselPin(this), 1);

	return this;
}


static void* ioEE25LCxxx_dtor (void* _this)
{
	// No tiene que liberar memoria alocada.
	// La instancia de ioSPI e ioDigital tienen que ser destruidas por la aplicación.

	return _this;
}


static uint32_t ioEE25LCxxx_differ (void* _this, void* _dst)
{
	struct ioEE25LCxxx* this = _this;
	struct ioEE25LCxxx* dst = _dst;

	return ( (cObject_differ(spiPort(this), spiPort(dst))) || (cObject_differ(sselPin(this), sselPin(dst))) ||
			(eeSize(this) != eeSize(dst)) );
}


static void ioEE25LCxxx_display (void* _this)
{

}


static void* ioEE25LCxxx_copy (void* _this, void* _src)
{
	struct ioEE25LCxxx* this = _this;
	struct ioEE25LCxxx* src = _src;

	cObject_copy(spiPort(this), spiPort(src));
	cObject_copy(sselPin(this), sselPin(src));
	this->eeSize = src->eeSize;


	return this;
}







void ioEE25LCxxx_writeStatus (void* _this, uint8_t data)
{
	struct ioEE25LCxxx* this = _this;

	ioObject_write(sselPin(this), 0);
	ioObject_write(spiPort(this), EE25LCXXX_WRSR);
	ioObject_write(spiPort(this), data);
	ioObject_write(sselPin(this), 1);
}


uint8_t ioEE25LCxxx_readStatus (void* _this)
{
	struct ioEE25LCxxx* this = _this;
	uint8_t data;

	ioObject_write(sselPin(this), 0);
	ioObject_write(spiPort(this), EE25LCXXX_RDSR);
	data = ioObject_read(spiPort(this));
	ioObject_write(sselPin(this), 1);

	return (data);
}


void ioEE25LCxxx_busyPolling (void* _this)
{
	struct ioEE25LCxxx* this = _this;
	uint8_t data;

	do
	{
		ioObject_write(sselPin(this), 0);
		ioObject_write(spiPort(this), EE25LCXXX_RDSR);
		data = ioObject_read(spiPort(this));
		ioObject_write(sselPin(this), 1);

	} while (data & 0x01); // se queda en el loop mientras la EEPROM está escribiendo.
}


void ioEE25LCxxx_setWriteEnable (void* _this)
{
	struct ioEE25LCxxx* this = _this;

	ioObject_write(sselPin(this), 0);
	ioObject_write(spiPort(this), EE25LCXXX_WREN);
	ioObject_write(sselPin(this), 1);
}


void ioEE25LCxxx_writeData (void* _this, uint32_t address, uint8_t* data, uint32_t len)
{
	struct ioEE25LCxxx* this = _this;
	void* spi = spiPort(this);
	uint8_t data8;


	ioObject_write(sselPin(this), 0);
	ioObject_write(spi, EE25LCXXX_WRITE);

	// EN las 25LC1024, las direccionesson de 24 bits. En todas las otras son de 16 bits.
	if (eeSize(this) == IOEE25LCXXX_SIZE_1024K)
		ioObject_write(spi, (address >> 16) & 0x000000ff);

	ioObject_write(spi, (address >> 8) & 0x000000ff);
	ioObject_write(spi, address & 0x000000ff);


	// Se escriben los datos.
	while (len > 0)
	{
		data8 = *data;
		ioObject_write(spi, data8);

		data++;
		len--;
	}

	ioObject_write(sselPin(this), 1);
}


void ioEE25LCxxx_readData (void* _this, uint32_t address, uint8_t* data, uint32_t len)
{
	struct ioEE25LCxxx* this = _this;
	void* spi = spiPort(this);
	uint8_t data8;


	ioObject_write(sselPin(this), 0);
	ioObject_write(spi, EE25LCXXX_READ);

	// EN las 25LC1024, las direccionesson de 24 bits. En todas las otras son de 16 bits.
	if (eeSize(this) == IOEE25LCXXX_SIZE_1024K)
		ioObject_write(spi, (address >> 16) & 0x000000ff);

	ioObject_write(spi, (address >> 8) & 0x000000ff);
	ioObject_write(spi, address & 0x000000ff);


	// Se leen los datos
	while (len > 0)
	{
		data8 = ioObject_read(spi);
		*data = data8;

		data++;
		len--;
	}

	ioObject_write(sselPin(this), 1);
}


void ioEE25LCxxx_writeString (void* _this, uint32_t address, uint8_t* data)
{
	struct ioEE25LCxxx* this = _this;
	void* spi = spiPort(this);
	uint8_t data8;


	ioObject_write(sselPin(this), 0);
	ioObject_write(spi, EE25LCXXX_WRITE);

	// EN las 25LC1024, las direccionesson de 24 bits. En todas las otras son de 16 bits.
	if (eeSize(this) == IOEE25LCXXX_SIZE_1024K)
		ioObject_write(spi, (address >> 16) & 0x000000ff);

	ioObject_write(spi, (address >> 8) & 0x000000ff);
	ioObject_write(spi, address & 0x000000ff);


	// Se escriben los datos.
	while (*data)
	{
		data8 = *data;
		ioObject_write(spi, data8);

		data++;
	}
	ioObject_write(spi, '\0');


	ioObject_write(sselPin(this), 1);
}


void ioEE25LCxxx_erase (void* _this, uint32_t address, uint32_t len)
{
	struct ioEE25LCxxx* this = _this;
	void* spi = spiPort(this);


	ioObject_write(sselPin(this), 0);
	ioObject_write(spi, EE25LCXXX_WRITE);

	// EN las 25LC1024, las direccionesson de 24 bits. En todas las otras son de 16 bits.
	if (eeSize(this) == IOEE25LCXXX_SIZE_1024K)
		ioObject_write(spi, (address >> 16) & 0x000000ff);

	ioObject_write(spi, (address >> 8) & 0x000000ff);
	ioObject_write(spi, address & 0x000000ff);


	// Se escriben los datos.
	while (len > 0)
	{
		ioObject_write(spi, 0);

		len--;
	}

	ioObject_write(sselPin(this), 1);
}
