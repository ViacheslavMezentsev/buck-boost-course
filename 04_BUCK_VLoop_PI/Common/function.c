/* USER CODE BEGIN Header */
	
/* USER CODE END Header */

#include "function.h"  // Основные функции
#include "CtlLoop.h"   // Цикл управления
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct  _ADI SADC = {0,0,2048,0,0,0,0,2048,0,0,0,0};       // Выборка и усреднение входных и выходных параметров
struct  _Ctr_value CtrValue = {0,0,0,MIN_BUKC_DUTY,0,0,0}; // Параметры управления
struct  _FLAG DF = {0,0,0,0,0,0,0,0};                      // Контрольные флаги
uint16_t ADC1_RESULT[4]={0,0,0,0};                         // Регистр сохранения данных DMA из периферийного устройства выборки АЦП в память
SState_M 	STState = SSInit;                                // Флаг состояния плавного пуска
uint16_t OLEDShowCnt = 0;                                  // Счетчик обновления экрана 
/*
** ===================================================================
**     Funtion Name :   void ADCSample(void)
**     Description :    Функция получения значений напрежяения и токов
**     Parameters  :
**     Returns     :
** ===================================================================
*/
CCMRAM void ADCSample(void)
{
	// Суммы для вычисления среднего значения 
	static uint32_t VinAvgSum=0,
                  IinAvgSum=0,
	                VoutAvgSum=0,
	                IoutAvgSum=0;
	
	// Чтение данных и линейная коррекция
	SADC.Vin  = ((uint32_t )ADC1_RESULT[0]*CAL_VIN_K>>12)+CAL_VIN_B;
	SADC.Iin  = (((int32_t )ADC1_RESULT[1]-SADC.IinOffset)*CAL_IIN_K>>12)+CAL_IIN_B;
	SADC.Vout = ((uint32_t )ADC1_RESULT[2]*CAL_VOUT_K>>12)+CAL_VOUT_B;
	SADC.Iout = (((int32_t )ADC1_RESULT[3] - SADC.IoutOffset)*CAL_IOUT_K>>12)+CAL_IOUT_B;

	if(SADC.Vin <100 ) // Напряжение на входе слишком мало
		SADC.Vin = 0;	
	if(SADC.Iin < 0 )  
		SADC.Iin = 0;
	if(SADC.Vout <100 )
		SADC.Vout = 0;
	if(SADC.Iout <0 )
		SADC.Iout = 0;

	// Вычисление средних значений. Метод скользящего среднего
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
**     Description :   Функция конечного автомата. Запускается прерыванием таймера каждые 5 миллисекунд
**     Parameters  :
**     Returns     :
** ===================================================================*/
void StateM(void)
{
	switch(DF.SMFlag)
	{
		// Начальное состояние
		case  Init :StateMInit();
		break;
		// Ожидание запуска
		case  Wait :StateMWait();
		break;
		// Запуск
		case  Rise :StateMRise();
		break;
		// Работа
		case  Run :StateMRun();
		break;
		// Ошибка, неисправность
		case  Err :StateMErr();
		break;
	}
}

/** ===================================================================
**     Funtion Name :void StateMInit(void)
**     Description :   Инициализация параметров
**     Parameters  :
**     Returns     :
** ===================================================================*/
void StateMInit(void)
{
  // Инициализация значений
	ValInit();
	// Установка начального флага состояния
	DF.SMFlag  = Wait;
}

/** ===================================================================
**     Funtion Name :void StateMWait(void)
**     Description :   Ожидание запуска. Ждем 1 сек и начинаем плавный запуск
**     Parameters  :
**     Returns     :
** ===================================================================*/
void StateMWait(void)
{
  // Счетчики
	static uint16_t CntS = 0;
	static uint32_t	IinSum = 0,
		              IoutSum = 0;
	
	// Остановка генерации PWM
	DF.PWMENFlag=0;
	// Накопительный счетчик
	CntS ++;
	// Ожидаем установку тока и если неисправности не обнаружены готовимся к запуску
	if(CntS > 256)
	{
		CntS = 256;
		HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); // Запускаем генерацию сигнала PWM
		if((DF.ErrFlag==F_NOERR)&&(DF.KeyFlag1==1))
		{
			// Сбрасываем счетчики
			CntS = 0;
			IinSum = 0;
			IoutSum = 0;
			// Меняем флаг состояния
			DF.SMFlag  = Rise;
			// Начинаем мягкий старт
			STState = SSInit;
		}
	}
	else // Среднее значение смещения 1.65V
	{
	  // Суммирование
    IinSum += ADC1_RESULT[1];
		IoutSum += ADC1_RESULT[3];
    // 256 раз
    if(CntS==256)
    {
      // Среднее
			SADC.IinOffset = IinSum >>8;
      SADC.IoutOffset = IoutSum >>8;
    }	
	}
}

