/**
 * @addtogroup grp_ioCS5490 ioCS5490
 *
 * @brief Implementa la interfaz @ref grp_cObject.
 * @details Esta clase permite comunicarse con el front-end analógico CS5490 (two channel energy measurement IC).
 * Este integrado tiene una salida de pulsos configurable. Esta clase la configura para que genere pulsos proporcionales a la
 * energía activa medida.
 * @{
 */

/**
 * @file ioCS5490.h
 * @author Mariano Mondani
 *
 * @brief Métodos públicos de la clase ioCS5490.
 */

#ifndef IOCS5490_H
#define IOCS5490_H

#include "cObject.h"



// ********************************************************************************
/**
 * @var ioCS5490
 * @brief Nombre de la clase.
 * @details La forma de instanciar esta clase es la siguiente:
 * @code
 * void* cs5490 = cObject_new(ioCS5490, uart, gpioReset, gpioDO, wordRate, vMax, iMax, iCal, constant, minLoad)
 * @endcode
 * @param ioCS5490 		nombre de la clase a instanciar.
 * @param uart			instancia de la clase @ref grp_ioUART con el que se comunica con el CS5490. Debe estar configurada en modo bloqueante. Baud-rate: 600.
 * @param gpioReset		instancia de la clase @ref grp_ioDigital que maneja el pin RESET del CS5490. Debe estar configurado como salida.
 * @param gpioDO		instancia de la clase @ref grp_ioDigital que maneja el pin DO del CS5490. Debe estar configurado como entrada, alto-activa.
 * @param wordRate		cantidad de muestras por segundo que toma el CS5490 internamente.
 * @param vMax			tensión máxima del medidor.
 * @param iMax			corriente máxima del medidor.
 * @param iCal			corriente de calibración.
 * @param constant		constante del medidor. Pulsos por kWh.
 * @param minLoad		carga mínima del medidor.
 */
extern const void* ioCS5490;

// ********************************************************************************



// ********************************************************************************
/**
 * Enumeración con los registros del CS5490.
 */
enum ioCS5490_Registers {
	IOCS5490_REG_CONFIG0 = 0b000000,
	IOCS5490_REG_CONFIG1 = 0b000001,
	IOCS5490_REG_MASK = 0b000011,
	IOCS5490_REG_PC	= 0b000101,
	IOCS5490_REG_SERIALCTRL = 0b000111,
	IOCS5490_REG_PULSEWIDTH = 0b001000,
	IOCS5490_REG_PULSECTRL = 0b001001,
	IOCS5490_REG_STATUS0 = 0b010111,
	IOCS5490_REG_STATUS1 = 0b011000,
	IOCS5490_REG_STATUS2 = 0b011001,
	IOCS5490_REG_REGLOCK = 0b100010,
	IOCS5490_REG_VPEAK = 0b100100,
	IOCS5490_REG_IPEAK = 0b100101,
	IOCS5490_REG_PSDC = 0b110000,
	IOCS5490_REG_ZX_NUM	= 0b110111,
	IOCS5490_REG_CONFIG2 = 0b000000,
	IOCS5490_REG_REGCHK = 0b000001,
	IOCS5490_REG_I_REG = 0b000010,
	IOCS5490_REG_V_REG = 0b000011,
	IOCS5490_REG_P_REG = 0b000100,
	IOCS5490_REG_P_AVG = 0b000101,
	IOCS5490_REG_I_RMS = 0b000110,
	IOCS5490_REG_V_RMS = 0b000111,
	IOCS5490_REG_Q_AVG = 0b001110,
	IOCS5490_REG_Q_REG = 0b001111,
	IOCS5490_REG_S_REG = 0b010100,
	IOCS5490_REG_PF = 0b010101,
	IOCS5490_REG_T_REG = 0b011011,
	IOCS5490_REG_P_SUM = 0b011101,
	IOCS5490_REG_S_SUM = 0b011110,
	IOCS5490_REG_Q_SUM = 0b011111,
	IOCS5490_REG_I_DCOFF = 0b100000,
	IOCS5490_REG_I_GAIN	= 0b100001,
	IOCS5490_REG_V_DCOFF = 0b100010,
	IOCS5490_REG_V_GAIN	= 0b100011,
	IOCS5490_REG_P_OFF = 0b100100,
	IOCS5490_REG_I_ACOFF = 0b100101,
	IOCS5490_REG_EPSILON = 0b110001,
	IOCS5490_REG_SAMPLECOUNT = 0b110011,
	IOCS5490_REG_T_GAIN = 0b110110,
	IOCS5490_REG_T_OFF = 0b110111,
	IOCS5490_REG_T_SETTLE = 0b111001,
	IOCS5490_REG_LOAD_MIN = 0b111010,
	IOCS5490_REG_SYS_GAIN = 0b111100,
	IOCS5490_REG_TIME = 0b111101,
	IOCS5490_REG_VSAG_DUR = 0b000000,
	IOCS5490_REG_VSAG_LEVEL = 0b000001,
	IOCS5490_REG_IOVER_DUR = 0b000100,
	IOCS5490_REG_IOVER_LEVEL = 0b000101,
	IOCS5490_REG_IZX_LEVEL = 0b011000,
	IOCS5490_REG_PULSERATE = 0b011100,
	IOCS5490_REG_INT_GAIN = 0b101011,
	IOCS5490_REG_VSWELL_DUR = 0b101110,
	IOCS5490_REG_VSWELL_LEVEL = 0b101111,
	IOCS5490_REG_VZX_LEVEL = 0b111010,
	IOCS5490_REG_CYCLECOUNT = 0b111110,
	IOCS5490_REG_SCALE = 0b111111
};
// ********************************************************************************


