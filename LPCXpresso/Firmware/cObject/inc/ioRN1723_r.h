/**
 * @addtogroup grp_ioRN1723 ioRN1723
 * @{
 */

/**
 * @file ioRN1723_r.h
 * @author Mariano Mondani
 *
 * @brief Estructura interna de la clase ioRN1723.
 * @details Este archivo no debe ser incluido en la aplicación del usuario.
 */

#ifndef IORN1723_R_H
#define IORN1723_R_H


#include "ioComm_r.h"


// ********************************************************************************
/**
 * @brief      Estructura de la clase ioRN1723.
 */
struct ioRN1723
{
	const void* class;				///< Puntero a la interfaz @ref grp_ioComm
	void* uart;						///< Instancia de la clase @ref grp_ioUART a tavés de la cual se comunica con el módulo RN1723.
	void* gpioNetwork;				///< Instancia de la clase @ref grp_ioDigital configurado como entrada y conectado al GPIO4 del módulo RN1723.
	void* gpioTCP;					///< Instancia de la clase @ref grp_ioDigital configurado como entrada y conectado al GPIO6 del módulo RN1723.
	void* outBuffer;				///< Buffer en donde se guardan los datos a enviar al módulo.
	void* inBuffer;					///< Buffer en donde se guardan los datos recibidos desde el módulo.
	void* cmdBuffer;				///< Buffer en donde se guarda el comando a enviar al módulo hasta entrar en el modo comando.
	uint32_t events;				///< Eventos que recibe la FSM.
	uint32_t retries;				///< Cantidad de reintentos restantes para distintas operaciones.
	uint8_t responseIndex;			///< Variable usada para parsear las respuestas del módulo.
	uint32_t fsm_state;				///< Estado actual de la FSM del módulo.
	uint32_t fsm_sub_state;			///< Sub-estado actual
	uint32_t answerFilter;			///< Usado para filtrar las respuestas deseadas.
	uint32_t tcpConnected:1;		///< Indica si hay conexión TCP abierta o no.
	uint32_t authenticated:1;		///< Indica si está autenticado en una red WiFi o no.
	uint32_t cmdMode:1;				///< Indica si está en modo comando o no.
};

// ********************************************************************************


// ********************************************************************************
/**
 * @name Macros privados
 * @brief Macros de get y set para ser usados de forma privada por la clase ioRN1723 y por las que la heredan.
 * @{
*/
#define uart(p)						(((const struct ioRN1723*)p)->uart)
#define gpioNetwork(p)				(((const struct ioRN1723*)p)->gpioNetwork)
#define gpioTCP(p)			    	(((const struct ioRN1723*)p)->gpioTCP)
#define cmdBuffer(p)				(((const struct ioRN1723*)p)->cmdBuffer)

#define set_uart(p, v)				(((struct ioRN1723*)p)->uart = (v))
#define set_gpioNetwork(p, v)		(((struct ioRN1723*)p)->gpioNetwork = (v))
#define set_gpioTCP(p, v)			(((struct ioRN1723*)p)->gpioTCP = (v))
#define set_cmdBuffer(p, v)			(((struct ioRN1723*)p)->cmdBuffer = (v))

///@}
// ********************************************************************************


#endif

///@}
