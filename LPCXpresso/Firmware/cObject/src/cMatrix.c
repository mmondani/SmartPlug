#include "cMatrix.h"
#include "cMatrix_r.h"
#include <stdio.h>


static void* cMatrix_ctor  (void* _this, va_list* va);
static void* cMatrix_dtor (void* _this);
static uint32_t cMatrix_differ (void* _this, void* _dst);
static void cMatrix_display (void* _this);
static void* cMatrix_copy (void* _this, void* _src);
static void* cMatrix_add (void* _this, void* _other, void* _result);
static void* cMatrix_subtract (void* _this, void* _other, void* _result);
static void* cMatrix_multiply (void* _this, void* _other, void* _result);
static void* cMatrix_divide (void* _this, void* _other, void* _result);
static float cMatrix_module (void* _this);



// ********************************************************************************
// Declaración de la clase cMatrix
// ********************************************************************************
static const struct cNumeric _cMatrix = {{sizeof(struct cMatrix),
											cMatrix_ctor,
											cMatrix_dtor,
											cMatrix_differ,
											cMatrix_display,
											cMatrix_copy},
											cMatrix_add,
											cMatrix_subtract,
											cMatrix_multiply,
											cMatrix_divide,
											cMatrix_module};

const void* cMatrix = &_cMatrix;
// ********************************************************************************



static void* cMatrix_ctor  (void* _this, va_list* va)
{
	struct cMatrix* this = _this;
	uint32_t i, j;
	float aux;

	this->row = va_arg(*va, uint32_t);
	this->col = va_arg(*va, uint32_t);

	this->matrix = memAlloc(sizeof (float) * this->col * this->row);

	for (i = 0; i < this->row; i++)
	{
		for (j = 0; j < this->col; j++)
		{
			aux = va_arg(*va, double);
			this->matrix[i * this->col + j] = aux;
		}
	}


	return this;
}


static void* cMatrix_dtor (void* _this)
{
	struct cMatrix* this = _this;

	memFree(this->matrix);
	this->matrix = 0;
	this->row = 0;
	this->col = 0;

	return this;
}


uint32_t cMatrix_differ (void* _this, void* _dst)
{
	struct cMatrix* this = _this;
	struct cMatrix* dst = _dst;
	uint32_t i, j;


	assert((this->row == dst->row) && (this->col == dst->col));

	for (i = 0; i < this->row; i++)
	{
		for (j = 0; j < this->col; j++)
		{
			if (this->matrix[i * this->col + j] != dst->matrix[i * dst->col + j])
			{
				return 1;
			}
		}
	}

	return 0;
}


static void cMatrix_display (void* _this)
{

	/*
	struct cMatrix* this = _this;
	uint32_t i, j;


	for (i = 0; i < this->row; i++)
	{
		for (j = 0; j < this->col; j++)
		{
			printf (" %f ", this->matrix[i * this->col + j]);
		}

		printf ("\n\r");
	}
	*/
}


static void* cMatrix_copy (void* _this, void* _src)
{
	struct cMatrix* this = _this;
	struct cMatrix* src = _src;
	uint32_t i, j;


	assert((this->row == src->row) && (this->col == src->col));

	for (i = 0; i < this->row; i++)
	{
		for (j = 0; j < this->col; j++)
		{
			this->matrix[i * this->col + j] = src->matrix[i * src->col + j];
		}
	}

	return 0;
}


static void* cMatrix_add (void* _this, void* _other, void* _result)
{
	struct cMatrix* this = _this;
	struct cMatrix* other = _other;
	struct cMatrix* result = _result;
	uint32_t i, j;


	assert((this->row == other->row) && (this->col == other->col) && (this->row == result->row) && (this->col == result->col));


	for (i = 0; i < this->row; i++)
	{
		for (j = 0; j < this->col; j++)
		{
			result->matrix[i * result->col + j] = this->matrix[i * this->col + j] + other->matrix[i * other->col + j];
		}
	}


	return result;
}


static void* cMatrix_subtract (void* _this, void* _other, void* _result)
{
	struct cMatrix* this = _this;
	struct cMatrix* other = _other;
	struct cMatrix* result = _result;
	uint32_t i, j;


	assert((this->row == other->row) && (this->col == other->col) && (this->row == result->row) && (this->col == result->col));


	for (i = 0; i < this->row; i++)
	{
		for (j = 0; j < this->col; j++)
		{
			result->matrix[i * result->col + j] = this->matrix[i * this->col + j] - other->matrix[i * other->col + j];
		}
	}


	return result;
}


static void* cMatrix_multiply (void* _this, void* _other, void* _result)
{
	struct cMatrix* this = _this;
	struct cMatrix* other = _other;
	struct cMatrix* result = _result;
	uint32_t i, j, k;


	assert((this->col == other->row) && (this->row == result->row) && (other->col == result->col));


	for (i = 0; i < this->row; i++)
	{
		for (j = 0; j < other->col; j++)
		{
			result->matrix[i * result->col + j] = 0.0;

			for (k = 0; k < this->col; k++)
			{
				result->matrix[i * result->col + j] += (this->matrix[i * this->col + k] * other->matrix[k * other->col + j]);
			}
		}
	}


	return result;
}


