/* USER CODE BEGIN Header */
	
/* USER CODE END Header */
#include "CtlLoop.h"

/**************** Переменные **********************/
int32_t   VErr0=0; // Ошибка напряжения Q12
int32_t		u0=0;    // Выход контура напряжения

/*
** ===================================================================
**     Funtion Name :  void BoostVLoopCtlPI(void)
**     Description :   Позиционный PI контур регулирования
**     Parameters  :
**     Returns     :
** ===================================================================
*/
#define BOOST_VKP    	  50     // Контур напряжения PI-регулятора P значение Q12
#define BOOST_VKI     	5      // Контур напряжения PI-регулятора I значение Q12
#define BOOST_MIN_INTE  335544 // Минимальный коэффициент заполнения 2%, интегральная величина

CCMRAM void BoostVLoopCtlPI(void)
{
	static  int32_t   V_Integral = 0;
	int32_t VoutTemp = 0;  // После коррекции
	
	// Коррекция выходного напряжения
	VoutTemp = ((uint32_t )ADC1_RESULT[2]*CAL_VOUT_K>>12)+CAL_VOUT_B;
	// Рассчет ошибки регулирования
	VErr0= CtrValue.Voref  -  VoutTemp;
	// Интегральная составляющая регулирования
	u0= V_Integral + VErr0*BOOST_VKP;
	V_Integral = V_Integral + VErr0*BOOST_VKI;
	// Пределы регулирования
	if(V_Integral < BOOST_MIN_INTE )
		V_Integral = BOOST_MIN_INTE ;
	// Интегральный предел регулирования
	if(V_Integral> (CtrValue.BoostMaxDuty<<12))
		V_Integral = (CtrValue.BoostMaxDuty<<12);
	
	CtrValue.BuckDuty = MAX_BUCK_DUTY;
	CtrValue.BoostDuty = u0>>12;  			
	// Ограничения для рабочего цикла
	if(CtrValue.BoostDuty > CtrValue.BoostMaxDuty)
		CtrValue.BoostDuty = CtrValue.BoostMaxDuty;	
	if(CtrValue.BoostDuty < MIN_BOOST_DUTY)
		CtrValue.BoostDuty = MIN_BOOST_DUTY;
	// Проверка флага разрешающего PWM
	if(DF.PWMENFlag==0)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	//
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = CtrValue.BuckDuty * PERIOD>>12; 
  HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP3xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR>>1; 
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP1xR = PERIOD - (CtrValue.BoostDuty * PERIOD>>12);
}


