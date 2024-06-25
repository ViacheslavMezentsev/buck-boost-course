/* USER CODE BEGIN Header */
	
/* USER CODE END Header */
#include "CtlLoop.h"

/**************** Переменные **********************/
int32_t   VErr0=0; // Ошибка напряжения Q12
int32_t		u0=0;    // Выход контура напряжения
int32_t		IErr0=0; // Ошибка тока
int32_t		i0=0;    // Выход контура тока

/*
** ===================================================================
**     Funtion Name :  void BUCKVLoopCtlPI(void)
**     Description :   Позиционный PI контур регулирования
**     Parameters  :
**     Returns     :
** ===================================================================
*/
#define BUCK_VKP    	50          // Контур напряжения PI-регулятора P значение Q12
#define BUCK_VKI     	5           // Контур напряжения PI-регулятора I значение Q12
#define BUCK_MIN_INTE      327680 //Минимальный коэффициент заполнения Q24 - 2%, интегральная величина

CCMRAM void BUCKVLoopCtlPI(void)
{
	// Интегральный цикл Q24
	static  int32_t   V_Integral = 0;
	int32_t VoutTemp = 0;  // После коррекции
	
	// Коррекция выходного напряжения
	VoutTemp = ((uint32_t )ADC1_RESULT[2]*CAL_VOUT_K>>12)+CAL_VOUT_B;
	// Контур ограничения выходного тока
	ILimitLoopCtl();
	//Рассчет ошибки напряжения. Когда опорное напряжение превышает выходное напряжение, рабочий цикл увеличивается.
  //CtrValue.Ilimitout — выход цикла ограничения тока. Когда ток меньше значения ограничения тока, значение равно 0 и не имеет никакого эффекта.
  //Когда ток превышает значение ограничения тока, значение является положительным, и выходной ток ограничивается, чтобы продолжать увеличиваться за счет уменьшения CtrValue.Voref.
	VErr0= CtrValue.Voref  -  VoutTemp - CtrValue.Ilimitout;
	// Интегральная составляющая регулирования = целая сумма + KP * величина ошибки Q12
	u0= V_Integral + VErr0*BUCK_VKP;
	//Целочисленная сумма=целая сумма+KI*сумма ошибки
	V_Integral = V_Integral + VErr0*BUCK_VKI;
	// Пределы регулирования
	if(V_Integral < BUCK_MIN_INTE )
		V_Integral = BUCK_MIN_INTE ;
	// Интегральный предел регулирования
	if(V_Integral> (CtrValue.BUCKMaxDuty<<12))
		V_Integral = (CtrValue.BUCKMaxDuty<<12);
	
	CtrValue.BuckDuty= u0>>12;
	CtrValue.BoostDuty=MIN_BOOST_DUTY1; //BOOST имеет фиксированный рабочий цикл 93% для верхнего ключа и 7% для нижнего.	
	// Ограничения для рабочего цикла
	if(CtrValue.BuckDuty > CtrValue.BUCKMaxDuty)
		CtrValue.BuckDuty = CtrValue.BUCKMaxDuty;	
	if(CtrValue.BuckDuty < MIN_BUKC_DUTY)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	// Проверка флага разрешающего PWM
	if(DF.PWMENFlag==0)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	// Обновление значения регистров
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = CtrValue.BuckDuty * PERIOD>>12; 
  HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP3xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR>>1; 
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP1xR = PERIOD - (CtrValue.BoostDuty * PERIOD>>12);
}

/*
** ===================================================================
**     Funtion Name :  void ILimitLoopCtl(void)
**     Description  :  Контур ограничения тока
**     Когда выходной ток меньше значения ограничения тока, выходной сигнал контура ограничения тока равен нулю.
**     Когда выходной ток превышает значение ограничения тока, выходной сигнал контура ограничения тока является положительным.
**     Его выходное значение накладывается на опорное значение напряжения контура напряжения.При уменьшении опорного значения контура напряжения выходной ток уменьшается.
**     Parameters  :
**     Returns     :
** ===================================================================
*/
#define ILIMIT_KP     5 // Значение PI ключей Q12
#define ILIMIT_KI     1 // Значение PI ключей Q12

CCMRAM void ILimitLoopCtl(void)
{
	// Интегральный контур Q24
	static  int32_t 	I_Integral=0;

	// Рассчет текущей ошибки. Когда выходной ток превышает опорный ток, выход увеличивается, и медленный контур рассчитывается с использованием среднего значения.
	IErr0 = SADC.IoutAvg - CtrValue.ILimit;
	// Выход токового контура = целая сумма + KP * сумма ошибки
	i0=I_Integral + IErr0*ILIMIT_KP;
	// Интегральная сумма = Интегральная сумма + KI * Сумма ошибки
	I_Integral = I_Integral+ IErr0*ILIMIT_KI ;

	// Предел интегральной суммы, минимальный предел интегральной суммы, обратное смещение -0,5 В (0,5/68) *Q24
	if( I_Integral < -123400)
		I_Integral = -123400;
	// Предел интегральной суммы, максимальный предел интегральной суммы, цифровое напряжение 48 В соответствует Q48 (48/68) * Q24
	if(I_Integral >11840000)
		I_Integral=11840000 ;
	// Минимальный предел вывода
	if(i0 < 0)
		i0 = 0;
	
	CtrValue.Ilimitout = i0>>12;
}
