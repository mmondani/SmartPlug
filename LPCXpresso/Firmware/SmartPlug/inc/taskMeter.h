/**
 * @addtogroup grp_taskMeter taskMeter
 * @brief Se encarga de obtener periódicamente las mediciones del front-end analógico CS5490.
 * @details Además acumula los pulsos generados por el mismo integrado para llevar un registro de la energía consumida por la carga.
			Es una tarea periódica.
 * @{
 */

/**
 * @file taskMeter.h
 * @author Mariano Mondani
 *
 * @brief Interfaz de la tarea taskMeter
 */


#ifndef TASK_METER_H_
#define TASK_METER_H_


#include "chip.h"


/**
 * @brief      Inicializa la tarea y la alarma alarmRunTaskMeter.
 */
void taskMeter_init (void* _eeprom);


/**
 * @brief      Retorna el valor de la variable variableID.
 *
 * @param      variableID  ID de la variable de la que se quiere conocer el valor. Ver @ref variableID_t.
 *
 * @return	   valor de la variable.
 */
float taskMeter_getMeterValue (uint32_t variableID);


/**
 * @brief      Reinicia la energía acumulada.
 */
void taskMeter_resetEnergyPulses (void);


/**
 * @brief      Comienza el proceso de calibración indicado.
 *
 * @param		processID    ID del proceso de calibración que se quiere realizar. Ver @ref processID_t
 */
void taskMeter_initCalibrationProcess (uint32_t processID);



/**
 * Enumeración con los ID de las variables informadas por el CS5490
 */
enum variableID_t{
	   ID_VRMS = 0,			    	///< Tensión RMS
	   ID_IRMS,						///< Corriente RMS
	   ID_FREQUENCY,				///< Frecuencia de línea
	   ID_ENERGY,					///< Energía consumida
	   ID_POWER_FACTOR,				///< Factor de potencia
	   ID_ACTIVE_POWER				///< Potencia activa
};


/**
 * Enumeración con los ID de procesos de calibración.
 */
enum processID_t{
	   ID_VDC_OFFSET = 0,			///< Calibración de offset de continua en el canal de tensión
	   ID_IDC_OFFSET,				///< Calibración de offset de continua en el canal de corrient
	   ID_VAC_GAIN,					///< Calibración de ganancia en el canal de tensión
	   ID_IAC_GAIN					///< Calibración de ganancia en el canal de corriente
};


#endif


/** @}*/
