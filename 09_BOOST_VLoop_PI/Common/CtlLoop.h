#ifndef __CTLLOOP_H
#define __CTLLOOP_H	 

#include "stm32f3xx_hal.h"
#include "function.h"

void BoostVLoopCtlPI(void);

extern int32_t  VErr0;
extern int32_t	u0;
	
// ���� ����
#define PERIOD 10240	 
#endif

