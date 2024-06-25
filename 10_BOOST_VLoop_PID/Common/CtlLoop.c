/* USER CODE BEGIN Header */
	
/* USER CODE END Header */
#include "CtlLoop.h"

/**************** Переменные **********************/
int32_t   VErr0 = 0, VErr1 = 0, VErr2 = 0; // Ошибка напряжения Q12
int32_t		u0 = 0, u1 = 0, u2 = 0;          // Выход контура напряжения


/*
** ===================================================================
**     Funtion Name :  void BoostVLoopCtlPID(void)
**     Description :   Позиционный PID контур регулирования
**     Parameters  :
**     Returns     :
** ===================================================================
*/
// Основные константы - коэффициенты PID регулятора·
#define BOOSTPIDb0	8860		//Q8
#define BOOSTPIDb1	-17445	//Q8
#define BOOSTPIDb2	8588		//Q8

CCMRAM void BoostVLoopCtlPID(void)
{
	int32_t VoutTemp=0; // После коррекции выходного напряжения
	
	// Коррекция выходного напряжения
	VoutTemp = ((uint32_t )ADC1_RESULT[2]*CAL_VOUT_K>>12)+CAL_VOUT_B;
	// Вычисление ошибки регулирования по напряжению
	// Когда референсное напряжение выше чем выходное рабочий цикл удлиняется и мощность увеличивается
	VErr0= CtrValue.Voref  - VoutTemp;
	// Формула расчета значения PID регулирования. Смотри описание функции в документации
	u0 = u1 + VErr0*BOOSTPIDb0 + VErr1*BOOSTPIDb1 + VErr2*BOOSTPIDb2;	
	// Сохранение предыдущих значений
	VErr2 = VErr1;
	VErr1 = VErr0;
	u1 = u0;
	// значение u0 сдвигается на 8 бит вправо к BUCKPID.b0-2 представляет собой цифровую величину, искусственно усиленную в Q8 раз
	CtrValue.BuckDuty = MAX_BUCK_DUTY;
	CtrValue.BoostDuty = u0>>8; 
	// Ограничения для рабочего цикла
	if(CtrValue.BoostDuty > CtrValue.BoostMaxDuty)
		CtrValue.BoostDuty = CtrValue.BoostMaxDuty;	
	if(CtrValue.BoostDuty < MIN_BOOST_DUTY)
		CtrValue.BoostDuty = MIN_BOOST_DUTY;
	// Проверка флага разрешающего PWM
	if(DF.PWMENFlag==0)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	//
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = CtrValue.BuckDuty * PERIOD>>12;                          // buck
  HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP3xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR>>1; // ADC
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP1xR = PERIOD - (CtrValue.BoostDuty * PERIOD>>12);              // Boost
}

