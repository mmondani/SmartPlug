#ifndef CMATRIX_H_
#define CMATRIX_H_

#include "cNumeric.h"


// ********************************************************************************
// Instanciación de cMatrix
//
// void* matrix = cObject_new(cMatrix, rows, cols, M11, M12, M13,..., M1m,
//												   M21, M22, M23,..., M2m,
//												   ......................,
//												   Mn1, Mn2, Mn3,..., Mnm)
// ********************************************************************************
extern const void* cMatrix;



// ********************************************************************************
// Métodos públicos de la clase cMatrix : cNumeric
// ********************************************************************************

/*
 * cMatrix_invert
 *
 * Invierte la matriz _this.
 *
 * 	-> _this: matriz a invertir.
 * 	-> _result: objeto en el que almacenar el resultado. La función
 * 				NO aloca la memoria para _result.
 *
 * 	<- Puntero al objeto resultado. Permite concatenar el llamado de funciones.
 */
void* cMatrix_invert (void* _this, void* _result);


/*
 * cMatrix_transpose
 *
 * Transpone la matriz _this.
 *
 * 	-> _this: matriz a transponer.
 * 	-> _result: objeto en el que almacenar el resultado. La función
 * 				NO aloca la memoria para _result.
 *
 * 	<- Puntero al objeto resultado. Permite concatenar el llamado de funciones.
 */
void* cMatrix_transpose (void* _this, void* _result);


/*
 * cMatrix_transpose
 *
 * Instancia una matriz identidad de nxn.
 *
 * 	-> n: dimensión de la matriz a crear.
 *
 * 	<- Puntero al objeto instanciado.
 */
void* cMatrix_newIdentity (uint32_t n);


/*
 * cMatrix_swapRows
 *
 * Intercambia dos filas de una matriz.
 *
 * 	-> _this: matriz sobre la que operar.
 * 	-> row1, row2: filas que intercambiar. Se numeran apartir de 0
 *
 * 	<- Puntero a la matriz _this. Permite concatenar el llamado de funciones.
 */
void* cMatrix_swapRows (void* _this, uint32_t row1, uint32_t row2);


/*
 * cMatrix_swapCols
 *
 * Intercambia dos columnas de una matriz.
 *
 * 	-> _this: matriz sobre la que operar.
 * 	-> col1, col2: columnas que intercambiar. Se numeran apartir de 0
 *
 * 	<- Puntero a la matriz _this. Permite concatenar el llamado de funciones.
 */
void* cMatrix_swapCols (void* _this, uint32_t col1, uint32_t col2);


/*
 * cMatrix_setValue
 *
 * Establece el valor del elemento (row, col) de la matriz _this.
 *
 * 	-> _this: matriz sobre la que operar.
 * 	-> row, col: fila y columna del elemento a modificar. Se numeran desde 0.
 * 	-> value: valor a insertar en la matriz.
 *
 * 	<- Puntero a la matriz _this. Permite concatenar el llamado de funciones.
 */
void* cMatrix_setValue (void* _this, uint32_t row, uint32_t col, float value);


/*
 * cMatrix_setRow
 *
 * Establece los valores de la fila row de la matriz _this.
 *
 * 	-> _this: matriz sobre la que operar.
 * 	-> row: fila a modificar. Se numera desde 0.
 * 	-> ... : valores a cargar en la fila row. Es de la forma: value1, value2, ...., valueN
 *
 * 	<- Puntero a la matriz _this. Permite concatenar el llamado de funciones.
 */
void* cMatrix_setRow (void* _this, uint32_t row, ...);


/*
 * cMatrix_setCol
 *
 * Establece los valores de la columna col de la matriz _this.
 *
 * 	-> _this: matriz sobre la que operar.
 * 	-> col: columna a modificar. Se numera desde 0.
 * 	-> ... : valores a cargar en la columna col. Es de la forma: value1, value2, ...., valueN
 *
 * 	<- Puntero a la matriz _this. Permite concatenar el llamado de funciones.
 */
void* cMatrix_setCol (void* _this, uint32_t col, ...);


/*
 * cMatrix_getValue
 *
 * Obtiene el valor del elemento (row, col) de la matriz _this.
 *
 * 	-> _this: matriz sobre la que operar.
 * 	-> row, col: fila y columna del elemento a modificar. Se numeran desde 0.
 *
 * 	<- Valor (row, col) de la matriz.
 */
float cMatrix_getValue (void* _this, uint32_t row, uint32_t col);


/*
 * cMatrix_addConstant
 *
 * Suma una constante a la matriz _this.
 *
 * 	-> _this: matriz sobre la que operar.
 * 	-> constant: constante a sumarle a la matriz _this
 * 	-> _result: objeto en el que almacenar el resultado. La función
 * 				NO aloca la memoria para _result.
 *
 * 	<- Puntero a la matriz _this. Permite concatenar el llamado de funciones.
 */
void* cMatrix_addConstant (void* _this, float constant, void* _result);


/*
 * cMatrix_multiplyConstant
 *
 * Multiplica a la matriz _this por una constante.
 *
 * 	-> _this: matriz sobre la que operar.
 * 	-> constant: constante por la que se multiplica a la matriz _this
 * 	-> _result: objeto en el que almacenar el resultado. La función
 * 				NO aloca la memoria para _result.
 *
 * 	<- Puntero a la matriz _this. Permite concatenar el llamado de funciones.
 */
void* cMatrix_multiplyConstant (void* _this, float constant, void* _result);


/*
 * cMatrix_divideConstant
 *
 * Divide a la matriz _this por una constante.
 *
 * 	-> _this: matriz sobre la que operar.
 * 	-> constant: constante por la que se divide a la matriz _this
 * 	-> _result: objeto en el que almacenar el resultado. La función
 * 				NO aloca la memoria para _result.
 *
 * 	<- Puntero a la matriz _this. Permite concatenar el llamado de funciones.
 */
void* cMatrix_divideConstant (void* _this, float constant, void* _result);


// ********************************************************************************


#endif

