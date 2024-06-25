/* USER CODE BEGIN Header */
	
/* USER CODE END Header */

#include "function.h"  // �������� �������
#include "CtlLoop.h"   // ���� ����������
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct  _ADI SADC = {0,0,2048,0,0,0,0,2048,0,0,0,0};       // ������� � ���������� ������� � �������� ����������
struct  _Ctr_value CtrValue = {0,0,0,MIN_BUKC_DUTY,0,0,0}; // ��������� ����������
struct  _FLAG DF = {0,0,0,0,0,0,0,0};                      // ����������� �����
uint16_t ADC1_RESULT[4]={0,0,0,0};                         // ������� ���������� ������ DMA �� ������������� ���������� ������� ��� � ������
SState_M 	STState = SSInit;                                // ���� ��������� �������� �����
uint16_t OLEDShowCnt = 0;                                  // ������� ���������� ������ 
/*
** ===================================================================
**     Funtion Name :   void ADCSample(void)
**     Description :    ������� ��������� �������� ����������� � �����
**     Parameters  :
**     Returns     :
** ===================================================================
*/
CCMRAM void ADCSample(void)
{
	// ����� ��� ���������� �������� �������� 
	static uint32_t VinAvgSum=0,
                  IinAvgSum=0,
	                VoutAvgSum=0,
	                IoutAvgSum=0;
	
	// ������ ������ � �������� ���������
	SADC.Vin  = ((uint32_t )ADC1_RESULT[0]*CAL_VIN_K>>12)+CAL_VIN_B;
	SADC.Iin  = (((int32_t )ADC1_RESULT[1]-SADC.IinOffset)*CAL_IIN_K>>12)+CAL_IIN_B;
	SADC.Vout = ((uint32_t )ADC1_RESULT[2]*CAL_VOUT_K>>12)+CAL_VOUT_B;
	SADC.Iout = (((int32_t )ADC1_RESULT[3] - SADC.IoutOffset)*CAL_IOUT_K>>12)+CAL_IOUT_B;

	if(SADC.Vin <100 ) // ���������� �� ����� ������� ����
		SADC.Vin = 0;	
	if(SADC.Iin < 0 )  
		SADC.Iin = 0;
	if(SADC.Vout <100 )
		SADC.Vout = 0;
	if(SADC.Iout <0 )
		SADC.Iout = 0;

	// ���������� ������� ��������. ����� ����������� ��������
	VinAvgSum = VinAvgSum + SADC.Vin -(VinAvgSum>>2);
	SADC.VinAvg = VinAvgSum>>2;
	IinAvgSum = IinAvgSum + SADC.Iin -(IinAvgSum>>2);
	SADC.IinAvg = IinAvgSum >>2;
	VoutAvgSum = VoutAvgSum + SADC.Vout -(VoutAvgSum>>2);
	SADC.VoutAvg = VoutAvgSum>>2;
	IoutAvgSum = IoutAvgSum + SADC.Iout -(IoutAvgSum>>2);
	SADC.IoutAvg = IoutAvgSum>>2;	
}

/** ===================================================================
**     Funtion Name :void StateM(void)
**     Description :   ������� ��������� ��������. ����������� ����������� ������� ������ 5 �����������
**     Parameters  :
**     Returns     :
** ===================================================================*/
void StateM(void)
{
	switch(DF.SMFlag)
	{
		// ��������� ���������
		case  Init :StateMInit();
		break;
		// �������� �������
		case  Wait :StateMWait();
		break;
		// ������
		case  Rise :StateMRise();
		break;
		// ������
		case  Run :StateMRun();
		break;
		// ������, �������������
		case  Err :StateMErr();
		break;
	}
}

/** ===================================================================
**     Funtion Name :void StateMInit(void)
**     Description :   ������������� ����������
**     Parameters  :
**     Returns     :
** ===================================================================*/
void StateMInit(void)
{
  // ������������� ��������
	ValInit();
	// ��������� ���������� ����� ���������
	DF.SMFlag  = Wait;
}

