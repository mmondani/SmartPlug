/**
 * @addtogroup grp_ioDebounce ioDebounce
 *
 * @brief Implementa la interfaz @ref grp_cObject.
 * @details Esta clase implementa el anti-rebote sobre un pin de entrada representado
 * por una instacia de la clase @ref grp_ioDigital.
 * @{
 */

/**
 * @file ioDebounce.h
 * @author Mariano Mondani
 *
 * @brief Métodos públicos de la clase ioDebounce.
 */

#ifndef IODEBOUNCE_H
#define IODEBOUNCE_H

#include "ioObject.h"
#include "ioDigital.h"



// ********************************************************************************
/**
 * @var ioDebounce
 * @brief Nombre de la clase.
 * @details La forma de instanciar esta clase es la siguiente:
 * @code
 * void* debounce = cObject_new(ioDebounce, inputPin, level, debounceCount)
 * @endcode
 * @param ioDebounce 		nombre de la clase a instanciar.
 * @param inputPin			instancia de la clase @ref ioDigital, configurado como entrada.
 * @param level				nivel que en el que se considera que la entrada está activada. Ver @ref ioDigital_Level.
 * @param debounceCount		cantidad de veces que se debe ver la entrada con el nivel level para considerarla válida.
 */
extern const void* ioDebounce;

// ********************************************************************************



// ********************************************************************************
/**
 * @name Métodos de la clase ioDebounce
 * @{
 */



/**
 * @brief      Ejecuta la lógica del anti-rebote.
 * @details	   Debe ser llamado periódicamente. El intervalo de debounce es:
 * @code
 * debounceTime = debounceCount * call_interval
 * @endcode
 * 			   Si por ejemplo, se llama a ioDebounce_handler cada 10ms y se configuró
 * 			   un debounceCount de 8, el tiempo de anti-rebote será de 80ms mínimo.
 * @param      _this  instancia de la clase ioDebounce.
 */
void ioDebounce_handler (void* _this);



/**
 * @brief      devuelve si la entrada está activa de acuerdo al nivel que se eligió
 *
 * @param      _this  instancia de la clase ioDebounce.
 *
 * @return     0 si la entrada no está activa.
 * 			   1 si la entrada está activa.
 */
uint32_t ioDebounce_isActive (void* _this);



/**
 * @brief      Retorna si se produjo un flanco de nivel pasivo a nivel activo.
 *
 * @param      _this  instancia de la clase ioEE25LCxxx.
 *
 * @return	   0 si no se produjo flanco a activo.
 * 			   1 si se produjo un flanco a activo. La próxima vez que se la llame va a retornar 0.
 */
uint32_t ioDebounce_getActiveEdge (void* _this);



/**
 * @brief      Retorna si se produjo un flanco de nivel activo a nivel pasivo.
 *
 * @param      _this  instancia de la clase ioEE25LCxxx.
 *
 * @return	   0 si no se produjo flanco a pasivo.
 * 			   1 si se produjo un flanco a pasivo. La próxima vez que se la llame va a retornar 0.
 */
uint32_t ioDebounce_getPassiveEdge (void* _this);


///@}
// ********************************************************************************



#endif


///@}
