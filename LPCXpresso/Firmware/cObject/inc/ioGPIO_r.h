/**
 * @addtogroup grp_ioGPIO ioGPIO
 * @{
 */

/**
 * @file ioGPIO_r.h
 * @author Mariano Mondani
 *
 * @brief Estructura interna de la clase ioGPIO.
 * @details Este archivo no debe ser incluido en la aplicación del usuario.
 */

#ifndef IOGPIO_R_H
#define IOGPIO_R_H


#include "ioObject_r.h"


// ********************************************************************************
/**
 * @brief      Estructura de la clase ioGPIO.
 */
struct ioGPIO
{
	const void* class;			///< Puntero a la interfaz @ref grp_ioObject
	void* periphMem;			///< Dirección base de memoria del periférico.
	uint32_t direction;			///< Dirección del GPIO. Ver @ref ioGPIO_Direction.
	uint32_t port;				///< Número de puerto en el que se encuentra el GPIO.
	uint32_t pin;				///< Número de pin dentro del puerto.
};

// ********************************************************************************


// ********************************************************************************
/**
 * @name Macros privados
 * @brief Macros de get y set para ser usados de forma privada por la clase ioGPIO y por las que la heredan.
 * @{
*/
#define periphMem(p)			(((const struct ioGPIO*)p)->periphMem)
#define direction(p)			(((const struct ioGPIO*)p)->direction)
#define port(p)					(((const struct ioGPIO*)p)->port)
#define pin(p)					(((const struct ioGPIO*)p)->pin)

#define set_periphMem(p, v)			(((struct ioGPIO*)p)->periphMem = (v))
#define set_direction(p, v)			(((struct ioGPIO*)p)->direction = (v))
#define set_port(p, v)				(((struct ioGPIO*)p)->port = (v))
#define set_pin(p, v)				(((struct ioGPIO*)p)->pin = (v))


///@}
// ********************************************************************************

#endif


///@}