/** ===================================================================
**     Funtion Name :void StateMWait(void)
**     Description :   �������� �������. ���� 1 ��� � �������� ������� ������
**     Parameters  :
**     Returns     :
** ===================================================================*/
void StateMWait(void)
{
  // ��������
	static uint16_t CntS = 0;
	static uint32_t	IinSum = 0,
		              IoutSum = 0;
	
	// ��������� ��������� PWM
	DF.PWMENFlag=0;
	// ������������� �������
	CntS ++;
	// ������� ��������� ���� � ���� ������������� �� ���������� ��������� � �������
	if(CntS > 256)
	{
		CntS = 256;
		HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); // ��������� ��������� ������� PWM
		if((DF.ErrFlag==F_NOERR)&&(DF.KeyFlag1==1))
		{
			// ���������� ��������
			CntS = 0;
			IinSum = 0;
			IoutSum = 0;
			// ������ ���� ���������
			DF.SMFlag  = Rise;
			// �������� ������ �����
			STState = SSInit;
		}
	}
	else // ������� �������� �������� 1.65V
	{
	  // ������������
    IinSum += ADC1_RESULT[1];
		IoutSum += ADC1_RESULT[3];
    // 256 ���
    if(CntS==256)
    {
      // �������
			SADC.IinOffset = IinSum >>8;
      SADC.IoutOffset = IoutSum >>8;
    }	
	}
}

/*
** ===================================================================
**     Funtion Name : void StateMRise(void)
**     Description  : ���� �������� �������
**     ������������� �������� ������� - �������� - ������
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
#define MAX_SSCNT       20  // ������������ ����� 100ms

void StateMRise(void)
{
	// ������
	static  uint16_t  Cnt = 0;
	// �������� ������������ ������� ������
	static  uint16_t	BUCKMaxDutyCnt = 0,
		                BoostMaxDutyCnt = 0;

	// ��������� �������� �����
	switch(STState)
	{
		case    SSInit:
		{
			// ��������� PWM
			DF.PWMENFlag=0;
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); // �������
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); // �������
			// �������� ������ � ����������� �������� �����
			CtrValue.BUCKMaxDuty  = MIN_BUKC_DUTY;
			CtrValue.BoostMaxDuty = MIN_BOOST_DUTY;
			// ��������� � ��������� �������� �����
			STState = SSWait;
			break;
		}
		case    SSWait:
		{
			// ������������� �������
			Cnt++;
			// ������� 100ms
			if(Cnt> MAX_SSCNT)
			{
				// �������� �������
				Cnt = 0;
				// ������������ ������� ���� �������� PWM
				CtrValue.BuckDuty = MIN_BUKC_DUTY;
				CtrValue.BUCKMaxDuty= MIN_BUKC_DUTY;
				CtrValue.BoostDuty = MIN_BOOST_DUTY;
				CtrValue.BoostMaxDuty = MIN_BOOST_DUTY;
				HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); // �������� ����� � ������
				HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); // �������� ����� � ������										
				STState = SSRun;	// �������� ������� ����			
			}
			break;
		}
		case    SSRun:
		{
			// ������������� ����
			DF.PWMENFlag=1;
			// ���������� ����������� ������� ������� ������
			BUCKMaxDutyCnt++;
			BoostMaxDutyCnt++;
			// ������������ ���������� �������� �����
			CtrValue.BUCKMaxDuty = CtrValue.BUCKMaxDuty + BUCKMaxDutyCnt*5;
			CtrValue.BoostMaxDuty = CtrValue.BoostMaxDuty + BoostMaxDutyCnt*5;
			// �����������
			if(CtrValue.BUCKMaxDuty > MAX_BUCK_DUTY)
				CtrValue.BUCKMaxDuty  = MAX_BUCK_DUTY ;
			if(CtrValue.BoostMaxDuty > MAX_BOOST_DUTY)
				CtrValue.BoostMaxDuty  = MAX_BOOST_DUTY ;
			
			if((CtrValue.BUCKMaxDuty==MAX_BUCK_DUTY)&&(CtrValue.BoostMaxDuty==MAX_BOOST_DUTY))			
			{
				// ��������� ������� ��������� � ��������� ��������
				DF.SMFlag  = Run;
				// ������ ����� ��������� � ��������� �������������
				STState = SSInit;	
			}
			break;
		}
		default:
		break;
	}
}

/*
** ===================================================================
**     Funtion Name :void StateMRun(void)
**     Description : �������� ������� �������� ����� ���������� �� ����������
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
void StateMRun(void)
{

}

/*
** ===================================================================
**     Funtion Name :void StateMErr(void)
**     Description : �������������
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
void StateMErr(void)
{
	// ���������� PWM
	DF.PWMENFlag=0;
	HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); // �������
	HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); // �������
	// ���� ������ ��������� �� ������������ � ��������� ������� ������
	if(DF.ErrFlag==F_NOERR)
			DF.SMFlag  = Wait;
}

/** ===================================================================
**     Funtion Name :void ValInit(void)
**     Description :  ������������� ��������� ����������
**     Parameters  :
**     Returns     :
** ===================================================================*/
void ValInit(void)
{
	// ��������� PWM
	DF.PWMENFlag=0;
	HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); // �������
	HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); // �������
	// ������� ����� ������
	DF.ErrFlag=0;
	// ������������� ����������� �������� �����
	CtrValue.BuckDuty = MIN_BUKC_DUTY;
	CtrValue.BUCKMaxDuty= MIN_BUKC_DUTY;
	CtrValue.BoostDuty = MIN_BOOST_DUTY;
	CtrValue.BoostMaxDuty = MIN_BOOST_DUTY;
}

