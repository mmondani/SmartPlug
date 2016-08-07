#ifndef RTC_TIME_T_R_H_
#define RTC_TIME_T_R_H_


/**
 * @brief      Estructura usada por la interfaz ioRTC y por las clases que la implementar.
 * @details    Permite manipular datos de fecha y hora.
 */
typedef struct
{
	uint32_t second;			///< Segundos.
	uint32_t minute;			///< Minutos.
	uint32_t hour;				///< Horas.
	uint32_t dayOfMonth;		///< Número de día del mes. Intervalo: 1 a 28/29/30/31
	uint32_t dayOfWeek;			///< Número de día de la semana. Intervalo: 0 a 6
	uint32_t dayOfYear;			///< Número de día del año. Intervalo: 1 a 365/366.
	uint32_t month;				///< Mes. Intervalo: 1 a 12.
	uint32_t year;				///< Año.
}rtc_time_t;



#endif
