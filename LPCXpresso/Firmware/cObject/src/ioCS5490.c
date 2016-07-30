/**
 * @file ioCS5490.c
 * @author Mariano Mondani
 *
 * @brief Implementación de los métodos de la clase ioCS5490.
 */

#include "ioCS5490.h"
#include "ioCS5490_r.h"

#include "ioUART.h"


static void* ioCS5490_ctor  (void* _this, va_list* va);
static void* ioCS5490_dtor (void* _this);
static uint32_t ioCS5490_differ (void* _this, void* _dst);
static void ioCS5490_display (void* _this);
static void* ioCS5490_copy (void* _this, void* _src);




// ********************************************************************************
// Declaración de la clase ioCS5490
// ********************************************************************************
static const struct cObject _ioCS5490 = {sizeof(struct ioCS5490),
												ioCS5490_ctor,
												ioCS5490_dtor,
												ioCS5490_differ,
												ioCS5490_display,
												ioCS5490_copy};

const void* ioCS5490 = &_ioCS5490;
// ********************************************************************************



static void* ioCS5490_ctor  (void* _this, va_list* va)
{
	struct ioCS5490* this = _this;

	this->uart = va_arg(*va, void*);
	this->gpioReset = va_arg(*va, void*);
	this->gpioDO = va_arg(*va, void*);

	// Se pone en 1 el pin de RESET para sacar del reset al CS5490
	ioObject_write(gpioReset(this), 1);

	return this;
}


static void* ioCS5490_dtor (void* _this)
{
	// No tiene que liberar memoria alocada.
	// La instancia de ioUART e ioDigital tienen que ser destruidas por la aplicación.

	return _this;
}


static uint32_t ioCS5490_differ (void* _this, void* _dst)
{
	struct ioCS5490* this = _this;
	struct ioCS5490* dst = _dst;

	return ( (cObject_differ(uart(this), uart(dst))) || (cObject_differ(gpioReset(this), gpioReset(dst))) ||
			(cObject_differ(gpioDO(this), gpioDO(dst))) );
}


static void ioCS5490_display (void* _this)
{

}


static void* ioCS5490_copy (void* _this, void* _src)
{
	struct ioCS5490* this = _this;
	struct ioCS5490* src = _src;

	cObject_copy(uart(this), uart(src));
	cObject_copy(gpioReset(this), gpioReset(src));
	cObject_copy(gpioDO(this), gpioDO(src));


	return this;
}




void ioCS5490_init (void* _this)
{

}


void ioCS5490_registerWrite (void* _this, uint8_t reg, uint32_t value)
{
	struct ioCS5490* this = _this;
	uint8_t aux;

	aux = 0b01000000 | reg;
	ioObject_write(uart(this), aux);

	aux = value & 0xff;
	ioObject_write(uart(this), aux);

	aux = (value >> 8) & 0xff;
	ioObject_write(uart(this), aux);

	aux = (value >> 16) & 0xff;
	ioObject_write(uart(this), aux);
}


uint32_t ioCS4390_registerRead (void* _this, uint8_t reg)
{
	struct ioCS5490* this = _this;
	uint8_t aux;
	uint32_t data;


	aux = 0b00000000 | reg;
	ioObject_write(uart(this), aux);

	while(ioComm_dataAvailable(uart(this)) < 1);
	aux = ioObject_read(uart(this));
	*((uint8_t*)&data + 0) = aux;

	while(ioComm_dataAvailable(uart(this)) < 1);
	aux = ioObject_read(uart(this));
	*((uint8_t*)&data + 1) = aux;

	while(ioComm_dataAvailable(uart(this)) < 1);
	aux = ioObject_read(uart(this));
	*((uint8_t*)&data + 2) = aux;


	return data;
}


void ioCS4390_pageSelect (void* _this, uint8_t page)
{
	struct ioCS5490* this = _this;

	ioObject_write(uart(this), page);
}


void ioCS4390_instructionWrite (void* _this, uint8_t instruction)
{
	struct ioCS5490* this = _this;

	ioObject_write(uart(this), instruction);
}


float ioCS5490_signedFract2Float (uint32_t value, uint32_t m, uint32_t n)
{
	uint32_t resolution = 1L << (m + n + 1);	// 2^(n)
	uint32_t fullRange = 1L << (m + n + 1);		// 2^(m+n+1)
	uint32_t halfRange = 1L << (n + m);			// 2^(n+m)
	float decimalValue = 0.0;


	if (value >= halfRange)
		value -= fullRange;

	decimalValue = ( (float) value ) / ( (float) resolution );

	return decimalValue;
}


float ioCS5490_unsignedFract2Float (uint32_t value, uint32_t m, uint32_t n)
{
	uint32_t resolution = 1L << n;		// 2^(n)
	float decimalValue = 0.0;

	decimalValue = ( (float) value ) / ( (float) resolution );

	return decimalValue;
}


uint32_t ioCS5490_signedFloat2Fract (float value, uint32_t m, uint32_t n)
{
	uint32_t resolution = 1L << n;				// 2^(n)
	uint32_t fullRange = 1L << (m + n + 1);		// 2^(m+n+1)
	uint32_t fractionValue = 0;

	if (value < 0.0)
	{
		fractionValue = (-1.0) * (value * resolution);
		fractionValue = fullRange - fractionValue;
	}
	else
	{
		fractionValue = value * resolution;
	}

	return fractionValue;
}