/** ===================================================================
**     Funtion Name :void VrefGet(void)
**     Description :  ��������� �������� ���������� ��� �������������
**     Parameters  :
**     Returns     :
** ===================================================================*/
#define MAX_VREF    2921  // ������������ �������� ������� ���������� 48 � 0,5 � ����� 48,5 �/68 �*Q12
#define MIN_VREF    271   // ����������� ������� �������� ���������� 5 � 0,5 � ����� 4,5 �/68 �*2^Q12
#define VREF_K      10    // ���

void VrefGet(void)
{
	// ������������� ���������� �������� �������� ����������
	int32_t VTemp = 0;	
	// ���������� �������
	static int32_t VadjSum = 0;

	// �������� �������� ������� ��� - ��������� ����������
	SADC.Vadj = HAL_ADC_GetValue(&hadc2);
	// ���������� � ���������� ���������� ��������
	VadjSum = VadjSum + SADC.Vadj -(VadjSum>>8);
	SADC.VadjAvg = VadjSum>>8;
	
	// ������� ����������
	VTemp = MIN_VREF + SADC.Vadj;
	
	// �������� ��������������
	if( VTemp> ( CtrValue.Voref + VREF_K))
			CtrValue.Voref = CtrValue.Voref + VREF_K;
	else if( VTemp < ( CtrValue.Voref - VREF_K ))
			CtrValue.Voref =CtrValue.Voref - VREF_K;
	else
			CtrValue.Voref = VTemp ;

	// ��������� ������ �������������
	if(CtrValue.Voref >((SADC.VinAvg*3482)>>12))
		CtrValue.Voref =((SADC.VinAvg*3482)>>12);
}

