/**
 * @file cStaticBuffer.c
 *
 * @brief Implementación de los métodos de la clase cWindow.
 */


#include "cWindow.h"
#include "cWindow_r.h"
#include <stdio.h>



static void* cWindow_ctor  (void* _this, va_list* va);
static void* cWindow_dtor (void* _this);
static uint32_t cWindow_differ (void* _this, void* _dst);
static void cWindow_display (void* _this);
static void* cWindow_copy (void* _this, void* _src);
static void* cWindow_put (void* _this, void* _object);
static void* cWindow_remove (void* _this, void* _object);
static void* cWindow_clear (void* _this);
static void cWindow_at (void* _this, uint32_t pos, void* _object);
static uint32_t cWindow_getFreeSpace (void* _this);
static uint32_t cWindow_getPending (void* _this);
static uint32_t cWindow_length (void* _this);



// ********************************************************************************
// Declaración de la clase cWindow
// ********************************************************************************
static const struct cBuffer _cWindow = {{sizeof(struct cWindow),
											cWindow_ctor,
											cWindow_dtor,
											cWindow_differ,
											cWindow_display,
											cWindow_copy},
											cWindow_put,
											cWindow_remove,
											cWindow_clear,
											cWindow_at,
											cWindow_getFreeSpace,
											cWindow_getPending,
											cWindow_length};

const void* cWindow = &_cWindow;
// ********************************************************************************



static void* cWindow_ctor  (void* _this, va_list* va)
{
	// Constructor de la clase que es heredada
	struct cWindow* this = ((const struct cObject*) cStaticBuffer)->ctor (_this, va);

	return this;
}


static void* cWindow_dtor (void* _this)
{
	((const struct cObject*) cStaticBuffer)->dtor (_this);

	return _this;
}


static uint32_t cWindow_differ (void* _this, void* _dst)
{
	// Se llama a la implementación de differ de la clase superior
	return ((const struct cObject*) cStaticBuffer)->differ (_this, _dst);
}


static void cWindow_display (void* _this)
{
	/*
	struct cWindow* this = _this;
	uint32_t i;
	double objeto;

	for (i = 0; i < len(this); i++)
	{
		objeto = *((double*)(buffer(this) + i* objectSize(this)));
		printf (" %f ", objeto);
	}
	*/
}


static void* cWindow_copy (void* _this, void* _src)
{
	// Se llama a la implementación de copy de la clase superior
	return ((const struct cObject*) cStaticBuffer)->copy (_this, _src);

	// Si hubiera otros elementos que copiar se copian.
}


static void* cWindow_put (void* _this, void* _object)
{
	struct cWindow* this = _this;
	int32_t i;
	void* buff;
	uint32_t objectS;

	assert(len(this) > 0);

	// Cada objecto se mueve una posición hacia el final del buffer.
	// El objeto más nuevo está en la primer posición
	buff = buffer(this);
	objectS = objectSize(this);
	if (len(this) >= 2)
	{
		for (i = len(this) - 2; i >= 0; i--)
		{
			memcpy(buff + objectS * (i+1), buff + objectS * i, objectS);
		}

		memcpy(buff, _object, objectS);
	}

	memcpy(buff, _object, objectS);


	return this;
}


static void* cWindow_remove (void* _this, void* _object)
{
	struct cWindow* this = _this;

	// En el caso de cWindow se la implementa a modo ilustrativo ya que
	// no tiene sentido remover un item en este tipo de objeto.

	// Devuelve en _object el primer objeto de cWindow
	memcpy(_object, buffer(this), objectSize(this));

	return this;
}


static void* cWindow_clear (void* _this)
{
	// Se llama a la implementación de clear de la clase superior
	return ((const struct cBuffer*) cStaticBuffer)->clear(_this);
}


static void cWindow_at (void* _this, uint32_t pos, void* _object)
{
	struct cWindow* this = _this;

	assert((pos >= 0) && ( pos < len(this)));

	memcpy(_object, (buffer(this) + objectSize(this) * pos), objectSize(this));
}


static uint32_t cWindow_getFreeSpace (void* _this)
{
	// En esta clase no tiene sentido hablar de espacio libre
	return len(_this);
}


static uint32_t cWindow_getPending (void* _this)
{
	// En esta clase no tiene sentido hablar de cantidad de objetos pendientes.
	return len(_this);
}


static uint32_t cWindow_length (void* _this)
{
	// Se llama a la implementación de la clase que herada
	return ((const struct cBuffer*) cStaticBuffer)->length (_this);
}
