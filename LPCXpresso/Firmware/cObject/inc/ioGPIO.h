/**
 * @addtogroup grp_ioGPIO ioGPIO
 *
 * @brief Implementa la interfaz @ref grp_ioObject.
 * @details Esta clase sirve de base para tipos más específicos de GPIO.
 * La mayoría de los métodos de la interfaz ioGPIO no están implementados ya que no se modela un tipo particular de GPIO.
 * @{
 */

/**
 * @file ioGPIO.h
 * @author Mariano Mondani
 *
 * @brief Métodos públicos de la clase ioGPIO.
 */

#ifndef IOGPIO_H
#define IOGPIO_H

#include "ioObject.h"



// ********************************************************************************
/**
 * @var ioGPIO
 * @brief Nombre de la clase.
 * @details La forma de instanciar esta clase es la siguiente:
 * @code
 * void* gpio = cObject_new(ioGPIO, periphMem, direction, port, pin)
 * @endcode
 * @param ioGPIO 			nombre de la clase a instanciar.
 * @param periphMem			dirección base de memoria del periférico.
 * @param direction			dirección del GPIO. Ver @ref ioGPIO_Direction.
 * @param port				número de puerto en el que se encuentra el GPIO.
 * @param pin				número de pin dentro del puerto.
 */
extern const void* ioGPIO;

// ********************************************************************************



// ********************************************************************************
/**
 * Enumeración con las posibles direcciones de un GPIO.
 */
enum ioGPIO_Direction {
	IOGPIO_INPUT = 0,		///< GPIO configurado como entrada.
	IOGPIO_OUTPUT			///< GPIO configurado como salida.
};
// ********************************************************************************



#endif


///@}