/*
** ===================================================================
**     Funtion Name : void StateMRise(void)
**     Description  : Фаза плавного запуска
**     Инициализация плавного запуска - ожидание - запуск
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
#define MAX_SSCNT       20  // Максимальное время 100ms

void StateMRise(void)
{
	// Таймер
	static  uint16_t  Cnt = 0;
	// Счетчики максимальных рабочих циклов
	static  uint16_t	BUCKMaxDutyCnt = 0,
		                BoostMaxDutyCnt = 0;

	// Состояние плавного пуска
	switch(STState)
	{
		case    SSInit:
		{
			// Отключаем PWM
			DF.PWMENFlag=0;
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); // Останов
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); // Останов
			// Начинаем запуск с минимальных значений цикла
			CtrValue.BUCKMaxDuty  = MIN_BUKC_DUTY;
			CtrValue.BoostMaxDuty = MIN_BOOST_DUTY;
			// Переходим в состояние плавного пуска
			STState = SSWait;
			break;
		}
		case    SSWait:
		{
			// Накопительный счетчик
			Cnt++;
			// ожидаем 100ms
			if(Cnt> MAX_SSCNT)
			{
				// Обнуляем счетчик
				Cnt = 0;
				// Ограничиваем рабочий цикл включаем PWM
				CtrValue.BuckDuty = MIN_BUKC_DUTY;
				CtrValue.BUCKMaxDuty= MIN_BUKC_DUTY;
				CtrValue.BoostDuty = MIN_BOOST_DUTY;
				CtrValue.BoostMaxDuty = MIN_BOOST_DUTY;
				HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); // Включаем выход и таймер
				HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); // Включаем выход и таймер										
				STState = SSRun;	// Начинаем плавный пуск			
			}
			break;
		}
		case    SSRun:
		{
			// Устанавливаем флаг
			DF.PWMENFlag=1;
			// Постепенно увеличиваем пределы рабочих циклов
			BUCKMaxDutyCnt++;
			BoostMaxDutyCnt++;
			// Максимальное накопление рабочего цикла
			CtrValue.BUCKMaxDuty = CtrValue.BUCKMaxDuty + BUCKMaxDutyCnt*5;
			CtrValue.BoostMaxDuty = CtrValue.BoostMaxDuty + BoostMaxDutyCnt*5;
			// Ограничение
			if(CtrValue.BUCKMaxDuty > MAX_BUCK_DUTY)
				CtrValue.BUCKMaxDuty  = MAX_BUCK_DUTY ;
			if(CtrValue.BoostMaxDuty > MAX_BOOST_DUTY)
				CtrValue.BoostMaxDuty  = MAX_BOOST_DUTY ;
			
			if((CtrValue.BUCKMaxDuty==MAX_BUCK_DUTY)&&(CtrValue.BoostMaxDuty==MAX_BOOST_DUTY))			
			{
				// Переводим автомат состояний в состояние работает
				DF.SMFlag  = Run;
				// Мягкий старт переводим в состояние инициализации
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
**     Description : Основная функция рабочего цикла вызывается из прерывания
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
**     Description : Неисправность
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
void StateMErr(void)
{
	// Отключение PWM
	DF.PWMENFlag=0;
	HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); // Останов
	HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); // Останов
	// Если ошибка устранена то возвращаемся в состояние мягкого старта
	if(DF.ErrFlag==F_NOERR)
			DF.SMFlag  = Wait;
}

/** ===================================================================
**     Funtion Name :void ValInit(void)
**     Description :  Инициализация связанных параметров
**     Parameters  :
**     Returns     :
** ===================================================================*/
void ValInit(void)
{
	// Отключаем PWM
	DF.PWMENFlag=0;
	HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); // Останов
	HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); // Останов
	// Очистка флага ошибки
	DF.ErrFlag=0;
	// Устанавливаем ограничения рабочего цикла
	CtrValue.BuckDuty = MIN_BUKC_DUTY;
	CtrValue.BUCKMaxDuty= MIN_BUKC_DUTY;
	CtrValue.BoostDuty = MIN_BOOST_DUTY;
	CtrValue.BoostMaxDuty = MIN_BOOST_DUTY;
}

