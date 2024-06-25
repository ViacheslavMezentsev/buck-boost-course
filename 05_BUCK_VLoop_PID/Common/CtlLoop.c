/* USER CODE BEGIN Header */
	
/* USER CODE END Header */
#include "CtlLoop.h"

/**************** ���������� **********************/
int32_t   VErr0 = 0, VErr1 = 0, VErr2 = 0; // ������ ���������� Q12
int32_t		u0 = 0, u1 = 0, u2 = 0;    // ����� ������� ����������


/*
** ===================================================================
**     Funtion Name :  void BUCKVLoopCtlPID(void)
**     Description :   ����������� PID ������ �������������
**     Parameters  :
**     Returns     :
** ===================================================================
*/
#define BUCK_VKP    	     50     // ������ ���������� PI-���������� P �������� Q12
#define BUCK_VKI     	     5      // ������ ���������� PI-���������� I �������� Q12
#define BUCK_MIN_INTE      327680 // ����������� ����������� ���������� 2%, ������������ ��������

#define BUCKPIDb0	5203		// Q8
#define BUCKPIDb1	-10246	// Q8
#define BUCKPIDb2	5044		// Q8

CCMRAM void BUCKVLoopCtlPID(void)
{
	int32_t VoutTemp=0; // ����� ��������� ��������� ����������
	
	// ��������� ��������� ����������
	VoutTemp = ((uint32_t )ADC1_RESULT[2]*CAL_VOUT_K>>12)+CAL_VOUT_B;
	// ���������� ������ ������������� �� ����������
	// ����� ����������� ���������� ���� ��� �������� ������� ���� ���������� � �������� �������������
	VErr0= CtrValue.Voref  - VoutTemp;
	// ������� ������� �������� PID �������������. ������ �������� ������� � ������������
	u0 = u1 + VErr0*BUCKPIDb0 + VErr1*BUCKPIDb1 + VErr2*BUCKPIDb2;	
	// ���������� ���������� ��������
	VErr2 = VErr1;
	VErr1 = VErr0;
	u1 = u0;
	// �������� u0 ���������� �� 8 ��� ������ � BUCKPID.b0-2 ������������ ����� �������� ��������, ������������ ��������� � Q8 ���
	CtrValue.BuckDuty= u0>>8;
	CtrValue.BoostDuty=MIN_BOOST_DUTY1; //BOOST ����� ������������� ������� ���� 93% ������� ����� � 7% ������.			
	// ����������� ��� �������� �����
	if(CtrValue.BuckDuty > CtrValue.BUCKMaxDuty)
		CtrValue.BuckDuty = CtrValue.BUCKMaxDuty;	
	if(CtrValue.BuckDuty < MIN_BUKC_DUTY)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	// �������� ����� ������������ PWM
	if(DF.PWMENFlag==0)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	//
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = CtrValue.BuckDuty * PERIOD>>12;                          // buck
  HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP3xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR>>1; // ADC �������������
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP1xR = PERIOD - (CtrValue.BoostDuty * PERIOD>>12);              // Boost
}

