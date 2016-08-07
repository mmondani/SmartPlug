/**
 * @addtogroup grp_ioInternalRTC ioInternalRTC
 * @{
 */

/**
 * @file ioInternalRTC_r.h
 * @author Mariano Mondani
 *
 * @brief Estructura interna de la clase ioInternalRTC.
 * @details Este archivo no debe ser incluido en la aplicación del usuario.
 */

#ifndef IOINTERNALRTC_R_H
#define IOINTERNALRTC_R_H


#include "ioRTC_r.h"


// ********************************************************************************
/**
 * @brief      Estructura de la clase ioInternalRTC.
 */
struct ioInternalRTC
{
	const void* class;			///< Puntero a la interfaz @ref grp_ioObject
	void* periphMem;			///< Dirección base de memoria del periférico.
};

// ********************************************************************************


// ********************************************************************************
/**
 * @name Macros privados
 * @brief Macros de get y set para ser usados de forma privada por la clase ioGPIO y por las que la heredan.
 * @{
*/
#define periphMem(p)			(((const struct ioInternalRTC*)p)->periphMem)

#define set_periphMem(p, v)			(((struct ioInternalRTC*)p)->periphMem = (v))


///@}
// ********************************************************************************

#endif


///@}
