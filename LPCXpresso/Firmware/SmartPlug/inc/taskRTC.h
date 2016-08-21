/**
 * @addtogroup grp_taskRTC taskRTC
 * @brief Se encarga de configurar y acceder al RTC interno del microcontrolador. Es una tarea periódica
 * @{
 */

/**
 * @file taskRTC.h
 * @author Mariano Mondani
 *
 * @brief Interfaz de la tarea taskRTC
 */


#ifndef TASK_RTC_H_
#define TASK_RTC_H_


#include "rtc_time_t_r.h"

/**
 * @brief      Inicializa la tarea y la alarma alarmRunTaskTimer.
 */
void taskRTC_init (void);


/**
 * @brief      Configura el tiempo pasado en fullTime en el RTC interno.
 *
 * @param      fullTime  puntero a estructura rtc_time_t que contiene la fecha y la hora a cargar en el RTC.
 */
void taskRTC_setTime (rtc_time_t* fullTime);


/**
 * @brief      Devuelve el tiempo actual del RTC interno en el puntero a la estructura fullTime.
 *
 * @param      fullTime  puntero a estructura rtc_time_t donde se va a cargar la fecha y la hora del RTC interno.
 * 						 Si no está sincronizado el RTC, se devuelve la estructura fullTime cargada con 0 en los campos.
 */
void taskRTC_getTime (rtc_time_t* fullTime);


/**
 * @brief      Devuelve el tiempo actual del RTC interno en el puntero a la estructura fullTime.
 *
 * @return     0 si el RTC no fue sincronizado por el taskWiFi. 1 si fue sincronizado.
 */
uint32_t taskRTC_isTimeSynchronized (void);


#endif


/** @}*/
