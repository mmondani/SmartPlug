/**
 * @addtogroup grp_ioInternalRTC ioInternalRTC
 *
 * @brief Implementa la interfaz @ref grp_ioRTC.
 * @details Esta clase permite utilizar el RTC incluido en el microcontrolador.
 * @{
 */

/**
 * @file ioInternalRTC.h
 * @author Mariano Mondani
 *
 * @brief Métodos públicos de la clase ioInternalRTC.
 */

#ifndef IOINTERNALRTC_H
#define IOINTERNALRTC_H

#include "ioRTC.h"



// ********************************************************************************
/**
 * @var ioInternalRTC
 * @brief Nombre de la clase.
 * @details La forma de instanciar esta clase es la siguiente:
 * @code
 * void* rtc = cObject_new(ioInternalRTC, periphMem)
 * @endcode
 * @param ioGPIO 			nombre de la clase a instanciar.
 * @param periphMem			dirección base de memoria del periférico.
 */
extern const void* ioGPIO;

// ********************************************************************************




#endif


///@}