/** ===================================================================
**     Funtion Name :void VrefGet(void)
**     Description :  Получение опорного напряжения для регулирования
**     Parameters  :
**     Returns     :
** ===================================================================*/
#define MAX_VREF    2921  // Максимальное выходное опорное напряжение 48 В 0,5 В запас 48,5 В/68 В*Q12
#define MIN_VREF    271   // Минимальное опорное значение напряжения 5 В 0,5 В запас 4,5 В/68 В*2^Q12
#define VREF_K      10    // Шаг

void VrefGet(void)
{
	// Промежуточная переменная опорного значения напряжения
	int32_t VTemp = 0;	
	// Скользящее среднее
	static int32_t VadjSum = 0;

	// Получить значение выборки АЦП - прочитать напряжение
	SADC.Vadj = HAL_ADC_GetValue(&hadc2);
	// Скольжение и усреднение выборочных значений
	VadjSum = VadjSum + SADC.Vadj -(VadjSum>>8);
	SADC.VadjAvg = VadjSum>>8;
	
	// Опорное напряжение
	VTemp = MIN_VREF + SADC.Vadj;
	
	// Медленно подстраиваемся
	if( VTemp> ( CtrValue.Voref + VREF_K))
			CtrValue.Voref = CtrValue.Voref + VREF_K;
	else if( VTemp < ( CtrValue.Voref - VREF_K ))
			CtrValue.Voref =CtrValue.Voref - VREF_K;
	else
			CtrValue.Voref = VTemp ;

	// Проверяем предел регулирования
	if(CtrValue.Voref >((SADC.VinAvg*3482)>>12))
		CtrValue.Voref =((SADC.VinAvg*3482)>>12);
}

