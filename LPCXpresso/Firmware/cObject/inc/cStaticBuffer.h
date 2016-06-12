/**
 * @addtogroup grp_cStaticBuffer cStaticBuffer
 *
 * @brief Implementa la interfaz @ref grp_cBuffer.
 * @details Es un buffer de tamaño fijo. Esta clase sirve de base para tipos más específicos de buffer.
 * La mayoría de los métodos de la interfaz cBuffer no están implementados ya que no se modela un tipo particular de buffer.
 * @{
 */

/**
 * @file cStaticBuffer.h
 * @brief Métodos públicos de la clase cStatucBuffer.
 */
#ifndef CSTATICBUFFER_H
#define CSTATICBUFFER_H

#include "cBuffer.h"
#include <stdint.h>
#include <string.h>


// ********************************************************************************
/**
 * @var cStaticBuffer
 * @brief Nombre de la clase.
 * @details La forma de instanciar esta clase es la siguiente:
 * @code
 * void* staticBuffer = cObject_new(cStaticBuffer, len, objectSize)
 * @endcode
 * @param cStaticBuffer nombre de la clase a instanciar.
 * @param len			cantidad de objetos que puede contener el buffer.
 * @param objectSize	tamaño, en bytes, de cada objeto que contiene el buffer.
 */
extern const void* cStaticBuffer;

// ********************************************************************************

#endif

///@}
