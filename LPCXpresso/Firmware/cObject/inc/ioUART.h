/**
 * @addtogroup grp_ioUART ioUART
 *
 * @brief Implementa la interfaz @ref grp_ioComm.
 * @details Implementa una UART. La recepción se realiza de forma no bloqueante mediante interrupciones. La transmisión puede ser
 * llevada a cabo de forma bloqueante o mediante interrupciones.
 * @{
 */

/**
 * @file ioUART.h
 * @author Mariano Mondani
 *
 * @brief Métodos públicos de la clase ioUART.
 */


#ifndef IOUART_H
#define IOUART_H

#include "ioComm.h"
#include "cQueue.h"



// ********************************************************************************
/**
 * @var ioUART
 * @brief Nombre de la clase.
 * @details La forma de instanciar esta clase es la siguiente:
 * @code
 * void* uart = cObject_new(ioUART, periphMem, baudRate, dataLen, parity, stopBit, mode, txQueueLen, rxQueueLen)
 * @endcode
 * @param ioUART 			nombre de la clase a instanciar.
 * @param periphMem			dirección base de memoria del periférico.
 * @param baudRate			baudrate de la UART. Ver @ref ioUART_BR.
 * @param dataLen			cantidad de bits de cada dato. Ver @ref ioUART_Data.
 * @param parity			paridad. Ver @ref ioUART_Parity.
 * @param stopBit			bits de stop. Ver @ref ioUART_Stop.
 * @param mode				modo de transmisión. Ver @ref ioUART_Mode.
 * @param txQueueLen		longitud de la cola de transmisión, expresada en bytes. Esta cola solamente se instancia en el modo IOUART_MODE_NON_BLOCKING.
 * @param rxQueueLen		longitud de la cola de recepción, expresada en bytes.
 */
extern const void* ioUART;




// ********************************************************************************

/**
 * Enumeración con las posibles velocidades para la UART.
 */
enum ioUART_BR{
	   IOUART_BR_2400 = 2400,						///< 2400 bauds
	   IOUART_BR_4800 = 4800,						///< 4800 bauds
	   IOUART_BR_9600 = 9600,						///< 9600 bauds
	   IOUART_BR_19200 = 19200,						///< 19200 bauds
	   IOUART_BR_57600 = 57600,						///< 57600 bauds
	   IOUART_BR_115200 = 115200					///< 115200 bauds
};


/**
 * Enumeración con las posibles longitudes de datos.
 */
enum ioUART_Data{
	    IOUART_DATA_5BIT = UART_LCR_WLEN5,			///< 5 bits
	    IOUART_DATA_6BIT = UART_LCR_WLEN6,			///< 6 bits
		IOUART_DATA_7BIT = UART_LCR_WLEN7,			///< 7 bits
		IOUART_DATA_8BIT = UART_LCR_WLEN8,			///< 8 bits
};


/**
 * Enumeración con las posibles configuraciones de paridad.
 */
enum ioUART_Parity{
	    IOUART_PAR_NONE = UART_LCR_PARITY_DIS,							///< Sin paridad
	    IOUART_PAR_ODD = UART_LCR_PARITY_EN | UART_LCR_PARITY_ODD,		///< Paridad impar
		IOUART_PAR_EVEN = UART_LCR_PARITY_EN | UART_LCR_PARITY_EVEN		///< Paridad par
};


/**
 * Enumeración con las posibles configuraciones de bits de stop.
 */
enum ioUART_Stop{
	    IOUART_STOP_1BIT = UART_LCR_SBS_1BIT,			///< 1 bit de stop
	    IOUART_STOP_2BIT = UART_LCR_SBS_2BIT			///< 2 bits de stop
};


/**
 * Enumeración con los niveles de llenado del FIFO de recepción para generar la interrupción de RX.
 */
enum ioUART_FIFO{
	   IOUART_FIFO_LEVEL0 = UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS | UART_FCR_TRG_LEV0,		///< 1 byte en LPC17xx
	   IOUART_FIFO_LEVEL1 = UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS | UART_FCR_TRG_LEV1,		///< 4 bytes en LPC17xx
	   IOUART_FIFO_LEVEL2 = UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS | UART_FCR_TRG_LEV2,		///< 8 bytes en LPC17xx
	   IOUART_FIFO_LEVEL3 = UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS | UART_FCR_TRG_LEV3		///< 14 bytes en LPC17xx
};


/**
 * Enumeración con los posibles modos de funcionamiento para la transmisión de la UART.
 */
enum ioUART_Mode{
	    IOUART_MODE_BLOCKING = 0,		///< La transmisión se realiza de forma bloqueante. Se espera hasta transmitir todos los datos.
	    IOUART_MODE_NON_BLOCKING		///< La transmisión se realiza de forma no bloqueante, mediante interrupciones.
};


/**
 * Enumeración con las interrupciones de la UART que se pueden habilitar.
 */
enum ioUART_Interrupt_Enable{
	    IOUART_INT_RX = UART_IER_RBRINT,					///< Interrupción de recepción.
		IOUART_INT_TX = UART_IER_THREINT,					///< Interrupción de transmisión
		IOUART_INT_RX_LINE_STATUS = UART_IER_RLSINT			///< Interrupcion por el status de la recepción
};


/**
 * Enumeración con los ID de las interrupciones
 */
enum ioUART_Interrupt_ID{
	    IOUART_INT_ID_RX = UART_IIR_INTID_RDA,					///< ID de la interrupción de recepcipón
	    IOUART_INT_ID_TX = UART_IIR_INTID_THRE,					///< ID de la interrupción de transmisión
		IOUART_INT_UID_RX_LINE_STATUS = UART_IIR_INTID_RLS		///< ID de la interrupción de status de la recepción
};

// ********************************************************************************



// ********************************************************************************
/**
 * @name Métodos de la clase ioUART
 * @{
 */



/**
 * @brief      Configura los niveles de interrupción de los FIFO de transmisión y recepción
 *
 * @param      _this   instancia de la clase ioUART.
 * @param      config  nivel de los FIFO. Ver @ref ioUART_FIFO.
 */
void ioUART_configFIFO (void* _this, uint32_t config);



/**
 * @brief      Método que debe ser llamado en el handler de la interrupción de recepción.
 *
 * @param      _this  instancia de la clase ioUART.
 */
void ioUART_rxHandler (void* _this);



/**
 * @brief      Método que debe ser llamado en el handler de la interrupción de transmisión.
 * @details    Solo se va a llamar en el caso que la instancia de ioUART se haya configurado
 * 			   para trabajar en modo IOUART_MODE_NON_BLOCKING.
 *
 * @param      _this  instancia de la clase ioUART.
 */
void ioUART_txHandler (void* _this);



/**
 * @brief      Deshabilita la transmisión de la UART.
 *
 * @param      _this  instancia de la clase ioUART.
 */
void ioUART_disableTx (void* _this);



/**
 * @brief      Habilita la transmisión de la UART.
 *
 * @param      _this  instancia de la clase ioUART.
 */
void ioUART_enableTx (void* _this);



/**
 * @brief      Deshabilita la recepción de la UART.
 *
 * @param      _this  instancia de la clase ioUART.
 */
void ioUART_disableRx (void* _this);




/**
 * @brief      Habilita la recepción de la UART.
 *
 * @param      _this  instancia de la clase ioUART.
 */
void ioUART_enableRx (void* _this);

///@}
// ********************************************************************************

#endif


///@}
