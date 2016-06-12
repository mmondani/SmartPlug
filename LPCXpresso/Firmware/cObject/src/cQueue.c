/**
 * @file cQueue.c
 *
 * @brief Implementación de los métodos de la clase cQueue.
 */


#include <cQueue.h>
#include <cQueue_r.h>
#include <stdio.h>



static void* cQueue_ctor  (void* _this, va_list* va);
static void* cQueue_dtor (void* _this);
static uint32_t cQueue_differ (void* _this, void* _dst);
static void cQueue_display (void* _this);
static void* cQueue_copy (void* _this, void* _src);
static void* cQueue_put (void* _this, void* _object);
static void* cQueue_remove (void* _this, void* _object);
static void* cQueue_clear (void* _this);
static void cQueue_at (void* _this, uint32_t pos, void* _object);
static uint32_t cQueue_getFreeSpace (void* _this);
static uint32_t cQueue_getPending (void* _this);
static uint32_t cQueue_length (void* _this);


// ********************************************************************************
// Declaración de la clase cQueue
// ********************************************************************************
static const struct cBuffer _cQueue = {{sizeof(struct cQueue),
											cQueue_ctor,
											cQueue_dtor,
											cQueue_differ,
											cQueue_display,
											cQueue_copy},
											cQueue_put,
											cQueue_remove,
											cQueue_clear,
											cQueue_at,
											cQueue_getFreeSpace,
											cQueue_getPending,
											cQueue_length};

const void* cQueue = &_cQueue;
// ********************************************************************************



static void* cQueue_ctor  (void* _this, va_list* va)
{
	// Constructor de la clase herada
	struct cQueue* this = ((const struct cObject*) cStaticBuffer)->ctor (_this, va);

	this->ptrIn = 0;
	this->ptrOut = 0;

	return this;
}


static void* cQueue_dtor (void* _this)
{
	// Destructor de la clase herada
	_this = ((const struct cObject*) cStaticBuffer)->dtor (_this);

	return _this;
}


static uint32_t cQueue_differ (void* _this, void* _dst)
{
	struct cQueue* this = _this;
	struct cQueue* dst = _dst;
	uint32_t res = 0;


	// Se llama a la implementación de differ de la clase superior
	res = ((const struct cObject*) cStaticBuffer)->differ (_this, _dst);

	return res;
}


static void cQueue_display (void* _this)
{

}


static void* cQueue_copy (void* _this, void* _src)
{
	struct cQueue* this = _this;
	struct cQueue* src = _src;

	// Se llama a la implementación de copy de la clase superior
	this = ((const struct cObject*) cStaticBuffer)->copy (_this, _src);

	// Si hubiera otros elementos que copiar se copian.
	this->ptrIn = src->ptrIn;
	this->ptrOut = src->ptrOut;

	return this;
}


static void* cQueue_put (void* _this, void* _object)
{
	struct cQueue* this = _this;

	assert(len(this) > 0);

	if (cQueue_getFreeSpace(_this) > 0)
	{
		memcpy(buffer(this) + this->ptrIn * objectSize(this), _object, objectSize(this));

		this->ptrIn ++;
		if (this->ptrIn >= len(this))
			this->ptrIn = 0;
	}

	return this;
}


static void* cQueue_remove (void* _this, void* _object)
{
	struct cQueue* this = _this;

	assert(len(this) > 0);

	if (cQueue_getPending(this) > 0)
	{
		memcpy(_object, buffer(this) + this->ptrOut * objectSize(this), objectSize(this));

		this->ptrOut ++;
		if (this->ptrOut >= len(this))
			this->ptrOut = 0;
	}

	return this;
}


static void* cQueue_clear (void* _this)
{
	// Se llama a la implementación de clear de la clase que hereda
	return ((const struct cBuffer*) cStaticBuffer)->clear(_this);
}


static void cQueue_at (void* _this, uint32_t pos, void* _object)
{
	struct cQueue* this = _this;

	assert((pos >= 0) && ( pos < len(this)));

	memcpy(_object, (buffer(this) + objectSize(this) * pos), objectSize(this));
}


static uint32_t cQueue_getFreeSpace (void* _this)
{
	struct cQueue* this = _this;

	if (this->ptrOut <= this->ptrIn)
	{
		return (len(this) - (this->ptrIn - this->ptrOut));
	}
	else
	{
		return (this->ptrOut - this->ptrIn);
	}
}


static uint32_t cQueue_getPending (void* _this)
{
	struct cQueue* this = _this;

	if (this->ptrOut <= this->ptrIn)
	{
		return (this->ptrIn - this->ptrOut);
	}
	else
	{
		return (len(this) - (this->ptrOut - this->ptrIn));
	}
}


static uint32_t cQueue_length (void* _this)
{
	// Se llama a la implementación de la clase que herada
	return ((const struct cBuffer*) cStaticBuffer)->length (_this);
}
