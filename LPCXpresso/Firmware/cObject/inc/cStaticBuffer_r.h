/**
 * @addtogroup grp_cStaticBuffer cStaticBuffer
 * @{
 */

/**
 * @file cStaticBuffer_r.h
 * @brief Estructura interna de la clase cStaticBuffer.
 * @details Este archivo no debe ser incluido en la aplicación del usuario.
 */

#ifndef CSTATICBUFFER_R_H
#define CSTATICBUFFER_R_H


#include "cBuffer_r.h"



// ********************************************************************************
/**
 * @brief      Estructura de la clase cStaticBuffer.
 */
struct cStaticBuffer
{
	const void* class;		///< Puntero a la interfaz cBuffer
	void* buffer;			///< Puntero al área de memoria reservada para guardar los objetos contenidos en el cStaticBuffer.
	uint32_t len;			///< Longitud del cStaticBuffer, expresado en cantidad de objetos que puede contener.
	size_t objectSize;		///< Tamaño, en bytes, del objeto que debe contener el buffer.
};

// ********************************************************************************


// ********************************************************************************
/**
 * @name Macros privados
 * @brief Macros de get y set para ser usados de forma privada por la clase cStaticBuffer y por las que la heredan.
 * @{
*/
#define buffer(p)		(((const struct cStaticBuffer*)p)->buffer)
#define len(p)			(((const struct cStaticBuffer*)p)->len)
#define objectSize(p)	(((const struct cStaticBuffer*)p)->objectSize)

#define set_buffer(p, v)			(((struct cStaticBuffer*)p)->buffer = (v))
#define set_len(p, v)				(((struct cStaticBuffer*)p)->len = (v))
#define set_objectSize(p, v)		(((struct cStaticBuffer*)p)->objectSize = (v))


///@}
// ********************************************************************************


#endif

///@}
