/**
 * @addtogroup grp_ioDebounce ioDebounce
 * @{
 */

/**
 * @file ioDebounce_r.h
 * @author Mariano Mondani
 *
 * @brief Estructura interna de la clase ioDebounce.
 * @details Este archivo no debe ser incluido en la aplicación del usuario.
 */

#ifndef IODEBOUNCE_R_H_
#define IODEBOUNCE_R_H_

#include "ioGPIO_r.h"


// ********************************************************************************
/**
 * @brief      Estructura de la clase ioDebounce.
 */
struct ioDebounce
{
	const void* class;			///< Puntero a la interfaz @ref grp_cObject.
	void* inputPin;				///< Instancia de la clase ioDigital que maneja el pin de entrada.
	uint32_t activeLevel;		///< Nivel que se considera activo para la entrada.
	uint32_t currentLevel;		///< Nivel actual, después de aplicar el anti-rebote.
	uint32_t debounceCount;		///< Cantidad de veces que se debe ver la entrada con el nivel level para considerarla válida.
	uint32_t count;				///< Contador que es conparado con debounceCount para determinar si hay nivel activo o no.
	uint32_t edgeDetected;		///< Se pone en 1 cuando se detecta un flanco a activo y 2 cuando detecta uno a pasivo. Se pone en 0 cuando se lo consulta.
};

// ********************************************************************************



// ********************************************************************************
/**
 * @name Macros privados
 * @brief Macros de get y set para ser usados de forma privada por la clase ioDebounce y por las que la heredan.
 * @{
*/
#define inputPin(p)						(((const struct ioDebounce*)p)->inputPin)
#define activeLevel(p)					(((const struct ioDebounce*)p)->activeLevel)
#define currentLevel(p)					(((const struct ioDebounce*)p)->currentLevel)
#define debounceCount(p)				(((const struct ioDebounce*)p)->debounceCount)
#define count(p)						(((const struct ioDebounce*)p)->count)
#define edgeDetected(p)					(((const struct ioDebounce*)p)->edgeDetected)

#define set_inputPin(p, v)				(((struct ioDebounce*)p)->inputPin = (v))
#define set_activeLevel(p, v)			(((struct ioDebounce*)p)->activeLevel = (v))
#define set_currentLevel(p, v)			(((struct ioDebounce*)p)->currentLevel = (v))
#define set_debounceCount(p, v)			(((struct ioDebounce*)p)->debounceCount = (v))
#define set_count(p, v)					(((struct ioDebounce*)p)->count = (v))
#define set_edgeDetected(p, v)			(((struct ioDebounce*)p)->edgeDetected = (v))



///@}
// ********************************************************************************



#endif


///@}
