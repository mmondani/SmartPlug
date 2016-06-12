#ifndef GSIGNAL_R_H_
#define GSIGNAL_R_H_


#include "cObject_r.h"


struct gSignal
{
	const void* class;
	struct GSignal* next;
	void (*slot) (void);
};



#endif
