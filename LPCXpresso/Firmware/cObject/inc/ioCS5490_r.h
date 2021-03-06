/**
 * @addtogroup grp_ioCS5490 ioCS5490
 * @{
 */

/**
 * @file ioCS5490_r.h
 * @author Mariano Mondani
 *
 * @brief Estructura interna de la clase ioCS5490.
 * @details Este archivo no debe ser incluido en la aplicación del usuario.
 */

#ifndef IOCS5490_R_H_
#define IOCS5490_R_H_

#include "cObject_r.h"


// ********************************************************************************
/**
 * @brief      Estructura de la clase ioCS5490.
 */
struct ioCS5490
{
	const void* class;			///< Puntero a la interfaz @ref grp_cObject.
	void* uart;					///< Instancia de la clase @ref grp_ioUART con el que se comunica con el CS5490. Debe estar configurada en modo bloqueante. Baud-rate: 600.
	void* gpioReset;			///< Instancia de la clase @ref grp_ioDigital que maneja el pin RESET del CS5490. Debe estar configurado como salida.
	float wordRate;				///< Cantidad de muestras por segundo que toma el CS5490 internamente.
	float vMax;					///< Tensión máxima del medidor.
	float iMax;					///< Corriente máxima del medidor.
	float iCal;					///< Corriente de calibración.
	float scale;				///< 0,6 * iCal / iMax
	float maxPower;				///< vMax * iCal
	float powerScale;			///< 0,6 * scale
	float meterConstant;		///< Constante del medidor. Pulsos por kWh.
	float minimumLoad;			///< Carga mínima del medidor.
};

// ********************************************************************************



// ********************************************************************************
/**
 * @name Macros privados
 * @brief Macros de get y set para ser usados de forma privada por la clase ioCS5490 y por las que la heredan.
 * @{
*/
#define uart(p)						(((const struct ioCS5490*)p)->uart)
#define gpioReset(p)				(((const struct ioCS5490*)p)->gpioReset)

#define set_uart(p, v)				(((struct ioCS5490*)p)->uart = (v))
#define set_gpioReset(p, v)			(((struct ioCS5490*)p)->gpioReset = (v))

///@}
// ********************************************************************************



#endif


///@}
