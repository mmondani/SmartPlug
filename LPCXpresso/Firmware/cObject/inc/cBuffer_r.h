/**
 * @addtogroup grp_cBuffer cBuffer
 * @{
 */

/**
 * @file 		cBuffer_r.h
 * @author 		Mariano Mondan

 * @brief 		Estructura interna de la interfaz cBuffer. Extiende a cObject
 * @details     Este archivo no debe ser incluido en la aplicación del usuario.
 */

#ifndef CBUFFER_R_H_
#define CBUFFER_R_H_

#include "cObject_r.h"


/**
 * @brief      Estructura de la interfaz cBuffer.
 * @details    Esta estructura es usada por las clases que implementan la interfaz cBuffer.
 */
struct cBuffer
{
	struct cObject _;											///< Estructura cObject que es extendida por la interfaz cBuffer.
	void* (*put) (void* _this, void* _object);					///< Puntero a put de la clase que implemente la interfaz cBuffer.
	void* (*remove) (void* _this, void* _object);				///< Puntero a remove de la clase que implemente la interfaz cBuffer.
	void* (*clear) (void* _this);								///< Puntero a clear de la clase que implemente la interfaz cBuffer.
	void (*at) (void* _this, uint32_t pos, void* _object);		///< Puntero a at de la clase que implemente la interfaz cBuffer.
	uint32_t (*getFreeSpace) (void* _this);						///< Puntero a getFreeSpace de la clase que implemente la interfaz cBuffer.
	uint32_t (*getPending) (void* _this);						///< Puntero a getPending de la clase que implemente la interfaz cBuffer.
	uint32_t (*length) (void* _this);							///< Puntero a length de la clase que implemente la interfaz cBuffer.
};

#endif

/** @}*/
