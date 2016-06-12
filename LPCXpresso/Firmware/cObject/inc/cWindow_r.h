/**
 * @addtogroup grp_cWindow cWindow
 * @{
 */

/**
 * @file 		cWindow_r.h
 * @brief Estructura interna de la clase cWindow.
 * @details Este archivo no debe ser incluido en la aplicación del usuario.
 */

#ifndef CWINDOW_R_H_
#define CWINDOW_R_H_

#include "cStaticBuffer_r.h"


/**
 * @brief      Estructura de la clase cWindow.
 */
struct cWindow
{
	struct cStaticBuffer _;			///< Extiende la clase cStaticBuffer.
};

#endif

///@}