static void* cMatrix_divide (void* _this, void* _other, void* _result)
{
	return (void*) 0;
}


static float cMatrix_module (void* _this)
{
	struct cMatrix* this = _this;
	int32_t d, z, p, f, c;
	float pivot, aux, det = 1.0;
	struct cMatrix* matrixAux;

	// Se crea una copia auxiliar de la matriz
	matrixAux = cObject_new(cMatrix, this->row, this->col, 0.0);
	cObject_copy(matrixAux, this);

	z = 1;

	for (d = 0; d < matrixAux->col; d++)
	{
		p = 0;
		if (matrixAux->matrix[d * matrixAux->col + d] == 0.0)
		{
			p = -1;
			f = d;
			while (f <= matrixAux->row && p == -1)
			{
				if (matrixAux->matrix[f * matrixAux->col + d] != 0.0)
				{
					p = f;
					cMatrix_swapRows(matrixAux, d, p);
					z *= -1;
				}

				f += 1;
			}
		}

		pivot = matrixAux->matrix[d * matrixAux->col + d];

		if (p != -1)
		{
			for (f = d+1; f < matrixAux->row; f++)
			{
				aux = matrixAux->matrix[f * matrixAux->col + d];

				for (c = 0; c < matrixAux->col; c++)
				{
					matrixAux->matrix[f * matrixAux->col + c] = matrixAux->matrix[f * matrixAux->col + c] - (matrixAux->matrix[d * matrixAux->col + c] * (aux/pivot));
				}
			}
		}
	}

	//Obtiene el determinante de la diagonal
	for (d = 0; d < matrixAux->col; d++)
	{
		det *= matrixAux->matrix[d * matrixAux->col + d];
	}

	det *= z;


	// Se destruye la matriz auxiliar
	cObject_delete(matrixAux);

	return det;
}






// ********************************************************************************
// Métodos linkeados estáticamente a la clase cMatrix
// ********************************************************************************

void* cMatrix_invert (void* _this, void* _result)
{

	struct cMatrix* this = _this;
	struct cMatrix* result = _result;
	struct cMatrix* identity;
	float factor;
	uint32_t pivot, i, j;


	assert((this->row == this->col) && (this->row == result->row) && (this->col == result->col));

	// Se copia la matriz this en result.
	cMatrix_copy(result, this);

	// Se crea una matriz identidad de las mismas dimensiones que this
	identity = cMatrix_newIdentity(this->row);

	for(pivot = 0; pivot < result->row; pivot++)
	{
		factor = result->matrix[pivot * result->col + pivot];

		for(i = 0; i < result->row; i++)
		{
		   result->matrix[pivot * result->col + i] /= factor;
		   identity->matrix[pivot * identity->col + i] /= factor;
		}

		for(i = 0; i < result->row; i++)
		{
			if(i!=pivot)
			{
				factor = result->matrix[i * result->col + pivot];
				for(j = 0;j < result->row; j++)
				{
					result->matrix[i * result->col + j] = result->matrix[i * result->col + j] - result->matrix[pivot * result->col + j] * factor;
					identity->matrix[i * identity->col + j] = identity->matrix[i * identity->col + j] - identity->matrix[pivot * identity->col + j] * factor;
				}
			}
		}
	}


	// El resultado de la inversión está en la matriz identity. Se la copia a result
	cMatrix_copy(result, identity);

	// Se destruye identity
	cObject_delete(identity);


	return result;



	/*
	struct cMatrix* this = _this;
	struct cMatrix* result = _result;
	float det, aux;

	// Sólamente se implementa para matrices de 2x2.
	// QUEDA PENDIENTE LA GENERALIZACIÓN A NxN			!!!!!!!!!!!!!!!!!!!!!!!!
	assert((this->row == 2) && (this->col == 2));


	// Se copia la matriz this en result.
	cMatrix_copy(result, this);



	det = cMatrix_module(result);


	aux = cMatrix_getValue(result, 0, 0);
	cMatrix_setValue(result, 0, 0, cMatrix_getValue(result, 1, 1));
	cMatrix_setValue(result, 1, 1, aux);
	cMatrix_setValue(result, 0, 1, -1.0 * cMatrix_getValue(result, 0, 1));
	cMatrix_setValue(result, 1, 0, -1.0 * cMatrix_getValue(result, 1, 0));



	cMatrix_divideConstant(result, det, result);

	return result;
	*/
}


