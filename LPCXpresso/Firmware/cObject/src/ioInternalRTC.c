/**
 * @file ioInternalRTC.c
 * @author Mariano Mondani
 *
 * @brief Implementación de los métodos de la clase ioInternalRTC.
 */

#include "ioInternalRTC.h"
#include "ioInternalRTC_r.h"


static void* ioInternalRTC_ctor  (void* _this, va_list* va);
static void* ioInternalRTC_dtor (void* _this);
static uint32_t ioInternalRTC_differ (void* _this, void* _dst);
static void ioInternalRTC_display (void* _this);
static void* ioInternalRTC_copy (void* _this, void* _src);
static uint32_t ioInternalRTC_init (void* _this);
static uint32_t ioInternalRTC_deInit (void* _this);
static uint32_t ioInternalRTC_enable (void* _this);
static uint32_t ioInternalRTC_disable (void* _this);
static uint32_t ioInternalRTC_read (void* _this);
static uint32_t ioInternalRTC_write (void* _this, uint32_t data);
static void ioInternalRTC_reset (void* _this);
static uint32_t ioInternalRTC_setFullTime (void* _this, rtc_time_t* time);
static uint32_t ioInternalRTC_getTimeFull (void* _this, rtc_time_t* time);
static uint32_t ioInternalRTC_setTime (void* _this, uint32_t element, uint32_t value);
static uint32_t ioInternalRTC_getTime (void* _this, uint32_t element);


// ********************************************************************************
// Declaración de la clase ioInternalRTC
// ********************************************************************************
static const struct ioRTC _ioInternalRTC = {{{sizeof(struct ioInternalRTC),
												ioInternalRTC_ctor,
												ioInternalRTC_dtor,
												ioInternalRTC_differ,
												ioInternalRTC_display,
												ioInternalRTC_copy},
												ioInternalRTC_init,
												ioInternalRTC_deInit,
												ioInternalRTC_enable,
												ioInternalRTC_disable,
												ioInternalRTC_read,
												ioInternalRTC_write},
												ioInternalRTC_reset,
												ioInternalRTC_setFullTime,
												ioInternalRTC_getTimeFull,
												ioInternalRTC_setTime,
												ioInternalRTC_getTime};

const void* ioInternalRTC = &_ioInternalRTC;
// ********************************************************************************



static void* ioInternalRTC_ctor  (void* _this, va_list* va)
{
	struct ioInternalRTC* this = _this;

	this->periphMem = va_arg(*va, void*);


	return this;
}


static void* ioInternalRTC_dtor (void* _this)
{
	// No tiene que liberar memoria alocada.

	return _this;
}


static uint32_t ioInternalRTC_differ (void* _this, void* _dst)
{
	struct ioInternalRTC* this = _this;
	struct ioInternalRTC* dst = _dst;

	return ((this->periphMem != dst->periphMem));
}


static void ioInternalRTC_display (void* _this)
{

}


static void* ioInternalRTC_copy (void* _this, void* _src)
{
	struct ioInternalRTC* this = _this;
	struct ioInternalRTC* src = _src;

	this->periphMem = src->periphMem;

	return this;
}


static uint32_t ioInternalRTC_init (void* _this)
{
	struct ioInternalRTC* this = _this;

	Chip_RTC_Init(periphMem(this));

	return 0;
}


static uint32_t ioInternalRTC_deInit (void* _this)
{
	struct ioInternalRTC* this = _this;

	Chip_RTC_DeInit(periphMem(this));

	return 0;
}


static uint32_t ioInternalRTC_enable (void* _this)
{
	struct ioInternalRTC* this = _this;

	Chip_RTC_Enable(periphMem(this), ENABLE);

	return 0;
}


static uint32_t ioInternalRTC_disable (void* _this)
{
	struct ioInternalRTC* this = _this;

	Chip_RTC_Enable(periphMem(this), DISABLE);

	return 0;
}


static uint32_t ioInternalRTC_read (void* _this)
{
	return 0;
}


static uint32_t ioInternalRTC_write (void* _this, uint32_t data)
{
	return 0;
}


static void ioInternalRTC_reset (void* _this)
{
	struct ioInternalRTC* this = _this;

	Chip_RTC_ResetClockTickCounter(periphMem(this));
}


