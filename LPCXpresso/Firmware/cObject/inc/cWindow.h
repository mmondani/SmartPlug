/**
 * @addtogroup grp_cWindow cWindow
 *
 * @brief Extiende la clase cStaticBuffer.
 * @details Implementa un buffer de tipo ventana. Se define una longitud fija y cada vez que se agrega un elemento,
 * se lo inserta al inicio del buffer (posición 0) y el resto de los elementos se desplaza una posición hacia el final, eliminándose
 * el último objeto.
 * @{
 */

/**
 * @file cWindow.h
 * @brief Métodos públicos de la clase cWindow.
 */

#ifndef CWINDOW_H_
#define CWINDOW_H_

#include "cStaticBuffer.h"


// ********************************************************************************
/**
 * @var cWindow
 * @brief Nombre de la clase.
 * @details La forma de instanciar esta clase es la siguiente:
 * @code
 * void* window = cObject_new(cWindow, len, objectSize);
 * @endcode
 * @param cWindow		nombre de la clase a instanciar.
 * @param len			cantidad de objetos que puede contener el cWindow.
 * @param objectSize	tamaño, en bytes, de cada objeto que contiene el cWindow.
 */
extern const void* cWindow;

// ********************************************************************************



#endif

///@}

