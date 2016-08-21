/**
 * @file taskLeds.c
 * @author Mariano Mondani
 *
 * @brief Implementación de la tarea taskTimer
 */


#include "taskTimer.h"
#include "os.h"
#include "chip.h"
#include "cObject.h"
#include "memAlloc.h"
#include "cTimer.h"




void taskTimer_init (void)
{
	SetRelAlarm(alarmRunTaskTimer, 1, 1);
}


TASK (taskTimer)
{
	/*
	cTimer_handler();

*/
	TerminateTask();
}
