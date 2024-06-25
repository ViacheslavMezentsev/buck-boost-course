/* USER CODE BEGIN Header */
	
/* USER CODE END Header */
#include "CtlLoop.h"

/**************** ���������� **********************/
int32_t   VErr0=0; // ������ ���������� Q12
int32_t		u0=0;    // ����� ������� ����������
int32_t		IErr0=0; // ������ ����
int32_t		i0=0;    // ����� ������� ����

/*
** ===================================================================
**     Funtion Name :  void BUCKVLoopCtlPI(void)
**     Description :   ����������� PI ������ �������������
**     Parameters  :
**     Returns     :
** ===================================================================
*/
#define BUCK_VKP    	50          // ������ ���������� PI-���������� P �������� Q12
#define BUCK_VKI     	5           // ������ ���������� PI-���������� I �������� Q12
#define BUCK_MIN_INTE      327680 //����������� ����������� ���������� Q24 - 2%, ������������ ��������

CCMRAM void BUCKVLoopCtlPI(void)
{
	// ������������ ���� Q24
	static  int32_t   V_Integral = 0;
	int32_t VoutTemp = 0;  // ����� ���������
	
	// ��������� ��������� ����������
	VoutTemp = ((uint32_t )ADC1_RESULT[2]*CAL_VOUT_K>>12)+CAL_VOUT_B;
	// ������ ����������� ��������� ����
	ILimitLoopCtl();
	//������� ������ ����������. ����� ������� ���������� ��������� �������� ����������, ������� ���� �������������.
  //CtrValue.Ilimitout � ����� ����� ����������� ����. ����� ��� ������ �������� ����������� ����, �������� ����� 0 � �� ����� �������� �������.
  //����� ��� ��������� �������� ����������� ����, �������� �������� �������������, � �������� ��� ��������������, ����� ���������� ������������� �� ���� ���������� CtrValue.Voref.
	VErr0= CtrValue.Voref  -  VoutTemp - CtrValue.Ilimitout;
	// ������������ ������������ ������������� = ����� ����� + KP * �������� ������ Q12
	u0= V_Integral + VErr0*BUCK_VKP;
	//������������� �����=����� �����+KI*����� ������
	V_Integral = V_Integral + VErr0*BUCK_VKI;
	// ������� �������������
	if(V_Integral < BUCK_MIN_INTE )
		V_Integral = BUCK_MIN_INTE ;
	// ������������ ������ �������������
	if(V_Integral> (CtrValue.BUCKMaxDuty<<12))
		V_Integral = (CtrValue.BUCKMaxDuty<<12);
	
	CtrValue.BuckDuty= u0>>12;
	CtrValue.BoostDuty=MIN_BOOST_DUTY1; //BOOST ����� ������������� ������� ���� 93% ��� �������� ����� � 7% ��� �������.	
	// ����������� ��� �������� �����
	if(CtrValue.BuckDuty > CtrValue.BUCKMaxDuty)
		CtrValue.BuckDuty = CtrValue.BUCKMaxDuty;	
	if(CtrValue.BuckDuty < MIN_BUKC_DUTY)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	// �������� ����� ������������ PWM
	if(DF.PWMENFlag==0)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	// ���������� �������� ���������
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = CtrValue.BuckDuty * PERIOD>>12; 
  HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP3xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR>>1; 
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP1xR = PERIOD - (CtrValue.BoostDuty * PERIOD>>12);
}

/*
** ===================================================================
**     Funtion Name :  void ILimitLoopCtl(void)
**     Description  :  ������ ����������� ����
**     ����� �������� ��� ������ �������� ����������� ����, �������� ������ ������� ����������� ���� ����� ����.
**     ����� �������� ��� ��������� �������� ����������� ����, �������� ������ ������� ����������� ���� �������� �������������.
**     ��� �������� �������� ������������� �� ������� �������� ���������� ������� ����������.��� ���������� �������� �������� ������� ���������� �������� ��� �����������.
**     Parameters  :
**     Returns     :
** ===================================================================
*/
#define ILIMIT_KP     5 // �������� PI ������ Q12
#define ILIMIT_KI     1 // �������� PI ������ Q12

CCMRAM void ILimitLoopCtl(void)
{
	// ������������ ������ Q24
	static  int32_t 	I_Integral=0;

	// ������� ������� ������. ����� �������� ��� ��������� ������� ���, ����� �������������, � ��������� ������ �������������� � �������������� �������� ��������.
	IErr0 = SADC.IoutAvg - CtrValue.ILimit;
	// ����� �������� ������� = ����� ����� + KP * ����� ������
	i0=I_Integral + IErr0*ILIMIT_KP;
	// ������������ ����� = ������������ ����� + KI * ����� ������
	I_Integral = I_Integral+ IErr0*ILIMIT_KI ;

	// ������ ������������ �����, ����������� ������ ������������ �����, �������� �������� -0,5 � (0,5/68) *Q24
	if( I_Integral < -123400)
		I_Integral = -123400;
	// ������ ������������ �����, ������������ ������ ������������ �����, �������� ���������� 48 � ������������� Q48 (48/68) * Q24
	if(I_Integral >11840000)
		I_Integral=11840000 ;
	// ����������� ������ ������
	if(i0 < 0)
		i0 = 0;
	
	CtrValue.Ilimitout = i0>>12;
}
