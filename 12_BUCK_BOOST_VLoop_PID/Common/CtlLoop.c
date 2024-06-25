/* USER CODE BEGIN Header */
	
/* USER CODE END Header */
#include "CtlLoop.h"

/**************** ����������� ���������� **********************/
int32_t   VErr0 = 0, VErr1 = 0, VErr2 = 0; // ������ ���������� Q12
int32_t		u0 = 0, u1 = 0, u2 = 0;          // ����� ������� ����������


/*
** ===================================================================
**     Funtion Name :  void BuckBoostVLoopCtlPID(void)
**     Description :   ���� ������������� ��� ������� BUCK-BOOST 
**                     ������������ ������� ����� ������� � �������� ������������ � ������������ ����� ������
**                     BUCK, BOOST ��� BUCK-BOOST (��������) ������
**                     � ������ BUCK, ����� ��������������� ��� BOOST �������� � ������������� ���������� � ������� BUCK ������������ �����.
**                     � ������ BOOST, ����� BUCK �������� � ������������� ������� ������, � BOOST ������������ �������� ����������.
**                     � �������� BUCK-BOOST, BUCK - ������������� ������� ����
**     Parameters  :
**     Returns     :
** ===================================================================
*/
// ��������� PID ���������� ������ buck
#define BUCKPIDb0	5203		// Q8
#define BUCKPIDb1	-10246	// Q8
#define BUCKPIDb2	5044		// Q8

// ��������� PID ���������� ������ BOOST
#define BOOSTPIDb0	8860		// Q8
#define BOOSTPIDb1	-17445	// Q8
#define BOOSTPIDb2	8588		// Q8

CCMRAM void BuckBoostVLoopCtlPID(void)
{
	int32_t VoutTemp=0; // ���������� ����� ���������
	
	// ��������� �������� ��������� ����������
	VoutTemp = ((uint32_t )ADC1_RESULT[2]*CAL_VOUT_K>>12)+CAL_VOUT_B;
	// ���������� �������� ������ �������������
	VErr0= CtrValue.Voref  - VoutTemp; 
	// ��� ������������ ������ ���������� ��������� ������� ���� ����� �� ��������� ������ �� ���������� ��������
	//BBModeChange - ���� ������������ ������
	if(DF.BBModeChange)
	{
		u1 = 0;
		DF.BBModeChange =0;
	}
	// �������� ������������� � ����������� �� ������ ������
	switch(DF.BBFlag)
	{
		case  NA : // ��������� ���� (������)
		{
			VErr0=0;
			VErr1=0;
			VErr2=0;
			u0 = 0;
			u1 = 0;
			break;
		}
		case  Buck:	//BUCK	
		{
			// ������� ������� PID ����������
			u0 = u1 + VErr0*BUCKPIDb0 + VErr1*BUCKPIDb1 + VErr2*BUCKPIDb2;	
			// �������
			VErr2 = VErr1;
			VErr1 = VErr0;
			u1 = u0;			
			// ������������ ����������� ������� ������
			CtrValue.BuckDuty= u0>>8;
			CtrValue.BoostDuty=MIN_BOOST_DUTY1; //BOOST ������������� ������� ���� 93% � 7%			
			// �������� �� ������ �� �������
			if(CtrValue.BuckDuty > CtrValue.BUCKMaxDuty)
				CtrValue.BuckDuty = CtrValue.BUCKMaxDuty;	
			if(CtrValue.BuckDuty < MIN_BUKC_DUTY)
				CtrValue.BuckDuty = MIN_BUKC_DUTY;
			break;
		}				
		case  Boost: //Boost
		{
			// ������� ������� PID ����������
			u0 = u1 + VErr0*BOOSTPIDb0 + VErr1*BOOSTPIDb1 + VErr2*BOOSTPIDb2;			
			// �������
			VErr2 = VErr1;
			VErr1 = VErr0;
			u1 = u0;			
			// ������������ ����������� ������� ������
			CtrValue.BuckDuty = MAX_BUCK_DUTY; // ������� ���� 93%
			CtrValue.BoostDuty = u0>>8;	
			// �������� �� ������ �� �������
			if(CtrValue.BoostDuty > CtrValue.BoostMaxDuty)
				CtrValue.BoostDuty = CtrValue.BoostMaxDuty;	
			if(CtrValue.BoostDuty < MIN_BOOST_DUTY)
				CtrValue.BoostDuty = MIN_BOOST_DUTY;
			break;
		}
		case  Mix: //Mix
		{
			// ������� ������� PID ����������
			u0 = u1 + VErr0*BOOSTPIDb0 + VErr1*BOOSTPIDb1 + VErr2*BOOSTPIDb2;			
			// �������
			VErr2 = VErr1;
			VErr1 = VErr0;
			u1 = u0;			
			// ������������ ����������� ������� ������
			CtrValue.BuckDuty = MAX_BUCK_DUTY1; // ������������� ������� ���� 80%
			CtrValue.BoostDuty = u0>>8;	
			// �������� �� ������ �� �������
			if(CtrValue.BoostDuty > CtrValue.BoostMaxDuty)
				CtrValue.BoostDuty = CtrValue.BoostMaxDuty;	
			if(CtrValue.BoostDuty < MIN_BOOST_DUTY)
				CtrValue.BoostDuty = MIN_BOOST_DUTY;
			break;
		}	
	}
	
	//PWMENFlag - ���� ��������� ��������� �������
	if(DF.PWMENFlag==0)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	// ���������� ��������� HRTIM
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = CtrValue.BuckDuty * PERIOD>>12; //buck
  HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP3xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR>>1; //ADC �������
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP1xR = PERIOD - (CtrValue.BoostDuty * PERIOD>>12); //Boost
}

