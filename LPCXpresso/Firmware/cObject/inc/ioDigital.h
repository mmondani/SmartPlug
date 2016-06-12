/**
 * @addtogroup grp_ioDigital ioDigital
 *
 * @brief Extiende la clase @ref grp_ioGPIO.
 * @details Implementa un GPIO de tipo digital que puede ser entrada o salida.
 * @{
 */

/**
 * @file ioDigital.h
 * @author Mariano Mondani
 *
 * @brief Métodos públicos de la clase ioDigital.
 */

#ifndef IODIGITAL_H_
#define IODIGITAL_H_

#include "ioGPIO.h"



// ********************************************************************************
/**
 * @var ioDigital
 * @brief Nombre de la clase.
 * @details La forma de instanciar esta clase es la siguiente:
 * @code
 * void* digitalPin = cObject_new(ioDigital, periphMem, direction, port, pin)
 * @endcode
 * @param ioDigital 		nombre de la clase a instanciar.
 * @param periphMem			dirección base de memoria del periférico.
 * @param direction			dirección del GPIO. Ver @ref ioGPIO_Direction.
 * @param port				número de puerto en el que se encuentra el GPIO.
 * @param pin				número de pin dentro del puerto.
 */
extern const void* ioDigital;

// ********************************************************************************



// ********************************************************************************
/**
 * @name Métodos de la clase ioDigital
 * @{
 */


/**
 * @brief      Hace un toggle del estado del pin digital (si es salida).
 *
 * @param      _this  instancia de ioDigital.
 */
void ioDigital_toggle (void* _this);


///@}
// ********************************************************************************


#endif


///@}
