/**
 * @addtogroup grp_cObject cObject
 * @{
 */

/**
 * @file 		cObject_r.h
 * @brief 		Estructura interna de la interfaz cObject.
 * @details     Este archivo no debe ser incluido en la aplicación del usuario.
 */

#ifndef COBJECT_R_H_
#define COBJECT_R_H_


#include <stdint.h>
#include <stdarg.h>


// ********************************************************************************
/**
 * @struct     cObject
 * @brief      Estructura de la interfaz cObject.
 * @details    Esta estructura es usada por las clases que implementan la interfaz cObject.
 */
struct cObject
{
	int32_t size;									///< Tamaño en bytes de la estructura que describe la clase.
	void* (*ctor) (void* _this, va_list* va);		///< Puntero al constructor de la clase que implemente la interfaz cObject
	void* (*dtor) (void* _this);					///< Puntero al destructor de la clase que implemente la interfaz cObject
	uint32_t (*differ) (void* _this, void* _dst);	///< Puntero a differ de la clase que implemente la interfaz cObject
	void (*display) (void* _this);					///< Puntero a display de la clase que implemente la interfaz cObject
	void* (*copy) (void* _this, void* _src);		///< Puntero a copy de la clase que implemente la interfaz cObject
};
// ********************************************************************************



//#define NO_ASSERT


// ********************************************************************************
/**
 * @brief      Permite evaluar una expresión. En caso de ser falsa, el programa se detiene
 * @details    Para habilitar la función assert() se debe comentar la línea @code #define NO_ASSERT @endcode
 * @param      expr   Expresión a evaluar
 */
#ifdef NO_ASSERT
#define assert(expr)
#else
#define assert(expr) 		\
{ 							\
    if (!(expr))			\
    { 						\
        while(1);			\
    } 						\
}
#endif

#endif

// ********************************************************************************

/** @}*/
