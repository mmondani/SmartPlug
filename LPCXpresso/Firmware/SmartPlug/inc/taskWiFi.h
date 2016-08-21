/**
 * @addtogroup grp_taskWiFi taskWiFi
 * @brief Se encarga de recibir las conexiones desde la aplicación móvil
 * @details Va a interaccionar con otras tareas para enviar los datos pedidos por la app móvil o para configurar el Smart Plug de acuerdo a lo indicado por la app móvil.
			La tarea es periódica para controlar lo que llega desde el módulo RN1723  de forma asincrónica.
 * @{
 */

/**
 * @file taskWiFi.h
 * @author Mariano Mondani
 *
 * @brief Interfaz de la tarea taskWiFi
 */


#ifndef TASK_WIFI_H_
#define TASK_WIFI_H_


#include "chip.h"


/**
 * @brief      Inicializa la tarea y la alarma alarmRunTaskWiFi.
 */
void taskWiFi_init (void* _eeprom);


/**
 * @brief      Le indica a la tarea que inicie el proceso de WPS
 *
 */
void taskLeds_initWPS (void);


/**
 * @brief      Le indica a la tarea que inicie el modo Soft-AP para poder configurar la red WiFi por medio del web server embebido.
 *
 */
void taskLeds_initWebServer (void);


/**
 * @brief      Retorna si el módulo está unido a una red WiFi o no.
 *
 * @return     0 si no está unido. 1 si está unido.
 */
void taskLeds_isAuthenticated (void);


/**
 * @brief      Retorna si el módulo está conectado a un socket TCP o no.
 *
 * @return     0 si no está conectado. 1 si está conectado.
 */
void taskLeds_isTPCConnected (void);



#endif


/** @}*/
