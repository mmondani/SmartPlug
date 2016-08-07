/**
 * @addtogroup grp_ioRTC ioRTC
 *
 * @brief Interfaz que extiende a @ref grp_ioObject.
 * @details Esta interfaz tiene los métodos básicos que puede necesitar cualquier clase de RTC.
 * @{
 */

/**
 * @file ioRTC.h
 *
 * @brief Métodos de la interfaz ioRTC.
 * @author Mariano Mondani
 */


#ifndef IORTC_H_
#define IORTC_H_


#include "ioObject.h"
#include "rtc_time_t_r.h"

// ********************************************************************************
/**
 * Enumeración con los elementos de la fecha y hora.
 */
enum RTC_TIME_ELEMENTS{
	   RTC_TIME_SECOND = 0,			    	///< Segundos
	   RTC_TIME_MINUTE = 1,					///< Minutos
	   RTC_TIME_HOUR = 2,					///< Horas
	   RTC_TIME_DAY_OF_MONTH = 3,			///< Día del mes
	   RTC_TIME_DAY_OF_WEEK = 4,			///< Día de la semana
	   RTC_TIME_DAY_OF_YEAR = 5,			///< Día del año
	   RTC_TIME_MONTH = 6,					///< Mes
	   RTC_TIME_YEAR = 7					///< Año
};
// ********************************************************************************



/**
 * @name Métodos de la interfaz ioRTC
 * @{
 */


/**
 * @brief      Reinicia el contador del RTC.
 *
 * @param      _this  instancia de una clase que implementa a ioRTC.
 *
 */
void ioRTC_reset (void* _this);



/**
 * @brief      Configura una fecha y hora en el RTC
 *
 * @param      _this  instancia de una clase que implementa a ioRTC.
 * @param	   time   puntero a la estructura @ref rtc_time_t
 *
 * @return     0 en caso de éxito. Otro valor si hubo un error.
 */
uint32_t ioRTC_setFullTime (void* _this, rtc_time_t* time);



/**
 * @brief      Obtiene la fecha y hora actual.
 *
 * @param      _this  instancia de una clase que implementa a ioRTC.
 * @param	   time   puntero a la estructura @ref rtc_time_t
 *
 * @return     0 en caso de éxito. Otro valor si hubo un error.
 */
uint32_t ioRTC_getFullTime (void* _this, rtc_time_t* time);



/**
 * @brief      Configura un valor específico de la fecha o la hora.
 *
 * @param      _this    instancia de una clase que implementa a ioRTC.
 * @param      element  elemento que se quiere cargar de la fecha o la hora. Ver @ref RTC_TIME_ELEMENTS.
 *
 * @return     0 en caso de éxito. Otro valor si hubo un error.
 */
uint32_t ioRTC_setTime (void* _this, uint32_t element, uint32_t value);



/**
 * @brief      Lee un valor específico de la fecha o la hora
 *
 * @param      _this    instancia de una clase que implementa a ioRTC.
 * @param      element  elemento que se quiere leer de la fecha o la hora. Ver @ref RTC_TIME_ELEMENTS.
 *
 * @return     Valor leido.
 */
uint32_t ioRTC_getTime (void* _this, uint32_t element);



///@}

#endif


///@}
