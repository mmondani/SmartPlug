/**
 * @file ioObject.c
 * @author Mariano Mondani
 *
 * @brief Implementación de los métodos de la interfaz ioObject.
 */

#include "ioObject.h"
#include "ioObject_r.h"



uint32_t ioObject_init (void* _this)
{
	/**
	 * Este método llama a la función init de la clase que implementa la interfaz ioObject
	 */
	const struct ioObject* const * class = _this;
	uint32_t res = 1;

	if (_this && class && (*class)->init)
	{
		res = (*class)->init(_this);
	}

	 return res;
}


uint32_t ioObject_deInit (void* _this)
{
	/**
	 * Este método llama a la función deInit de la clase que implementa la interfaz ioObject
	 */
	const struct ioObject* const * class = _this;
	uint32_t res = 1;

	if (_this && class && (*class)->deInit)
	{
		res = (*class)->deInit(_this);
	}

	return res;
}


uint32_t ioObject_enable (void* _this)
{
	/**
	 * Este método llama a la función enable de la clase que implementa la interfaz ioObject
	 */
	const struct ioObject* const * class = _this;
	uint32_t res = 1;

	if (_this && class && (*class)->enable)
	{
		res = (*class)->enable(_this);
	}

	return res;
}


uint32_t ioObject_disable (void* _this)
{
	/**
	 * Este método llama a la función disable de la clase que implementa la interfaz ioObject
	 */
	const struct ioObject* const * class = _this;
	uint32_t res = 1;

	if (_this && class && (*class)->disable)
	{
		res = (*class)->disable(_this);
	}

	return res;
}


uint32_t ioObject_read (void* _this)
{
	/**
	 * Este método llama a la función read de la clase que implementa la interfaz ioObject
	 */
	const struct ioObject* const * class = _this;
	uint32_t res = 1;

	if (_this && class && (*class)->read)
	{
		res = (*class)->read(_this);
	}

	return res;
}


uint32_t ioObject_write (void* _this, uint32_t data)
{
	/**
	 * Este método llama a la función write de la clase que implementa la interfaz ioObject
	 */
	const struct ioObject* const * class = _this;
	uint32_t res = 1;

	if (_this && class && (*class)->write)
	{
		res = (*class)->write(_this, data);
	}

	return res;
}
