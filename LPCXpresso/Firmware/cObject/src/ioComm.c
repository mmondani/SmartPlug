/**
 * @file ioComm.c
 * @author Mariano Mondani
 *
 * @brief Implementación de los métodos de la interfaz ioComm.
 */

#include "ioComm.h"
#include "ioComm_r.h"


uint32_t ioComm_writeBytes (void* _this, uint32_t len, uint8_t* data)
{
	const struct ioComm* const * class = _this;
	uint32_t nBytes;

	if (_this && class && (*class)->writeBytes)
	{
		nBytes = (*class)->writeBytes(_this, len, data);
	}

	 return nBytes;
}


uint32_t ioComm_readBytes (void* _this, uint32_t len, uint8_t* data)
{
	const struct ioComm* const * class = _this;
	uint32_t nBytes;

	if (_this && class && (*class)->readBytes)
	{
		nBytes = (*class)->readBytes(_this, len, data);
	}

	 return nBytes;
}


uint32_t ioComm_freeSpace (void* _this)
{
	const struct ioComm* const * class = _this;
	uint32_t res;

	if (_this && class && (*class)->freeSpace)
	{
		res = (*class)->freeSpace(_this);
	}

	 return res;
}


uint32_t ioComm_dataAvailable (void* _this)
{
	const struct ioComm* const * class = _this;
	uint32_t nBytes;

	if (_this && class && (*class)->dataAvailable)
	{
		nBytes = (*class)->dataAvailable(_this);
	}

	 return nBytes;
}


void	 ioComm_intEnable (void* _this, uint32_t mask)
{
	const struct ioComm* const * class = _this;


	if (_this && class && (*class)->intEnable)
	{
		(*class)->intEnable(_this, mask);
	}
}


void	 ioComm_intDisable (void* _this, uint32_t mask)
{
	const struct ioComm* const * class = _this;


	if (_this && class && (*class)->intDisable)
	{
		(*class)->intDisable(_this, mask);
	}
}


uint32_t ioComm_getInt (void* _this, uint32_t intID)
{
	const struct ioComm* const * class = _this;
	uint32_t res;

	if (_this && class && (*class)->getInt)
	{
		res = (*class)->getInt(_this, intID);
	}

	return res;
}
