/*
===============================================================================
 Name        : cObject_test_ioDigital.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/


#include "chip.h"

#include <cr_section_macros.h>
#include <stdint.h>
#include "memAlloc.h"
#include "cObject.h"
#include "ioDigital.h"
#include "ioSPI.h"
#include "ioEE25LCxxx.h"

void* pinSSelEE;
void* spiPort1;
void* ee25LC256;
void* pinLed;


void SysTick_Handler(void)
{
	ioDigital_toggle(pinLed);
}

int main(void)
{
	uint8_t byteWrite, byteRead;
	uint32_t intWrite, intRead;
	uint32_t i;


    SystemCoreClockUpdate();


    initMemHeap();

    pinLed = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 0, 22);
    ioObject_init(pinLed);

    pinSSelEE = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 0, 5);
    ioObject_init(pinSSelEE);

    spiPort1 = cObject_new(ioSPI, LPC_SSP1, 1000000, IOSPI_DATA_8BITS, IOSPI_CLOCKMODE_MODE0, IOSPI_SPIMODE_MASTER);
    ioObject_init(spiPort1);

    ee25LC256 = cObject_new(ioEE25LCxxx, spiPort1, pinSSelEE, IOEE25LCXXX_SIZE_256K);
    ioEE25LCxxx_setWriteEnable(ee25LC256);
    ioEE25LCxxx_writeStatus(ee25LC256, 0x00);

    for (i = 0; i < 100000; i++);



    SysTick_Config(SystemCoreClock / 20);



    byteWrite = 55;
    ioEE25LCxxx_setWriteEnable(ee25LC256);
    ioEE25LCxxx_writeData(ee25LC256, 0x10, (uint8_t*)&byteWrite, sizeof(uint8_t));
    ioEE25LCxxx_busyPolling(ee25LC256);
    ioEE25LCxxx_readData(ee25LC256, 0x10, (uint8_t*)&byteRead, sizeof(uint8_t));


    intWrite = 112147;
    ioEE25LCxxx_setWriteEnable(ee25LC256);
	ioEE25LCxxx_writeData(ee25LC256, 0x05, (uint8_t*)&intWrite, sizeof(uint32_t));
	ioEE25LCxxx_busyPolling(ee25LC256);
	ioEE25LCxxx_readData(ee25LC256, 0x05, (uint8_t*)&intRead, sizeof(uint32_t));



    while(1)
    {
    }


    return 0 ;
}
