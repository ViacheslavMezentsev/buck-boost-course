/* USER CODE BEGIN Header */
	
/* USER CODE END Header */
#include "CtlLoop.h"

/*
** ===================================================================
**     Funtion Name :  void BUCKOpenLoop(void)
**     Description :   BUCK ��� �������� �����, ������������� ������� ����, ��������� ��������� ���������� ��� ��������� �������� ���������� (������� ���� 93 %)
**     Parameters  :
**     Returns     :
** ===================================================================
*/
void BUCKOpenLoop(void)
{
	CtrValue.BuckDuty= 1024; //������� ���� ��� �������� �����, Q12 ������������� 100%, �� ���� 4096 ������������� 100% �������� �����
	
	CtrValue.BoostDuty=MIN_BOOST_DUTY1; //������������� ������� ���� ������� ���� BOOST 93%, ������ ���� 7%			
	// ������� ������������� � ������������ �������� �����
	if(CtrValue.BuckDuty > CtrValue.BUCKMaxDuty)
		CtrValue.BuckDuty = CtrValue.BUCKMaxDuty;	
	if(CtrValue.BuckDuty < MIN_BUKC_DUTY)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	//PWMENFlag � ��� ���� ��������� ���, ����� ��� ����� 0, ������� ���� ����������� ��������������� ����� 0, ������ ���;
	if(DF.PWMENFlag==0)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	// ��������� ��������������� �������
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = CtrValue.BuckDuty * PERIOD>>12;
  HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP3xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR>>1;
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP1xR = PERIOD - (CtrValue.BoostDuty * PERIOD>>12);
}

