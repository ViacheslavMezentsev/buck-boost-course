/* USER CODE BEGIN Header */
	
/* USER CODE END Header */
#include "CtlLoop.h"

/**************** Переменные **********************/
int32_t   VErr0 = 0, VErr1 = 0, VErr2 = 0, VErr3 = 0; //Ошибка по напряжению Q12
int32_t		u0 = 0, u1 = 0, u2 = 0, u3 = 0; //Выход напряжения

/*
** ===================================================================
**     Funtion Name :  void BUCKVLoopCtlTYPE3(void)
**     Description  :   Расчет контура напряжения — расчет контура напряжения типа 3 — (см. документ по расчету контура BUCK-TYPE3)
**     Parameters   :
**     Returns      :
** ===================================================================
*/

// Эти коэффициенты были посчитаны в Machcad
#define BUCKTYPETb0	4995		//Q8
#define BUCKTYPETb1	-4838		//Q8
#define BUCKTYPETb2	-4992		//Q8
#define BUCKTYPETb3	4838		//Q8
#define BUCKTYPETa1	5079		//Q12
#define BUCKTYPETa2	-1043		//Q12
#define BUCKTYPETa3	60			//Q12

CCMRAM void BUCKVLoopCtlTYPE3(void)
{
	int32_t VoutTemp = 0; //После коррекции выходного напряжения
	
	// Коррекция выходного напряжения
	VoutTemp = ((uint32_t )ADC1_RESULT[2]*CAL_VOUT_K>>12)+CAL_VOUT_B;
	// Рассчет ошибки напряжения. Если опорное напряжение превышает выходное напряжение, рабочий цикл увеличивается
	VErr0 = CtrValue.Voref  - VoutTemp;
	// Расчет PID
	u0 = (( u1*BUCKTYPETa1 + u2*BUCKTYPETa2 + u3*BUCKTYPETa3)>>12) + ((VErr0*BUCKTYPETb0 + VErr1*BUCKTYPETb1 + VErr2*BUCKTYPETb2 + VErr3*BUCKTYPETb3)>>8);	
	// Исторические данные об амплитуде
	VErr3 = VErr2;
	VErr2 = VErr1;
	VErr1 = VErr0;
	u3 = u2;
	u2 = u1;
	u1 = u0;
	// Формирование управляющего значения
	CtrValue.BuckDuty = u0;
	CtrValue.BoostDuty = MIN_BOOST_DUTY1; // BOOST имеет фиксированный рабочий цикл 93% для верхнего ключа и 7% для нижнего ключа			
	// Минимальные и максимальные пределы
	if(CtrValue.BuckDuty > CtrValue.BUCKMaxDuty)
		CtrValue.BuckDuty = CtrValue.BUCKMaxDuty;	
	if(CtrValue.BuckDuty < MIN_BUKC_DUTY)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	//PWMENFlag — бит флага включения ШИМ. Когда этот бит равен 0, рабочий цикл = 0 и выходной сигнал отсутствует
	if(DF.PWMENFlag==0)
		CtrValue.BuckDuty = MIN_BUKC_DUTY;
	// Обновляем данные в регистрах
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = CtrValue.BuckDuty * PERIOD>>12; // buck рабочий цикл
  HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP3xR = HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR>>1; // Точка выборки ADC
	HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP1xR = PERIOD - (CtrValue.BoostDuty * PERIOD>>12); // Boost рабочий цикл
}

