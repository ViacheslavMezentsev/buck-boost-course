/* USER CODE BEGIN Header */
	
/* USER CODE END Header */
#include "CtlLoop.h"

/*
** ===================================================================
**     Funtion Name :  void BoostOpenLoop(void)
**     Description :   Boost, ������������ ������� ����, �������� ���������� �������� � ����������� �� �������� ����������, BUCK upper tube (93% duty cycle)
**     Parameters  :
**     Returns     :
** ===================================================================
*/
void BoostOpenLoop(void)
{
	CtrValue.BoostDuty = 2048; //������� ���� ��� �������� �����, Q12 ������������� 100%, �� ���� 4096 ������������� 100% �������� �����
	
	CtrValue.BuckDuty = MAX_BUCK_DUTY; //������������� ������� ���� ������� ���� BOOST 93%, ������ ���� 7%			
	// ������� ������������� � ������������ �������� �����
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

