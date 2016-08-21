/**
 * @addtogroup grp_taskTimer taskTimer
 * @brief Tarea usada por las clases que necesitan generar una base de tiempo. Por ejemplo la clase cTimer. Es una tarea periódica.
 * @{
 */

/**
 * @file taskTimer.h
 * @author Mariano Mondani
 *
 * @brief Interfaz de la tarea taskTimer
 */


#ifndef TASK_TIMER_H_
#define TASK_TIMER_H_


/**
 * @brief      Inicializa la tarea y la alarma alarmRunTaskTimer.
 */
void taskTimer_init (void);


#endif


/** @}*/
