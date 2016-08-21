/**
 * @addtogroup grp_moduleLog moduleLog
 * @brief Se encarga de enviar por la UART1 los mensajes que son enviados por otros tasks para ayudar en el debug de la aplicación.
 * @{
 */

/**
 * @file moduleLog.h
 * @author Mariano Mondani
 *
 * @brief Interfaz del módulo moduleLog
 */


#ifndef MODULE_LOG_H_
#define MODULE_LOG_H_


#include "chip.h"


/**
 * @brief      Inicializa el módulo.
 */
void moduleLog_init (void);


/**
 * @brief      Le indica al módulo moduleLog el mensaje que se debe loguear.
 *
 * @details    En cada línea del log, el módulo va a agregar la hora y la fecha.
 *
 * @param      str  string a loguear. Debe terminar en '\0'.
 */
void moduleLog_log (uint8_t* str);



#endif


/** @}*/
