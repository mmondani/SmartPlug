#ifndef CMATRIX_R_H_
#define CMATRIX_R_H_

#include "cNumeric_r.h"

struct cMatrix
{
	const void* class;
	uint32_t row, col;
	float* matrix;
};

#endif
