#ifndef MEMALLOC_H_
#define MEMALLOC_H_

#include <stdint.h>

#define MEM_HEAP_SIZE		1024		// Dimensión del Heap en bytes


// ********************************************************************************
// Funciones públicas de memAlloc
// ********************************************************************************

/*
 * initMemHeap
 *
 * Inicializa el heap. DEBE ser llamada antes de llamar a memAlloc
 *
 */
void initMemHeap (void);


/*
 * memAlloc
 *
 * Aloca la cantidad de memoria requiredSize en el heap.
 *
 * 	-> requiredSize: cantidad de bytes a alocar
 *
 * 	<- Puntero al área de memoria alocada.
 */
void* memAlloc (uint32_t requiredSize);


/*
 * memFree
 *
 * Libera un área de memoria alocada con memAlloc
 *
 * 	-> pMem: puntero al ára de memoria alocada por memAlloc.
 *
 */
void memFree (void* pMem);


// ********************************************************************************

#endif
