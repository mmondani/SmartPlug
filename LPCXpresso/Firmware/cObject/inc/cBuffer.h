/**
 * @addtogroup grp_cBuffer cBuffer
 * @brief Interfaz que extiende a @ref grp_cObject.
 * @details Esta interfaz tiene los métodos básicos que puede necesitar cualquier clase de buffer.
 * @{
 */

/**
 * @file cBuffer.h
 * @author Mariano Mondani
 *
 * @brief Métodos de la interfaz cBuffer.
 * @author Mariano Mondani
 */

#ifndef CBUFFER_H_
#define CBUFFER_H_


#include "cObject.h"


/**
 * @name Métodos de la interfaz cBuffer
 * @{
 */



/**
 * @brief      Agrega un elemento del buffer _this.
 *
 * @param      _this    buffer al que agregar el elemento _object.
 * @param      _object  elemento a agregar.
 *
 * @return     Puntero al buffer _this. Permite concatenar el llamado de funciones.
 */
void* cBuffer_put (void* _this, void* _object);



/**
 * @brief      Elimina un elemento del buffer _this.
 *
 * @param      _this    buffer del que se elimina un elemento.
 * @param      _object  objeto que se quiere eliminar.
 *
 * @return     Puntero al buffer _this. Permite concatenar el llamado de funciones.
 */
void* cBuffer_remove (void* _this, void* _object);



/**
 * @brief      Limpia el buffer _this.
 *
 * @param      _this  buffer a limpiar.
 *
 * @return     Puntero al buffer _this. Permite concatenar el llamado de funciones.
 */
void* cBuffer_clear (void* _this);



/**
 * @brief      Devuelve el elemento en la posición pos del buffer _this.
 *
 * @param      _this    buffer en el que buscar el elemento
 * @param[in]  pos      posición del elemento a devolver. Se numera desde 0.
 * @param      _object  elemento en la posición pos del buffer _this.
 */
void cBuffer_at (void* _this, uint32_t pos, void* _object);




/**
 * @brief      Devuelve la cantidad de espacio disponible en el buffer.
 *
 * @details    La cantidad de espacio no está dada en bytes, el tamaño del objeto almacenado es la unidad.
 *
 * @param      _this  instancia de una clase que implementa cBuffer.
 *
 * @return     Cantidad de lugar disponible
 */
uint32_t cBuffer_getFreeSpace (void* _this);



/**
 * @brief      Devuelve la cantidad de objetos que hay que extraer del buffer.
 *
 * @details    La cantidad no está dada en bytes, el tamaño del objeto almacenado es la unidad.
 *
 * @param      _this  instancia de una clase que implementa cBuffer.
 *
 * @return     Cantidad de objetos pendientes de extraer.
 */
uint32_t cBuffer_getPending (void* _this);



/**
 * @brief      Devuelve la longitud del buffer
 *
 * @details    La longitud no está dada en bytes, el tamaño del objeto almacenado es la unidad.
 *
 * @param      _this  instancia de una clase que implementa cBuffer.
 *
 * @return     longitud del buffer
 */
uint32_t cBuffer_length (void* _this);


 /// @}

#endif

/** @}*/
