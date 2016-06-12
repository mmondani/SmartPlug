/**
 * @addtogroup grp_cObject cObject
 * @brief Interfaz básica que debe ser extendida o implementada por cualquier otra interfaz o clase, respectivamente.
 * @{
 */

/**
 * @file cObject.h
 *
 * @brief Métodos de la interfaz cObject
 * @author Mariano Mondani
 */

#ifndef COBJECT_H_
#define COBJECT_H_

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include "memAlloc.h"



/**
 * @name Métodos de la interfaz cObject
 * @{
 */


/**
 * @brief Crea un objeto perteneciente a la clase _class.
 * @details Reserva la memoria requerida por el objeto (usando memAlloc)
 * 	y llama al constructor de la clase requerida y de las clases de las que hereda.
 *
 * @param  _class     Clase que se quiere instanciar. El identificador de cada clase esta en el
 * 	           			  archivo .h correspondiente a la clase.
 * @param  ...  	  lista de parámetros dependiente de cada clase. Para ver cómo instanciar cada clase
 * 						  referirse al .h de la clase.
 *
 * @return     Puntero al objeto creado
 */
void* cObject_new (const void* _class, ...);




/**
 * @brief      Libera la memoria alocada con cObject_new
 *
 * @param      _this  oOjeto que se quiere destruir.
 */
void cObject_delete (void* _this);




/**
 * @brief      Determina si dos objetos son distintos.
 *
 * @param      _this  Primer objeto a comparar.
 * @param      _dst   Segundo objeto a comparar.
 *
 * @return     1 si son distintos, 0 si son iguales.
 */
uint32_t cObject_differ (void* _this, void* _dst);



/**
 * @brief      Muestra el objeto _this
 *
 * @param      _this  Objeto a mostrar.
 */
void cObject_display (void* _this);


/**
 * @brief      Copia un objeto a otro.
 *
 * @param      _this  Objeto destino de la copia.
 * @param      _src   Objeto origen de la copia.
 *
 * @return     Puntero al objeto destino.
 */
void* cObject_copy (void* _this, void* _src);


 /// @}

#endif

/** @}*/

