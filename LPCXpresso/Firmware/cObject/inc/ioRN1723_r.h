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
	uint32_t tcpConnected;			///< Indica si hay conexión TCP abierta o no.
	uint32_t authenticated;			///< Indica si está autenticado en una red WiFi o no.
};

// ********************************************************************************


// ********************************************************************************
/**
 * @name Macros privados
 * @brief Macros de get y set para ser usados de forma privada por la clase ioRN1723 y por las que la heredan.
 * @{
*/
#define uart(p)						(((const struct ioSPI*)p)->uart)
#define gpioNetwork(p)				(((const struct ioSPI*)p)->gpioNetwork)
#define gpioTCP(p)			    	(((const struct ioSPI*)p)->gpioTCP)
#define tcpConnected(p)				(((const struct ioSPI*)p)->tcpConnected)
#define authenticated(p)	    	(((const struct ioSPI*)p)->authenticated)

#define set_uart(p, v)				(((struct ioSPI*)p)->uart = (v))
#define set_gpioNetwork(p, v)		(((struct ioSPI*)p)->gpioNetwork = (v))
#define set_gpioTCP(p, v)			(((struct ioSPI*)p)->gpioTCP = (v))
#define set_tcpConnected(p, v)		(((struct ioSPI*)p)->tcpConnected = (v))
#define set_authenticated(p, v)		(((struct ioSPI*)p)->authenticated = (v))

///@}
// ********************************************************************************


#endif

///@}
