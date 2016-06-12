/**
 * @addtogroup grp_ioComm ioComm
 * @{
 */

/**
 * @file 		ioComm_r.h
 * @author 		Mariano Mondani
 *
 * @brief 		Estructura interna de la interfaz ioObject. Extiende a ioObject
 * @details     Este archivo no debe ser incluido en la aplicación del usuario.
 */

#ifndef IOCOMM_R_H_
#define IOCOMM_R_H_

#include "ioObject_r.h"


/**
 * @brief      Estructura de la interfaz ioComm.
 * @details    Esta estructura es usada por las clases que implementan la interfaz ioComm.
 */
struct ioComm
{
	struct ioObject _;														///< Extiende la interfaz ioObject.
	uint32_t (*writeBytes) (void* _this, uint32_t len, uint8_t* data);		///< Puntero a writeBytes de la clase que implemente la interfaz ioComm.
	uint32_t (*readBytes) (void* _this, uint32_t len, uint8_t* data);		///< Puntero a readBytes de la clase que implemente la interfaz ioComm.
	uint32_t (*freeSpace) (void* _this);									///< Puntero a freeSpace de la clase que implemente la interfaz ioComm.
	uint32_t (*dataAvailable) (void* _this);								///< Puntero a dataAvailable de la clase que implemente la interfaz ioComm.
	void	 (*intEnable) (void* _this, uint32_t mask);					///< Puntero a intEnable de la clase que implemente la interfaz ioComm.
	void	 (*intDisable) (void* _this, uint32_t mask);					///< Puntero a intDisable de la clase que implemente la interfaz ioComm.
	uint32_t (*getInt) (void* _this, uint32_t intID);						///< Puntero a getInt de la clase que implemente la interfaz ioComm.
};

#endif


///@}
