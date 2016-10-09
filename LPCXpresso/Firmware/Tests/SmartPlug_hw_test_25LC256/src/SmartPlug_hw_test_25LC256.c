/**
 * @file SmartPlug_hw_test_25LC256.c
 *
 * @brief Prueba de hardware sobre la placa del Smart Plug en su versión 1.
 *
 * Prueba el funcionamiento de la memoria EEPROM 25LC256. Escribe un byte
 * y lo vuelve a leer. Si coinciden, informa OK por la UART1. Si no coinciden
 * informa ERROR.
 */


#include "chip.h"


#include <cr_section_macros.h>
#include <stdio.h>

#include "memAlloc.h"
#include "cObject.h"
#include "ioDigital.h"
#include "ioUART.h"
#include "cTimer.h"
#include "ioSPI.h"
#include "ioEE25LCxxx.h"

void* uartDebug;
void* pinSSelEE;
void* spiPort1;
void* ee25LC256;
void* writeEEPROMTimer;


void term_clear (void* uart);
void term_home (void* uart);




void SysTick_Handler(void)
{
	cTimer_handler();
}



int main(void) {
	uint8_t byteWrite, byteRead;

	SystemCoreClockUpdate();


    initMemHeap();



    // ===================== [25LC256] =====================
	pinSSelEE = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 0, 5);
	ioObject_init(pinSSelEE);

    spiPort1 = cObject_new(ioSPI, LPC_SSP1, 1000000, IOSPI_DATA_8BITS, IOSPI_CLOCKMODE_MODE0, IOSPI_SPIMODE_MASTER);
    ioObject_init(spiPort1);

    ee25LC256 = cObject_new(ioEE25LCxxx, spiPort1, pinSSelEE, IOEE25LCXXX_SIZE_256K);

    ioEE25LCxxx_setWriteEnable(ee25LC256);
    ioEE25LCxxx_writeStatus(ee25LC256, 0x00);

    writeEEPROMTimer = cObject_new(cTimer);
    cTimer_start(writeEEPROMTimer, 1000);
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



    while(1)
    {
    	if (cTimer_hasExpired(writeEEPROMTimer))
    	{
    		cTimer_stop(writeEEPROMTimer);

    		byteWrite = 112;
    		ioEE25LCxxx_busyPolling(ee25LC256);
			ioEE25LCxxx_setWriteEnable(ee25LC256);
			ioEE25LCxxx_writeData(ee25LC256, 0x05, &byteWrite, sizeof(uint8_t));
			ioEE25LCxxx_busyPolling(ee25LC256);
			ioEE25LCxxx_readData(ee25LC256, 0x05, &byteRead, sizeof(uint8_t));

			if (byteWrite == byteRead)
				ioUART_writeString(uartDebug, "OK");
			else
				ioUART_writeString(uartDebug, "ERROR");
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