// ********************************************************************************
/**
 * Enumeración con las instrucciones del CS5490
 */
enum ioCS5490_Instructions {
	IOCS5490_INS_SOFTWARE_RESET = 0b11000001,
	IOCS5490_INS_STANDBY = 0b11000010,
	IOCS5490_INS_WAKEUP = 0b11000011,
	IOCS5490_INS_SINGLE_CONV = 0b11010100,
	IOCS5490_INS_CONTINUOUS_CONV = 0b11010101,
	IOCS5490_INS_HALT_CONV = 0b11011000,
	IOCS5490_INS_CAL_DC_OFFSET_I = 0b11100001,
	IOCS5490_INS_CAL_DC_OFFSET_V = 0b11100010,
	IOCS5490_INS_CAL_DC_OFFSET_I_V = 0b11100110,
	IOCS5490_INS_CAL_AC_OFFSET_I = 0b11110001,
	IOCS5490_INS_CAL_GAIN_I = 0b11111001,
	IOCS5490_INS_CAL_GAIN_V = 0b11111010,
	IOCS5490_INS_CAL_GAIN_I_V = 0b11111110
};
// ********************************************************************************

// ********************************************************************************
/**
 * Enumeración con las páginas de registros del CS5490.
 */
enum ioCS5490_Pages {
	IOCS5490_PAGE_0 = 0b10000000,
	IOCS5490_PAGE_16 = 0b10010000,
	IOCS5490_PAGE_17 = 0b10010001,
	IOCS5490_PAGE_18 = 0b10010010
};
// ********************************************************************************

// ********************************************************************************
/**
 * @name Métodos de la clase ioCS5490
 * @{
 */



/**
 * @brief      Inicializa el CS5490.
 *
 * @param      _this  instancia de la clase ioCS5490.
 */
void ioCS5490_init (void* _this, uint32_t vDCOffset, uint32_t iDCOffset, uint32_t vGain, uint32_t iGain);


/**
 * @brief      Escribe el registro reg con el valor value en el CS5490.
 *
 * @param      _this  instancia de la clase ioCS5490.
 * @param      reg    registro a escribir. Ver @ref ioCS5490_Registers
 * @param      value  valor a escribir en el registro.
 */
