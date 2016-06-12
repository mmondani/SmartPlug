/**
 * @addtogroup grp_ioEE25LCxxx ioEE25LCxxx
 * @{
 */

/**
 * @file ioEE25LCxxx_r.h
 * @author Mariano Mondani
 *
 * @brief Estructura interna de la clase ioEE25LCxxx.
 * @details Este archivo no debe ser incluido en la aplicación del usuario.
 */

#ifndef IOEE25LCXXX_R_H_
#define IOEE25LCXXX_R_H_

#include "ioGPIO_r.h"


// ********************************************************************************
/**
 * @brief      Estructura de la clase ioEE25LCxxx.
 */
struct ioEE25LCxxx
{
	const void* class;			///< Puntero a la interfaz @ref grp_cObject.
	void* spiPort;				///< Instancia de la clase ioSPI que comunica a la EEPROM externa.
	void* sselPin;				///< Instancia de la clase ioDigital que maneja el pin Slave Select de la EEPROM externa.
	uint32_t eeSize;			///< Tamaño de la EEPROM. Ver @ref ioEE25LCxxx_EEPROM_Size.
};

// ********************************************************************************



// ********************************************************************************
/**
 * @name Macros privados
 * @brief Macros de get y set para ser usados de forma privada por la clase ioEE25LCxxx y por las que la heredan.
 * @{
*/
#define spiPort(p)					(((const struct ioEE25LCxxx*)p)->spiPort)
#define sselPin(p)					(((const struct ioEE25LCxxx*)p)->sselPin)
#define eeSize(p)					(((const struct ioEE25LCxxx*)p)->eeSize)

#define set_spiPort(p, v)			(((struct ioEE25LCxxx*)p)->spiPort = (v))
#define set_sselPin(p, v)			(((struct ioEE25LCxxx*)p)->sselPin = (v))
#define set_eeSize(p, v)			(((struct ioEE25LCxxx*)p)->eeSize = (v))


///@}
// ********************************************************************************



#endif


///@}
