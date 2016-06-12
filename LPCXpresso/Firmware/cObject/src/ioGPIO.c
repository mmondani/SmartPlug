/**
 * @file ioGPIO.c
 * @author Mariano Mondani
 *
 * @brief Implementación de los métodos de la clase ioGPIO.
 */

#include "ioGPIO.h"
#include "ioGPIO_r.h"


static void* ioGPIO_ctor  (void* _this, va_list* va);
static void* ioGPIO_dtor (void* _this);
static uint32_t ioGPIO_differ (void* _this, void* _dst);
static void ioGPIO_display (void* _this);
static void* ioGPIO_copy (void* _this, void* _src);
static uint32_t ioGPIO_init (void* _this);
static uint32_t ioGPIO_deInit (void* _this);
static uint32_t ioGPIO_enable (void* _this);
static uint32_t ioGPIO_disable (void* _this);
static uint32_t ioGPIO_read (void* _this);
static uint32_t ioGPIO_write (void* _this, uint32_t data);



// ********************************************************************************
// Declaración de la clase ioGPIO
// ********************************************************************************
static const struct ioObject _ioGPIO = {{sizeof(struct ioGPIO),
												ioGPIO_ctor,
												ioGPIO_dtor,
												ioGPIO_differ,
												ioGPIO_display,
												ioGPIO_copy},
												ioGPIO_init,
												ioGPIO_deInit,
												ioGPIO_enable,
												ioGPIO_disable,
												ioGPIO_read,
												ioGPIO_write};

const void* ioGPIO = &_ioGPIO;
// ********************************************************************************



static void* ioGPIO_ctor  (void* _this, va_list* va)
{
	struct ioGPIO* this = _this;

	this->periphMem = va_arg(*va, void*);
	this->direction = va_arg(*va, uint32_t);
	this->port = va_arg(*va, uint32_t);
	this->pin = va_arg(*va, uint32_t);


	return this;
}


static void* ioGPIO_dtor (void* _this)
{
	// No tiene que liberar memoria alocada.

	return _this;
}


static uint32_t ioGPIO_differ (void* _this, void* _dst)
{
	struct ioGPIO* this = _this;
	struct ioGPIO* dst = _dst;

	return ((this->periphMem != dst->periphMem) || (this->direction != dst->direction) ||
			(this->port != dst->port) || (this->pin != dst->pin));
}


static void ioGPIO_display (void* _this)
{

}


static void* ioGPIO_copy (void* _this, void* _src)
{
	struct ioGPIO* this = _this;
	struct ioGPIO* src = _src;

	this->periphMem = src->periphMem;
	this->direction = src->direction;
	this->port = src->port;
	this->pin = src->pin;


	return this;
}


static uint32_t ioGPIO_init (void* _this)
{
	return 0;
}


static uint32_t ioGPIO_deInit (void* _this)
{
	return 0;
}


static uint32_t ioGPIO_enable (void* _this)
{
	return 0;
}


static uint32_t ioGPIO_disable (void* _this)
{
	return 0;
}


static uint32_t ioGPIO_read (void* _this)
{
	return 0;
}


static uint32_t ioGPIO_write (void* _this, uint32_t data)
{
	return 0;
}
