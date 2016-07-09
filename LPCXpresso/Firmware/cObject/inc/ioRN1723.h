/**
 * @addtogroup grp_ioRN1723 ioRN1723
 *
 * @brief Implementa la interfaz @ref grp_ioComm.
 * @details Driver para el módulo WiFi RN1723 de Microchip.
 * @{
 */

/**
 * @file ioRN1723.h
 * @author Mariano Mondani
 *
 * @brief Métodos públicos de la clase ioRN1723.
 */


#ifndef IORN1723_H
#define IORN1723_H

#include "ioComm.h"
#include "ioDigital.h"
#include "ioUART.h"
#include "cQueue.h"



// ********************************************************************************
/**
 * @var ioRN1723
 * @brief Nombre de la clase.
 * @details La forma de instanciar esta clase es la siguiente:
 * @code
 * void* module = cObject_new(ioRN1723, uart, network, tcp, inBuff, outBuff)
 * @endcode
 * @param ioRN1723 			nombre de la clase a instanciar.
 * @param uart				instancia de la clase @ref grp_ioUART a través de la que se va a establecer la comunicación con el módulo.
 * @param network			instancia de la clase @ref grp_ioDigital configurado como entrada y conectado al GPIO4 del módulo RN1723.
 * @param tcp				instancia de la clase @ref grp_ioDigital configurado como entrada y conectado al GPIO6 del módulo RN1723.
 * @param inBuff			instancia de la clase @ref grp_cQueue usada para enviar datos al módulo
 * @param outBuff			instancia de la clase @ref grp_cQueue usada para recibir datos del módulo.
 */
extern const void* ioRN1723;




// ********************************************************************************
/**
 * @name Métodos de la clase ioRN1723
 * @{
 */


/**
 * @brief      Se encarga de ejecutar la máquina de estado del módulo de WiFi. Debe ser llamada cada 10 msegs.
 *
 * @param      _this   instancia de la clase ioRN1723.
 */
void ioRN1723_handler (void* _this);


/**
 * @brief      Devuelve si la lógica está en estado idle o no.
 *
 * @param      _this   instancia de la clase ioRN1723.
 */
uint32_t ioRN1723_isIdle (void* _this);


/**
 * @brief      Comienza el proceso de WPS en el módulo. Va a intentar conectarse la cantidad de retries indicada.
 *
 * @param      _this   instancia de la clase ioRN1723.
 * @param      retries cantidad de intentos para poder configurarse por WPS.
 */
void ioRN1723_runWPS (void* _this, uint32_t retries);


/**
 * @brief      Inicia en el módulo el web server de configuración. La URL de la página de configuración es http://config.
 *
 * @param      _this   instancia de la clase ioRN1723.
 */
void ioRN1723_runConfigWebServer (void* _this);


/**
 * @brief      Devuelve si el módulo está unido a una red WiFi o no.
 *
 * @param      _this   instancia de la clase ioRN1723.
 *
 * @return     0 si no está unido a una red WiFi.
 * 			   1 si está unido a una red WiFi.
 */
uint32_t ioRN1723_isAuthenticated (void* _this);


/**
 * @brief      Abandona la red WiFi a la que está unido.
 *
 * @param      _this   instancia de la clase ioRN1723.
 */
void ioRN1723_leaveNetwork (void* _this);


/**
 * @brief      Establece una conexión TCP con el socket <ip : port>.
 *
 * @param      _this   instancia de la clase ioRN1723.
 * @param      ip	   string de la dirección IP a la que se debe conectar.
 * @param	   port	   string del puerto TCP al que se debe conectar.
 */
void ioRN1723_connectTCP (void* _this, uint8_t* ip, uint8_t* port);


/**
 * @brief      Cierra la conexión TCP actual.
 *
 * @param      _this   instancia de la clase ioRN1723.
 */
void ioRN1723_disconnectTCP (void* _this);


/**
 * @brief      Devuelve si hay una conexión TCP abierta o no.
 *
 * @param      _this   instancia de la clase ioRN1723.
 *
 * @return	   0 si no hay una conexión TCP abierta.
 * 			   1 si hay una conexión TCP abierta.
 */
uint32_t ioRN1723_isTCPConnected (void* _this);


/**
 * @brief      Configura la dirección ip y el puerto port del server de SNTP.
 *
 * @param      _this   instancia de la clase ioRN1723.
 * @param      ip      string de la dirección IP del server SNTP.
 * @param      port    string del puerto del server SNTP.
 */
void ioRN1723_setSNTPServer (void* _this, uint8_t* ip, uint8_t* port);


/**
 * @brief      Configura la zona horaria.
 *
 * @param      _this   instancia de la clase ioRN1723.
 * @param      zone    zona horaria.
 */
void ioRN1723_setTimeZone (void* _this, uint32_t zone);


/**
 * @brief      Fuerza la sincronización del RTC interno del módulo WiFi con el server SNTP.
 *
 * @param      _this   instancia de la clase ioRN1723.
 */
void ioRN1723_synchronizeTime (void* _this);


/**
 * @brief      Obtiene la hora actual del RTC interno del módulo WiFi.
 *
 * @param      _this   instancia de la clase ioRN1723.
 * @param      hours   horas.
 * @param      minutes minutos.
 * @param      seconds segundos.
 */
void ioRN1723_getTime (void* _this, uint32_t hours, uint32_t minutes, uint32_t seconds);


/**
 * @brief      Configura el puerto UDP al cual se van a enviar los mensajes periódicos.
 *
 * @param      _this   instancia de la clase ioRN1723.
 * @param      port    string del puerto al que enviar los mensajes periódicos.
 */
void ioRN1723_setHeartbeatPort (void* _this, uint8_t* port);


/**
 * @brief      Configura el intervalo de tiempo con que se envían los mensajes UDP periódicos.
 *
 * @param      _this   	instancia de la clase ioRN1723.
 * @param      interval	indica el intervalo de tiempo entre los mensajes. Es una máscara con la que se hace una AND con un contador
 * 						free-running de segundos. Cuando la AND da 0 se envía el paquete. El valor mínimo es 1 (cada 2 segundos) y el
 * 						máximo es 0xFF (cada 256 segundos).
 */
void ioRN1723_setHeartbeatInterval (void* _this, uint8_t* interval);


/**
 * @brief      Configura el puerto en el que escucha el server TCP.
 *
 * @param      _this   instancia de la clase ioRN1723.
 * @param      port    string del puerto TCP que escucha el server TCP.
 */
void ioRN1723_setTCPServerPort (void* _this, uint8_t* port);


/**
 * @brief      Resetea el módulo por software.
 *
 * @param      _this   instancia de la clase ioRN1723.
 */
void ioRN1723_reboot (void* _this, uint8_t* port);



///@}
// ********************************************************************************



#endif


///@}