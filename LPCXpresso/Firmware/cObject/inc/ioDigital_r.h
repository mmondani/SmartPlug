/**
 * @addtogroup grp_ioDigital ioDigital
 * @{
 */

/**
 * @file ioDigital_r.h
 * @author Mariano Mondani
 *
 * @brief Estructura interna de la clase ioDigital.
 * @details Este archivo no debe ser incluido en la aplicación del usuario.
 */

#ifndef IODIGITAL_R_H_
#define IODIGITAL_R_H_

#include "ioGPIO_r.h"

/**
 * @brief      Estructura de la clase ioDigital.
 */
struct ioDigital
{
	struct ioGPIO _;			///< Extiende la clase ioGPIO.
};

#endif


///@}
