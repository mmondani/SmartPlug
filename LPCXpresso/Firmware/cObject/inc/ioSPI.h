/**
 * @addtogroup grp_ioSPI ioSPI
 *
 * @brief Implementa la interfaz @ref grp_ioComm.
 * @details Implementa un SPI. Tanto la recepción como la transmisión se realizan de forma bloqueante.
 * @{
 */

/**
 * @file ioSPI.h
 * @author Mariano Mondani
 *
 * @brief Métodos públicos de la clase ioSPI.
 */


#ifndef IOSPI_H
#define IOSPI_H

#include "ioComm.h"
#include "ioDigital.h"



// ********************************************************************************
/**
 * @var ioSPI
 * @brief Nombre de la clase.
 * @details La forma de instanciar esta clase es la siguiente:
 * @code
 * void* spi = cObject_new(ioSPI, periphMem, clockRate, dataLen, clockMode, spiMode)
 * @endcode
 * @param ioSPI 			nombre de la clase a instanciar.
 * @param periphMem			dirección base de memoria del periférico.
 * @param clockRate			velocidad de clock del SPI.
 * @param dataLen			cantidad de bits de cada dato. Ver @ref ioSPI_Data.
 * @param clockMode			modo del clock. Ver @ref ioSPI_ClockMode.
 * @param spiMode			modo de funcionamiento del SPI. Ver @ref ioSPI_SPIMode.
 */
extern const void* ioSPI;



// ********************************************************************************

/**
 * Enumeración con las posibles longitudes de datos.
 */
enum ioSPI_Data{
	   IOSPI_DATA_4BITS = SSP_BITS_4,						///< Datos de 4 bits
	   IOSPI_DATA_5BITS = SSP_BITS_5,						///< Datos de 5 bits
	   IOSPI_DATA_6BITS = SSP_BITS_6,						///< Datos de 6 bits
	   IOSPI_DATA_7BITS = SSP_BITS_7,						///< Datos de 7 bits
	   IOSPI_DATA_8BITS = SSP_BITS_8,						///< Datos de 8 bits
};


/**
 * Enumeración con las posibles modos del clock de SPI.
 */
enum ioSPI_ClockMode{
	   IOSPI_CLOCKMODE_MODE0 = SSP_CLOCK_MODE0,						///< CPHA = 0, CPOL = 0
	   IOSPI_CLOCKMODE_MODE1 = SSP_CLOCK_MODE1,						///< CPHA = 0, CPOL = 1
	   IOSPI_CLOCKMODE_MODE2 = SSP_CLOCK_MODE2,						///< CPHA = 1, CPOL = 0
	   IOSPI_CLOCKMODE_MODE3 = SSP_CLOCK_MODE3,						///< CPHA = 1, CPOL = 1
};


/**
 * Enumeración con las posibles modos del clock de SPI.
 */
enum ioSPI_SPIMode{
	   IOSPI_SPIMODE_MASTER = SSP_MODE_MASTER,						///< SPI en modo Master.
	   IOSPI_SPIMODE_SLAVE = SSP_MODE_SLAVE,						///< SPI en modo Slave.
};


/**
 * Enumeración con los ID de las interrupciones
 */
enum ioSPI_Interrupt_ID{
	   IOSPI_INT_ID_OVERRUN = SSP_RORRIS,						///< Interrupción por overrun. Se recibió un dato con el FIFO de Rx lleno.
	   IOSPI_INT_ID_TIMEOUT = SSP_RTRIS,						///< En el FIFO de Rx hay un dato que no fue leído por el "período de timeout" (ver hoja de datos)
	   IOSPI_INT_ID_RX = SSP_RXRIS,								///< El FIFO de Rx está al menos lleno por la mitad.
	   IOSPI_INT_ID_TX = SSP_TXRIS								///< El FIFO de Tx está al menos vacío por la mitad.
};
// ********************************************************************************


// ********************************************************************************
/**
 * @name Métodos de la clase ioSPI
 * @{
 */


/**
 * @brief      Configura la velocidad del clock del SPI.
 *
 * @param      _this   instancia de la clase ioSPI.
 * @param      clock   valocidad de clock a configurar.
 */
void ioSPI_setClock (void* _this, uint32_t clock);


/**
 * @brief		Envía la cantidad de bytes lenTx de buffTx y recibe lenRx bytes en buffRx.
 *
 * @param		_this	instancia de la clase ioSPI.
 * @param		lenTx	cantidad de bytes a enviar.
 * @param		buffTx	buffer de datos a enviar.
 * @param		lenRx	cantidad de bytes a recibir.
 * @param		buffRx	buffer donde se guardarán los bytes recibidos.
 */
void ioSPI_sendReceive (void* _this, uint32_t lenTx, uint8_t* buffTx, uint32_t lenRx, uint8_t* buffRx);


///@}
// ********************************************************************************



#endif


///@}
