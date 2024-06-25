#ifndef __CTLLOOP_H
#define __CTLLOOP_H	 

#include "stm32f3xx_hal.h"
#include "function.h"

void BUCKVLoopCtlTYPE3(void);

extern int32_t  VErr0, VErr1, VErr2, VErr3;
extern int32_t	u0, u1, u2, u3;
	
// Один цикл
#define PERIOD 10240	 
#endif

