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
	void* gpioReset;				///< Instancia de la clase @ref grp_ioDigital conectado al pin RESET del módulo RN1723.
	void* outBuffer;				///< Buffer en donde se guardan los datos a enviar al módulo.
	void* inBuffer;					///< Buffer en donde se guardan los datos recibidos desde el módulo.
	void* cmdBuffer;				///< Buffer en donde se guarda el comando a enviar al módulo hasta entrar en el modo comando.
	void* timer;					///< Instancia de la clase @ref grp_cTimer usado para el timeout de las operaciones. Se debe proveer una base de tiempo de 1ms.
	uint8_t param[25];				///< guarda temporalmente un parámetro de un comando.
	uint32_t events;				///< Eventos que recibe la FSM.
	uint32_t retries;				///< Cantidad de reintentos restantes para distintas operaciones.
	uint32_t possibleResponses;		///< Cada bit i indicasi la i-esima respuesta es posible que llegue o no de acuerdo al parseo de lo recibido por la UART desde el módulo.
	uint8_t responseIndex;			///< Variable usada para parsear las respuestas del módulo.
	uint32_t fsm_state;				///< Estado actual de la FSM del módulo.
	uint32_t fsm_sub_state;			///< Sub-estado actual
	uint32_t answerFilter;			///< Usado para filtrar las respuestas deseadas.
	uint32_t lastRTC;				///< Indica el último valor del RTC que se obtuvo del NTP.
	int8_t  timeZone;				///< Zona horaria usada por las funciones que deveuvlen la fecha y la hora actual. Es un número con signo.
	uint8_t indexSerial;			///< Indice auxiliar usado en la recepción serial desde el módulo.
	uint32_t tcpConnected:1;		///< Indica si hay conexión TCP abierta o no.
	uint32_t authenticated:1;		///< Indica si está autenticado en una red WiFi o no.
	uint32_t cmdMode:1;				///< Indica si está en modo comando o no.
	uint32_t readingRTCValue:1;		///< Indica si se está recibiendo el valor del RTC desde el módulo o no.
};

// ********************************************************************************


// ********************************************************************************
/**
 * @name Macros privados
 * @brief Macros de get y set para ser usados de forma privada por la clase ioRN1723 y por las que la heredan.
 * @{
*/
#define uart(p)						(((const struct ioRN1723*)p)->uart)
#define gpioReset(p)				(((const struct ioRN1723*)p)->gpioReset)
#define cmdBuffer(p)				(((const struct ioRN1723*)p)->cmdBuffer)
#define timer(p)					(((const struct ioRN1723*)p)->timer)
#define outBuffer(p)				(((const struct ioRN1723*)p)->outBuffer)
#define inBuffer(p)					(((const struct ioRN1723*)p)->inBuffer)

#define set_uart(p, v)				(((struct ioRN1723*)p)->uart = (v))
#define set_gpioReset(p, v)			(((struct ioRN1723*)p)->gpioReset = (v))
#define set_cmdBuffer(p, v)			(((struct ioRN1723*)p)->cmdBuffer = (v))
#define set_timer(p, v)				(((struct ioRN1723*)p)->timer = (v))
#define set_outBuffer(p, v)			(((struct ioRN1723*)p)->outBuffer = (v))
#define set_inBuffer(p, v)			(((struct ioRN1723*)p)->inBuffer = (v))

///@}
// ********************************************************************************


#endif

///@}
