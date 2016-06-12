/**
 * @addtogroup grp_ioObject ioObject
 *
 * @brief Interfaz que extiende a @ref grp_cObject.
 * @details Esta interfaz tiene los métodos básicos que puede necesitar cualquier clase de entrada/salida.
 * @{
 */

/**
 * @file ioObject.h
 *
 * @brief Métodos de la interfaz ioObject.
 * @author Mariano Mondani
 */


#ifndef IOOBJECT_H_
#define IOOBJECT_H_


#include "cObject.h"
#include "chip.h"



/**
 * @name Métodos de la interfaz ioObject
 * @{
 */


/**
 * @brief      Inicializa el hardware relacionado con el ioObject.
 *
 * @param      _this  instancia de una clase que implementa a ioObject.
 *
 * @return     0 si la inicialización fue exitosa. Otro valor si hubo un error.
 */
uint32_t ioObject_init (void* _this);



/**
 * @brief      Des-inicializa el hardware asociado al iObject.
 *
 * @param      _this  instancia de una clase que implementa a ioObject.
 *
 * @return     0 en caso de éxito. Otro valor si hubo un error.
 */
uint32_t ioObject_deInit (void* _this);



/**
 * @brief      Habilita el periférico.
 *
 * @param      _this  instancia de una clase que implementa a ioObject.
 *
 * @return     0 en caso de éxito. Otro valor si hubo un error.
 */
uint32_t ioObject_enable (void* _this);



/**
 * @brief      Deshabilita el periférico.
 *
 * @param      _this  instancia de una clase que implementa a ioObject.
 *
 * @return     0 en caso de éxito. Otro valor si hubo un error.
 */
uint32_t ioObject_disable (void* _this);



/**
 * @brief      Lee un valor desde el periférico.
 *
 * @param      _this  instancia de una clase que implementa a ioObject.
 *
 * @return     Valor leido.
 */
uint32_t ioObject_read (void* _this);



/**
 * @brief      Escribe un valor al periférico.
 *
 * @param      _this  instancia de una clase que implementa a ioObject.
 * @param	   data   valor a escribir.
 *
 * @return     0 en caso de éxito. Otro valor si hubo un error.
 */
uint32_t ioObject_write (void* _this, uint32_t data);


///@}

#endif


///@}
