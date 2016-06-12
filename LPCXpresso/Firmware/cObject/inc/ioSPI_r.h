/**
 * @addtogroup grp_ioSPI ioSPI
 * @{
 */

/**
 * @file ioSPI_r.h
 * @author Mariano Mondani
 *
 * @brief Estructura interna de la clase ioSPI.
 * @details Este archivo no debe ser incluido en la aplicación del usuario.
 */

#ifndef IOSPI_R_H
#define IOSPI_R_H


#include "ioComm_r.h"


// ********************************************************************************
/**
 * @brief      Estructura de la clase ioSPI.
 */
struct ioSPI
{
	const void* class;				///< Puntero a la interfaz @ref grp_ioComm
	void* periphMem;				///< Dirección base de memoria del periférico.
	uint32_t clockRate;				///< Velocidad del clock del SPI.
	uint32_t dataLen;				///< Cantidad de bits de cada dato. Ver @ref ioSPI_Data.
	uint32_t clockMode;				///< Modo del clock. Ver @ref ioSPI_ClockMode.
	uint32_t spiMode;				///< Modo de funcionamiento del SPI. Ver @ref ioSPI_SPIMode.
};

// ********************************************************************************


// ********************************************************************************
/**
 * @name Macros privados
 * @brief Macros de get y set para ser usados de forma privada por la clase ioSPI y por las que la heredan.
 * @{
*/
#define periphMem(p)				(((const struct ioSPI*)p)->periphMem)
#define clockRate(p)				(((const struct ioSPI*)p)->clockRate)
#define dataLen(p)			    	(((const struct ioSPI*)p)->dataLen)
#define clockMode(p)				(((const struct ioSPI*)p)->clockMode)
#define spiMode(p)			    	(((const struct ioSPI*)p)->spiMode)

#define set_periphMem(p, v)			(((struct ioSPI*)p)->periphMem = (v))
#define set_clockRate(p, v)			(((struct ioSPI*)p)->clockRate = (v))
#define set_dataLen(p, v)			(((struct ioSPI*)p)->dataLen = (v))
#define set_clockMode(p, v)			(((struct ioSPI*)p)->clockMode = (v))
#define set_spiMode(p, v)			(((struct ioSPI*)p)->spiMode = (v))

///@}
// ********************************************************************************


#endif

///@}
