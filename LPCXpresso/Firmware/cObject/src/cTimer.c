/**
 * @file cTimer.c
 * @author Mariano Mondani
 *
 * @brief Implementación de los métodos de la clase cTimer.
 */

#include "cTimer.h"
#include "cTimer_r.h"


static void* cTimer_ctor  (void* _this, va_list* va);
static void* cTimer_dtor (void* _this);
static uint32_t cTimer_differ (void* _this, void* _dst);
static void cTimer_display (void* _this);
static void* cTimer_copy (void* _this, void* _src);




// ********************************************************************************
// Declaración de la clase cTimer
// ********************************************************************************
static const struct cObject _cTimer = {sizeof(struct cTimer),
												cTimer_ctor,
												cTimer_dtor,
												cTimer_differ,
												cTimer_display,
												cTimer_copy};

const void* cTimer = &_cTimer;
// ********************************************************************************


// ********************************************************************************
// Miembros estáticos de la clase cTimer
// ********************************************************************************
static uint32_t ticks = 0;
// ********************************************************************************



static void* cTimer_ctor  (void* _this, va_list* va)
{
	struct cTimer* this = _this;

	this->start = 0;
	this->interval = 0;
	this->running = 0;

	return this;
}


static void* cTimer_dtor (void* _this)
{
	// No tiene que liberar memoria alocada.

	return _this;
}


static uint32_t cTimer_differ (void* _this, void* _dst)
{
	struct cTimer* this = _this;
	struct cTimer* dst = _dst;

	return ( (interval(this) != interval(dst)) || (start(this) != start(dst)) || (running(this) != running(dst)));
}


static void cTimer_display (void* _this)
{

}


static void* cTimer_copy (void* _this, void* _src)
{
	struct cTimer* this = _this;
	struct cTimer* src = _src;

	this->start = src->start;
	this->interval = src->interval;
	this->running = src->running;


	return this;
}


void cTimer_handler (void)
{
	ticks++;
}


void cTimer_start (void* _this, uint32_t interval)
{
	struct cTimer* this = _this;

	this->start = ticks;
	this->interval = interval;
	this->running = 1;
}


void cTimer_restart (void* _this)
{
	struct cTimer* this = _this;

	this->start = ticks;
	this->running = 1;
}


uint32_t cTimer_hasExpired (void* _this)
{
	struct cTimer* this = _this;

	if(this->running)
		return ( (ticks - this->start) > this->interval);
	else
		return 0;
}


void cTimer_stop (void* _this)
{
	struct cTimer* this = _this;

	this->running = 0;
}


uint32_t cTimer_isRunning (void* _this)
{
	struct cTimer* this = _this;

	return (this->running);
}


