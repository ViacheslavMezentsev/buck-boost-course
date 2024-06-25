/* USER CODE BEGIN Header */
	
/* USER CODE END Header */
#include "CtlLoop.h"

/**************** ���������� **********************/
int32_t   IErr0=0; // ������ ����
int32_t		i0=0;    // ����� ������� ����

/*
** ===================================================================
**     Funtion Name :  void BUCKILoopCtlPI(void)
**     Description :   ����������� ����������� ����
**     Parameters  :
**     Returns     :
** ===================================================================
*/
#define ILOOP_KP        8      // ������� ������ PI �������� P Q12
#define ILOOP_KI        2      // ������� ������ PI �������� I Q12
#define BUCK_MIN_INTE   327680 // ����������� ����������� ���������� Q24 - 2%

CCMRAM void BUCKILoopCtlPI(void)
{
	// �������� Q24
	static  int32_t   I_Integral = 0;
	int32_t IoutTemp = 0; 
	
	// ��������� ������������������� �������� ����
	IoutTemp = (((int32_t )ADC1_RESULT[3] - SADC.IoutOffset)*CAL_IOUT_K>>12)+CAL_IOUT_B;
	
	// ���������� ������ ������������� ����.
	IErr0= CtrValue.Ioref  -  IoutTemp;
	// �������� �������� 
	i0=I_Integral + IErr0*ILOOP_KP;
	// �������� ���������
	I_Integral = I_Integral+ IErr0*ILOOP_KI ;
	// ����� ������������� ��������
	if(I_Integral < BUCK_MIN_INTE )
		I_Integral = BUCK_MIN_INTE ;
	if(I_Integral> (CtrValue.BUCKMaxDuty<<12))
		I_Integral = (CtrValue.BUCKMaxDuty<<12);
	
	CtrValue.BuckDuty= i0>>12;
	CtrValue.BoostDuty=MIN_BOOST_DUTY1; //BOOST ����� ������������� ������� ���� 93% �������� ����� � 7% �������			
	// ����������� ��� �������� �����
	if(CtrValue.BuckDuty > CtrValue.BUCKMaxDuty)
		CtrValue.BuckDuty = CtrValue.BUCKMaxDuty;	
	if(CtrValue.BuckDuty < MIN_BUKC_DUTY)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	// �������� ����������������� PWM
	if(DF.PWMENFlag==0)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	//
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = CtrValue.BuckDuty * PERIOD>>12; 
  HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP3xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR>>1; 
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP1xR = PERIOD - (CtrValue.BoostDuty * PERIOD>>12);
}


