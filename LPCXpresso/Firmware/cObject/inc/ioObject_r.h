/**
 * @addtogroup grp_ioObject ioObject
 * @{
 */

/**
 * @file 		ioObject_r.h
 * @author 		Mariano Mondani
 *
 * @brief 		Estructura interna de la interfaz ioObject. Extiende a cObject
 * @details     Este archivo no debe ser incluido en la aplicación del usuario.
 */

#ifndef IOOBJECT_R_H_
#define IOOBJECT_R_H_

#include "cObject_r.h"


/**
 * @brief      Estructura de la interfaz ioObject.
 * @details    Esta estructura es usada por las clases que implementan la interfaz ioobject.
 */
struct ioObject
{
	struct cObject _;										///< Estructura cObject que es extendida por la interfaz ioObject.
	uint32_t (*init) (void* _this);						///< Puntero a init de la clase que implemente la interfaz ioObject.
	uint32_t (*deInit) (void* _this);						///< Puntero a deInit de la clase que implemente la interfaz ioObject.
	uint32_t (*enable) (void* _this);						///< Puntero a enable de la clase que implemente la interfaz ioObject.
	uint32_t (*disable) (void* _this);						///< Puntero a disable de la clase que implemente la interfaz ioObject.
	uint32_t (*read) (void* _this);						///< Puntero a read de la clase que implemente la interfaz ioObject.
	uint32_t (*write) (void* _this, uint32_t data);		///< Puntero a write de la clase que implemente la interfaz ioObject.
};

#endif


///@}
