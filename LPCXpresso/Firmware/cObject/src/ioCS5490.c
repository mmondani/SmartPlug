/**
 * @file ioCS5490.c
 * @author Mariano Mondani
 *
 * @brief Implementación de los métodos de la clase ioCS5490.
 */

#include "ioCS5490.h"
#include "ioCS5490_r.h"

#include "ioUART.h"
#include "cTimer.h"


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


// ********************************************************************************
// Funciones privadas de la clase ioCS5490
// ********************************************************************************
void blocking_delay (void* timer, uint32_t interval);

// ********************************************************************************





static void* ioCS5490_ctor  (void* _this, va_list* va)
{
	struct ioCS5490* this = _this;

	this->uart = va_arg(*va, void*);
	this->gpioReset = va_arg(*va, void*);
	this->gpioDO = va_arg(*va, void*);
	this->wordRate = va_arg(*va, double);
	this->vMax = va_arg(*va, double);
	this->iMax = va_arg(*va, double);
	this->iCal = va_arg(*va, double);
	this->meterConstant = va_arg(*va, double);
	this->minimumLoad = va_arg(*va, double);

	this->scale = 0.6 * this->iCal / this->iMax;
	this->maxPower = this->vMax * this->iCal;
	this->powerScale = 0.6 * this->scale;


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




void ioCS5490_init (void* _this, uint32_t vDCOffset, uint32_t iDCOffset, uint32_t vGain, uint32_t iGain)
{
	struct ioCS5490* this = _this;
	void* timer;
	uint32_t aux1;
	float auxFloat;


	timer = cObject_new(cTimer);

	// Se resetea el CS5490
	ioObject_write(gpioReset(this), 0);
	blocking_delay(timer, 1000);
	ioObject_write(gpioReset(this), 1);


	ioCS5490_instructionWrite(this, IOCS5490_INS_SOFTWARE_RESET);
	blocking_delay(timer, 200);


	// Se resetea el CS5490
	ioObject_write(gpioReset(this), 0);
	blocking_delay(timer, 1000);
	ioObject_write(gpioReset(this), 1);


	ioCS5490_instructionWrite(this, IOCS5490_INS_SOFTWARE_RESET);
	blocking_delay(timer, 200);


	ioCS5490_pageSelect(this, IOCS5490_PAGE_0);


	aux1 = ioCS5490_registerRead(this, IOCS5490_REG_STATUS0);
	ioCS5490_registerWrite(this, IOCS5490_REG_STATUS0, 0b111001010101010101111101);
	aux1 = ioCS5490_registerRead(this, IOCS5490_REG_STATUS0);


	ioCS5490_pageSelect(this, IOCS5490_PAGE_18);


	aux1 = ioCS5490_registerRead(this, IOCS5490_REG_SCALE);
	aux1 = ioCS5490_signedFloat2Fract(this->scale, 0, 23);
	ioCS5490_registerWrite(this, IOCS5490_REG_SCALE, aux1);
	aux1 = ioCS5490_registerRead(this, IOCS5490_REG_SCALE);


	// Calibración
	blocking_delay(timer, 250);
	ioCS5490_pageSelect(this, IOCS5490_PAGE_16);

	blocking_delay(timer, 250);
	ioCS5490_registerWrite(this, IOCS5490_REG_V_DCOFF, vDCOffset);

	blocking_delay(timer, 250);
	ioCS5490_registerWrite(this, IOCS5490_REG_I_DCOFF, iDCOffset);

	blocking_delay(timer, 250);
	ioCS5490_registerWrite(this, IOCS5490_REG_V_GAIN, vGain);

	blocking_delay(timer, 250);
	ioCS5490_registerWrite(this, IOCS5490_REG_I_GAIN, iGain);

	blocking_delay(timer, 250);
	aux1 = ioCS5490_registerRead(this, IOCS5490_REG_LOAD_MIN);
	auxFloat = this->minimumLoad * this->powerScale / this->maxPower;
	aux1 = ioCS5490_signedFloat2Fract(auxFloat, 0 ,23);
	ioCS5490_registerWrite(this, IOCS5490_REG_LOAD_MIN, aux1);
	aux1 = ioCS5490_registerRead(this, IOCS5490_REG_LOAD_MIN);


	// Configuración de los Energy Pulses
	blocking_delay(timer, 250);
	ioCS5490_pageSelect(this, IOCS5490_PAGE_0);


	ioCS5490_registerWrite(this, IOCS5490_REG_PULSEWIDTH, 0x080010);
	blocking_delay(timer, 250);
	aux1 = ioCS5490_registerRead(this, IOCS5490_REG_PULSEWIDTH);


	ioCS5490_pageSelect(this, IOCS5490_PAGE_18);

	ioCS5490_registerWrite(this, IOCS5490_REG_PULSERATE, 0x4BDA12);
	blocking_delay(timer, 250);
	aux1 = ioCS5490_registerRead(this, IOCS5490_REG_PULSERATE);


	ioCS5490_pageSelect(this, IOCS5490_PAGE_0);

	// Los pulsos se generan con la P_Avg
	ioCS5490_registerWrite(this, IOCS5490_REG_PULSECTRL, 0x000000);
	blocking_delay(timer, 250);
	aux1 = ioCS5490_registerRead(this, IOCS5490_REG_PULSECTRL);


	// Se activa el EPG
	ioCS5490_registerWrite(this, IOCS5490_REG_CONFIG1, 0x10EEEE);
	blocking_delay(timer, 250);
	aux1 = ioCS5490_registerRead(this, IOCS5490_REG_CONFIG1);

	blocking_delay(timer, 500);

	// EPG conectado al DO
	ioCS5490_registerWrite(this, IOCS5490_REG_CONFIG1, 0x10EEE0);
	blocking_delay(timer, 250);
	aux1 = ioCS5490_registerRead(this, IOCS5490_REG_CONFIG1);



	// Comienzan las conversiones
	ioCS5490_instructionWrite(this, IOCS5490_INS_CONTINUOUS_CONV);
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


uint32_t ioCS5490_registerRead (void* _this, uint8_t reg)
{
	struct ioCS5490* this = _this;
	uint8_t aux1, aux2, aux3;
	uint32_t data;

	ioUART_flushRx(uart(this));

	aux1 = 0b00000000 | reg;
	ioObject_write(uart(this), aux1);

	while(ioComm_dataAvailable(uart(this)) < 1);
	aux1 = ioObject_read(uart(this));
	//*((uint8_t*)&data + 0) = aux;

	while(ioComm_dataAvailable(uart(this)) < 1);
	aux2 = ioObject_read(uart(this));
	//*((uint8_t*)&data + 1) = aux;

	while(ioComm_dataAvailable(uart(this)) < 1);
	aux3 = ioObject_read(uart(this));
	//*((uint8_t*)&data + 2) = aux;

	data = (aux3 << 16) | (aux2 << 8) | (aux1);

	return data;
}


void ioCS5490_pageSelect (void* _this, uint8_t page)
{
	struct ioCS5490* this = _this;

	ioObject_write(uart(this), page);
}


void ioCS5490_instructionWrite (void* _this, uint8_t instruction)
{
	struct ioCS5490* this = _this;

	ioObject_write(uart(this), instruction);
}


float ioCS5490_getIcalibration (void* _this)
{
	struct ioCS5490* this = _this;

	return this->iCal;
}


float ioCS5490_getMeterScale (void* _this)
{
	struct ioCS5490* this = _this;

	return this->scale;
}


float ioCS5490_getVmax (void* _this)
{
	struct ioCS5490* this = _this;

	return this->vMax;
}


float ioCS5490_getMaxPower (void* _this)
{
	struct ioCS5490* this = _this;

	return this->maxPower;
}


float ioCS5490_getPowerScale (void* _this)
{
	struct ioCS5490* this = _this;

	return this->powerScale;
}


float ioCS5490_getWordRate (void* _this)
{
	struct ioCS5490* this = _this;

	return this->wordRate;
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




void blocking_delay (void* timer, uint32_t interval)
{
	cTimer_start(timer, interval);
	while (!cTimer_hasExpired(timer));
}
