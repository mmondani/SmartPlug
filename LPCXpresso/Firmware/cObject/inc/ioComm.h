/**
 * @addtogroup grp_ioComm ioComm
 *
 * @brief Interfaz que extiende a @ref grp_ioObject.
 * @details Esta interfaz tiene los métodos básicos que puede necesitar cualquier periférico de comunicación.
 * @{
 */

/**
 * @file ioComm.h
 *
 * @brief Métodos de la interfaz ioComm.
 * @author Mariano Mondani
 */


#ifndef IOCOMM_H_
#define IOCOMM_H_


#include "ioObject.h"


// ********************************************************************************
/**
 * @name Métodos de la interfaz ioObject
 * @{
 */



/**
 * @brief      Envía la cantidad de bytes len del buffer data.
 *
 * @param      _this  instancia de una clase que implementa a ioComm.
 * @param      len    cantidad de bytes a enviar.
 * @param      data   buffer que contiene los bytes a enviar.
 *
 * @return     Cantidad de bytes realmente enviados.
 */
uint32_t ioComm_writeBytes (void* _this, uint32_t len, uint8_t* data);



/**
 * @brief      Recibe la cantidad de bytes len en el buffer data.
 *
 * @param      _this  instancia de una clase que implementa a ioComm.
 * @param[in]  len    cantidad de bytes a recibir.
 * @param      data   buffer donde se van a guardar los bytes recibidos.
 *
 * @return     Cantidad de bytes realmente recibidos.
 */
uint32_t ioComm_readBytes (void* _this, uint32_t len, uint8_t* data);



/**
 * @brief      Devuelve la cantidad de espacio disponible en el buffer de salida
 *
 * @param      _this  instancia de una clase que implementa a ioComm.
 *
 * @return     Cantidad de bits disponibles
 */
uint32_t ioComm_freeSpace (void* _this);



/**
 * @brief      Devuelve la cantidad de bytes para ser leídos.
 *
 * @param      _this  instancia de una clase que implementa a ioComm.
 *
 * @return     Cantidad de bytes para ser leídos.
 */
uint32_t ioComm_dataAvailable (void* _this);



/**
 * @brief      Habilita las interrupciones indicadas en mask.
 *
 * @param      _this  instancia de una clase que implementa a ioComm.
 * @param      mask   máscara de las interrupciones unidas por ORs. Las definiciones
 * 	           de las interrupciones disponibles se deben buscar en el header de la
 * 	           clase que implementa esta interfaz.
 */
void	 ioComm_intEnable (void* _this, uint32_t mask);



/**
 * @brief      Deshabilita las interrupciones indicadas en mask.
 *
 * @param      _this  instancia de una clase que implementa a ioComm.
 * @param 	   mask   máscara de las interrupciones unidas por ORs. Las definiciones
 * 	           de las interrupciones disponibles se deben buscar en el header de la
 * 	           clase que implementa esta interfaz.
 */
void	 ioComm_intDisable (void* _this, uint32_t mask);



/**
 * @brief      Devuelve si la interrupción intID se produjo o no.
 *
 * @param      _this  instancia de una clase que implementa a ioComm.
 * @param      intID  identificación de la interrupción. Las identficaciones de las interruciones
 * 	            se deben buscar en el header de la clase que implementa esta interfaz.
 *
 * @return     0 no se produjo la interrupción.
 * 	   		   1 se produjo la interrupción.
 */
uint32_t ioComm_getInt (void* _this, uint32_t intID);

///@}
// ********************************************************************************

#endif


///@}