/*
** ===================================================================
**     Funtion Name :void ShortOff(void)
**     Description : Реакция на короткое замыкание
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
#define MAX_SHORT_I     1396  // Ток короткого замыкания
#define MIN_SHORT_V     289   // Напряжение короткого замыкания

void ShortOff(void)
{
	static int32_t RSCnt = 0;
	static uint8_t RSNum =0 ;

	// Если выходной ток больше а напряжение ниже то это КЗ
	if((SADC.Iout> MAX_SHORT_I)&&(SADC.Vout <MIN_SHORT_V))
	{
		// Отключить PWM
		DF.PWMENFlag=0;
		HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); // Останов
		HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); // Останов
		// Установить флаг ошибки
		setRegBits(DF.ErrFlag,F_SW_SHORT);
		// Перейти в ошибку
		DF.SMFlag  =Err;
	}
	// Восстановление после КЗ
	// Подождать 4 секунды очистить ошибку и перейти к запуску
	if(getRegBits(DF.ErrFlag,F_SW_SHORT))
	{
		// Ожидаем исчезновения ошибки
		RSCnt++;
		// Примерно 2 секунды
		if(RSCnt >400)
		{
			// Сброс счетчика
			RSCnt=0;
			// Номер попытки перезапуска. Не больше 10 попыток
			if(RSNum > 10)
			{
				// Устанавливаем 11 чтобы больше не перезапустить
				RSNum =11;
				// Отключаем PWM
				DF.PWMENFlag=0;
				HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); // Останов
				HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); // Останов			
			}
			else
			{
				// Накопление счетчика
				RSNum++;
				// Сброс флага
				clrRegBits(DF.ErrFlag,F_SW_SHORT);
			}
		}
	}
}

/*
** ===================================================================
**     Funtion Name :void SwOCP(void)
**     Description : Защита от перегрузки по току (перезапускаемая)
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
#define MAX_OCP_VAL     1117  // Точка защиты
void SwOCP(void)
{
	// Определения счетчиков превышения тока и сброса защиты
	static  uint16_t  OCPCnt = 0;
	static  uint16_t  RSCnt = 0;
	static  uint16_t  RSNum = 0;

	// Когда выходной ток больше ограничесния дольше чем 50ms
	if((SADC.Iout > MAX_OCP_VAL)&&(DF.SMFlag  ==Run))
	{
		// Увеличиваем счетчик
		OCPCnt++;
		// Если состояние сохраняется дольше чем 50 миллисекунд считаем что защита сработала
		if(OCPCnt > 10)
		{
			OCPCnt  = 0;
			// Останавливаем PWM
			DF.PWMENFlag=0;
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); 
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); 							
			// Устанавливаем флаг защиты
			setRegBits(DF.ErrFlag,F_SW_IOUT_OCP);
			// Устанавливаем флаг ошибки
			DF.SMFlag  =Err;
		}
	}
	else
		// Очищаем счетчик
		OCPCnt  = 0;

	// Восстановление после срабатывания
	// Ожидаем 4 секунды перед очисткой ошибки и перезапуска
	if(getRegBits(DF.ErrFlag,F_SW_IOUT_OCP))
	{
		// Увеличиваем счетчик
		RSCnt++;
		// Ожидаем 2 секунды
		if(RSCnt > 400)
		{
			RSCnt=0;
			// Количесьво перегрузок по току
			RSNum++;
			// Если ошибок больше 10 то блокируемся
			if(RSNum > 10 )
			{
				// Блокируем защиту от сброса
				RSNum =11;
				// Останавливаем PWM
				DF.PWMENFlag = 0;
				HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); 
				HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); 				
			}
			else
			{
			 // Очищаем флаг защиты
				clrRegBits(DF.ErrFlag,F_SW_IOUT_OCP);
			}
		}
	}
}

/*
** ===================================================================
**     Funtion Name :void SwOVP(void)
**     Description : Защита от превышения напряжения на выходе
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
#define MAX_VOUT_OVP_VAL    3012 // Защита от перенапряжения 50 В (50/68) *Q12

void VoutSwOVP(void)
{
	// Счетчик
	static  uint16_t  OVPCnt=0;

	// Проверяем что напряжение превышено 10ms
	if (SADC.Vout > MAX_VOUT_OVP_VAL)
	{
		// Увеличиваем счетчик
		OVPCnt++;
		// 10 мсек
		if(OVPCnt > 2)
		{
			// Сбрасываем счетчик
			OVPCnt=0;
			// Останавливаем PWM
			DF.PWMENFlag=0;
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); 
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); 									
			// Устанавливаем флаг защиты
			setRegBits(DF.ErrFlag,F_SW_VOUT_OVP);
			// Устанавливаем состояние аварии
			DF.SMFlag  =Err;
		}
	}
	else
		OVPCnt = 0;
}

/*
** ===================================================================
**     Funtion Name :void VinSwUVP(void)
**     Description : Защита от пониженного напряжения на входе (восстанавливаемая)
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
#define MIN_UVP_VAL    686 // Защита от пониженного напряжения 11,4 В (11,4/68) *Q12
#define MIN_UVP_VAL_RE  795 //Восстановление защиты от пониженного напряжения 13,2 В (13,2/68) *Q12

void VinSwUVP(void)
{
	// Счетчики
	static  uint16_t  UVPCnt = 0;
	static  uint16_t	RSCnt = 0;

	// Когда входное напряжение ниже 11.4V дольше чем 200ms
	if ((SADC.Vin < MIN_UVP_VAL) && (DF.SMFlag != Init ))
	{
		UVPCnt++;
		// Время истекло 10ms
		if(UVPCnt > 2)
		{
			UVPCnt = 0;
			RSCnt = 0;
			// Остановка PWM
			DF.PWMENFlag=0;
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); // Останов
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); // Останов						
			// Установка флага
			setRegBits(DF.ErrFlag,F_SW_VIN_UVP);
			// Состояние ошибки
			DF.SMFlag  =Err;
		}
	}
	else
		UVPCnt = 0;
	
	// Восстановление защиты
	// Если напряжение вернется к заданному значению то сбрасываем ошибку и перезапускаемся
	if(getRegBits(DF.ErrFlag,F_SW_VIN_UVP))
	{
		if(SADC.Vin > MIN_UVP_VAL_RE) 
		{
			RSCnt++;
			// Ожидаем 1S
			if(RSCnt > 200)
			{
				RSCnt = 0;
				UVPCnt = 0;
				// Очищаем флаг защиты
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
**     Description : Защита от перенапряжения на входе (без перезапуска)
**     Parameters  : none
**     Returns     : none
** ===================================================================
*/
#define MAX_VIN_OVP_VAL    3012  // Защита от перенапряжения 50 В (50/68) *Q12
void VinSwOVP(void)
{
	static  uint16_t  OVPCnt = 0;

	// Когда входное напряжение больше 50 В в течение 100 мс
	if ((SADC.Vin*6800>>12) > MAX_VIN_OVP_VAL )
	{
		OVPCnt++;
		if(OVPCnt > 2)
		{
			OVPCnt = 0;
			// Отключаем PWM
			DF.PWMENFlag = 0;
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); 
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); 									
			// Установка бита защиты
			setRegBits(DF.ErrFlag,F_SW_VIN_OVP);
			// Переход в режим ошибки
			DF.SMFlag  =Err;
		}
	}
	else
		OVPCnt = 0;
}