/*
** ===================================================================
**     Funtion Name :void ShortOff(void)
**     Description : ������� �� �������� ���������
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
#define MAX_SHORT_I     1396  // ��� ��������� ���������
#define MIN_SHORT_V     289   // ���������� ��������� ���������

void ShortOff(void)
{
	static int32_t RSCnt = 0;
	static uint8_t RSNum =0 ;

	// ���� �������� ��� ������ � ���������� ���� �� ��� ��
	if((SADC.Iout> MAX_SHORT_I)&&(SADC.Vout <MIN_SHORT_V))
	{
		// ��������� PWM
		DF.PWMENFlag=0;
		HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); // �������
		HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); // �������
		// ���������� ���� ������
		setRegBits(DF.ErrFlag,F_SW_SHORT);
		// ������� � ������
		DF.SMFlag  =Err;
	}
	// �������������� ����� ��
	// ��������� 4 ������� �������� ������ � ������� � �������
	if(getRegBits(DF.ErrFlag,F_SW_SHORT))
	{
		// ������� ������������ ������
		RSCnt++;
		// �������� 2 �������
		if(RSCnt >400)
		{
			// ����� ��������
			RSCnt=0;
			// ����� ������� �����������. �� ������ 10 �������
			if(RSNum > 10)
			{
				// ������������� 11 ����� ������ �� �������������
				RSNum =11;
				// ��������� PWM
				DF.PWMENFlag=0;
				HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); // �������
				HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); // �������			
			}
			else
			{
				// ���������� ��������
				RSNum++;
				// ����� �����
				clrRegBits(DF.ErrFlag,F_SW_SHORT);
			}
		}
	}
}

/*
** ===================================================================
**     Funtion Name :void SwOCP(void)
**     Description : ������ �� ���������� �� ���� (���������������)
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
#define MAX_OCP_VAL     1117  // ����� ������
void SwOCP(void)
{
	// ����������� ��������� ���������� ���� � ������ ������
	static  uint16_t  OCPCnt = 0;
	static  uint16_t  RSCnt = 0;
	static  uint16_t  RSNum = 0;

	// ����� �������� ��� ������ ������������ ������ ��� 50ms
	if((SADC.Iout > MAX_OCP_VAL)&&(DF.SMFlag  ==Run))
	{
		// ����������� �������
		OCPCnt++;
		// ���� ��������� ����������� ������ ��� 50 ����������� ������� ��� ������ ���������
		if(OCPCnt > 10)
		{
			OCPCnt  = 0;
			// ������������� PWM
			DF.PWMENFlag=0;
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); 
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); 							
			// ������������� ���� ������
			setRegBits(DF.ErrFlag,F_SW_IOUT_OCP);
			// ������������� ���� ������
			DF.SMFlag  =Err;
		}
	}
	else
		// ������� �������
		OCPCnt  = 0;

	// �������������� ����� ������������
	// ������� 4 ������� ����� �������� ������ � �����������
	if(getRegBits(DF.ErrFlag,F_SW_IOUT_OCP))
	{
		// ����������� �������
		RSCnt++;
		// ������� 2 �������
		if(RSCnt > 400)
		{
			RSCnt=0;
			// ���������� ���������� �� ����
			RSNum++;
			// ���� ������ ������ 10 �� �����������
			if(RSNum > 10 )
			{
				// ��������� ������ �� ������
				RSNum =11;
				// ������������� PWM
				DF.PWMENFlag = 0;
				HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); 
				HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); 				
			}
			else
			{
			 // ������� ���� ������
				clrRegBits(DF.ErrFlag,F_SW_IOUT_OCP);
			}
		}
	}
}

/*
** ===================================================================
**     Funtion Name :void SwOVP(void)
**     Description : ������ �� ���������� ���������� �� ������
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
#define MAX_VOUT_OVP_VAL    3012 // ������ �� �������������� 50 � (50/68) *Q12

void VoutSwOVP(void)
{
	// �������
	static  uint16_t  OVPCnt=0;

	// ��������� ��� ���������� ��������� 10ms
	if (SADC.Vout > MAX_VOUT_OVP_VAL)
	{
		// ����������� �������
		OVPCnt++;
		// 10 ����
		if(OVPCnt > 2)
		{
			// ���������� �������
			OVPCnt=0;
			// ������������� PWM
			DF.PWMENFlag=0;
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); 
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); 									
			// ������������� ���� ������
			setRegBits(DF.ErrFlag,F_SW_VOUT_OVP);
			// ������������� ��������� ������
			DF.SMFlag  =Err;
		}
	}
	else
		OVPCnt = 0;
}

/*
** ===================================================================
**     Funtion Name :void VinSwUVP(void)
**     Description : ������ �� ����������� ���������� �� ����� (�����������������)
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
#define MIN_UVP_VAL    686 // ������ �� ����������� ���������� 11,4 � (11,4/68) *Q12
#define MIN_UVP_VAL_RE  795 //�������������� ������ �� ����������� ���������� 13,2 � (13,2/68) *Q12

void VinSwUVP(void)
{
	// ��������
	static  uint16_t  UVPCnt = 0;
	static  uint16_t	RSCnt = 0;

	// ����� ������� ���������� ���� 11.4V ������ ��� 200ms
	if ((SADC.Vin < MIN_UVP_VAL) && (DF.SMFlag != Init ))
	{
		UVPCnt++;
		// ����� ������� 10ms
		if(UVPCnt > 2)
		{
			UVPCnt = 0;
			RSCnt = 0;
			// ��������� PWM
			DF.PWMENFlag=0;
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); // �������
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); // �������						
			// ��������� �����
			setRegBits(DF.ErrFlag,F_SW_VIN_UVP);
			// ��������� ������
			DF.SMFlag  =Err;
		}
	}
	else
		UVPCnt = 0;
	
	// �������������� ������
	// ���� ���������� �������� � ��������� �������� �� ���������� ������ � ���������������
	if(getRegBits(DF.ErrFlag,F_SW_VIN_UVP))
	{
		if(SADC.Vin > MIN_UVP_VAL_RE) 
		{
			RSCnt++;
			// ������� 1S
			if(RSCnt > 200)
			{
				RSCnt = 0;
				UVPCnt = 0;
				// ������� ���� ������
				clrRegBits(DF.ErrFlag,F_SW_VIN_UVP);
			}	
		}
		else	
			RSCnt = 0;	
	}
	else
		RSCnt = 0;
}

/*
** ===================================================================
**     Funtion Name :void VinSwOVP(void)
**     Description : ������ �� �������������� �� ����� (��� �����������)
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
#define MAX_VIN_OVP_VAL    3012  // ������ �� �������������� 50 � (50/68) *Q12
void VinSwOVP(void)
{
	static  uint16_t  OVPCnt = 0;

	// ����� ������� ���������� ������ 50 � � ������� 100 ��
	if ((SADC.Vin*6800>>12) > MAX_VIN_OVP_VAL )
	{
		OVPCnt++;
		if(OVPCnt > 2)
		{
			OVPCnt = 0;
			// ��������� PWM
			DF.PWMENFlag = 0;
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); 
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); 									
			// ��������� ���� ������
			setRegBits(DF.ErrFlag,F_SW_VIN_OVP);
			// ������� � ����� ������
			DF.SMFlag  =Err;
		}
	}
	else
		OVPCnt = 0;
}

/** ===================================================================
**     Funtion Name :void LEDShow(void)
**     Description :  ��������� LED
**     Parameters  :
**     Returns     :
** ===================================================================*/
// ������� ��������� ��������� ������
#define SET_LED_G()	HAL_GPIO_WritePin(GPIOB, LED_G_Pin,GPIO_PIN_SET)
#define SET_LED_Y()	HAL_GPIO_WritePin(GPIOB, LED_Y_Pin,GPIO_PIN_SET)
#define SET_LED_R()	HAL_GPIO_WritePin(GPIOB, LED_R_Pin,GPIO_PIN_SET)
#define CLR_LED_G()	HAL_GPIO_WritePin(GPIOB, LED_G_Pin,GPIO_PIN_RESET)
#define CLR_LED_Y()	HAL_GPIO_WritePin(GPIOB, LED_Y_Pin,GPIO_PIN_RESET)
#define CLR_LED_R()	HAL_GPIO_WritePin(GPIOB, LED_R_Pin,GPIO_PIN_RESET)

