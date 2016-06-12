/**
 * @addtogroup grp_cQueue cQueue
 * @{
 */

/**
 * @file 		cQueue_r.h
 * @brief Estructura interna de la clase cQueue.
 * @details Este archivo no debe ser incluido en la aplicación del usuario.
 */


#ifndef CQUEUE_R_H_
#define CQUEUE_R_H_

#include "cStaticBuffer_r.h"


// ********************************************************************************
/**
 * @brief      Estructura de la clase cQueue.
 */
struct cQueue
{
	struct cStaticBuffer _;			///< Extiende la clase cStaticBuffer.
	uint32_t ptrIn;					///< Puntero de escritura de cQueue
	uint32_t ptrOut;				///< Puntero de lectura de cQueue.
};

// ********************************************************************************


// ********************************************************************************
/**
 * @name Macros privados
 * @brief Macros de get y set para ser usados de forma privada por la clase cQueue y por las que la heredan.
 * @{
*/
#define ptrIn(p)					(((const struct cQueue*)p)->ptrIn)
#define ptrOut(p)					(((const struct cQueue*)p)->ptrOut)

#define set_ptrIn(p, v)			    (((struct cQueue*)p)->ptrIn = (v))
#define set_ptrOut(p, v)			(((struct cQueue*)p)->ptrOut = (v))


///@}
// ********************************************************************************

#endif


///@}
