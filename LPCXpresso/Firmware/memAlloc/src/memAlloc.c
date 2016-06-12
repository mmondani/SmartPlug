#include "memAlloc.h"


static union
{
	volatile unsigned long Dummy;		// Fuerza la alineación de 4 bytes
	unsigned char heap[ MEM_HEAP_SIZE ];
} MemHeap;


//static uint32_t nextFreeByte = 0;



// ********************************************************************************
// Funciones privadas de memAlloc
// ********************************************************************************

uint16_t blockMerge(uint16_t* blockHeader);

// ********************************************************************************


// ********************************************************************************
// Macros
// ********************************************************************************
#define getBlockLength(header)			(header & 0x7FFF)
#define blockIsAlllocated(header) 		((header & 0x8000) != 0)
#define allocateBlock(header)			(header |= 0x8000)
#define deallocateBlock(header)			(header &= ~0x8000)
// ********************************************************************************



void initMemHeap (void)
{
	uint16_t* p = (uint16_t*)(MemHeap.heap);
	uint16_t size = MEM_HEAP_SIZE;

	// Se inicia el primer header con la totalidad del tamaño del heap
	*p = size;
}


void* memAlloc (uint32_t requiredSize)
{
	//void* pAlloc = (void*) 0;
	uint16_t* pHeader;
	uint16_t* temp_pHeader;
	uint16_t blockHeader, blockLength;

	if (requiredSize > MEM_HEAP_SIZE)
	{
		return ((void*)0);
	}


	pHeader = (uint16_t*)(MemHeap.heap);

	while (1)
	{
		// Obtengo el header del bloque
		blockHeader = *pHeader;

		// La longitud del bloque está en los 15 bits menos significativos del header
		blockLength = getBlockLength(blockHeader) - 2;

		// Si el header es 0x0000, indica el final del heap
		if (blockHeader == 0)
		{
			return ((void*)0);
		}

		// Si el bloque no está alocado, se lo intenta alocar.
		if (!blockIsAlllocated(blockHeader))
		{
			// Si el tamaño del bloque es demasiado pequeño, se intenta fusionar bloques
			if (requiredSize > blockLength)
			{

				if (!(blockMerge(pHeader)))
				{
					// Si no se pueden fusionar, se desplaza el puntero al siguiente header
					// Se castea a (uint8_t*) para que la aritmética de punteros coincida con los bytes
					pHeader = (uint8_t*)pHeader + getBlockLength(blockHeader);
				}
				else
				{
					// Actualiza la longitud del bloque fusionado
					blockLength = getBlockLength(*pHeader) - 2;
				}
			}
			//else

			// Si la longitud del bloque es igual a la requerida, entonces se crea
			// el header y se devuelve el puntero.
			if (requiredSize == blockLength)
			{
				// Se indica que el bloque está alocado.
				allocateBlock(*pHeader);

				// Retorna el puntero.
				return ((void*)((uint8_t*)pHeader + 2));
			}

			// En caso contrario, se crea un nuevo bloque
			else
			{
				// Se guarda la longitud del segmento
				*pHeader = requiredSize + 2;
				allocateBlock(*pHeader);

				// Se guarda el puntero a devolver
				temp_pHeader = (uint8_t*)pHeader + 2;

				// Se apunta al bloque nuevo
				pHeader = (uint8_t*)pHeader + (requiredSize + 2);

				// Inserta el header del nuevo bloque
				*pHeader = blockLength - requiredSize;

				// Retorna el puntero
				return ((void*)temp_pHeader);
			}
		}
		else
		{
			// En caso contrario, se apunta al siguiente bloque
			pHeader = (uint8_t*)pHeader + getBlockLength(blockHeader);
		}
	}
}



void memFree (void* pMem)
{
	//uint16_t* pHeader = pMem - 2;
	uint16_t* pHeader = (uint8_t*)pMem - 2;

	// Se libera el bloque
	deallocateBlock((*pHeader));
}



uint16_t blockMerge(uint16_t* pHeaderA)
{
	uint16_t* pHeaderB;
	uint16_t blockHeaderA, blockHeaderB, blockHeaderRet = 0;


	// Puntero al siguiente header
	pHeaderB = (uint8_t*)pHeaderA + getBlockLength(*pHeaderA);


	// Se obtienen los dos headers
	blockHeaderA = *pHeaderA;
	blockHeaderB = *pHeaderB;

	// Se evalua si se llegó al final del heap
	if (blockHeaderB == 0)
	{
		return (0);
	}

	// Si alguno de los bloques está alocado, no se fusionan
	if (blockIsAlllocated(blockHeaderA) || blockIsAlllocated(blockHeaderB))
	{
		return (0);
	}


	// Suma de los dos bloques
	blockHeaderRet = getBlockLength(blockHeaderA) + getBlockLength(blockHeaderB);


	// Se combinan los dos bloques en uno solo
	return ((*pHeaderA) = blockHeaderRet);
}
