/**
 * @file SmartPlug_hw_test_CS5490.c
 *
 * @brief Prueba de hardware sobre la placa del Smart Plug en su versión 1.
 *
 * Prueba el funcionamiento del front-end analógico CS5490. Levanta algunas
 * de sus mediciones y las muestra por la UART.
 */


#include "chip.h"


#include <cr_section_macros.h>
#include <stdio.h>

#include "memAlloc.h"
#include "cObject.h"
#include "ioDigital.h"
#include "ioDebounce.h"
#include "ioUART.h"
#include "ioCS5490.h"
#include "cTimer.h"


void* uartDebug;
void* pinRelay;
void* gpioDO;
void* gpioReset;
void* uartCS5490;
void* cs5490;
void* refreshTimer;



uint32_t pulsos;


void term_clear (void* uart);
void term_home (void* uart);




void SysTick_Handler(void)
{
	cTimer_handler();
}


void UART1_IRQHandler(void)
{
	if (ioComm_getInt(uartDebug, IOUART_INT_ID_TX))
	{
		ioUART_txHandler(uartDebug);
	}
	else if(ioComm_getInt(uartDebug, IOUART_INT_ID_RX))
	{
		ioUART_rxHandler(uartDebug);
	}
}


void UART3_IRQHandler(void)
{
	if (ioComm_getInt(uartCS5490, IOUART_INT_ID_TX))
	{
		ioUART_txHandler(uartCS5490);
	}
	else if(ioComm_getInt(uartCS5490, IOUART_INT_ID_RX))
	{
		ioUART_rxHandler(uartCS5490);
	}
}


void EINT3_IRQHandler(void)
{
	Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, GPIOINT_PORT0, 1 << 27);
	pulsos++;
}




int main(void) {
	uint8_t buff[50];
	uint32_t muestras, irms, vrms, potencia_activa;
	uint32_t factor_potencia, epsilon;
	float conversion, irms_linea, vrms_linea, potencia_activa_linea, frecuencia_linea;


    // Read clock settings and update SystemCoreClock variable
	SystemCoreClockUpdate();


    initMemHeap();


    // ===================== [RELAY] =====================
	pinRelay = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 2, 7);
	ioObject_init(pinRelay);
	// =====================================================

    // ===================== [UART DEBUG] =====================
    uartDebug = cObject_new(ioUART, LPC_UART1, IOUART_BR_115200, IOUART_DATA_8BIT, IOUART_PAR_NONE, IOUART_STOP_1BIT, IOUART_MODE_BLOCKING, 2, 10);
    ioObject_init(uartDebug);
    ioComm_intEnable(uartDebug, IOUART_INT_TX);
    ioComm_intEnable(uartDebug, IOUART_INT_RX);
    ioUART_configFIFO(uartDebug, IOUART_FIFO_LEVEL0);

    NVIC_SetPriority(UART1_IRQn, 1);
    NVIC_EnableIRQ(UART1_IRQn);
    // =====================================================


    // ===================== [SYS TICK] =====================
    SysTick_Config(SystemCoreClock / 1000);
    // =====================================================


    // ===================== [CS5490] =====================
    gpioDO = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 0, 27);
    ioObject_init(gpioDO);
    Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, GPIOINT_PORT0, 1 << 27);
    NVIC_ClearPendingIRQ(EINT3_IRQn);
    NVIC_EnableIRQ(EINT3_IRQn);

    gpioReset = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 1, 30);
    ioObject_init(gpioReset);

    uartCS5490 = cObject_new(ioUART, LPC_UART3, IOUART_BR_600, IOUART_DATA_8BIT, IOUART_PAR_NONE, IOUART_STOP_1BIT, IOUART_MODE_BLOCKING, 2, 20);
	ioObject_init(uartCS5490);
	ioComm_intEnable(uartCS5490, IOUART_INT_TX);
	ioComm_intEnable(uartCS5490, IOUART_INT_RX);
	ioUART_configFIFO(uartCS5490, IOUART_FIFO_LEVEL0);

	NVIC_SetPriority(UART3_IRQn, 1);
	NVIC_EnableIRQ(UART3_IRQn);

	cs5490 = cObject_new (ioCS5490, uartCS5490, gpioReset);
	ioCS5490_init(cs5490, 0xFE5657, 0xFFE604, 0x3A62AA, 0x73AC71, 4000.0, 220.0, 5.0, 1.75, 5000.0, 8.0);
	// =====================================================




    refreshTimer = cObject_new(cTimer);
    cTimer_start(refreshTimer, 1000);

    ioObject_write(pinRelay, 0);


    while(1)
    {
    	if (cTimer_hasExpired(refreshTimer))
		{
			cTimer_start(refreshTimer, 2000);

			ioCS5490_pageSelect(cs5490, IOCS5490_PAGE_16);
			muestras = ioCS5490_registerRead(cs5490, IOCS5490_REG_TIME);
			irms = ioCS5490_registerRead(cs5490, IOCS5490_REG_I_RMS);
			vrms = ioCS5490_registerRead(cs5490, IOCS5490_REG_V_RMS);
			potencia_activa = ioCS5490_registerRead(cs5490, IOCS5490_REG_P_AVG);
			factor_potencia = ioCS5490_registerRead(cs5490, IOCS5490_REG_PF);
			epsilon = ioCS5490_registerRead(cs5490, IOCS5490_REG_EPSILON);


			term_clear(uartDebug);
			term_home(uartDebug);

			conversion = ioCS5490_unsignedFract2Float(irms, 0, 24);
			irms_linea = conversion * ioCS5490_getIcalibration(cs5490) / ioCS5490_getMeterScale(cs5490);
			sprintf(buff, "IRMS: %f\n\r", irms_linea);
			ioUART_writeString(uartDebug, buff);

			conversion = ioCS5490_unsignedFract2Float(vrms, 0, 24);
			vrms_linea = conversion * ioCS5490_getVmax(cs5490) / 0.6;
			sprintf(buff, "VRMS: %f\n\r", vrms_linea);
			ioUART_writeString(uartDebug, buff);

			conversion = ioCS5490_signedFract2Float(potencia_activa, 0, 23);
			potencia_activa_linea = conversion * ioCS5490_getMaxPower(cs5490) / ioCS5490_getPowerScale(cs5490);
			sprintf(buff, "Potencia activa: %f\n\r", potencia_activa_linea);
			ioUART_writeString(uartDebug, buff);

			conversion = ioCS5490_signedFract2Float(factor_potencia, 0, 23);
			sprintf(buff, "Factor de potencia: %f\n\r", conversion);
			ioUART_writeString(uartDebug, buff);

			conversion = ioCS5490_signedFract2Float(epsilon, 0, 23);
			frecuencia_linea = conversion * ioCS5490_getWordRate(cs5490);
			sprintf(buff, "Frecuencia linea: %f\n\r", frecuencia_linea);
			ioUART_writeString(uartDebug, buff);

			sprintf(buff, "Pulsos: %d\n\r", pulsos);
			ioUART_writeString(uartDebug, buff);
		}
    }


    return 0 ;
}


void term_clear (void* uart)
{
	ioObject_write(uart, 0x1B);
	ioComm_writeBytes(uart, 3, "[2J");
}


void term_home (void* uart)
{
	ioObject_write(uart, 0x1B);
	ioComm_writeBytes(uart, 2, "[H");
}
