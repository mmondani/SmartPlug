/*
===============================================================================
 Name        : cObject_test_ioCS5490.c
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
#include "ioUART.h"
#include "ioCS5490.h"
#include "ioDigital.h"
#include "ioDebounce.h"
#include "cTimer.h"




void* uart1;
void* uart3;
void* cs5490;
void* gpioReset;
void* gpioDO;
void* gpioA;
void* gpioB;
void* pushA;
void* pushB;
void* timer;
void* refreshTimer;


enum {FSM_WAITING_PUSH = 0, FSM_OFFSET_CALIBRATION, FSM_WAITING_RETURN, FSM_GAIN_CALIBRATION, FSM_V_GAIN_CALIBRATION, FSM_I_GAIN_CALIBRATION};
uint32_t state = FSM_WAITING_PUSH;


void term_clear (void* uart);
void term_home (void* uart);




void SysTick_Handler(void)
{
	cTimer_handler();

	ioDebounce_handler(pushA);
	ioDebounce_handler(pushB);
}


void UART3_IRQHandler(void)
{
	if (ioComm_getInt(uart3, IOUART_INT_ID_TX))
	{
		ioUART_txHandler(uart3);
	}
	else if(ioComm_getInt(uart3, IOUART_INT_ID_RX))
	{
		ioUART_rxHandler(uart3);
	}
}

void UART1_IRQHandler(void)
{
	if (ioComm_getInt(uart1, IOUART_INT_ID_TX))
	{
		ioUART_txHandler(uart1);
	}
	else if(ioComm_getInt(uart1, IOUART_INT_ID_RX))
	{
		ioUART_rxHandler(uart1);
	}
}




int main(void)
{
	uint8_t buff[50];
	uint8_t evPushA = 0, evPushB = 0;
	uint32_t aux32;
	uint32_t temperatura, muestras, irms, vrms, potencia_activa, potencia_reactiva, potencia_aparente;
	uint32_t i_offset, v_offset, i_ganancia, v_ganancia, factor_potencia, escala, epsilon, settle;
	float conversion, irms_linea, vrms_linea, potencia_activa_linea, potencia_reactiva_linea, frecuencia_linea;
	float potencia_aparente_linea;


    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();



    initMemHeap();

    uart1 = cObject_new(ioUART, LPC_UART1, IOUART_BR_600, IOUART_DATA_8BIT, IOUART_PAR_NONE, IOUART_STOP_1BIT, IOUART_MODE_BLOCKING, 2, 20);
    ioObject_init(uart1);
    ioComm_intEnable(uart1, IOUART_INT_TX);
    ioComm_intEnable(uart1, IOUART_INT_RX);
    ioUART_configFIFO(uart1, IOUART_FIFO_LEVEL0);

    NVIC_SetPriority(UART1_IRQn, 1);
    NVIC_EnableIRQ(UART1_IRQn);


    uart3 = cObject_new(ioUART, LPC_UART3, IOUART_BR_115200, IOUART_DATA_8BIT, IOUART_PAR_NONE, IOUART_STOP_1BIT, IOUART_MODE_BLOCKING, 2, 10);
    ioObject_init(uart3);
    ioComm_intEnable(uart3, IOUART_INT_TX);
    ioComm_intEnable(uart3, IOUART_INT_RX);
    ioUART_configFIFO(uart3, IOUART_FIFO_LEVEL0);

    NVIC_SetPriority(UART3_IRQn, 1);
    NVIC_EnableIRQ(UART3_IRQn);


    gpioReset = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 2, 1);
    ioObject_init(gpioReset);

    gpioDO = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 2, 0);
    ioObject_init(gpioDO);

    gpioA = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 0, 3);
    ioObject_init(gpioA);
    pushA = cObject_new(ioDebounce, gpioA, IODIGITAL_LEVEL_HIGH, 40);

    gpioB = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 0, 27);
    ioObject_init(gpioB);
    pushB = cObject_new(ioDebounce, gpioB, IODIGITAL_LEVEL_HIGH, 40);


    SysTick_Config(SystemCoreClock / 1000);


    cs5490 = cObject_new (ioCS5490, uart1, gpioReset, gpioDO, 4000.0, 220.0, 5.0, 1.75, 5000.0, 8.0);
    ioCS5490_init(cs5490, 0, 0, 1, 1);




    timer = cObject_new(cTimer);
    refreshTimer = cObject_new(cTimer);

    ioCS5490_instructionWrite(cs5490, IOCS5490_INS_CONTINUOUS_CONV);

    cTimer_start(refreshTimer, 1000);

    while(1)
    {
    	if (ioDebounce_getActiveEdge(pushA))
    		evPushA = 1;

    	if (ioDebounce_getActiveEdge(pushB))
    		evPushB = 1;
    	//-----------------------------------------------------------------

    	switch (state)
    	{
    		case FSM_WAITING_PUSH:
    			if (evPushA)
    			{
    				term_clear(uart3);
    				term_home(uart3);
    				sprintf(buff, "CALIBRACION OFFSET DC\n\r");
					ioUART_writeString(uart3, buff);

    				state = FSM_OFFSET_CALIBRATION;
    			}
    			else if (evPushB)
    			{
    				term_clear(uart3);
    				term_home(uart3);

    				sprintf(buff, "CALIBRACION GANANCIA\n\r");
					ioUART_writeString(uart3, buff);
    				sprintf(buff, "Push A -> Calibracion V\n\r");
					ioUART_writeString(uart3, buff);
					sprintf(buff, "Push B -> Calibracion I");
					ioUART_writeString(uart3, buff);

    				state = FSM_GAIN_CALIBRATION;
    			}
    			else
    			{
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


						term_clear(uart3);
						term_home(uart3);

						conversion = ioCS5490_signedFract2Float(temperatura, 7, 16);
						sprintf(buff, "Temperatura: %f\n\r", conversion);
						ioUART_writeString(uart3, buff);
						sprintf(buff, "Muestras: %lX\n\r", muestras);
						ioUART_writeString(uart3, buff);

						conversion = ioCS5490_unsignedFract2Float(irms, 0, 24);
						irms_linea = conversion * ioCS5490_getIcalibration(cs5490) / ioCS5490_getMeterScale(cs5490);
						sprintf(buff, "IRMS: %f\t\t%f\n\r", conversion, irms_linea);
						ioUART_writeString(uart3, buff);

						conversion = ioCS5490_unsignedFract2Float(vrms, 0, 24);
						vrms_linea = conversion * ioCS5490_getVmax(cs5490) / 0.6;
						sprintf(buff, "VRMS: %f\t\t%f\n\r", conversion, vrms_linea);
						ioUART_writeString(uart3, buff);

						conversion = ioCS5490_signedFract2Float(potencia_activa, 0, 23);
						potencia_activa_linea = conversion * ioCS5490_getMaxPower(cs5490) / ioCS5490_getPowerScale(cs5490);
						sprintf(buff, "Potencia activa: %f\t\t%f\n\r", conversion, potencia_activa_linea);
						ioUART_writeString(uart3, buff);

						conversion = ioCS5490_signedFract2Float(potencia_reactiva, 0, 23);
						potencia_reactiva_linea = conversion * ioCS5490_getMaxPower(cs5490) / ioCS5490_getPowerScale(cs5490);
						sprintf(buff, "Potencia reactiva: %f\t\t%f\n\r", conversion, potencia_reactiva_linea);
						ioUART_writeString(uart3, buff);

						conversion = ioCS5490_signedFract2Float(potencia_aparente, 0, 23);
						potencia_aparente_linea = conversion * ioCS5490_getMaxPower(cs5490) / ioCS5490_getPowerScale(cs5490);
						sprintf(buff, "Potencia aparente: %f\t\t%f\n\r", conversion, potencia_aparente_linea);
						ioUART_writeString(uart3, buff);

						conversion = ioCS5490_signedFract2Float(factor_potencia, 0, 23);
						sprintf(buff, "Factor de potencia: %f\n\r", conversion);
						ioUART_writeString(uart3, buff);

						conversion = ioCS5490_signedFract2Float(epsilon, 0, 23);
						frecuencia_linea = conversion * ioCS5490_getWordRate(cs5490);
						sprintf(buff, "Frecuencia linea: %f\t\t%f\n\r", conversion, frecuencia_linea);
						ioUART_writeString(uart3, buff);

						conversion = ioCS5490_signedFract2Float(i_offset, 0, 23);
						sprintf(buff, "I DC Offset: %f\n\r", conversion);
						ioUART_writeString(uart3, buff);

						conversion = ioCS5490_signedFract2Float(v_offset, 0, 23);
						sprintf(buff, "V DC Offset: %f\n\r", conversion);
						ioUART_writeString(uart3, buff);

						conversion = ioCS5490_unsignedFract2Float(i_ganancia, 2, 22);
						sprintf(buff, "I Ganancia: %f\n\r", conversion);
						ioUART_writeString(uart3, buff);

						conversion = ioCS5490_unsignedFract2Float(v_ganancia, 2, 22);
						sprintf(buff, "V Ganancia: %f\n\r", conversion);
						ioUART_writeString(uart3, buff);
    				}
    			}
    			break;


    		case FSM_OFFSET_CALIBRATION:
    			ioCS5490_pageSelect(cs5490, IOCS5490_PAGE_16);
    			ioCS5490_registerWrite(cs5490, IOCS5490_REG_V_DCOFF, 0);
    			ioCS5490_registerWrite(cs5490, IOCS5490_REG_I_DCOFF, 0);
    			ioCS5490_registerWrite(cs5490, IOCS5490_REG_V_GAIN, 0x400000);
    			ioCS5490_registerWrite(cs5490, IOCS5490_REG_I_GAIN, 0x400000);

    			ioCS5490_instructionWrite(cs5490, IOCS5490_INS_CAL_DC_OFFSET_I);
    			cTimer_start(timer, 3000);
    			while (!cTimer_hasExpired(timer));

    			ioCS5490_instructionWrite(cs5490, IOCS5490_INS_CAL_DC_OFFSET_V);
				cTimer_start(timer, 3000);
				while (!cTimer_hasExpired(timer));

				aux32 = ioCS5490_registerRead(cs5490, IOCS5490_REG_I_DCOFF);
				sprintf(buff, "I OFFSET: 0x%lX\n\r", aux32);
				ioUART_writeString(uart3, buff);

				aux32 = ioCS5490_registerRead(cs5490, IOCS5490_REG_V_DCOFF);
				sprintf(buff, "V OFFSET: 0x%lX\n\r", aux32);
				ioUART_writeString(uart3, buff);

				sprintf(buff, "Presione Push A para volver.");
				ioUART_writeString(uart3, buff);


				state = FSM_WAITING_RETURN;
    			break;


    		case FSM_WAITING_RETURN:
    			if (evPushA || evPushB)
    			{
    				term_clear(uart3);
					term_home(uart3);

    				sprintf(buff, "Push A -> Calibracion Offset\n\r");
					ioUART_writeString(uart3, buff);

    				sprintf(buff, "Push B -> Calibracion Ganancia");
					ioUART_writeString(uart3, buff);

					ioCS5490_instructionWrite(cs5490, IOCS5490_INS_CONTINUOUS_CONV);

					state = FSM_WAITING_PUSH;
    			}
    			break;


    		case FSM_GAIN_CALIBRATION:
    			if (evPushA)
    			{
    				term_clear(uart3);
					term_home(uart3);

					sprintf(buff, "CALIBRACION GANANCIA V\n\r");
					ioUART_writeString(uart3, buff);

					state = FSM_V_GAIN_CALIBRATION;
    			}
    			else if (evPushB)
    			{
    				term_clear(uart3);
					term_home(uart3);

					sprintf(buff, "CALIBRACION GANANCIA I\n\r");
					ioUART_writeString(uart3, buff);

					state = FSM_I_GAIN_CALIBRATION;
    			}
    			break;


    		case FSM_V_GAIN_CALIBRATION:
    			ioCS5490_pageSelect(cs5490, IOCS5490_PAGE_16);
    			ioCS5490_registerWrite(cs5490, IOCS5490_REG_V_GAIN, 0x400000);

    			ioCS5490_instructionWrite(cs5490, IOCS5490_INS_CAL_GAIN_V);
    			cTimer_start(timer, 15000);
    			while (!cTimer_hasExpired(timer));

				aux32 = ioCS5490_registerRead(cs5490, IOCS5490_REG_V_GAIN);
				sprintf(buff, "V GAIN: 0x%lX\n\r", aux32);
				ioUART_writeString(uart3, buff);

				sprintf(buff, "Push B para volver.");
				ioUART_writeString(uart3, buff);

				state = FSM_WAITING_RETURN;
    			break;


    		case FSM_I_GAIN_CALIBRATION:
    			ioCS5490_pageSelect(cs5490, IOCS5490_PAGE_16);
    			ioCS5490_registerWrite(cs5490, IOCS5490_REG_V_GAIN, 0x400000);

    			ioCS5490_instructionWrite(cs5490, IOCS5490_INS_CAL_GAIN_I);
    			cTimer_start(timer, 15000);
    			while (!cTimer_hasExpired(timer));

				aux32 = ioCS5490_registerRead(cs5490, IOCS5490_REG_I_GAIN);
				sprintf(buff, "I GAIN: 0x%lX\n\r", aux32);
				ioUART_writeString(uart3, buff);

				sprintf(buff, "Push B para volver.");
				ioUART_writeString(uart3, buff);

				state = FSM_WAITING_RETURN;
    			break;
    	}

    	//-----------------------------------------------------------------
    	evPushA = 0;
    	evPushB = 0;
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
