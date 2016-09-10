/*
===============================================================================
 Name        : SmartPlug.c
 Author      : Mariano Mondani
 Version     :
 Copyright   : (c)2016, Mariano Mondani
 Description :
===============================================================================
*/


#include "chip.h"


#include <stdio.h>
#include <cr_section_macros.h>

#include "os.h"               /* <= operating system header */

#include "memAlloc.h"
#include "cObject.h"
#include "ioDigital.h"
#include "ioSPI.h"
#include "ioEE25LCxxx.h"

#include "taskSwitch.h"
#include "taskLeds.h"
#include "taskTimer.h"
#include "taskRTC.h"
#include "taskMeter.h"
#include "taskWiFi.h"
#include "taskSmartPlug.h"
#include "moduleLog.h"


void* pinSSelEE;
void* spiPort1;
void* ee25LC256;


int main(void)
{
	// Lee las configuraciones del clock y actualiza la variable SystemCoreClock
	SystemCoreClockUpdate();


	initMemHeap();


	// Comienza el RTOS en el modo AppMode1
    StartOS(AppMode1);

    // No debe llegar hasta acá
    while(1);


    return 0 ;
}




TASK(TaskInit)
{
	// Se instancia el driver de la EEPROM que va a ser usado por varias tareas
	// Pinde Slave Select
	pinSSelEE = cObject_new(ioDigital, LPC_GPIO, IOGPIO_OUTPUT, 0, 5);
	ioObject_init(pinSSelEE);

	// Puerto SPI
    spiPort1 = cObject_new(ioSPI, LPC_SSP1, 1000000, IOSPI_DATA_8BITS, IOSPI_CLOCKMODE_MODE0, IOSPI_SPIMODE_MASTER);
    ioObject_init(spiPort1);

    // Driver de las memorias EEPROM 25LCxxx
    ee25LC256 = cObject_new(ioEE25LCxxx, spiPort1, pinSSelEE, IOEE25LCXXX_SIZE_256K);

    ioEE25LCxxx_setWriteEnable(ee25LC256);
    ioEE25LCxxx_writeStatus(ee25LC256, 0x00);


    // Se inician las tareas
	moduleLog_init();
	taskTimer_init();
	taskSwitch_init();
	taskLeds_init();
	taskRTC_init();
	taskWiFi_init(ee25LC256);
	taskSmartPlug_init(ee25LC256);
	taskMeter_init(ee25LC256);



	TerminateTask();
}
