/**
 * @addtogroup grp_cTimer cTimer
 *
 * @brief Implementa la interfaz @ref grp_cObject.
 * @details Esta clase implementa un timer básico. Su handler debe ser llamado periódicamente
 * para generar la base de tiempo que usan todas las instancias de cTimer.
 * @{
 */

/**
 * @file cTimer.h
 * @author Mariano Mondani
 *
 * @brief Métodos públicos de la clase cTimer.
 */

#ifndef CTIMER_H
#define CTIMER_H

#include "cObject.h"



// ********************************************************************************
/**
 * @var cTimer
 * @brief Nombre de la clase.
 * @details La forma de instanciar esta clase es la siguiente:
 * @code
 * void* timer = cObject_new(cTimer)
 * @endcode
 * @param cTimer	 		nombre de la clase a instanciar.
 */
extern const void* cTimer;

// ********************************************************************************



// ********************************************************************************
/**
 * @name Métodos de la clase cTimer
 * @{
 */


/**
 * @brief      inicia el timer con el intervalo interval
 *
 * @param      _this  instancia de la clase cTimer.
 * @param      interval  duración del timer, expresada en ticks.
 *
 */
void cTimer_start (void* _this, uint32_t interval);


/**
 * @brief      reinicia el timer con el mismo intervalo con que fue iniciado la última vez.
 *
 * @param      _this  instancia de la clase cTimer.
 *
 */
void cTimer_restart (void* _this);


/**
 * @brief      devuelve si el timer expiró o no
 *
 * @param      _this  instancia de la clase cTimer.
 *
 * @return     0 el timer no expiró.
 * 			   1 el timer expiró.
 */
uint32_t cTimer_hasExpired (void* _this);


/**
 * @brief      detiene el timer.
 *
 * @param      _this  instancia de la clase cTimer.
 *
 */
void cTimer_stop (void* _this);


/**
 * @brief      devuelve si el timer está activo o no.
 *
 * @param      _this  instancia de la clase ioDebounce.
 *
 * @return     0 si el timer no está activo.
 * 			   1 si el timer está activo.
 */
uint32_t cTimer_isRunning (void* _this);


///@}
// ********************************************************************************



#endif


///@}
