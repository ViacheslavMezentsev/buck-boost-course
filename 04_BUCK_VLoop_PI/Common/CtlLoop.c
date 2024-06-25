/* USER CODE BEGIN Header */
	
/* USER CODE END Header */
#include "CtlLoop.h"

/****************��·��������**********************/
int32_t   VErr0=0; // ������ ���������� Q12
int32_t		u0=0;    // ����� ������� ����������

/*
** ===================================================================
**     Funtion Name :  void BUCKVLoopCtlPI(void)
**     Description :   ����������� PI ������ �������������
**     Parameters  :
**     Returns     :
** ===================================================================
*/
#define BUCK_VKP    	50 // ������ ���������� PI-���������� P �������� Q12
#define BUCK_VKI     	5  // ������ ���������� PI-���������� I �������� Q12
#define BUCK_MIN_INTE      327680 //����������� ����������� ���������� 2%, ������������ ��������

CCMRAM void BUCKVLoopCtlPI(void)
{
	static  int32_t   V_Integral = 0;
	int32_t VoutTemp = 0;  // ����� ���������
	
	// ��������� ��������� ����������
	VoutTemp = ((uint32_t )ADC1_RESULT[2]*CAL_VOUT_K>>12)+CAL_VOUT_B;
	// ������� ������ �������������
	VErr0= CtrValue.Voref  -  VoutTemp;
	// ������������ ������������ �������������
	u0= V_Integral + VErr0*BUCK_VKP;
	V_Integral = V_Integral + VErr0*BUCK_VKI;
	// ������� �������������
	if(V_Integral < BUCK_MIN_INTE )
		V_Integral = BUCK_MIN_INTE ;
	// ������������ ������ �������������
	if(V_Integral> (CtrValue.BUCKMaxDuty<<12))
		V_Integral = (CtrValue.BUCKMaxDuty<<12);
	
	CtrValue.BuckDuty= u0>>12;
	CtrValue.BoostDuty=MIN_BOOST_DUTY1;//BOOST�Ϲ̶ܹ�ռ�ձ�93%���¹�7%			
	// ����������� ��� �������� �����
	if(CtrValue.BuckDuty > CtrValue.BUCKMaxDuty)
		CtrValue.BuckDuty = CtrValue.BUCKMaxDuty;	
	if(CtrValue.BuckDuty < MIN_BUKC_DUTY)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	// �������� ����� ������������ PWM
	if(DF.PWMENFlag==0)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	//
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = CtrValue.BuckDuty * PERIOD>>12; 
  HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP3xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR>>1; 
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP1xR = PERIOD - (CtrValue.BoostDuty * PERIOD>>12);
}


