/* USER CODE BEGIN Header */
	
/* USER CODE END Header */
#include "CtlLoop.h"

/**************** ���������� **********************/
int32_t   VErr0 = 0, VErr1 = 0, VErr2 = 0, VErr3 = 0; //������ �� ���������� Q12
int32_t		u0 = 0, u1 = 0, u2 = 0, u3 = 0; //����� ����������

/*
** ===================================================================
**     Funtion Name :  void BUCKVLoopCtlTYPE3(void)
**     Description  :   ������ ������� ���������� � ������ ������� ���������� ���� 3 � (��. �������� �� ������� ������� BUCK-TYPE3)
**     Parameters   :
**     Returns      :
** ===================================================================
*/

// ��� ������������ ���� ��������� � Machcad
#define BUCKTYPETb0	4995		//Q8
#define BUCKTYPETb1	-4838		//Q8
#define BUCKTYPETb2	-4992		//Q8
#define BUCKTYPETb3	4838		//Q8
#define BUCKTYPETa1	5079		//Q12
#define BUCKTYPETa2	-1043		//Q12
#define BUCKTYPETa3	60			//Q12

CCMRAM void BUCKVLoopCtlTYPE3(void)
{
	int32_t VoutTemp = 0; //����� ��������� ��������� ����������
	
	// ��������� ��������� ����������
	VoutTemp = ((uint32_t )ADC1_RESULT[2]*CAL_VOUT_K>>12)+CAL_VOUT_B;
	// ������� ������ ����������. ���� ������� ���������� ��������� �������� ����������, ������� ���� �������������
	VErr0 = CtrValue.Voref  - VoutTemp;
	// ������ PID
	u0 = (( u1*BUCKTYPETa1 + u2*BUCKTYPETa2 + u3*BUCKTYPETa3)>>12) + ((VErr0*BUCKTYPETb0 + VErr1*BUCKTYPETb1 + VErr2*BUCKTYPETb2 + VErr3*BUCKTYPETb3)>>8);	
	// ������������ ������ �� ���������
	VErr3 = VErr2;
	VErr2 = VErr1;
	VErr1 = VErr0;
	u3 = u2;
	u2 = u1;
	u1 = u0;
	// ������������ ������������ ��������
	CtrValue.BuckDuty = u0;
	CtrValue.BoostDuty = MIN_BOOST_DUTY1; // BOOST ����� ������������� ������� ���� 93% ��� �������� ����� � 7% ��� ������� �����			
	// ����������� � ������������ �������
	if(CtrValue.BuckDuty > CtrValue.BUCKMaxDuty)
		CtrValue.BuckDuty = CtrValue.BUCKMaxDuty;	
	if(CtrValue.BuckDuty < MIN_BUKC_DUTY)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	//PWMENFlag � ��� ����� ��������� ���. ����� ���� ��� ����� 0, ������� ���� = 0 � �������� ������ �����������
	if(DF.PWMENFlag==0)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	// ��������� ������ � ���������
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = CtrValue.BuckDuty * PERIOD>>12; // buck ������� ����
  HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP3xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR>>1; // ����� ������� ADC
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP1xR = PERIOD - (CtrValue.BoostDuty * PERIOD>>12); // Boost ������� ����
}

