/**
 * @file cBuffer.c
 * @author Mariano Mondani
 *
 * @brief Implementación de los métodos de la interfaz cBuffer.
 */

#include "cBuffer.h"
#include "cBuffer_r.h"



void* cBuffer_put (void* _this, void* _object)
{
	/**
	 * Este método llama a la función put de la clase que implementa la interfaz cBuffer
	 */
	const struct cBuffer* const * class = _this;

	if (_this && class && (*class)->put)
	{
		_this = ((*class)->put(_this, _object));
	}

	return _this;
}


void* cBuffer_remove (void* _this, void* _object)
{
	/**
	 * Este método llama a la función remove de la clase que implementa la interfaz cBuffer
	 */
	const struct cBuffer* const * class = _this;

	if (_this && class && (*class)->remove)
	{
		_this = ((*class)->remove(_this, _object));
	}

	return _this;
}


void* cBuffer_clear (void* _this)
{
	/**
	 * Este método llama a la función clear de la clase que implementa la interfaz cBuffer
	 */
	const struct cBuffer* const * class = _this;

	if (_this && class && (*class)->clear)
	{
		_this = ((*class)->clear(_this));
	}

	return _this;
}


void cBuffer_at (void* _this, uint32_t pos, void* _object)
{
	/**
	 * Este método llama a la función at de la clase que implementa la interfaz cBuffer
	 */
	const struct cBuffer* const * class = _this;

	if (_this && class && (*class)->at)
	{
		((*class)->at(_this, pos, _object));
	}
}


uint32_t cBuffer_getFreeSpace (void* _this)
{
	/**
	 * Este método llama a la función getFreeSpace de la clase que implementa la interfaz cBuffer
	 */
	const struct cBuffer* const * class = _this;
	uint32_t num = 0;

	if (_this && class && (*class)->getFreeSpace)
	{
		num = ((*class)->getFreeSpace(_this));
	}

	return num;
}


uint32_t cBuffer_getPending (void* _this)
{
	/**
	 * Este método llama a la función getPending de la clase que implementa la interfaz cBuffer
	 */
	const struct cBuffer* const * class = _this;
	uint32_t num = 0;

	if (_this && class && (*class)->getPending)
	{
		num = ((*class)->getPending(_this));
	}

	return num;
}


uint32_t cBuffer_length (void* _this)
{
	/**
	 * Este método llama a la función length de la clase que implementa la interfaz cBuffer
	 */
	const struct cBuffer* const * class = _this;
	uint32_t num = 0;

	if (_this && class && (*class)->length)
	{
		num = ((*class)->length(_this));
	}

	return num;
}
