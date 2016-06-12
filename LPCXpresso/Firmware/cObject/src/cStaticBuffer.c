/**
 * @file cStaticBuffer.c
 *
 * @brief Implementación de los métodos de la clase cStaticBuffer.
 */


#include "cStaticBuffer.h"
#include "cStaticBuffer_r.h"



static void* cStaticBuffer_ctor  (void* _this, va_list* va);
static void* cStaticBuffer_dtor (void* _this);
static uint32_t cStaticBuffer_differ (void* _this, void* _dst);
static void cStaticBuffer_display (void* _this);
static void* cStaticBuffer_copy (void* _this, void* _src);
static void* cStaticBuffer_put (void* _this, void* _object);
static void* cStaticBuffer_remove (void* _this, void* _object);
static void* cStaticBuffer_clear (void* _this);
static void cStaticBuffer_at (void* _this, uint32_t pos, void* _object);
static uint32_t cStaticBuffer_getFreeSpace (void* _this);
static uint32_t cStaticBuffer_getPending (void* _this);
static uint32_t cStaticBuffer_length (void* _this);


// ********************************************************************************
// Declaración de la clase cStaticBuffer
// ********************************************************************************
static const struct cBuffer _cStaticBuffer = {{sizeof(struct cStaticBuffer),
													cStaticBuffer_ctor,
													cStaticBuffer_dtor,
													cStaticBuffer_differ,
													cStaticBuffer_display,
													cStaticBuffer_copy},
													cStaticBuffer_put,
													cStaticBuffer_remove,
													cStaticBuffer_clear,
													cStaticBuffer_at,
													cStaticBuffer_getFreeSpace,
													cStaticBuffer_getPending,
													cStaticBuffer_length};


const void* cStaticBuffer = &_cStaticBuffer;
// ********************************************************************************



static void* cStaticBuffer_ctor  (void* _this, va_list* va)
{
	struct cStaticBuffer* this = _this;

	this->len = va_arg(*va, uint32_t);
	this->objectSize = va_arg(*va, size_t);

	this->buffer = memAlloc(this->len * this->objectSize);
	cStaticBuffer_clear(this);

	return this;
}


static void* cStaticBuffer_dtor (void* _this)
{
	struct cStaticBuffer* this = _this;

	memFree(this->buffer);
	this->buffer = 0;
	this->len = 0;
	this->objectSize = 0;

	return this;
}


static uint32_t cStaticBuffer_differ (void* _this, void* _dst)
{
	struct cStaticBuffer* this = _this;
	struct cStaticBuffer* dst = _dst;
	int32_t res = 1;


	if ( (this->len == dst->len) && (this->objectSize == dst->objectSize ))
	{
		res = memcmp(this, dst, this->len * this->objectSize);
	}

	if (res != 0)
		return 1;
	else
		return 0;
}


static void cStaticBuffer_display (void* _this)
{

}


static void* cStaticBuffer_copy (void* _this, void* _src)
{
	struct cStaticBuffer* this = _this;
	struct cStaticBuffer* src = _src;

	assert((this->len == src->len) && (this->objectSize == src->objectSize));


	memcpy(this->buffer, src->buffer, src->len * src->objectSize);


	return this;
}


static void* cStaticBuffer_put (void* _this, void* _object)
{
	return ((void*)0);
}


static void* cStaticBuffer_remove (void* _this, void* _object)
{
	return ((void*)0);
}


static void* cStaticBuffer_clear (void* _this)
{
	struct cStaticBuffer* this = _this;


	memset(this->buffer, 0, this->len * this->objectSize);


	return this;
}


static void cStaticBuffer_at (void* _this, uint32_t pos, void* _object)
{

}


static uint32_t cStaticBuffer_getFreeSpace (void* _this)
{
	return len(_this);
}


static uint32_t cStaticBuffer_getPending (void* _this)
{
	return (len(_this));
}


static uint32_t cStaticBuffer_length (void* _this)
{
	return (len(_this));
}
