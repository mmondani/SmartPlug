/**
 * @addtogroup grp_ioUART ioUART
 * @{
 */

/**
 * @file ioUART_r.h
 * @author Mariano Mondani
 *
 * @brief Estructura interna de la clase ioUART.
 * @details Este archivo no debe ser incluido en la aplicación del usuario.
 */

#ifndef IOUART_R_H
#define IOUART_R_H


#include "ioComm_r.h"


// ********************************************************************************
/**
 * @brief      Estructura de la clase ioGPIO.
 */
struct ioUART
{
	const void* class;				///< Puntero a la interfaz @ref grp_ioComm
	void* periphMem;				///< Dirección base de memoria del periférico.
	uint32_t baudRate;				///< Baudrate de la UART. Ver @ref ioUART_BR.
	uint32_t dataLen;				///< Cantidad de bits de cada dato. Ver @ref ioUART_Data.
	uint32_t parity;				///< Paridad. Ver @ref ioUART_Parity.
	uint32_t stopBit;				///< Bits de stop. Ver @ref ioUART_Stop.
	uint32_t mode;					///< Modo de transmisión. Ver @ref ioUART_Mode.
	void* txQueue;					///< Instancia de cQueue para transmisión. Solo se instancia en el modo IOUART_MODE_NON_BLOCKING.
	void* rxQueue;					///< Instancia de cQueue para recepción.
};

// ********************************************************************************


// ********************************************************************************
/**
 * @name Macros privados
 * @brief Macros de get y set para ser usados de forma privada por la clase ioUART y por las que la heredan.
 * @{
*/
#define periphMem(p)			(((const struct ioUART*)p)->periphMem)
#define baudRate(p)			    (((const struct ioUART*)p)->baudRate)
#define dataLen(p)			    (((const struct ioUART*)p)->dataLen)
#define parity(p)			    (((const struct ioUART*)p)->parity)
#define stopBit(p)			    (((const struct ioUART*)p)->stopBit)
#define mode(p)			        (((const struct ioUART*)p)->mode)
#define txQueue(p)			    (((const struct ioUART*)p)->txQueue)
#define rxQueue(p)			    (((const struct ioUART*)p)->rxQueue)

#define set_periphMem(p, v)			(((struct ioUART*)p)->periphMem = (v))
#define set_baudRate(p, v)			(((struct ioUART*)p)->baudRate = (v))
#define set_dataLen(p, v)			(((struct ioUART*)p)->dataLen = (v))
#define set_parity(p, v)			(((struct ioUART*)p)->parity = (v))
#define set_stopBit(p, v)			(((struct ioUART*)p)->stopBit = (v))
#define set_mode(p, v)			    (((struct ioUART*)p)->mode = (v))
#define set_txQueue(p, v)			(((struct ioUART*)p)->txQueue = (v))
#define set_rxQueue(p, v)			(((struct ioUART*)p)->rxQueue = (v))

///@}
// ********************************************************************************


#endif

///@}
