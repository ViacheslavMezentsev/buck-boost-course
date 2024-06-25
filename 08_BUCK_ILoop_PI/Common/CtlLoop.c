/* USER CODE BEGIN Header */
	
/* USER CODE END Header */
#include "CtlLoop.h"

/**************** Переменные **********************/
int32_t   IErr0=0; // Ошибка тока
int32_t		i0=0;    // Выход контура тока

/*
** ===================================================================
**     Funtion Name :  void BUCKILoopCtlPI(void)
**     Description :   Поддержание постоянного тока
**     Parameters  :
**     Returns     :
** ===================================================================
*/
#define ILOOP_KP        8      // Токовый контур PI параметр P Q12
#define ILOOP_KI        2      // Токовый контур PI параметр I Q12
#define BUCK_MIN_INTE   327680 // Минимальный коэффициент заполнения Q24 - 2%

CCMRAM void BUCKILoopCtlPI(void)
{
	// Интеграл Q24
	static  int32_t   I_Integral = 0;
	int32_t IoutTemp = 0; 
	
	// Получение откорректированного значения тока
	IoutTemp = (((int32_t )ADC1_RESULT[3] - SADC.IoutOffset)*CAL_IOUT_K>>12)+CAL_IOUT_B;
	
	// Вычисления ошибки регулирования тока.
	IErr0= CtrValue.Ioref  -  IoutTemp;
	// Выходное значение 
	i0=I_Integral + IErr0*ILOOP_KP;
	// Значение интеграла
	I_Integral = I_Integral+ IErr0*ILOOP_KI ;
	// Лимит интегрального значения
	if(I_Integral < BUCK_MIN_INTE )
		I_Integral = BUCK_MIN_INTE ;
	if(I_Integral> (CtrValue.BUCKMaxDuty<<12))
		I_Integral = (CtrValue.BUCKMaxDuty<<12);
	
	CtrValue.BuckDuty= i0>>12;
	CtrValue.BoostDuty=MIN_BOOST_DUTY1; //BOOST имеет фиксированный рабочий цикл 93% верхнего ключа и 7% нижнего			
	// Ограничения для рабочего цикла
	if(CtrValue.BuckDuty > CtrValue.BUCKMaxDuty)
		CtrValue.BuckDuty = CtrValue.BUCKMaxDuty;	
	if(CtrValue.BuckDuty < MIN_BUKC_DUTY)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	// Проверка флагаразрешающего PWM
	if(DF.PWMENFlag==0)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	//
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = CtrValue.BuckDuty * PERIOD>>12; 
  HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP3xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR>>1; 
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP1xR = PERIOD - (CtrValue.BoostDuty * PERIOD>>12);
}