void LEDShow(void)
{
	switch(DF.SMFlag)
	{
		//������ �������������, �������, ������ � �������
		case  Init :
		{
			SET_LED_G();
			SET_LED_Y();
			SET_LED_R();
			break;
		}
		// ��������� ��������, �������, ������ � ������� �����
		case  Wait :
		{
			SET_LED_G();
			SET_LED_Y();
			SET_LED_R();
			break;
		}
		//��������� �������� �����, ������ � �������
		case  Rise :
		{
			SET_LED_G();
			SET_LED_Y();
			CLR_LED_R();
			break;
		}
		//������� ���������, ����� �������
		case  Run :
		{
			SET_LED_G();
			CLR_LED_Y();
			CLR_LED_R();
			break;
		}
		//��������� �������������, ����� �������
		case  Err :
		{
			CLR_LED_G();
			CLR_LED_Y();
			SET_LED_R();
			break;
		}
	}
}

/** ===================================================================
**     Funtion Name :void KEYFlag(void)
**     Description : ����� ������
**     Parameters  :
**     Returns     :
** ===================================================================*/
#define READ_KEY1() HAL_GPIO_ReadPin(GPIOB, KEY_1_Pin)
#define READ_KEY2() HAL_GPIO_ReadPin(GPIOB, KEY_2_Pin)