static uint32_t ioInternalRTC_setFullTime (void* _this, rtc_time_t* time)
{
	struct ioInternalRTC* this = _this;
	RTC_TIME_T fullTime;

	fullTime.time[RTC_TIMETYPE_SECOND] = time->second;
	fullTime.time[RTC_TIMETYPE_MINUTE] = time->minute;
	fullTime.time[RTC_TIMETYPE_HOUR] = time->hour;
	fullTime.time[RTC_TIMETYPE_DAYOFMONTH] = time->dayOfMonth;
	fullTime.time[RTC_TIMETYPE_DAYOFWEEK] = time->dayOfWeek;
	fullTime.time[RTC_TIMETYPE_DAYOFYEAR] = time->dayOfYear;
	fullTime.time[RTC_TIMETYPE_MONTH] = time->month;
	fullTime.time[RTC_TIMETYPE_YEAR] = time->year;

	Chip_RTC_SetFullTime(periphMem(this), &fullTime);

	return 0;
}


static uint32_t ioInternalRTC_getTimeFull (void* _this, rtc_time_t* time)
{
	struct ioInternalRTC* this = _this;
	RTC_TIME_T fullTime;

	Chip_RTC_GetFullTime(periphMem(this), &fullTime);

	time->second = fullTime.time[RTC_TIMETYPE_SECOND];
	time->minute = fullTime.time[RTC_TIMETYPE_MINUTE];
	time->hour = fullTime.time[RTC_TIMETYPE_HOUR];
	time->dayOfMonth = fullTime.time[RTC_TIMETYPE_DAYOFMONTH];
	time->dayOfWeek = fullTime.time[RTC_TIMETYPE_DAYOFWEEK];
	time->dayOfYear = fullTime.time[RTC_TIMETYPE_DAYOFYEAR];
	time->month = fullTime.time[RTC_TIMETYPE_MONTH];
	time->year = fullTime.time[RTC_TIMETYPE_YEAR];

	return 0;
}


static uint32_t ioInternalRTC_setTime (void* _this, uint32_t element, uint32_t value)
{
	struct ioInternalRTC* this = _this;

	switch (element)
	{
		case RTC_TIME_SECOND:
			Chip_RTC_SetTime(periphMem(this), RTC_TIMETYPE_SECOND, value);
			break;

		case RTC_TIME_MINUTE:
			Chip_RTC_SetTime(periphMem(this), RTC_TIMETYPE_MINUTE, value);
			break;

		case RTC_TIME_HOUR:
			Chip_RTC_SetTime(periphMem(this), RTC_TIMETYPE_HOUR, value);
			break;

		case RTC_TIME_DAY_OF_MONTH:
			Chip_RTC_SetTime(periphMem(this), RTC_TIMETYPE_DAYOFMONTH, value);
			break;

		case RTC_TIME_DAY_OF_WEEK:
			Chip_RTC_SetTime(periphMem(this), RTC_TIMETYPE_DAYOFWEEK, value);
			break;

		case RTC_TIME_DAY_OF_YEAR:
			Chip_RTC_SetTime(periphMem(this), RTC_TIMETYPE_DAYOFYEAR, value);
			break;

		case RTC_TIME_MONTH:
			Chip_RTC_SetTime(periphMem(this), RTC_TIMETYPE_MONTH, value);
			break;

		case RTC_TIME_YEAR:
			Chip_RTC_SetTime(periphMem(this), RTC_TIMETYPE_YEAR, value);
			break;
	}

	return 0;
}


static uint32_t ioInternalRTC_getTime (void* _this, uint32_t element)
{
	struct ioInternalRTC* this = _this;
	uint32_t res = 0;

	switch (element)
	{
		case RTC_TIME_SECOND:
			res = Chip_RTC_GetTime(periphMem(this), RTC_TIMETYPE_SECOND);
			break;

		case RTC_TIME_MINUTE:
			res = Chip_RTC_GetTime(periphMem(this), RTC_TIMETYPE_MINUTE);
			break;

		case RTC_TIME_HOUR:
			res = Chip_RTC_GetTime(periphMem(this), RTC_TIMETYPE_HOUR);
			break;

		case RTC_TIME_DAY_OF_MONTH:
			res = Chip_RTC_GetTime(periphMem(this), RTC_TIMETYPE_DAYOFMONTH);
			break;

		case RTC_TIME_DAY_OF_WEEK:
			res = Chip_RTC_GetTime(periphMem(this), RTC_TIMETYPE_DAYOFWEEK);
			break;

		case RTC_TIME_DAY_OF_YEAR:
			res = Chip_RTC_GetTime(periphMem(this), RTC_TIMETYPE_DAYOFYEAR);
			break;

		case RTC_TIME_MONTH:
			res = Chip_RTC_GetTime(periphMem(this), RTC_TIMETYPE_MONTH);
			break;

		case RTC_TIME_YEAR:
			res = Chip_RTC_GetTime(periphMem(this), RTC_TIMETYPE_YEAR);
			break;
	}

	return res;
}
