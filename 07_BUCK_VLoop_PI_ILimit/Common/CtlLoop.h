#ifndef __CTLLOOP_H
#define __CTLLOOP_H	 

#include "stm32f3xx_hal.h"
#include "function.h"

void BUCKVLoopCtlPI(void);
void ILimitLoopCtl(void);

extern int32_t  VErr0; 
extern int32_t	u0;
extern int32_t	IErr0; // Ошибка по току
extern int32_t	i0;    // Токовый выход
	
// Один цикл
#define PERIOD 10240	 
#endif