void KEYFlag(void)
{
	static uint16_t	KeyDownCnt1 = 0,
		              KeyDownCnt2 = 0;
	
	// ������ ������
	if(READ_KEY1()==0)
	{
		// ������ ������ � ���� 150 ��
		KeyDownCnt1++;
		if(KeyDownCnt1 > 30)
		{
			KeyDownCnt1 = 0;
			if(DF.KeyFlag1==0) DF.KeyFlag1 = 1;
			else DF.KeyFlag1 = 0;
		}
	}
	else
		KeyDownCnt1 = 0;
	
	// ������ ������
	if(READ_KEY2()==0)
	{
		KeyDownCnt2++;
		if(KeyDownCnt2 > 30)
		{
			KeyDownCnt2 = 0;
			if(DF.KeyFlag2==0) DF.KeyFlag2 = 1;
			else DF.KeyFlag2 = 0;
		}
	}
	else
		KeyDownCnt2 = 0;

	if((DF.KeyFlag1==0)&&((DF.SMFlag==Rise)||(DF.SMFlag==Run)))
	{
		DF.SMFlag = Wait;
		// ��������� PWM
		DF.PWMENFlag=0;
		HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); 
		HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); 		
	}
}

/** ===================================================================
**     Funtion Name :void BBMode(void)
**     Description : ������ ������ ������
**     Parameters  :
**     Returns     :
** ===================================================================*/
void BBMode(void)
{
	DF.BBFlag = Buck; //buck mode
}

/** ===================================================================
**     Funtion Name :void MX_OLED_Init(void)
**     Description : �������������� ����� � �������� �������
**     Parameters  :
**     Returns     :
** ===================================================================*/
void MX_OLED_Init(void)
{
	HAL_GPIO_WritePin(IPS_BLK_GPIO_Port, IPS_BLK_Pin, GPIO_PIN_SET);
	ST7789_Init(240, 240);
	ST7789_FillScreen(BLACK);
  ST7789_SetBL(100);
	ST7789_print_7x11(10,0,GREEN,BLACK,1,"Buck-Boost V-Loop PI");
	ST7789_print_7x11(10,20,GREEN,BLACK,1,"State:");
	ST7789_print_7x11(60,60,GREEN,BLACK,1,"IN         OUT");
	ST7789_print_7x11(10,80,GREEN,BLACK,1,"V:");
	ST7789_print_7x11(10,100,GREEN,BLACK,1,"I:");
	ST7789_print_7x11(10,120,GREEN,BLACK,1,"P:");
	ST7789_print_7x11(10,150,GREEN,BLACK,1,"Vadj:");
}

