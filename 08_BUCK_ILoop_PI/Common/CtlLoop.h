#ifndef __CTLLOOP_H
#define __CTLLOOP_H	 

#include "stm32f3xx_hal.h"
#include "function.h"

void BUCKILoopCtlPI(void);

extern int32_t  IErr0;
extern int32_t	i0;
	
// Один цикл
#define PERIOD 10240	 
#endif

