/* USER CODE BEGIN Header */
	
/* USER CODE END Header */
#include "CtlLoop.h"

/**************** Переменные **********************/
int32_t   VErr0 = 0, VErr1 = 0, VErr2 = 0, VErr3 = 0; // Ошибка по напряжению Q12
int32_t		u0 = 0, u1 = 0, u2 = 0, u3 = 0;             // Выход напряжения

/*
** ===================================================================
**     Funtion Name :  void BoostVLoopCtlTYPE3(void)
**     Description  :   Расчет контура напряжения — расчет контура напряжения типа 3 — (см. документ по расчету контура BUCK-TYPE3)
**     Parameters   :
**     Returns      :
** ===================================================================
*/

// Параметры регулятора
#define BOOSTTYPETb0	6144		//Q8
#define BOOSTTYPETb1	-5952		//Q8
#define BOOSTTYPETb2	-6141		//Q8
#define BOOSTTYPETb3	5955		//Q8
#define BOOSTTYPETa1	5080		//Q12
#define BOOSTTYPETa2	-1043		//Q12
#define BOOSTTYPETa3	59			//Q12

CCMRAM void BoostVLoopCtlTYPE3(void)
{
	int32_t VoutTemp = 0; //После коррекции выходного напряжения
	
	// Коррекция выходного напряжения
	VoutTemp = ((uint32_t )ADC1_RESULT[2]*CAL_VOUT_K>>12)+CAL_VOUT_B;
	// Рассчет ошибки напряжения. Если опорное напряжение превышает выходное напряжение, рабочий цикл увеличивается
	VErr0 = CtrValue.Voref  - VoutTemp;
	// Расчет PID
	u0 = (( u1*BOOSTTYPETa1 + u2*BOOSTTYPETa2 + u3*BOOSTTYPETa3)>>12) + ((VErr0*BOOSTTYPETb0 + VErr1*BOOSTTYPETb1 + VErr2*BOOSTTYPETb2 + VErr3*BOOSTTYPETb3)>>8 );	
	// Исторические данные об амплитуде
	VErr3 = VErr2;
	VErr2 = VErr1;
	VErr1 = VErr0;
	u3 = u2;
	u2 = u1;
	u1 = u0;
	// Формирование управляющего значения
	CtrValue.BuckDuty = MAX_BUCK_DUTY;//???????93%
	CtrValue.BoostDuty = u0;			
	// Минимальные и максимальные пределы
	if(CtrValue.BoostDuty > CtrValue.BoostMaxDuty)
		CtrValue.BoostDuty = CtrValue.BoostMaxDuty;	
	if(CtrValue.BoostDuty < MIN_BOOST_DUTY)
		CtrValue.BoostDuty = MIN_BOOST_DUTY;
	//PWMENFlag — бит флага включения ШИМ. Когда этот бит равен 0, рабочий цикл = 0 и выходной сигнал отсутствует
	if(DF.PWMENFlag==0)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	// Обновляем данные в регистрах
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = CtrValue.BuckDuty * PERIOD>>12; // buck рабочий цикл
  HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP3xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR>>1; // Точка выборки ADC
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP1xR = PERIOD - (CtrValue.BoostDuty * PERIOD>>12); // Boost рабочий цикл
}