/** ===================================================================
**     Funtion Name :void OLEDShow(void)
**     Description : ����������� ���������� �� ������
**     Parameters  :
**     Returns     :
** ===================================================================*/
void OLEDShow(void)
{
	static uint32_t VoutT = 0,
	         IoutT = 0;
	static uint32_t VinT = 0,
	         IinT = 0,
	         VadjT = 0;
	static uint16_t BBFlagTemp = 10,
		              SMFlagTemp = 10;
	
	// �������������� ����������� �������� � ����������� �������� � ���������� ��� � 100 ���
	VoutT = SADC.VoutAvg*6800>>12;
	IoutT = SADC.IoutAvg*1400>>12;
	VinT = SADC.VinAvg*6800>>12;
	IinT = SADC.IinAvg*2200>>12;
	VadjT = CtrValue.Voref*6800>>12;
		
	if(BBFlagTemp!= DF.BBFlag)
	{
		BBFlagTemp = DF.BBFlag;
		switch(DF.BBFlag)
		{
			case  NA :		
			{
				ST7789_print_7x11(60,20,GREEN,BLACK,1,"MODE:*NA* ");
				break;
			}
			case  Buck :		
			{
				ST7789_print_7x11(60,20,GREEN,BLACK,1,"MODE:BUCK ");
				break;
			}
			case  Boost :		
			{
				ST7789_print_7x11(60,20,GREEN,BLACK,1,"MODE:BOOST");
				break;
			}
			case  Mix :		
			{
				ST7789_print_7x11(60,20,GREEN,BLACK,1,"MODE:MIX ");
				break;
			}
		}
	}
	
	if(SMFlagTemp!= DF.SMFlag)
	{	
		SMFlagTemp = DF.SMFlag;
		switch(DF.SMFlag)
		{
			case  Init :
			{
				ST7789_print_7x11(150,20,YELLOW,BLACK,0,"Init  ");
				break;
			}
			case  Wait :
			{
				ST7789_print_7x11(150,20,YELLOW,BLACK,0,"Waiting");
				break;
			}
			case  Rise :
			{
				ST7789_print_7x11(150,20,YELLOW,BLACK,0,"Rising");
				break;
			}
			case  Run :
			{
				ST7789_print_7x11(150,20,YELLOW,BLACK,0,"Running");
				break;
			}
			case  Err :
			{
				ST7789_print_7x11(150,20,YELLOW,BLACK,0,"Error  ");
				break;
			}
		}	
	}
	
	char dd[16];
	
	sprintf(dd, "%0*.*f V ",4,2,(float)VinT/100);
	ST7789_print_7x11(40,80,GREEN,BLACK,0,dd);
	sprintf(dd, "%0*.*f V ",4,2,(float)VoutT/100);
	ST7789_print_7x11(130,80,GREEN,BLACK,0,dd);
	sprintf(dd, "%0*.*f A ",4,2,(float)IinT/100);
	ST7789_print_7x11(40,100,GREEN,BLACK,0,dd);
	sprintf(dd, "%0*.*f A ",4,2,(float)IoutT/100);
	ST7789_print_7x11(130,100,GREEN,BLACK,0,dd);

	sprintf(dd, "%0*.*f W",4,2,((float)VoutT/100*(float)IoutT/100));
	ST7789_print_7x11(130,120,GREEN,BLACK,0,dd);
	sprintf(dd, "%0*.*f W",4,2,((float)VinT/100*(float)IinT/100));
	ST7789_print_7x11(40,120,GREEN,BLACK,0,dd);
	
	sprintf(dd, "%0*.*f V ",4,2,(float)VadjT/100);
	ST7789_print_7x11(60,150,GREEN,BLACK,0,dd);
}