void ioCS5490_registerWrite (void* _this, uint8_t reg, uint32_t value);


/**
 * @brief      Lee el registro reg del CS5490
 *
 * @param      _this  instancia de la clase ioCS5490.
 * @param      reg	  registro a leer. Ver @ref ioCS5490_Registers
 *
 * @param      valor del registro leído.
 */
uint32_t ioCS5490_registerRead (void* _this, uint8_t reg);


/**
 * @brief      Selecciona una página de registros en el CS5490 (ver el datasheet para determinar en qué pagina está cada registro).
 *
 * @param      _this  instancia de la clase ioCS5490.
 * @param      page   página de registros. Ver @ref ioCS5490_Pages.
 */
void ioCS5490_pageSelect (void* _this, uint8_t page);


/**
 * @brief      Envía una instrucción al CS5490.
 *
 * @param      _this  		 instancia de la clase ioCS5490.
 * @param      instruction   instrucción a ejecutar. Ver @ref ioCS5490_Instructions.
 */
void ioCS5490_instructionWrite (void* _this, uint8_t instruction);


/**
 * @brief      Retorna el valor de la corriente de calibración.
 *
 * @param      _this  		 instancia de la clase ioCS5490.
 *
 * @return     valor de la corriente de calibracion.
 */
float ioCS5490_getIcalibration (void* _this);


/**
 * @brief      Retorna el scale del CS5490.
 *
 * @param      _this  		 instancia de la clase ioCS5490.
 *
 * @return     valor del scale.
 */
float ioCS5490_getMeterScale (void* _this);


/**
 * @brief      Retorna la tensión máxima del CS5490.
 *
 * @param      _this  		 instancia de la clase ioCS5490.
 *
 * @return     valor de la tensión máxima.
 */
float ioCS5490_getVmax (void* _this);


/**
 * @brief      Retorna la potencia máxima del CS5490.
 *
 * @param      _this  		 instancia de la clase ioCS5490.
 *
 * @return     valor de la potencia máxima.
 */
float ioCS5490_getMaxPower (void* _this);


/**
 * @brief      Retorna el scale de la potencia del CS5490.
 *
 * @param      _this  		 instancia de la clase ioCS5490.
 *
 * @return     valor del scale de potencia.
 */
float ioCS5490_getPowerScale (void* _this);


/**
 * @brief      Retorna el word rate del CS5490
 *
 * @param      _this  		 instancia de la clase ioCS5490.
 *
 * @return     valor del word rate.
 */
float ioCS5490_getWordRate (void* _this);


/**
 * @brief      Convierte un número en formato Qm.n con signo a float.Cantidad de bits del número: m+n+1.
 *
 * @param      value   número a convertir a float.
 * @param	   m	   cantidad de bits en la parte entera, sin contar el bit de signo.
 * @param      n       cantidad de bits en la parte decimal.
 *
 * @return     valor float resultante de la conversión.
 */
float ioCS5490_signedFract2Float (int32_t value, uint32_t m, uint32_t n);


/**
 * @brief      Convierte un número en formato Qm.n sin signo a float.Cantidad de bits del número: m+n.
 *
 * @param      value   número a convertir a float.
 * @param	   m	   cantidad de bits en la parte entera.
 * @param      n       cantidad de bits en la parte decimal.
 *
 * @return     valor float resultante de la conversión.
 */
float ioCS5490_unsignedFract2Float (uint32_t value, uint32_t m, uint32_t n);


/**
 * @brief      Convierte un número float al formato Qm.n con signo. Cantidad de bits del número: m+n+1.
 *
 * @param      value   numero a convertir a punto fijo.
 * @param	   m	   cantidad de bits en la parte entera, sin contar el bit de signo.
 * @param      n       cantidad de bits en la parte decimal
 *
 * @return     valor en punto fijo resultante de la conversión.
 */
int32_t ioCS5490_signedFloat2Fract (float value, uint32_t m, uint32_t n);



///@}
// ********************************************************************************



#endif


///@}
