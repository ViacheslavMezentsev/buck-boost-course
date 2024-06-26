/* USER CODE BEGIN Header */
	
/* USER CODE END Header */
#include "CtlLoop.h"

/****************环路变量定义**********************/
int32_t   VErr0=0; // 硒栳赅 磬镳�驽龛� Q12
int32_t		u0=0;    // 蔓躅� 觐眚箴� 磬镳�驽龛�

/*
** ===================================================================
**     Funtion Name :  void BUCKVLoopCtlPI(void)
**     Description :   项玷鲨铐睇� PI 觐眚箴 疱泱腓痤忄龛�
**     Parameters  :
**     Returns     :
** ===================================================================
*/
#define BUCK_VKP    	50 // 暑眚箴 磬镳�驽龛� PI-疱泱��蝾疣 P 珥圜屙桢 Q12
#define BUCK_VKI     	5  // 暑眚箴 磬镳�驽龛� PI-疱泱��蝾疣 I 珥圜屙桢 Q12
#define BUCK_MIN_INTE      327680 //惕龛爨朦睇� 觐翳鲨屙� 玎镱腠屙�� 2%, 桧蝈沭嚯�� 忮腓麒磬

CCMRAM void BUCKVLoopCtlPI(void)
{
	static  int32_t   V_Integral = 0;
	int32_t VoutTemp = 0;  // 项耠� 觐痧尻鲨�
	
	// 暑痧尻鲨� 恹躅漤钽� 磬镳�驽龛�
	VoutTemp = ((uint32_t )ADC1_RESULT[2]*CAL_VOUT_K>>12)+CAL_VOUT_B;
	// 朽耨麇� 铠栳觇 疱泱腓痤忄龛�
	VErr0= CtrValue.Voref  -  VoutTemp;
	// 软蝈沭嚯�� 耦耱噔���� 疱泱腓痤忄龛�
	u0= V_Integral + VErr0*BUCK_VKP;
	V_Integral = V_Integral + VErr0*BUCK_VKI;
	// 橡邃咫� 疱泱腓痤忄龛�
	if(V_Integral < BUCK_MIN_INTE )
		V_Integral = BUCK_MIN_INTE ;
	// 软蝈沭嚯 镳邃咫 疱泱腓痤忄龛�
	if(V_Integral> (CtrValue.BUCKMaxDuty<<12))
		V_Integral = (CtrValue.BUCKMaxDuty<<12);
	
	CtrValue.BuckDuty= u0>>12;
	CtrValue.BoostDuty=MIN_BOOST_DUTY1;//BOOST上管固定占空比93%，下管7%			
	// 毋疣龛麇龛� 潆� 疣犷麇泐 鲨觌�
	if(CtrValue.BuckDuty > CtrValue.BUCKMaxDuty)
		CtrValue.BuckDuty = CtrValue.BUCKMaxDuty;	
	if(CtrValue.BuckDuty < MIN_BUKC_DUTY)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	// 橡钼屦赅 綦嚆� 疣琊屮帼泐 PWM
	if(DF.PWMENFlag==0)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	//
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = CtrValue.BuckDuty * PERIOD>>12; 
  HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP3xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR>>1; 
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP1xR = PERIOD - (CtrValue.BoostDuty * PERIOD>>12);
}


