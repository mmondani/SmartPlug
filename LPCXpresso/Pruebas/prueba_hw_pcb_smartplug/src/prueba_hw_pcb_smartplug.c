/*
===============================================================================
 Name        : prueba_hw_pcb_smartplug.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/


#include "chip.h"


#include <cr_section_macros.h>
#include <stdio.h>

#include "memAlloc.h"
#include "cObject.h"
#include "ioDigital.h"
#include "ioDebounce.h"
#include "ioUART.h"
#include "ioInternalRTC.h"
#include "ioCS5490.h"
#include "cTimer.h"
#include "ioRN1723.h"
#include "cQueue.h"
#include "ioSPI.h"
#include "ioEE25LCxxx.h"

void* pinLedVerde;
void* pinLedRojo;
void* pinSwitch;
void* pinRelay;
void* debounceSwitch;
void* uartDebug;
void* rtc;
void* gpioDO;
void* gpioReset;
void* uartCS5490;
void* cs5490;
void* refreshTimer;
void* rn1723;
void* gpioResetRN1723;
void* uartRN1723;
void* inBuffer;
void* outBuffer;
void* rn1723InitTimer;
void* tcpConnTimer;
void* tcpConnCloseTimer;
void* pinSSelEE;
void* spiPort1;
void* ee25LC256;
void* writeEEPROMTimer;



uint32_t tickCount;
uint32_t pulsos;


void term_clear (void* uart);
void term_home (void* uart);




void SysTick_Handler(void)
{
	cTimer_handler();

	ioRN1723_handler(rn1723);

	ioDebounce_handler(debounceSwitch);

	tickCount ++;
	if (tickCount >= 1000)
	{
		// Cada 1 seg
		tickCount = 0;

		ioDigital_toggle(pinLedVerde);
		ioDigital_toggle(pinLedRojo);

		//ioDigital_toggle(pinRelay);
	}
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


void UART2_IRQHandler(void)
{
	if (ioComm_getInt(uartRN1723, IOUART_INT_ID_TX))
	{
		ioUART_txHandler(uartRN1723);
	}
	else if(ioComm_getInt(uartRN1723, IOUART_INT_ID_RX))
	{
		ioUART_rxHandler(uartRN1723);
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
	rtc_time_t fullTime;
	uint8_t buff[50];
	uint32_t temperatura, muestras, irms, vrms, potencia_activa, potencia_reactiva, potencia_aparente;
	uint32_t i_offset, v_offset, i_ganancia, v_ganancia, factor_potencia, escala, epsilon, settle;
	float conversion, irms_linea, vrms_linea, potencia_activa_linea, potencia_reactiva_linea, frecuencia_linea;
	float potencia_aparente_linea;
	uint32_t sendTCPData, sendDisconnect;
	uint32_t intWrite, intRead;
	float floatWrite, floatRead;


    // Read clock settings and update SystemCoreClock variable
	SystemCoreClockUpdate();


    initMemHeap();


    // ===================== [LEDS] =====================
    pinLedVerde = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 0, 21);
    ioObject_init(pinLedVerde);

    pinLedRojo = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 2, 13);
    ioObject_init(pinLedRojo);
    // =====================================================


    // ===================== [SWITCH] =====================
    pinSwitch = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 0, 3);
    ioObject_init(pinSwitch);

    debounceSwitch = cObject_new(ioDebounce, pinSwitch, IODIGITAL_LEVEL_HIGH, 40);
    // =====================================================

    // ===================== [RELAY] =====================
	pinRelay = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 2, 7);
	ioObject_init(pinRelay);
	// =====================================================

    // ===================== [25LC256] =====================
	pinSSelEE = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 0, 5);
	ioObject_init(pinSSelEE);

    spiPort1 = cObject_new(ioSPI, LPC_SSP1, 1000000, IOSPI_DATA_8BITS, IOSPI_CLOCKMODE_MODE0, IOSPI_SPIMODE_MASTER);
    ioObject_init(spiPort1);

    ee25LC256 = cObject_new(ioEE25LCxxx, spiPort1, pinSSelEE, IOEE25LCXXX_SIZE_256K);

    ioEE25LCxxx_setWriteEnable(ee25LC256);
    ioEE25LCxxx_writeStatus(ee25LC256, 0x00);

    writeEEPROMTimer = cObject_new(cTimer);
    cTimer_start(writeEEPROMTimer, 3000);
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


    // ===================== [RN1723] =====================
    gpioResetRN1723 = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 2, 2);
    ioObject_init(gpioResetRN1723);


    uartRN1723 = cObject_new(ioUART, LPC_UART2, IOUART_BR_9600, IOUART_DATA_8BIT, IOUART_PAR_NONE, IOUART_STOP_1BIT, IOUART_MODE_NON_BLOCKING, 50, 50);
    ioObject_init(uartRN1723);
    ioComm_intEnable(uartRN1723, IOUART_INT_TX);
    ioComm_intEnable(uartRN1723, IOUART_INT_RX);
    ioUART_configFIFO(uartRN1723, IOUART_FIFO_LEVEL0);

    NVIC_SetPriority(UART2_IRQn, 1);
    NVIC_EnableIRQ(UART2_IRQn);


    inBuffer = cObject_new(cQueue, 10, sizeof(uint8_t));
    outBuffer = cObject_new(cQueue, 10, sizeof(uint8_t));


    rn1723 = cObject_new(ioRN1723, uartRN1723, gpioResetRN1723, inBuffer, outBuffer, IORN1723_INTERVAL_1_EVERY_4_SEC);
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
	//ioCS5490_init(cs5490, 0xFE5657, 0xFFE0FB, 0x3C7AE1, 0x741857, 4000.0, 220.0, 5.0, 1.75, 5000.0, 8.0);
	// =====================================================


	// ===================== [RTC] =====================
    rtc = cObject_new(ioInternalRTC, LPC_RTC);


    fullTime.second = 0;
    fullTime.minute = 50;
    fullTime.hour = 12;
    fullTime.dayOfMonth = 7;
    fullTime.dayOfWeek = 0;			// Domingo
    fullTime.month = 8;
    fullTime.year = 2016;

    ioRTC_setFullTime(rtc, &fullTime);

    ioObject_init(rtc);
    ioObject_enable(rtc);
    // =================================================





    refreshTimer = cObject_new(cTimer);
    cTimer_start(refreshTimer, 1000);

    rn1723InitTimer = cObject_new(cTimer);
    cTimer_start(rn1723InitTimer, 1000);

    tcpConnTimer = cObject_new(cTimer);
    cTimer_start(tcpConnTimer, 7000);

    tcpConnCloseTimer = cObject_new(cTimer);

    ioObject_write(pinLedVerde, 1);
    ioObject_write(pinLedRojo, 0);

    ioObject_write(pinRelay, 0);



    while(1)
    {
    	if (cTimer_hasExpired(writeEEPROMTimer))
    	{
    		cTimer_stop(writeEEPROMTimer);

    		intWrite = 112147;
			ioEE25LCxxx_setWriteEnable(ee25LC256);
			ioEE25LCxxx_writeData(ee25LC256, 0x05, &intWrite, sizeof(uint32_t));
			ioEE25LCxxx_busyPolling(ee25LC256);
			ioEE25LCxxx_readData(ee25LC256, 0x05, &intRead, sizeof(uint32_t));

    		floatWrite = 220.15;
			ioEE25LCxxx_setWriteEnable(ee25LC256);
			ioEE25LCxxx_writeData(ee25LC256, 0x10, &floatWrite, 4);
			ioEE25LCxxx_busyPolling(ee25LC256);
			ioEE25LCxxx_readData(ee25LC256, 0x10, &floatRead, 4);

			term_clear(uartDebug);
			term_home(uartDebug);

			sprintf(buff, "EEPROM: %d - %d\n\r", intWrite, intRead);
			ioUART_writeString(uartDebug, buff);
			sprintf(buff, "EEPROM: %f - %f\n\r", floatWrite, floatRead);
			ioUART_writeString(uartDebug, buff);
    	}

    	// Inicializa el módulo luego de vencido el timer rn1723InitTimer. Si no está en estado IDLE, espera otro segundo.
    	if (cTimer_hasExpired(rn1723InitTimer))
		{
    		// Intenta inicializar el módulo RN1723
    		if (ioRN1723_isIdle(rn1723))
    		{
    		    ioObject_init(rn1723);
    		    cTimer_stop(rn1723InitTimer);
    		}
    		else
    			cTimer_start(rn1723InitTimer, 1000);
		}


    	// Cada 7 segundos se conecta al socket indicado y envía "Hola".
    	if (cTimer_hasExpired(tcpConnTimer))
    	{
    		if (ioRN1723_isAuthenticated(rn1723))
    		{
				if (ioRN1723_isIdle(rn1723))
				{
					ioRN1723_connectTCP(rn1723, "192.168.0.101", "9871");
					cTimer_start(tcpConnTimer, 7000);
					sendTCPData = 1;
				}
				else
					cTimer_start(rn1723InitTimer, 1000);
    		}

    	}

    	// Envía el mensaje "Hola" si está conectado al socket.
    	if (ioRN1723_isAuthenticated(rn1723) && ioRN1723_isTCPConnected(rn1723))
    	{
    		if (sendTCPData == 1)
    		{
				ioComm_writeBytes(rn1723, 4, "Hola");
				sendTCPData = 0;
				cTimer_start(tcpConnCloseTimer, 1000);
    		}


    	}

    	// Luego de 1 segundo cierra la conexión si no hay más datos pendientes.
    	if (cTimer_hasExpired(tcpConnCloseTimer))
    	{
    		if (ioRN1723_getDataPendingToSend(rn1723) == 0)
    		{
    			cTimer_stop(tcpConnCloseTimer);
    			if (ioRN1723_isIdle(rn1723))
    				ioRN1723_disconnectTCP(rn1723);
    			else
    				cTimer_start(tcpConnCloseTimer, 200);
    		}
    		else
    			cTimer_start(tcpConnCloseTimer, 200);
    	}



    	if (ioDebounce_getActiveEdge(debounceSwitch))
    	{
    		// Inicia el proceso de WPS
    		if (ioRN1723_isIdle(rn1723))
				ioRN1723_runWPS(rn1723, 3);
    	}


    	if (cTimer_hasExpired(refreshTimer))
		{
			cTimer_start(refreshTimer, 2000);

			ioCS5490_pageSelect(cs5490, IOCS5490_PAGE_16);
			temperatura = ioCS5490_registerRead(cs5490, IOCS5490_REG_T_REG);
			muestras = ioCS5490_registerRead(cs5490, IOCS5490_REG_TIME);
			irms = ioCS5490_registerRead(cs5490, IOCS5490_REG_I_RMS);
			vrms = ioCS5490_registerRead(cs5490, IOCS5490_REG_V_RMS);
			potencia_activa = ioCS5490_registerRead(cs5490, IOCS5490_REG_P_AVG);
			potencia_reactiva = ioCS5490_registerRead(cs5490, IOCS5490_REG_Q_AVG);
			potencia_aparente = ioCS5490_registerRead(cs5490, IOCS5490_REG_S_REG);
			factor_potencia = ioCS5490_registerRead(cs5490, IOCS5490_REG_PF);
			epsilon = ioCS5490_registerRead(cs5490, IOCS5490_REG_EPSILON);
			i_offset = ioCS5490_registerRead(cs5490, IOCS5490_REG_I_DCOFF);
			v_offset = ioCS5490_registerRead(cs5490, IOCS5490_REG_V_DCOFF);
			i_ganancia = ioCS5490_registerRead(cs5490, IOCS5490_REG_I_GAIN);
			v_ganancia = ioCS5490_registerRead(cs5490, IOCS5490_REG_V_GAIN);
			settle = ioCS5490_registerRead(cs5490, IOCS5490_REG_T_SETTLE);

			ioCS5490_pageSelect(cs5490, IOCS5490_PAGE_18);
			escala = ioCS5490_registerRead(cs5490, IOCS5490_REG_SCALE);


			term_clear(uartDebug);
			term_home(uartDebug);

			conversion = ioCS5490_signedFract2Float(temperatura, 7, 16);
			sprintf(buff, "Temperatura: %f\n\r", conversion);
			ioUART_writeString(uartDebug, buff);
			sprintf(buff, "Muestras: %lX\n\r", muestras);
			ioUART_writeString(uartDebug, buff);

			conversion = ioCS5490_unsignedFract2Float(irms, 0, 24);
			irms_linea = conversion * ioCS5490_getIcalibration(cs5490) / ioCS5490_getMeterScale(cs5490);
			sprintf(buff, "IRMS: %f\t\t%f\n\r", conversion, irms_linea);
			ioUART_writeString(uartDebug, buff);

			conversion = ioCS5490_unsignedFract2Float(vrms, 0, 24);
			vrms_linea = conversion * ioCS5490_getVmax(cs5490) / 0.6;
			sprintf(buff, "VRMS: %f\t\t%f\n\r", conversion, vrms_linea);
			ioUART_writeString(uartDebug, buff);

			conversion = ioCS5490_signedFract2Float(potencia_activa, 0, 23);
			potencia_activa_linea = conversion * ioCS5490_getMaxPower(cs5490) / ioCS5490_getPowerScale(cs5490);
			sprintf(buff, "Potencia activa: %f\t\t%f\n\r", conversion, potencia_activa_linea);
			ioUART_writeString(uartDebug, buff);

			conversion = ioCS5490_signedFract2Float(potencia_reactiva, 0, 23);
			potencia_reactiva_linea = conversion * ioCS5490_getMaxPower(cs5490) / ioCS5490_getPowerScale(cs5490);
			sprintf(buff, "Potencia reactiva: %f\t\t%f\n\r", conversion, potencia_reactiva_linea);
			ioUART_writeString(uartDebug, buff);

			conversion = ioCS5490_signedFract2Float(potencia_aparente, 0, 23);
			potencia_aparente_linea = conversion * ioCS5490_getMaxPower(cs5490) / ioCS5490_getPowerScale(cs5490);
			sprintf(buff, "Potencia aparente: %f\t\t%f\n\r", conversion, potencia_aparente_linea);
			ioUART_writeString(uartDebug, buff);

			conversion = ioCS5490_signedFract2Float(factor_potencia, 0, 23);
			sprintf(buff, "Factor de potencia: %f\n\r", conversion);
			ioUART_writeString(uartDebug, buff);

			conversion = ioCS5490_signedFract2Float(epsilon, 0, 23);
			frecuencia_linea = conversion * ioCS5490_getWordRate(cs5490);
			sprintf(buff, "Frecuencia linea: %f\t\t%f\n\r", conversion, frecuencia_linea);
			ioUART_writeString(uartDebug, buff);

			conversion = ioCS5490_signedFract2Float(i_offset, 0, 23);
			sprintf(buff, "I DC Offset: %f\n\r", conversion);
			ioUART_writeString(uartDebug, buff);

			conversion = ioCS5490_signedFract2Float(v_offset, 0, 23);
			sprintf(buff, "V DC Offset: %f\n\r", conversion);
			ioUART_writeString(uartDebug, buff);

			conversion = ioCS5490_unsignedFract2Float(i_ganancia, 2, 22);
			sprintf(buff, "I Ganancia: %f\n\r", conversion);
			ioUART_writeString(uartDebug, buff);

			conversion = ioCS5490_unsignedFract2Float(v_ganancia, 2, 22);
			sprintf(buff, "V Ganancia: %f\n\r", conversion);
			ioUART_writeString(uartDebug, buff);

			sprintf(buff, "Pulsos: %d\n\r", pulsos);
			ioUART_writeString(uartDebug, buff);


			ioRTC_getFullTime(rtc, &fullTime);
			sprintf(buff, "%.2d:%.2d:%.2d  -  %.2d/%.2d/%.4d  - %.1d\n\r", fullTime.hour, fullTime.minute, fullTime.second,
																			fullTime.dayOfMonth, fullTime.month, fullTime.year,
																			fullTime.dayOfWeek);
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
