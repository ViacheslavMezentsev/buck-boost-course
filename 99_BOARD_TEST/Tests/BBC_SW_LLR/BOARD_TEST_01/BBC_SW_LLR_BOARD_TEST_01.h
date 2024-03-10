#ifndef BBC_SW_LLR_BOARD_TEST_01_H
#define BBC_SW_LLR_BOARD_TEST_01_H

#define UNITY_OUTPUT_CHAR(a)        SEGGER_RTT_PutChar(0,a)
#define UNITY_OUTPUT_FLUSH()
#define UNITY_OUTPUT_START()
#define UNITY_OUTPUT_COMPLETE()
#define UNITY_OUTPUT_COLOR            

#include "Unity/unity.h"

void BBC_SW_LLR_BOARD_TEST_01_01( void );

#endif
