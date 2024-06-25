/* USER CODE BEGIN Header */
	
/* USER CODE END Header */
#include "CtlLoop.h"

/*
** ===================================================================
**     Funtion Name :  void BUCKOpenLoop(void)
**     Description :   BUCK без обратной связи, фиксированный рабочий цикл, изменение выходного напряжения при изменении входного напряжения (рабочий цикл 93 %)
**     Parameters  :
**     Returns     :
** ===================================================================
*/
void BUCKOpenLoop(void)
{
	CtrValue.BuckDuty= 1024; //Рабочий цикл без обратной связи, Q12 соответствует 100%, то есть 4096 соответствует 100% рабочему циклу
	
	CtrValue.BoostDuty=MIN_BOOST_DUTY1; //Фиксированный рабочий цикл верхний ключ BOOST 93%, нижний ключ 7%			
	// Пределы максимального и минимального рабочего цикла
	if(CtrValue.BuckDuty > CtrValue.BUCKMaxDuty)
		CtrValue.BuckDuty = CtrValue.BUCKMaxDuty;	
	if(CtrValue.BuckDuty < MIN_BUKC_DUTY)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	//PWMENFlag — это флаг включения ШИМ, когда бит равен 0, рабочий цикл понижающего преобразователя равен 0, выхода нет;
	if(DF.PWMENFlag==0)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	// Обновляем соответствующий регистр
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = CtrValue.BuckDuty * PERIOD>>12;
  HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP3xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR>>1;
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP1xR = PERIOD - (CtrValue.BoostDuty * PERIOD>>12);
}

