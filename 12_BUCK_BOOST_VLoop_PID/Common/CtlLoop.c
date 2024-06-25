/* USER CODE BEGIN Header */
	
/* USER CODE END Header */
#include "CtlLoop.h"

/**************** Определение переменных **********************/
int32_t   VErr0 = 0, VErr1 = 0, VErr2 = 0; // Ошибка напряжения Q12
int32_t		u0 = 0, u1 = 0, u2 = 0;          // Выход контура напряжения


/*
** ===================================================================
**     Funtion Name :  void BuckBoostVLoopCtlPID(void)
**     Description :   Цикл регулирования для режимов BUCK-BOOST 
**                     Определяется разница между входным и выходным напряжениями и определяется режим работы
**                     BUCK, BOOST или BUCK-BOOST (смешаный) режимы
**                     В режиме BUCK, ключи задействованные для BOOST работают в фиксированной скважности и сторона BUCK контролирует выход.
**                     В режиме BOOST, ключи BUCK работают с фиксированным рабочим циклом, а BOOST контролируют выходное напряжение.
**                     В смешаном BUCK-BOOST, BUCK - фиксированный рабочий цикл
**     Parameters  :
**     Returns     :
** ===================================================================
*/
// Параметры PID регулятора режима buck
#define BUCKPIDb0	5203		// Q8
#define BUCKPIDb1	-10246	// Q8
#define BUCKPIDb2	5044		// Q8

// Параметры PID регулятора режима BOOST
#define BOOSTPIDb0	8860		// Q8
#define BOOSTPIDb1	-17445	// Q8
#define BOOSTPIDb2	8588		// Q8

CCMRAM void BuckBoostVLoopCtlPID(void)
{
	int32_t VoutTemp=0; // Напряжение после коррекции
	
	// Коррекция значения выходного напряжения
	VoutTemp = ((uint32_t )ADC1_RESULT[2]*CAL_VOUT_K>>12)+CAL_VOUT_B;
	// Вычисление величины ошибки регулирования
	VErr0= CtrValue.Voref  - VoutTemp; 
	// При переключении режима необходимо уменьшать рабочий цикл чтобы не допустить выхода за допустимый диапазон
	//BBModeChange - флаг переключения режима
	if(DF.BBModeChange)
	{
		u1 = 0;
		DF.BBModeChange =0;
	}
	// Алгоритм регулирования в зависимости от режима работы
	switch(DF.BBFlag)
	{
		case  NA : // Начальный этап (запуск)
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
			// Формула расчета PID регулятора
			u0 = u1 + VErr0*BUCKPIDb0 + VErr1*BUCKPIDb1 + VErr2*BUCKPIDb2;	
			// История
			VErr2 = VErr1;
			VErr1 = VErr0;
			u1 = u0;			
			// Формирование необходимых рабочих циклов
			CtrValue.BuckDuty= u0>>8;
			CtrValue.BoostDuty=MIN_BOOST_DUTY1; //BOOST фиксированный рабочий цикл 93% и 7%			
			// Проверка на выходы за пределы
			if(CtrValue.BuckDuty > CtrValue.BUCKMaxDuty)
				CtrValue.BuckDuty = CtrValue.BUCKMaxDuty;	
			if(CtrValue.BuckDuty < MIN_BUKC_DUTY)
				CtrValue.BuckDuty = MIN_BUKC_DUTY;
			break;
		}				
		case  Boost: //Boost
		{
			// Формула расчета PID регулятора
			u0 = u1 + VErr0*BOOSTPIDb0 + VErr1*BOOSTPIDb1 + VErr2*BOOSTPIDb2;			
			// История
			VErr2 = VErr1;
			VErr1 = VErr0;
			u1 = u0;			
			// Формирование необходимых рабочих циклов
			CtrValue.BuckDuty = MAX_BUCK_DUTY; // Рабочий цикл 93%
			CtrValue.BoostDuty = u0>>8;	
			// Проверка на выходы за пределы
			if(CtrValue.BoostDuty > CtrValue.BoostMaxDuty)
				CtrValue.BoostDuty = CtrValue.BoostMaxDuty;	
			if(CtrValue.BoostDuty < MIN_BOOST_DUTY)
				CtrValue.BoostDuty = MIN_BOOST_DUTY;
			break;
		}
		case  Mix: //Mix
		{
			// Формула расчета PID регулятора
			u0 = u1 + VErr0*BOOSTPIDb0 + VErr1*BOOSTPIDb1 + VErr2*BOOSTPIDb2;			
			// История
			VErr2 = VErr1;
			VErr1 = VErr0;
			u1 = u0;			
			// Формирование необходимых рабочих циклов
			CtrValue.BuckDuty = MAX_BUCK_DUTY1; // Фиксированный рабочий цикл 80%
			CtrValue.BoostDuty = u0>>8;	
			// Проверка на выходы за пределы
			if(CtrValue.BoostDuty > CtrValue.BoostMaxDuty)
				CtrValue.BoostDuty = CtrValue.BoostMaxDuty;	
			if(CtrValue.BoostDuty < MIN_BOOST_DUTY)
				CtrValue.BoostDuty = MIN_BOOST_DUTY;
			break;
		}	
	}
	
	//PWMENFlag - флаг включения генерации сигнала
	if(DF.PWMENFlag==0)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	// Обновление регистров HRTIM
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = CtrValue.BuckDuty * PERIOD>>12; //buck
  HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP3xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR>>1; //ADC триггер
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP1xR = PERIOD - (CtrValue.BoostDuty * PERIOD>>12); //Boost
}

