/* USER CODE BEGIN Header */
	
/* USER CODE END Header */
#include "CtlLoop.h"

/*
** ===================================================================
**     Funtion Name :  void BuckBoostOpenLoop(void)
**     Description :   –егулировка рабочего цикла BUCK и регулировка рабочего цикла Boost одновременно вли€ют на выходное напр€жение.
**     Parameters  :
**     Returns     :
** ===================================================================
*/
void BuckBoostOpenLoop(void)
{
	CtrValue.BoostDuty= 2048; // Boost open-loop рабочий цикл, Q12 -100%, это 4096 соответствует 100%
	CtrValue.BuckDuty = 3277; // buck open loop duty cycle		
	
	// ћаксимальные и минимальные пределы рабочего цикла Buck
	if(CtrValue.BuckDuty > CtrValue.BUCKMaxDuty)
		CtrValue.BuckDuty = CtrValue.BUCKMaxDuty;	
	if(CtrValue.BuckDuty < MIN_BUKC_DUTY)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	// ћаксимальные и минимальные пределы рабочего цикла Boost
	if(CtrValue.BoostDuty > CtrValue.BoostMaxDuty)
		CtrValue.BoostDuty = CtrValue.BoostMaxDuty;	
	if(CtrValue.BoostDuty < MIN_BOOST_DUTY)
		CtrValue.BoostDuty = MIN_BOOST_DUTY;		
	//PWMENFlag Ч это флаг включени€ Ў»ћ, когда бит равен 0, рабочий цикл понижающего преобразовател€ равен 0, выхода нет;
	if(DF.PWMENFlag==0)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	// ќбновл€ем соответствующий регистр
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = CtrValue.BuckDuty * PERIOD>>12; // buck рабочий цикл
  HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP3xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR>>1; // ADC trigger sampling point
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP1xR = PERIOD - (CtrValue.BoostDuty * PERIOD>>12); // Boost рабочий цикл
}

