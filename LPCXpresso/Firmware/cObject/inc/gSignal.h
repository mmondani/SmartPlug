#ifndef GSIGNAL_H_
#define GSIGNAL_H_

#include "cObject.h"


// ********************************************************************************
// Instanciación de gSignal
//
// void* signal = cObject_new(gSignal);
// ********************************************************************************
extern const void* gSignal;



// ********************************************************************************
// Métodos públicos de la clase gSignal : cObject

void* gSignal_deleteSignalChain (void* _this);
void* gSignal_copySignalChain (void** _this, void* _src);
// ********************************************************************************

#endif