/** ===================================================================
**     Funtion Name :void LEDShow(void)
**     Description :  Состояние LED
**     Parameters  :
**     Returns     :
** ===================================================================*/
// Макросы изменения состояния диодов
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
		//Статус инициализации, красный, желтый и зеленый
		case  Init :
		{
			SET_LED_G();
			SET_LED_Y();
			SET_LED_R();
			break;
		}
		// Состояние ожидания, красный, желтый и зеленый горят
		case  Wait :
		{
			SET_LED_G();
			SET_LED_Y();
			SET_LED_R();
			break;
		}
		//Состояние плавного пуска, желтый и зеленый
		case  Rise :
		{
			SET_LED_G();
			SET_LED_Y();
			CLR_LED_R();
			break;
		}
		//рабочее состояние, горит зеленый
		case  Run :
		{
			SET_LED_G();
			CLR_LED_Y();
			CLR_LED_R();
			break;
		}
		//состояние неисправности, горит красный
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
**     Description : Опрос кнопок
**     Parameters  :
**     Returns     :
** ===================================================================*/
#define READ_KEY1() HAL_GPIO_ReadPin(GPIOB, KEY_1_Pin)
#define READ_KEY2() HAL_GPIO_ReadPin(GPIOB, KEY_2_Pin)

void KEYFlag(void)
{
	static uint16_t	KeyDownCnt1 = 0,
		              KeyDownCnt2 = 0;
	
	// Кнопка нажата
	if(READ_KEY1()==0)
	{
		// Кнопка нажата и ждем 150 мс
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
	
	// Кнопка нажата
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
		// Отключаем PWM
		DF.PWMENFlag=0;
		HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); 
		HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); 		
	}
}

/** ===================================================================
**     Funtion Name :void BBMode(void)
**     Description : Оценка режима работы
**     Parameters  :
**     Returns     :
** ===================================================================*/
void BBMode(void)
{
	DF.BBFlag = Buck; //buck mode
}

/** ===================================================================
**     Funtion Name :void MX_OLED_Init(void)
**     Description : Инициализируем экран и основные надписи
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
**     Description : Отображение параметров на экране
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
	
	// Преобразование выборочного значения в фактическое значение и расширение его в 100 раз
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