void* cMatrix_transpose (void* _this, void* _result)
{
	struct cMatrix* this = _this;
	struct cMatrix* result = _result;
	uint32_t i, j;


	assert((this->row == result->col) && (this->col == result->row));


	for (i = 0; i < this->row; i++)
	{
		for (j = 0; j < this->col; j++)
		{
			result->matrix[j * result->col + i] = this->matrix[i * this->col + j];
		}
	}


	return result;
}


void* cMatrix_newIdentity (uint32_t n)
{
	void* ptr;
	uint32_t i, j;


	ptr = cObject_new(cMatrix, n, n, 0.0);

	for (i = 0; i < n ; i++)
	{
		for (j = 0; j < n; j ++)
		{
			if (i == j)
			{
				cMatrix_setValue(ptr, i, j, 1.0);
			}
			else
			{
				cMatrix_setValue(ptr, i, j, 0.0);
			}
		}
	}


	return ptr;
}


void* cMatrix_swapRows (void* _this, uint32_t row1, uint32_t row2)
{
	struct cMatrix* this = _this;
	uint32_t j;
	float aux;

	assert ((row1 >= 0) && (row2 >= 0) && (row1 < this->row) && (row2 < this->row));


	if (row1 != row2)
	{
		/*// Conversión de la notación de matrices a la notación de arreglos
		row1--;
		row2--;*/

		for (j = 0; j < this->col; j++)
		{
			aux = this->matrix[row1 * this->col + j];
			this->matrix[row1 * this->col + j] = this->matrix[row2 * this->col + j];
			this->matrix[row2 * this->col + j] = aux;
		}
	}

	return this;
}


void* cMatrix_swapCols (void* _this, uint32_t col1, uint32_t col2)
{
	struct cMatrix* this = _this;
	uint32_t i;
	float aux;

	assert ((col1 >= 0) && (col2 >= 0) && (col1 < this->col) && (col2 < this->col));


	if (col1 != col2)
	{
		/*// Conversión de la notación de matrices a la notación de arreglos
		col1--;
		col2--;*/

		for (i = 0; i < this->row; i++)
		{
			aux = this->matrix[i * this->col + col1];
			this->matrix[i * this->col + col1] = this->matrix[i * this->col + col2];
			this->matrix[i * this->col + col2] = aux;
		}
	}

	return this;
}


void* cMatrix_setValue (void* _this, uint32_t row, uint32_t col, float value)
{
	struct cMatrix* this = _this;


	assert ((row >= 0) && (col >= 0) && (row < this->row) && (col < this->col));


	this->matrix[row * this->col + col] = value;

	return this;
}


void* cMatrix_setRow (void* _this, uint32_t row, ...)
{
	struct cMatrix* this = _this;
	uint32_t j;
	float aux;


	assert ((row >= 0) && (row < this->row));

	va_list va;

	va_start(va, row);

	for (j = 0; j < this->col; j++)
	{
		aux = va_arg(va, double);
		this->matrix[row * this->col + j] = aux;
	}

	va_end(va);

	return this;
}


void* cMatrix_setCol (void* _this, uint32_t col, ...)
{
	struct cMatrix* this = _this;
	uint32_t i;
	float aux;


	assert ((col >= 0) && (col < this->col));

	va_list va;

	va_start(va, col);

	for (i = 0; i < this->row; i++)
	{
		aux = va_arg(va, double);
		this->matrix[i * this->col + col] = aux;
	}

	va_end(va);

	return this;
}


float cMatrix_getValue (void* _this, uint32_t row, uint32_t col)
{
	struct cMatrix* this = _this;


	assert ((row >= 0) && (col >= 0) && (row < this->row) && (col < this->col));

	return (this->matrix[row * this->col + col]);
}


void* cMatrix_addConstant (void* _this, float constant, void* _result)
{
	struct cMatrix* this = _this;
	struct cMatrix* result = _result;
	uint32_t i, j;


	assert((this->row == result->row) && (this->col == result->col));


	for (i = 0; i < this->row; i++)
	{
		for (j = 0; j < this->col; j++)
		{
			result->matrix[i * result->col + j] = this->matrix[i * this->col + j] + constant;
		}
	}


	return result;
}


void* cMatrix_multiplyConstant (void* _this, float constant, void* _result)
{
	struct cMatrix* this = _this;
	struct cMatrix* result = _result;
	uint32_t i, j;


	assert((this->row == result->row) && (this->col == result->col));


	for (i = 0; i < this->row; i++)
	{
		for (j = 0; j < this->col; j++)
		{
			result->matrix[i * result->col + j] = this->matrix[i * this->col + j] * constant;
		}
	}


	return result;
}


void* cMatrix_divideConstant (void* _this, float constant, void* _result)
{
	struct cMatrix* this = _this;
	struct cMatrix* result = _result;
	uint32_t i, j;


	assert((this->row == result->row) && (this->col == result->col));


	for (i = 0; i < this->row; i++)
	{
		for (j = 0; j < this->col; j++)
		{
			result->matrix[i * result->col + j] = this->matrix[i * this->col + j] / constant;
		}
	}


	return result;
}
