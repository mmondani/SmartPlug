/**
 * @addtogroup grp_cTimer cTimer
 * @{
 */

/**
 * @file cTimer_r.h
 * @author Mariano Mondani
 *
 * @brief Estructura interna de la clase cTimer.
 * @details Este archivo no debe ser incluido en la aplicación del usuario.
 */

#ifndef CTIMER_R_H_
#define CTIMER_R_H_

#include "cObject_r.h"


// ********************************************************************************
/**
 * @brief      Estructura de la clase cTimer.
 */
struct cTimer
{
	const void* class;			///< Puntero a la interfaz @ref grp_cObject.
	uint32_t start;				///< Guarda el valor de los ticks de la base de tiempo al momento de iniciar el timer.
	uint32_t interval;			///< Duración del timer.
	uint32_t running;			///< Indica si el timer está corriendo o no.
};

// ********************************************************************************



// ********************************************************************************
/**
 * @name Macros privados
 * @brief Macros de get y set para ser usados de forma privada por la clase cTimer y por las que la heredan.
 * @{
*/
#define start(p)						(((const struct cTimer*)p)->start)
#define interval(p)						(((const struct cTimer*)p)->interval)
#define running(p)						(((const struct cTimer*)p)->running)


#define set_start(p, v)					(((struct cTimer*)p)->start = (v))
#define set_interval(p, v)				(((struct cTimer*)p)->interval = (v))
#define set_running(p, v)				(((struct cTimer*)p)->running = (v))




///@}
// ********************************************************************************



#endif


///@}
