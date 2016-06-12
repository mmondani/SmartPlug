#ifndef CNUMERIC_H_
#define CNUMERIC_H_


#include "cObject.h"



// ********************************************************************************
// Métodos públicos de la interfaz cNumeric : cObject
// ********************************************************************************


/*
 * cNumeric_add
 *
 * Suma dos objetos que implementan a cNumeric.
 *
 * 	-> _this: primer operando
 * 	-> _other: segundo operando.
 * 	-> _result: objeto en el que almacenar el resultado. La función
 * 				NO aoca la memoria para _result.
 *
 * 	<- Puntero al objeto resultado. Permite concatenar el llamado de funciones.
 */
void* cNumeric_add (void* _this, void* _other, void* _result);


/*
 * cNumeric_subtract
 *
 * Resta dos objetos que implementan a cNumeric.
 *
 * 	-> _this: primer operando
 * 	-> _other: segundo operando.
 * 	-> _result: objeto en el que almacenar el resultado. La función
 * 				NO aoca la memoria para _result.
 *
 * 	<- Puntero al objeto resultado. Permite concatenar el llamado de funciones.
 */
void* cNumeric_subtract (void* _this, void* _other, void* _result);


/*
 * cNumeric_multiply
 *
 * Multiplica dos objetos que implementan a cNumeric.
 *
 * 	-> _this: primer operando
 * 	-> _other: segundo operando.
 * 	-> _result: objeto en el que almacenar el resultado. La función
 * 				NO aoca la memoria para _result.
 *
 * 	<- Puntero al objeto resultado. Permite concatenar el llamado de funciones.
 */
void* cNumeric_multiply (void* _this, void* _other, void* _result);


/*
 * cNumeric_divide
 *
 * Divide dos objetos que implementan a cNumeric.
 *
 * 	-> _this: primer operando
 * 	-> _other: segundo operando.
 * 	-> _result: objeto en el que almacenar el resultado. La función
 * 				NO aoca la memoria para _result.
 *
 * 	<- Puntero al objeto resultado. Permite concatenar el llamado de funciones.
 */
void* cNumeric_divide (void* _this, void* _other, void* _result);


/*
 * cNumeric_module
 *
 * Obtiene el módulo de un objeto que implementa a cNumeric.
 *
 * 	-> _this: primer operando
 *
 * 	<- Resultado del módulo.
 */
float cNumeric_module (void* _this);


// ********************************************************************************

#endif
