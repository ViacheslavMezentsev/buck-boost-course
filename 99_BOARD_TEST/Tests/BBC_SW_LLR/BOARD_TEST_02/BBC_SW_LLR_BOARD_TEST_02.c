#define UNITY_OUTPUT_CHAR(a)        SEGGER_RTT_PutChar(0,a)
#define UNITY_OUTPUT_FLUSH()
#define UNITY_OUTPUT_START()
#define UNITY_OUTPUT_COMPLETE()
#define UNITY_OUTPUT_COLOR

#include "Unity/unity.h"
#include "usart.h"

void BBC_SW_LLR_BOARD_TEST_02_01( void )
{
    extern void MX_USART1_UART_Init( void );

    MX_USART1_UART_Init();

    TEST_ASSERT_TRUE_MESSAGE( huart1.Instance == USART1, "huart1.Instance = USART1" );
    TEST_ASSERT_TRUE_MESSAGE( huart1.Init.BaudRate == 38400, "huart1.Init.BaudRate = 38400" );
    TEST_ASSERT_TRUE_MESSAGE( huart1.Init.WordLength == UART_WORDLENGTH_8B, "huart1.Init.WordLength = UART_WORDLENGTH_8B" );
    TEST_ASSERT_TRUE_MESSAGE( huart1.Init.StopBits == UART_STOPBITS_1, "huart1.Init.StopBits = UART_STOPBITS_1" );
    TEST_ASSERT_TRUE_MESSAGE( huart1.Init.Parity == UART_PARITY_NONE, "huart1.Init.Parity = UART_PARITY_NONE" );
    TEST_ASSERT_TRUE_MESSAGE( huart1.Init.Mode == UART_MODE_TX_RX, "huart1.Init.Mode = UART_MODE_TX_RX" );
    TEST_ASSERT_TRUE_MESSAGE( huart1.Init.HwFlowCtl == UART_HWCONTROL_NONE, "huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE" );
    TEST_ASSERT_TRUE_MESSAGE( huart1.Init.OverSampling == UART_OVERSAMPLING_16, "huart1.Init.OverSampling = UART_OVERSAMPLING_16" );
    TEST_ASSERT_TRUE_MESSAGE( huart1.Init.OneBitSampling == UART_ONE_BIT_SAMPLE_DISABLE, "huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE" );
    TEST_ASSERT_TRUE_MESSAGE( huart1.AdvancedInit.AdvFeatureInit == UART_ADVFEATURE_NO_INIT, "huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT" );
}
