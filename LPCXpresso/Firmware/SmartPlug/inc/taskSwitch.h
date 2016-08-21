/**
 * @addtogroup grp_taskSwitch taskSwitch
 * @brief Gestiona el pulsador de tipo tact switch que se encuentra en el PCB.
 * @details Realiza el anti-rebote y genera los eventos cuando es presionado.
			Va a generar un evento cuando se suelte el pulsador (evSwitch) y otro cuando se mantenga presionado por más de 5 segundos (evSwitch_5sec).
			Es una tarea de ejecución periódica.

 * @{
 */

/**
 * @file taskSwitch.h
 * @author Mariano Mondani
 *
 * @brief Interfaz de la tarea taskSwitch
 */


#ifndef TASK_SWITCH_H_
#define TASK_SWITCH_H_



/**
 * @brief      Inicializa la tarea y la alarma alarmRunTaskSwitch.
 */
void taskSwitch_init (void);


#endif


/** @}*/
