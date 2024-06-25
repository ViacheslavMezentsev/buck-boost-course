/* USER CODE BEGIN Header */
	
/* USER CODE END Header */
#include "CtlLoop.h"

/**************** ���������� **********************/
int32_t   VErr0=0; // ������ ���������� Q12
int32_t		u0=0;    // ����� ������� ����������

/*
** ===================================================================
**     Funtion Name :  void BoostVLoopCtlPI(void)
**     Description :   ����������� PI ������ �������������
**     Parameters  :
**     Returns     :
** ===================================================================
*/
#define BOOST_VKP    	  50     // ������ ���������� PI-���������� P �������� Q12
#define BOOST_VKI     	5      // ������ ���������� PI-���������� I �������� Q12
#define BOOST_MIN_INTE  335544 // ����������� ����������� ���������� 2%, ������������ ��������

CCMRAM void BoostVLoopCtlPI(void)
{
	static  int32_t   V_Integral = 0;
	int32_t VoutTemp = 0;  // ����� ���������
	
	// ��������� ��������� ����������
	VoutTemp = ((uint32_t )ADC1_RESULT[2]*CAL_VOUT_K>>12)+CAL_VOUT_B;
	// ������� ������ �������������
	VErr0= CtrValue.Voref  -  VoutTemp;
	// ������������ ������������ �������������
	u0= V_Integral + VErr0*BOOST_VKP;
	V_Integral = V_Integral + VErr0*BOOST_VKI;
	// ������� �������������
	if(V_Integral < BOOST_MIN_INTE )
		V_Integral = BOOST_MIN_INTE ;
	// ������������ ������ �������������
	if(V_Integral> (CtrValue.BoostMaxDuty<<12))
		V_Integral = (CtrValue.BoostMaxDuty<<12);
	
	CtrValue.BuckDuty = MAX_BUCK_DUTY;
	CtrValue.BoostDuty = u0>>12;  			
	// ����������� ��� �������� �����
	if(CtrValue.BoostDuty > CtrValue.BoostMaxDuty)
		CtrValue.BoostDuty = CtrValue.BoostMaxDuty;	
	if(CtrValue.BoostDuty < MIN_BOOST_DUTY)
		CtrValue.BoostDuty = MIN_BOOST_DUTY;
	// �������� ����� ������������ PWM
	if(DF.PWMENFlag==0)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	//
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = CtrValue.BuckDuty * PERIOD>>12; 
  HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP3xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR>>1; 
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP1xR = PERIOD - (CtrValue.BoostDuty * PERIOD>>12);
}


