/**
 * @file ioCS5490.c
 * @author Mariano Mondani
 *
 * @brief Implementación de los métodos de la clase ioCS5490.
 */

#include "ioCS5490.h"
#include "ioCS5490_r.h"


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







