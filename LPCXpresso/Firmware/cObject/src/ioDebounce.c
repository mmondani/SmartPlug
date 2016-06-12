/**
 * @file ioDebounce.c
 * @author Mariano Mondani
 *
 * @brief Implementación de los métodos de la clase ioDebounce.
 */

#include "ioDebounce.h"
#include "ioDebounce_r.h"


static void* ioDebounce_ctor  (void* _this, va_list* va);
static void* ioDebounce_dtor (void* _this);
static uint32_t ioDebounce_differ (void* _this, void* _dst);
static void ioDebounce_display (void* _this);
static void* ioDebounce_copy (void* _this, void* _src);




// ********************************************************************************
// Declaración de la clase ioDebounce
// ********************************************************************************
static const struct cObject _ioDebounce = {sizeof(struct ioDebounce),
												ioDebounce_ctor,
												ioDebounce_dtor,
												ioDebounce_differ,
												ioDebounce_display,
												ioDebounce_copy};

const void* ioDebounce = &_ioDebounce;
// ********************************************************************************




static void* ioDebounce_ctor  (void* _this, va_list* va)
{
	struct ioDebounce* this = _this;

	this->inputPin = va_arg(*va, void*);
	this->activeLevel = va_arg(*va, uint32_t);
	this->debounceCount = va_arg(*va, uint32_t);

	// EL nivel actual se considera que no es el nivel actual.
	this->currentLevel = (this->activeLevel == IODIGITAL_LEVEL_LOW)? IODIGITAL_LEVEL_HIGH : IODIGITAL_LEVEL_LOW;


	return this;
}


static void* ioDebounce_dtor (void* _this)
{
	// No tiene que liberar memoria alocada.
	// La instancia de ioDigital tiene que ser destruida por la aplicación.

	return _this;
}


static uint32_t ioDebounce_differ (void* _this, void* _dst)
{
	struct ioDebounce* this = _this;
	struct ioDebounce* dst = _dst;

	return ( (cObject_differ(inputPin(this), inputPin(dst))) || (activeLevel(this) != activeLevel(dst)) ||
			(debounceCount(this) != debounceCount(dst)) );
}


static void ioDebounce_display (void* _this)
{

}


static void* ioDebounce_copy (void* _this, void* _src)
{
	struct ioDebounce* this = _this;
	struct ioDebounce* src = _src;

	cObject_copy(inputPin(this), inputPin(src));
	this->activeLevel = activeLevel(src);
	this->debounceCount = debounceCount(src);


	return this;
}



void ioDebounce_handler (void* _this)
{
	struct ioDebounce* this = _this;

	if (ioObject_read(inputPin(this)) == activeLevel(this))
	{
		if (this->count < this->debounceCount)
			this->count ++;
	}
	else
	{
		if(this->count > 0)
			this->count --;
	}



	if (this->count >= this->debounceCount)
	{
		if (!ioDebounce_isActive(this))
			this->edgeDetected = 1;

		this->currentLevel = this->activeLevel;
	}
	else if (this->count == 0)
	{
		if (ioDebounce_isActive(this))
			this->edgeDetected = 2;

		this->currentLevel = (this->activeLevel == IODIGITAL_LEVEL_LOW)? IODIGITAL_LEVEL_HIGH : IODIGITAL_LEVEL_LOW;
	}
}


uint32_t ioDebounce_isActive (void* _this)
{
	return ( currentLevel(_this) == activeLevel(_this) );
}


uint32_t ioDebounce_getActiveEdge (void* _this)
{
	struct ioDebounce* this = _this;
	uint32_t ret = 0;

	if( (edgeDetected(this) == 1) )
	{
		ret = 1;
		this->edgeDetected = 0;
	}

	return ret;
}


uint32_t ioDebounce_getPasiveEdge (void* _this)
{
	struct ioDebounce* this = _this;
	uint32_t ret = 0;

	if( (edgeDetected(this) == 2) )
	{
		ret = 1;
		this->edgeDetected = 0;
	}

	return ret;
}


