/**
 * @file taskSwitch.c
 * @author Mariano Mondani
 *
 * @brief Implementación de la tarea taskSwitch
 */


#include "taskSwitch.h"
#include "chip.h"
#include "os.h"
#include "cObject.h"
#include "memAlloc.h"
#include "ioDigital.h"
#include "ioDebounce.h"



// Instancias de los objectos
static void* pinSwitch;
static void* debounceSwitch;

static uint32_t counter = 0;		// Usado para contar cuánto tiempo está presionada la tecla.
static uint32 pressed_5sec;			// Si está en 1 indica que se presionó la tecla por más de 5 segundos.



void taskSwitch_init (void)
{
	// Se instancia el objeto que controla al pin P0[3] conectado al tact-swtich.
	pinSwitch = cObject_new(ioDigital, LPC_GPIO, IOGPIO_INPUT, 0, 3);
	ioObject_init(pinSwitch);

	// Se instancia el objeto que realiza el anti-rebote de pinSwitch.
	// Se configura como una entrada alto-activa.
	// Se configura un tiempo de debounce de 200 ms (20 x 10 ms), ya que se pasa como debounceCount 10
	// y el handler se va a llamar cada 20 ms.
	debounceSwitch = cObject_new(ioDebounce, pinSwitch, IODIGITAL_LEVEL_HIGH, 10);


	// Se va a llamar a taskSwitch cada 20 ms
	SetRelAlarm(alarmRunTaskSwitch, 10, 20);
}


TASK(taskSwitch)
{
	// Se ejecuta el handler de la lógica del anti-rebote
	ioDebounce_handler(debounceSwitch);

	// Se produjo un flanco positivo?
	if (ioDebounce_getActiveEdge(debounceSwitch))
	{
		counter = 0;
		pressed_5sec = 0;
	}


	// Se produjo un flanco negativo?
	if (ioDebounce_getPassiveEdge(debounceSwitch))
	{
		// Si no se mantuvo presionada por más de 5 segundos la tecla, se genera el evento evSwitch
		if (pressed_5sec == 0)
			SetEvent(taskSmartPlug, evSwitch);
	}
	else
	{
		// Si no se produjo un flanco negativo y está presionado, se incrementa el counter.
		if (ioDebounce_isActive(debounceSwitch))
		{
			// Si counter llega a 250 (equivale a 5 segundo, ya que el task se llama cada 20 ms)
			// Se genera el evento evSwitch_5sec y se pone en 1 la variable pressed_5sec, para
			// no volver a generar el evento.
			counter++;
			if ( (counter >= 250) && (pressed_5sec == 0) )
			{
				SetEvent(taskSmartPlug, evSwitch_5sec);
				pressed_5sec = 1;
			}
		}
	}


	TerminateTask();
}
