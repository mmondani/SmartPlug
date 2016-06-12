/**
 * @file ioDigital.c
 * @author Mariano Mondani
 *
 * @brief Implementación de los métodos de la clase ioDigital.
 */


#include "ioDigital.h"
#include "ioDigital_r.h"


static void* ioDigital_ctor  (void* _this, va_list* va);
static void* ioDigital_dtor (void* _this);
static uint32_t ioDigital_differ (void* _this, void* _dst);
static void ioDigital_display (void* _this);
static void* ioDigital_copy (void* _this, void* _src);
static uint32_t ioDigital_init (void* _this);
static uint32_t ioDigital_deInit (void* _this);
static uint32_t ioDigital_enable (void* _this);
static uint32_t ioDigital_disable (void* _this);
static uint32_t ioDigital_read (void* _this);
static uint32_t ioDigital_write (void* _this, uint32_t data);



// ********************************************************************************
// Declaración de la clase cStaticBuffer
// ********************************************************************************
static const struct ioObject _ioDigital = {{sizeof(struct ioGPIO),
											ioDigital_ctor,
											ioDigital_dtor,
											ioDigital_differ,
											ioDigital_display,
											ioDigital_copy},
											ioDigital_init,
											ioDigital_deInit,
											ioDigital_enable,
											ioDigital_disable,
											ioDigital_read,
											ioDigital_write};

const void* ioDigital = &_ioDigital;
// ********************************************************************************



static void* ioDigital_ctor  (void* _this, va_list* va)
{
	// Constructor de la clase que hereda
	return (((const struct cObject*) ioGPIO)->ctor (_this, va));
}


static void* ioDigital_dtor (void* _this)
{
	// Destructor de la clase que hereda
	return (((const struct cObject*) ioGPIO)->dtor (_this));

}


static uint32_t ioDigital_differ (void* _this, void* _dst)
{
	// differ de la clase que hereda
	return (((const struct cObject*) ioGPIO)->differ (_this, _dst));

}


static void ioDigital_display (void* _this)
{
	// display de la clase que hereda
	((const struct cObject*) ioGPIO)->display (_this);

}


static void* ioDigital_copy (void* _this, void* _src)
{
	// copy de la clase que hereda
	return (((const struct cObject*) ioGPIO)->copy (_this, _src));

}


static uint32_t ioDigital_init (void* _this)
{
	struct ioDigital* this = _this;

	Chip_GPIO_Init(periphMem(this));			// Se habilita el clock del GPIO

	Chip_GPIO_SetDir(periphMem(this), port(this), pin(this), direction(this));

	return 0;
}


static uint32_t ioDigital_deInit (void* _this)
{
	return 0;
}


static uint32_t ioDigital_enable (void* _this)
{
	return 0;
}


static uint32_t ioDigital_disable (void* _this)
{
	return 0;
}


static uint32_t ioDigital_read (void* _this)
{
	return ( (Chip_GPIO_GetPinState(periphMem(_this), port(_this), pin(_this)) == true)? 1:0 );
}


static uint32_t ioDigital_write (void* _this, uint32_t data)
{

	if (data != 0)
	{
		Chip_GPIO_SetPinOutHigh(periphMem(_this), port(_this), pin(_this));
	}
	else
	{
		Chip_GPIO_SetPinOutLow(periphMem(_this), port(_this), pin(_this));
	}

	return 0;
}





// ********************************************************************************
// Métodos linkeados estáticamente a la clase ioDigital
// ********************************************************************************


void ioDigital_toggle (void* _this)
{
	Chip_GPIO_SetPinToggle(periphMem(_this), port(_this), pin(_this));
}
