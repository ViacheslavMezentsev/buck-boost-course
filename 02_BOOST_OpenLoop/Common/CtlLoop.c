/* USER CODE BEGIN Header */
	
/* USER CODE END Header */
#include "CtlLoop.h"

/*
** ===================================================================
**     Funtion Name :  void BoostOpenLoop(void)
**     Description :   Boost, фиксированый рабочий цикл, выходное напряжение меняется в зависимости от входного напряжения, BUCK upper tube (93% duty cycle)
**     Parameters  :
**     Returns     :
** ===================================================================
*/
void BoostOpenLoop(void)
{
	CtrValue.BoostDuty = 2048; //Рабочий цикл без обратной связи, Q12 соответствует 100%, то есть 4096 соответствует 100% рабочему циклу
	
	CtrValue.BuckDuty = MAX_BUCK_DUTY; //Фиксированный рабочий цикл верхний ключ BOOST 93%, нижний ключ 7%			
	// Пределы максимального и минимального рабочего цикла
	if(CtrValue.BoostDuty > CtrValue.BoostMaxDuty)
		CtrValue.BoostDuty = CtrValue.BoostMaxDuty;	
	if(CtrValue.BoostDuty < MIN_BOOST_DUTY)
		CtrValue.BoostDuty = MIN_BOOST_DUTY;	
	//PWMENFlag — это флаг включения ШИМ, когда бит равен 0, рабочий цикл понижающего преобразователя равен 0, выхода нет;
	if(DF.PWMENFlag==0)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	// Обновляем соответствующий регистр
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = CtrValue.BuckDuty * PERIOD>>12; // buck рабочий цикл
  HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP3xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR>>1; // ADC trigger sampling point
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP1xR = PERIOD - (CtrValue.BoostDuty * PERIOD>>12); // Boost рабочий цикл
}

