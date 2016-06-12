#include "cNumeric.h"
#include "cNumeric_r.h"


void* cNumeric_add (void* _this, void* _other, void* _result)
{
	const struct cNumeric* const * class = _this;

	if (_this && class && (*class)->add)
	{
		_result = (*class)->add(_this, _other, _result);
	}

	return _result;
}


void* cNumeric_subtract (void* _this, void* _other, void* _result)
{
	const struct cNumeric* const * class = _this;

	if (_this && class && (*class)->subtract)
	{
		_result = (*class)->subtract(_this, _other, _result);
	}

	return _result;
}


void* cNumeric_multiply (void* _this, void* _other, void* _result)
{
	const struct cNumeric* const * class = _this;

	if (_this && class && (*class)->multiply)
	{
		_result = (*class)->multiply(_this, _other, _result);
	}

	return _result;
}


void* cNumeric_divide (void* _this, void* _other, void* _result)
{
	const struct cNumeric* const * class = _this;

	if (_this && class && (*class)->divide)
	{
		_result = (*class)->divide(_this, _other, _result);
	}

	return _result;
}


float cNumeric_module (void* _this)
{
	const struct cNumeric* const * class = _this;

	if (_this && class && (*class)->module)
	{
		return ((*class)->module(_this));
	}

	return 0.0;
}
