/**
 * @addtogroup grp_cQueue cQueue
 *
 * @brief Extiende la clase @ref grp_cStaticBuffer.
 * @details Implementa un buffer de tipo cola. Se define la longitud de la cQueue, la cual va a tener un comportamiento FIFO: el primer
 * objeto que se agrega es el primero que se elimina.
 * @{
 */

/**
 * @file cQueue.h
 * @brief Métodos públicos de la clase cQueue.
 */


#ifndef CQUEUE_H_
#define CQUEUE_H_

#include "cStaticBuffer.h"


// ********************************************************************************
/**
 * @var cQueue
 * @brief Nombre de la clase.
 * @details La forma de instanciar esta clase es la siguiente:
 * @code
 * void* queue = cObject_new(cQueue, len, objectSize);
 * @endcode
 * @param cWindow		nombre de la clase a instanciar.
 * @param len			cantidad de objetos que puede contener el cQueue.
 * @param objectSize	tamaño, en bytes, de cada objeto que contiene el cQueue.
 */
extern const void* cQueue;

// ********************************************************************************



#endif

///@}
