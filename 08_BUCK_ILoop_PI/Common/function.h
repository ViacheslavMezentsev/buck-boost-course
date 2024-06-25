#ifndef __FUNCTION_H
#define __FUNCTION_H	 

#include "stm32f3xx_hal.h"
#include "hrtim.h"
#include "st7789.h"
#include "adc.h"

extern uint16_t ADC1_RESULT[4];
extern struct  _ADI SADC;
extern struct  _Ctr_value  CtrValue;
extern struct  _FLAG    DF;
extern uint16_t OLEDShowCnt;

void ADCSample(void);
void StateM(void);
void StateMInit(void);
void StateMWait(void);
void StateMRise(void);
void StateMRun(void);
void StateMErr(void);
void ValInit(void);
void IrefGet(void);
void ShortOff(void);
void SwOCP(void);
void VoutSwOVP(void);
void VinSwUVP(void);
void VinSwOVP(void);
void LEDShow(void);
void KEYFlag(void);
void BBMode(void);
void OLEDShow(void);
void MX_OLED_Init(void);

/***************** НЕИСПРАВНОСТИ *****************/
#define     F_NOERR      			0x0000 // нет ошибки
#define     F_SW_VIN_UVP  		0x0001 // низкое входное напряжение
#define     F_SW_VIN_OVP    	0x0002 // высокое входное напряжение
#define     F_SW_VOUT_UVP  		0x0004 // низкое напряжение на выходе
#define     F_SW_VOUT_OVP    	0x0008 // высокое напряжение на выходе
#define     F_SW_IOUT_OCP    	0x0010 // перегрузка по току на выходе
#define     F_SW_SHORT  			0x0020 // короткое замыкание на выходе·

/***************** КОНСТАНТЫ *****************/
#define MIN_BUKC_DUTY	80     // Минимальный рабочий цикл BUCK
#define MAX_BUCK_DUTY 3809   // Максимальный рабочий цикл BUCK, 93%*Q12
#define	MAX_BUCK_DUTY1 3277  // В режиме MIX BUCK имеет фиксированный рабочий цикл 80 %
#define MIN_BOOST_DUTY 80    // Минимальный рабочий цикл BOOST
#define MIN_BOOST_DUTY1 283  // Минимальная нагрузка BOOST 7%
#define MAX_BOOST_DUTY	2662 // максимальный рабочий цикл 65% максимальный рабочий цикл
#define MAX_BOOST_DUTY1	3809 // Максимальный рабочий цикл BUCK, 93% * Q12

// Структура измерений
struct _ADI
{
	int32_t   Iout;       // Выходной ток
	int32_t   IoutAvg;    // Выходной ток средний
	int32_t		IoutOffset; // Выходной ток Текущее значение дискретизации
	int32_t   Vout;       // Выходное напряжение
	int32_t   VoutAvg;    // Выходное напряжение среднее
	int32_t   Iin;        // Входной ток
	int32_t   IinAvg;     // Входной ток среднее
	int32_t		IinOffset;  // Входной ток Текущее значение дискретизации
	int32_t   Vin;        // Входное напряжение
	int32_t   VinAvg;     // Входное напряжение среднее
	int32_t   Vadj;       // Напряжение с подстроечного резистора
	int32_t   VadjAvg;    // Напряжение с подстроечного резистора среднее
};

/***************** КАЛИБРОВКА *****************/
#define CAL_VOUT_K 4068  // Q12 коррекция выходного напряжения K значение
#define CAL_VOUT_B 59    // Q12 коррекция выходного напряжения B значение
#define CAL_IOUT_K 4096  // Q12 значение коррекции выходного тока K
#define CAL_IOUT_B 0     // Значение коррекции выходного тока Q12 B
#define CAL_VIN_K 4101   // Q12 коррекция входного напряжения K значение
#define CAL_VIN_B 49     // Q12 коррекция входного напряжения B значение
#define CAL_IIN_K 4096   // Q12 значение коррекции входного тока K
#define CAL_IIN_B 0      // значение коррекции входного тока Q12 B

struct  _Ctr_value
{
	int32_t		Voref;        // Опорное напряжение
	int32_t		Ioref;        // Опорный ток
	int32_t		ILimit;       // Ограничение опорного тока
	int16_t		BUCKMaxDuty;  // Buck максимальный рабочий цикл
	int16_t		BoostMaxDuty; // Boost максимальный рабочий цикл
	int16_t		BuckDuty;     // Buck рабочий цикл
	int16_t		BoostDuty;    // Boost рабочий цикл
	int32_t		Ilimitout;    // Лимит выходного тока
};

struct  _FLAG
{
	uint16_t	SMFlag;      // Флаг машины состояний
	uint16_t	CtrFlag;     // Контрольный флаг
	uint16_t  ErrFlag;     // Флаг ошибки
	uint8_t	BBFlag;        // Флаг режима работы преобразователя	
	uint8_t PWMENFlag;     // Стартовый флаг
	uint8_t KeyFlag1;      // Ключ 1
	uint8_t KeyFlag2;      // Ключ 2	
	uint8_t BBModeChange;  // Флаг переключения рабочего режима
};

// Машина состояний
typedef enum
{
    Init, // Инициализация
    Wait, // Ожидание
    Rise, // Мягкий старт
    Run,  // Работа
    Err   // Ошибка
}STATE_M;

// Режим преобразования
typedef enum
{
    NA,     // Не определено
		Buck,   // BUCK
    Boost,  // BOOST
    Mix     // MIX смешанный
}BB_M;

// Стадии мягкого пуска
typedef enum
{
	SSInit,  // Инициализация
	SSWait,  // Ожидание
	SSRun    // Плавный пуск
}SState_M;

#define setRegBits(reg, mask)   (reg |= (unsigned int)(mask))
#define clrRegBits(reg, mask)  	(reg &= (unsigned int)(~(unsigned int)(mask)))
#define getRegBits(reg, mask)   (reg & (unsigned int)(mask))
#define getReg(reg)           	(reg)

#define CCMRAM  __attribute__((section("ccmram")))

#endif
