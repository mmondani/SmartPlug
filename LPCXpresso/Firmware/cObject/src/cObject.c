/**
 * @file cObject.c
 *
 * @brief Implementación de los métodos de la interfaz cObject.
 */

#include "cObject.h"
#include "cObject_r.h"


void* cObject_new (const void* _class, ...)
{
	/**
	 * Este método llama al constructor de la clase que implementa la interfaz cBuffer
	 */

	const struct cObject* class = _class;

	void* p = memAlloc(class->size);

	*(const struct cObject**)p = class;

	if (class->ctor)
	{
		va_list va;

		va_start(va, _class);
		class->ctor(p, &va);
		va_end(va);
	}

	return p;
}


void cObject_delete (void* _this)
{
	/**
	 * Este método llama al destructor put de la clase que implementa la interfaz cBuffer
	 */

	const struct cObject** class = _this;

	if (_this && class && (*class)->dtor)
	{
		_this = ((*class)->dtor(_this));
	}

	memFree(_this);
}


uint32_t cObject_differ (void* _this, void* _dst)
{
	/**
	 * Este método llama a la función differ de la clase que implementa la interfaz cBuffer
	 */

	const struct cObject* const * class = _this;

	if (_this && class && (*class)->differ)
	{
		return ((*class)->differ(_this, _dst));
	}

	return 1;
}

void cObject_display (void* _this)
{
	/**
	 * Este método llama a la función display de la clase que implementa la interfaz cBuffer
	 */

	const struct cObject* const * class = _this;

	if (_this && class && (*class)->display)
	{
		(*class)->display(_this);
	}
}


void* cObject_copy (void* _this, void* _src)
{
	/**
	 * Este método llama a la función copy de la clase que implementa la interfaz cBuffer
	 */

	const struct cObject* const * class = _this;
	void* p = 0;


	if (_this && class && (*class)->copy)
	{
		p = (*class)->copy(_this, _src);
	}

	return p;
}

