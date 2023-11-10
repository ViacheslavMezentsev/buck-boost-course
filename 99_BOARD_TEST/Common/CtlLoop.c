/* USER CODE BEGIN Header */
	
/* USER CODE END Header */
#include "CtlLoop.h"

/**************************************/
int32_t   VErr0=0; // Ошибка напряжения Q12
int32_t		u0=0;    // Выход контура напряжения

/*
** ===================================================================
**     Funtion Name :  void BUCKVLoopCtlPI(void)
**     Description :   Позиционный PI контур регулирования
**     Parameters  :
**     Returns     :
** ===================================================================
*/
#define BUCK_VKP    	     50     // Контур напряжения PI-регулятора P значение Q12
#define BUCK_VKI     	     5      // Контур напряжения PI-регулятора I значение Q12
#define BUCK_MIN_INTE      327680 // Минимальный коэффициент заполнения 2%, интегральная величина

CCMRAM void BUCKVLoopCtlPI(void)
{
	static  int32_t   V_Integral = 0;
	int32_t VoutTemp = 0;  // После коррекции
	
	// Коррекция выходного напряжения
	VoutTemp = ((uint32_t )ADC1_RESULT[2]*CAL_VOUT_K>>12)+CAL_VOUT_B;
	// Рассчет ошибки регулирования
	VErr0= CtrValue.Voref  -  VoutTemp;
	// Интегральная составляющая регулирования
	u0= V_Integral + VErr0*BUCK_VKP;
	V_Integral = V_Integral + VErr0*BUCK_VKI;
	// Пределы регулирования
	if(V_Integral < BUCK_MIN_INTE )
		V_Integral = BUCK_MIN_INTE ;
	// Интегральный предел регулирования
	if(V_Integral> (CtrValue.BUCKMaxDuty<<12))
		V_Integral = (CtrValue.BUCKMaxDuty<<12);
	
	CtrValue.BuckDuty= u0>>12;
	CtrValue.BoostDuty=MIN_BOOST_DUTY1; //BOOST Верхник ключ 93% нижний 7%			
	// Ограничения для рабочего цикла
	if(CtrValue.BuckDuty > CtrValue.BUCKMaxDuty)
		CtrValue.BuckDuty = CtrValue.BUCKMaxDuty;	
	if(CtrValue.BuckDuty < MIN_BUKC_DUTY)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	// Проверка флага разрешающего PWM
	if(DF.PWMENFlag==0)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	//
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = CtrValue.BuckDuty * PERIOD>>12; 
  HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP3xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR>>1; 
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP1xR = PERIOD - (CtrValue.BoostDuty * PERIOD>>12);
}


