/**
 * @addtogroup grp_ioRTC ioRTC
 * @{
 */

/**
 * @file 		ioRTC_r.h
 * @author 		Mariano Mondani
 *
 * @brief 		Estructura interna de la interfaz ioRTC. Extiende a ioObject
 * @details     Este archivo no debe ser incluido en la aplicación del usuario.
 */

#ifndef IORTC_R_H_
#define IORTC_R_H_

#include "ioObject_r.h"
#include "rtc_time_t_r.h"


/**
 * @brief      Estructura de la interfaz ioRTC.
 * @details    Esta estructura es usada por las clases que implementan la interfaz ioRTC.
 */
struct ioRTC
{
	struct ioObject _;																///< Estructura ioObject que es extendida por la interfaz ioRTC.
	void (*reset) (void* _this);													///< Puntero a reset de la clase que implemente la interfaz ioRTC.
	uint32_t (*setFullTime) (void* _this, rtc_time_t* time);						///< Puntero a setFullTime de la clase que implemente la interfaz ioRTC.
	uint32_t (*getFullTime) (void* _this, rtc_time_t* time);						///< Puntero a getFullTime de la clase que implemente la interfaz ioRTC.
	uint32_t (*setTime) (void* _this, uint32_t element, uint32_t value);			///< Puntero a setTime de la clase que implemente la interfaz ioRTC.
	uint32_t (*getTime) (void* _this, uint32_t element);							///< Puntero a getTime de la clase que implemente la interfaz ioRTC.
};

#endif


///@}
