/* USER CODE BEGIN Header */
	
/* USER CODE END Header */
#include "CtlLoop.h"

/*
** ===================================================================
**     Funtion Name :  void BuckBoostOpenLoop(void)
**     Description :   ����������� �������� ����� BUCK � ����������� �������� ����� Boost ������������ ������ �� �������� ����������.
**     Parameters  :
**     Returns     :
** ===================================================================
*/
void BuckBoostOpenLoop(void)
{
	CtrValue.BoostDuty= 2048; // Boost open-loop ������� ����, Q12 -100%, ��� 4096 ������������� 100%
	CtrValue.BuckDuty = 3277; // buck open loop duty cycle		
	
	// ������������ � ����������� ������� �������� ����� Buck
	if(CtrValue.BuckDuty > CtrValue.BUCKMaxDuty)
		CtrValue.BuckDuty = CtrValue.BUCKMaxDuty;	
	if(CtrValue.BuckDuty < MIN_BUKC_DUTY)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	// ������������ � ����������� ������� �������� ����� Boost
	if(CtrValue.BoostDuty > CtrValue.BoostMaxDuty)
		CtrValue.BoostDuty = CtrValue.BoostMaxDuty;	
	if(CtrValue.BoostDuty < MIN_BOOST_DUTY)
		CtrValue.BoostDuty = MIN_BOOST_DUTY;		
	//PWMENFlag � ��� ���� ��������� ���, ����� ��� ����� 0, ������� ���� ����������� ��������������� ����� 0, ������ ���;
	if(DF.PWMENFlag==0)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	// ��������� ��������������� �������
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = CtrValue.BuckDuty * PERIOD>>12; // buck ������� ����
  HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP3xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR>>1; // ADC trigger sampling point
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP1xR = PERIOD - (CtrValue.BoostDuty * PERIOD>>12); // Boost ������� ����
}

