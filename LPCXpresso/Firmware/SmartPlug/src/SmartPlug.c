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

#include "taskSwitch.h"
#include "taskLeds.h"
#include "taskTimer.h"
#include "taskSmartPlug.h"




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


void ErrorHook(void)
{
	/* kernel panic :( */
	ShutdownOS(0);
}

TASK(TaskInit)
{
	taskTimer_init();
	taskSwitch_init();
	taskLeds_init();
	taskRTC_init();
	taskSmartPlug_init(0);



	TerminateTask();
}
