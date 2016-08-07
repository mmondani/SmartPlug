/**
 * @file ioRTC.c
 * @author Mariano Mondani
 *
 * @brief Implementación de los métodos de la interfaz ioRTC.
 */

#include "ioRTC.h"
#include "ioRTC_r.h"



void ioRTC_reset (void* _this)
{
	/**
	 * Este método llama a la función reset de la clase que implementa la interfaz ioRTC
	 */
	const struct ioRTC* const * class = _this;

	if (_this && class && (*class)->reset)
	{
		(*class)->reset(_this);
	}
}


uint32_t ioRTC_setFullTime (void* _this, rtc_time_t* time)
{
	/**
	 * Este método llama a la función setFullTime de la clase que implementa la interfaz ioRTC
	 */
	const struct ioRTC* const * class = _this;
	uint32_t res = 1;

	if (_this && class && (*class)->setFullTime)
	{
		res = (*class)->setFullTime(_this, time);
	}

	return res;
}


uint32_t ioRTC_getFullTime (void* _this, rtc_time_t* time)
{
	/**
	 * Este método llama a la función getFullTime de la clase que implementa la interfaz ioRTC
	 */
	const struct ioRTC* const * class = _this;
	uint32_t res = 1;

	if (_this && class && (*class)->getFullTime)
	{
		res = (*class)->getFullTime(_this, time);
	}

	return res;
}


uint32_t ioRTC_setTime (void* _this, uint32_t element, uint32_t value)
{
	/**
	 * Este método llama a la función setTime de la clase que implementa la interfaz ioRTC
	 */
	const struct ioRTC* const * class = _this;
	uint32_t res = 1;

	if (_this && class && (*class)->setTime)
	{
		res = (*class)->setTime(_this, element, value);
	}

	return res;
}


uint32_t ioRTC_getTime (void* _this, uint32_t element)
{
	/**
	 * Este método llama a la función getTime de la clase que implementa la interfaz ioRTC
	 */
	const struct ioRTC* const * class = _this;
	uint32_t res = 0;

	if (_this && class && (*class)->getTime)
	{
		res = (*class)->getTime(_this, element);
	}

	return res;
}

