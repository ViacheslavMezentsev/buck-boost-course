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

/***************** ������������� *****************/
#define     F_NOERR      			0x0000 // ��� ������
#define     F_SW_VIN_UVP  		0x0001 // ������ ������� ����������
#define     F_SW_VIN_OVP    	0x0002 // ������� ������� ����������
#define     F_SW_VOUT_UVP  		0x0004 // ������ ���������� �� ������
#define     F_SW_VOUT_OVP    	0x0008 // ������� ���������� �� ������
#define     F_SW_IOUT_OCP    	0x0010 // ���������� �� ���� �� ������
#define     F_SW_SHORT  			0x0020 // �������� ��������� �� ������

/***************** ��������� *****************/
#define MIN_BUKC_DUTY	80     // ����������� ������� ���� BUCK
#define MAX_BUCK_DUTY 3809   // ������������ ������� ���� BUCK, 93%*Q12
#define	MAX_BUCK_DUTY1 3277  // � ������ MIX BUCK ����� ������������� ������� ���� 80 %
#define MIN_BOOST_DUTY 80    // ����������� ������� ���� BOOST
#define MIN_BOOST_DUTY1 283  // ����������� �������� BOOST 7%
#define MAX_BOOST_DUTY	2662 // ������������ ������� ���� 65% ������������ ������� ����
#define MAX_BOOST_DUTY1	3809 // ������������ ������� ���� BUCK, 93% * Q12

// ��������� ���������
struct _ADI
{
	int32_t   Iout;       // �������� ���
	int32_t   IoutAvg;    // �������� ��� �������
	int32_t		IoutOffset; // �������� ��� ������� �������� �������������
	int32_t   Vout;       // �������� ����������
	int32_t   VoutAvg;    // �������� ���������� �������
	int32_t   Iin;        // ������� ���
	int32_t   IinAvg;     // ������� ��� �������
	int32_t		IinOffset;  // ������� ��� ������� �������� �������������
	int32_t   Vin;        // ������� ����������
	int32_t   VinAvg;     // ������� ���������� �������
	int32_t   Vadj;       // ���������� � ������������� ���������
	int32_t   VadjAvg;    // ���������� � ������������� ��������� �������
};

/***************** ���������� *****************/
#define CAL_VOUT_K 4068  // Q12 ��������� ��������� ���������� K ��������
#define CAL_VOUT_B 59    // Q12 ��������� ��������� ���������� B ��������
#define CAL_IOUT_K 4096  // Q12 �������� ��������� ��������� ���� K
#define CAL_IOUT_B 0     // �������� ��������� ��������� ���� Q12 B
#define CAL_VIN_K 4101   // Q12 ��������� �������� ���������� K ��������
#define CAL_VIN_B 49     // Q12 ��������� �������� ���������� B ��������
#define CAL_IIN_K 4096   // Q12 �������� ��������� �������� ���� K
#define CAL_IIN_B 0      // �������� ��������� �������� ���� Q12 B

struct  _Ctr_value
{
	int32_t		Voref;        // ������� ����������
	int32_t		Ioref;        // ������� ���
	int32_t		ILimit;       // ����������� �������� ����
	int16_t		BUCKMaxDuty;  // Buck ������������ ������� ����
	int16_t		BoostMaxDuty; // Boost ������������ ������� ����
	int16_t		BuckDuty;     // Buck ������� ����
	int16_t		BoostDuty;    // Boost ������� ����
	int32_t		Ilimitout;    // ����� ��������� ����
};

struct  _FLAG
{
	uint16_t	SMFlag;      // ���� ������ ���������
	uint16_t	CtrFlag;     // ����������� ����
	uint16_t  ErrFlag;     // ���� ������
	uint8_t	BBFlag;        // ���� ������ ������ ���������������	
	uint8_t PWMENFlag;     // ��������� ����
	uint8_t KeyFlag1;      // ���� 1
	uint8_t KeyFlag2;      // ���� 2	
	uint8_t BBModeChange;  // ���� ������������ �������� ������
};

// ������ ���������
typedef enum
{
    Init, // �������������
    Wait, // ��������
    Rise, // ������ �����
    Run,  // ������
    Err   // ������
}STATE_M;

// ����� ��������������
typedef enum
{
    NA,     // �� ����������
		Buck,   // BUCK
    Boost,  // BOOST
    Mix     // MIX ���������
}BB_M;

// ������ ������� �����
typedef enum
{
	SSInit,  // �������������
	SSWait,  // ��������
	SSRun    // ������� ����
}SState_M;

#define setRegBits(reg, mask)   (reg |= (unsigned int)(mask))
#define clrRegBits(reg, mask)  	(reg &= (unsigned int)(~(unsigned int)(mask)))
#define getRegBits(reg, mask)   (reg & (unsigned int)(mask))
#define getReg(reg)           	(reg)

#define CCMRAM  __attribute__((section("ccmram")))

#endif
