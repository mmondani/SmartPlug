#ifndef CNUMERIC_R_H_
#define CNUMERIC_R_H_

#include "cObject_r.h"


struct cNumeric
{
	struct cObject _;
	void* (*add) (void* _this, void* _other, void* _result);
	void* (*subtract) (void* _this, void* _other, void* _result);
	void* (*multiply) (void* _this, void* _other, void* _result);
	void* (*divide) (void* _this, void* _other, void* _result);
	float (*module) (void* _this);
};

#endif
