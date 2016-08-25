/**
 * @addtogroup grp_taskSmartPlug taskSmartPlug
 * @brief Es la tarea principal del Smart Plug. Se encarga de implementar la lógica del dispositivo. Es una tarea que se bloquea esperando eventos.
 * @{
 */

/**
 * @file taskSmartPlug.h
 * @author Mariano Mondani
 *
 * @brief Interfaz de la tarea taskSmartPlug
 */


#ifndef TASK_SMART_PLUG_H_
#define TASK_SMART_PLUG_H_

#include "chip.h"


void taskSmartPlug_init (void* _eeprom);
uint32_t taskSmartPlug_getLoadState (void);

#endif

/** @}*/
