#define UNITY_OUTPUT_CHAR(a)        SEGGER_RTT_PutChar(0,a)
#define UNITY_OUTPUT_FLUSH()
#define UNITY_OUTPUT_START()
#define UNITY_OUTPUT_COMPLETE()
#define UNITY_OUTPUT_COLOR

#include "Unity/unity.h"
#include "adc.h"

void BBC_SW_LLR_BOARD_TEST_01_01( void )
{
    extern void MX_ADC1_Init( void );

    MX_ADC1_Init();

    TEST_ASSERT_TRUE_MESSAGE( hadc1.Instance == ADC1, "hadc1.Instance = ADC1" );
    TEST_ASSERT_TRUE_MESSAGE( hadc1.Init.ClockPrescaler == ADC_CLOCK_SYNC_PCLK_DIV4, "hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4" );
    TEST_ASSERT_TRUE_MESSAGE( hadc1.Init.Resolution == ADC_RESOLUTION_12B, "hadc1.Init.Resolution = ADC_RESOLUTION_12B" );
    TEST_ASSERT_TRUE_MESSAGE( hadc1.Init.ScanConvMode == ADC_SCAN_ENABLE, "hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE" );
    TEST_ASSERT_TRUE_MESSAGE( hadc1.Init.ContinuousConvMode == DISABLE, "hadc1.Init.ContinuousConvMode = DISABLE" );
    TEST_ASSERT_TRUE_MESSAGE( hadc1.Init.DiscontinuousConvMode == DISABLE, "hadc1.Init.DiscontinuousConvMode = DISABLE" );
    TEST_ASSERT_TRUE_MESSAGE( hadc1.Init.ExternalTrigConvEdge == ADC_EXTERNALTRIGCONVEDGE_RISING, "hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING" );
    TEST_ASSERT_TRUE_MESSAGE( hadc1.Init.ExternalTrigConv == ADC_EXTERNALTRIGCONVHRTIM_TRG1, "hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONVHRTIM_TRG1" );
    TEST_ASSERT_TRUE_MESSAGE( hadc1.Init.DataAlign == ADC_DATAALIGN_RIGHT, "hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT" );
    TEST_ASSERT_TRUE_MESSAGE( hadc1.Init.NbrOfConversion == 4, "hadc1.Init.NbrOfConversion = 4" );
    TEST_ASSERT_TRUE_MESSAGE( hadc1.Init.DMAContinuousRequests == ENABLE, "hadc1.Init.DMAContinuousRequests = ENABLE" );
    TEST_ASSERT_TRUE_MESSAGE( hadc1.Init.EOCSelection == ADC_EOC_SEQ_CONV, "hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV" );
    TEST_ASSERT_TRUE_MESSAGE( hadc1.Init.LowPowerAutoWait == DISABLE, "hadc1.Init.LowPowerAutoWait = DISABLE" );
    TEST_ASSERT_TRUE_MESSAGE( hadc1.Init.Overrun == ADC_OVR_DATA_OVERWRITTEN, "hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN" );
}
