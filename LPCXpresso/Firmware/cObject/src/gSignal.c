#include "gSignal.h"
#include "gSignal_r.h"
#include <stdio.h>



static void* gSignal_ctor  (void* _this, va_list* va);
static void* gSignal_dtor (void* _this);
static uint32_t gSignal_differ (void* _this, void* _dst);
static void gSignal_display (void* _this);
static void* gSignal_copy (void* _this, void* _src);


// ********************************************************************************
// Declaración de la clase gColor
// ********************************************************************************
static const struct cObject _gSignal = {sizeof(struct gSignal),
											gSignal_ctor,
											gSignal_dtor,
											gSignal_differ,
											gSignal_display,
											gSignal_copy};

const void* gSignal = &_gSignal;
// ********************************************************************************



static void* gSignal_ctor  (void* _this, va_list* va)
{
	struct gSignal* this = _this;

	this->next = 0;
	this->slot = 0;

	return this;
}


static void* gSignal_dtor (void* _this)
{
	// No hay que liberar memoria alocada

	return _this;
}


static uint32_t gSignal_differ (void* _this, void* _dst)
{
	struct gSignal* this = _this;
	struct gSignal* dst = _dst;


	return ((this->next != dst->next) || (this->slot != dst->slot));
}


static void gSignal_display (void* _this)
{

}


static void* gSignal_copy (void* _this, void* _src)
{
	struct gSignal* this = _this;
	struct gSignal* src = _src;

	this->next = src->next;
	this->slot = src->slot;


	return this;

	// Si hubiera otros elementos que copiar se copian.
}




// ********************************************************************************
// Métodos linkeados estáticamente a la clase gSignal
// ********************************************************************************

void* gSignal_deleteSignalChain (void* _this)
{
	struct gSignal* this = _this;


	if (this)
	{
		this = this->next;
		gSignal_deleteSignalChain (this);

		cObject_delete(this);
	}

	return this;
}


void* gSignal_copySignalChain (void** _this, void* _src)
{
	struct gSignal** this = _this;
	struct gSignal* src = _src;

	while (src->next != 0)
	{
		(*this) = cObject_new(gSignal);
		(*this)->slot = src->slot;
		this = &((*this)->next);
	}

	(*this) = cObject_new(gSignal);
	(*this)->slot = src->slot;

	return *this;
}
