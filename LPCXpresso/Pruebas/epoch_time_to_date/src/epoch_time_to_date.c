/*
===============================================================================
 Name        : epoch_time_to_date.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/


#include "chip.h"
#include <time.h>
#include <stdlib.h>


#include <cr_section_macros.h>




int main(void)
{
	uint8_t timeString[] = "1452986281";
	uint32_t epochTime = 0;
	struct tm* convertedTime;

    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();

    epochTime = atoi(timeString);

    /* Aclaraciones:
     *  Día: 1 a 28/30/31
     *  Mes: 0 (Enero) a 11 (Diciembre)
     *  Año: respecto a 1900. Ejemplo: 2016 es 116 (1900 + 116 = 2016)
     *  Día de la semana: 0 (Domingo) a 6 (Sábado)
     *  Hora: formato 24 hs.
     */
    convertedTime = localtime(&epochTime);

    while (1);

    return 0 ;
}
