/**
 * @addtogroup grp_ioEE25LCxxx ioEE25LCxxx
 *
 * @brief Implementa la interfaz @ref grp_cObject.
 * @details Esta clase permite comunicarse con una EEPROM SPI de la familia 25LCxxx de Microchip.
 * @{
 */

/**
 * @file ioEE25LCxxx.h
 * @author Mariano Mondani
 *
 * @brief Métodos públicos de la clase ioEE25LCxxx.
 */

#ifndef IOEE25LCXXX_H
#define IOEE25LCXXX_H

#include "ioObject.h"



// ********************************************************************************
/**
 * @var ioEE25LCxxx
 * @brief Nombre de la clase.
 * @details La forma de instanciar esta clase es la siguiente:
 * @code
 * void* ee25lcxxx = cObject_new(ioEE25LCxxx, spiPort, sselPin, eeSize)
 * @endcode
 * @param ioEE25LCxxx 		nombre de la clase a instanciar.
 * @param spiPort			instancia de la clase ioSPI que comunica a la EEPROM externa.
 * @param sselPin			instancia de la clase ioDigital que maneja el pin Slave Select de la EEPROM externa.
 * @param eeSize			tamaño de la EEPROM. Ver @ref ioEE25LCxxx_EEPROM_Size.
 */
extern const void* ioEE25LCxxx;

// ********************************************************************************



// ********************************************************************************
/**
 * Enumeración con los posibles tamaños de las EEPROM 25LCxxx.
 */
enum ioEE25LCxxx_EEPROM_Size {
	IOEE25LCXXX_SIZE_32K = 0,		///< EEPROM 25LC320. 32 Kbit. Tamaño de página: 32 bytes.
	IOEE25LCXXX_SIZE_64K,			///< EEPROM 25LC640. 64 Kbit. Tamaño de página: 32 bytes.
	IOEE25LCXXX_SIZE_128K,			///< EEPROM 25LC640. 128 Kbit. Tamaño de página: 64 bytes.
	IOEE25LCXXX_SIZE_256K,			///< EEPROM 25LC640. 256 Kbit. Tamaño de página: 64 bytes.
	IOEE25LCXXX_SIZE_512K,			///< EEPROM 25LC640. 512 Kbit. Tamaño de página: 128 bytes.
	IOEE25LCXXX_SIZE_1024K,			///< EEPROM 25LC640. 1024 Kbit. Tamaño de página: 256 bytes.
};
// ********************************************************************************


// ********************************************************************************
/**
 * @name Métodos de la clase ioEE25LCxxx
 * @{
 */



/**
 * @brief      Escribe el registro Status de la EEPROM con data
 *
 * @param      _this  instancia de la clase ioEE25LCxxx.
 * @param      data   byte a escribir en el registro Status.
 */
void ioEE25LCxxx_writeStatus (void* _this, uint8_t data);



/**
 * @brief      Lee el valor actual del registro Status de la EEPROM
 *
 * @param      _this  instancia de la clase ioEE25LCxxx.
 *
 * @return     valor actual del registro Status.
 */
uint8_t ioEE25LCxxx_readStatus (void* _this);



/**
 * @brief      Chequea el Busy Flag de la EEPROM hasta que deja de estar ocupada.
 *
 * @param      _this  instancia de la clase ioEE25LCxxx.
 */
void ioEE25LCxxx_busyPolling (void* _this);



/**
 * @brief      Habilita la escritura de la EEPROM
 *
 * @param      _this  instancia de la clase ioEE25LCxxx.
 */
void ioEE25LCxxx_setWriteEnable (void* _this);



/**
 * @brief      Escribe a partir de la direeción address la cantidad de bytes len del buffer data.
 * @details    A pesar que data es un uint8_t* cualquier tipo de dato puede ser escrito en la EEPROM.
 * Por ejemplo para escribir un float:
 * 
 * @code
 * float foo = 25.5;
 * 
 * ioEE25LCxxx_writeData (ee25lcxxx, 0x10, (uint8_t*)&foo, sizeof(float));
 * @endcode
 *
 * @param      _this    instancia de la clase ioEE25LCxxx.
 * @param      address  dirección de la EEPROM a partir de la cual escribir.
 * @param      data     puntero a los datos a escribir.
 * @param      len      cantidad de bytes a escribir.
 */
void ioEE25LCxxx_writeData (void* _this, uint32_t address, uint8_t* data, uint32_t len);



/**
 * @brief      Lee la EEPROMa partir de la dirección address, la cantidad de bytes len y los guarda en data.
 *
 * @param      _this    instancia de la clase ioEE25LCxxx.
 * @param      address  dirección de la EEPROM a partir de la cual leer.
 * @param      data     puntero donde guardar los datos leídos.
 * @param      len      cantidad de bytes a leer.
 */
void ioEE25LCxxx_readData (void* _this, uint32_t address, uint8_t* data, uint32_t len);



/**
 * @brief      Escribe en la EEPROM un string (terminado en '\0') a partir de la dirección address.
 *
 * @param      _this    instancia de la clase ioEE25LCxxx.
 * @param[in]  address  dirección de la EEPROM a partir de la cual escribir.
 * @param      data     puntero al string que se quiere guardar.
 */
void ioEE25LCxxx_writeString (void* _this, uint32_t address, uint8_t* data);



/**
 * @brief      Borra la EEPROM la cantidad de bytes len a partir de la dirección address.
 *
 * @param      _this    instancia de la clase ioEE25LCxxx.
 * @param[in]  address  dirección de la EEPROM a partir de la cual borrar.
 * @param[in]  len      cantidad de bytes a borrar.
 */
void ioEE25LCxxx_erase (void* _this, uint32_t address, uint32_t len);


///@}
// ********************************************************************************



#endif


///@}
