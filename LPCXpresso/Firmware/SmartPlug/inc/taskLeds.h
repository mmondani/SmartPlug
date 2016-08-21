/**
 * @addtogroup grp_taskLeds taskLeds
 * @brief Tarea que se encarga de realizar la señalizaciones indicadas por otras tareas.
 * @details Las señalizaciones se forman a partir de un led bicolor (rojo y verde), tanto encendiéndolos, apagándolos o haciéndolos destellar a distintas frecuencias.
			Es una tarea periódica para actualizar el valor de los leds.
 * @{
 */

/**
 * @file taskLeds.h
 * @author Mariano Mondani
 *
 * @brief Interfaz de la tarea taskLeds
 */


#ifndef TASK_LEDS_H_
#define TASK_LEDS_H_


#include "chip.h"


/**
 * @brief      Inicializa la tarea y la alarma alarmRunTaskLeds.
 */
void taskLeds_init (void);


/**
 * @brief      Configura la señalización de uno de los leds.
 *
 * @details    La longitud no está dada en bytes, el tamaño del objeto almacenado es la unidad.
 *  		   Si se tapa un tOn y un tOff distintos de cero, el led va a destellar. Si se pasa
 *  		   tOn == 0 y tOff != 0, el led se va a apagar. Si se pasa tOn != 0 y tOff == 0, el
 *  		   led se va a encender.
 *
 * @param      ledID  ID del led que se quiere configurar. Ver @ref ledID_t.
 * 			   tOn    tiempo, en milisegundos, que debe permanecer encendido el led.
 * 			   tOff   tiempo, en milisegundos, que debe permanecer apagado el led.
 */
void taskLeds_blinkLed (uint32_t ledID, uint32_t tOn, uint32_t tOff);


/**
 * Enumeración con los ID de los leds presentes en el Smart Plug
 */
enum ledID_t{
	   LED_ID_RED = 0,			    	///< Led rojo
	   LED_ID_GREEN = 1,				///< Led verde
};


#endif


/** @}*/
