#include <string.h>
#include "adc.h"
#include "tim.h"
#include "function.h"
#include "st7789.h"
#include "main.h"

extern uint8_t pics[];

/**
 * \brief   Выполняет инициализацию.
 *
 */
void init( void )
{
}


/**
 * \brief   Выполняет дополнительные настройки.
 *
 */
void setup( void )
{
	HAL_GPIO_WritePin( IPS_BLK_GPIO_Port, IPS_BLK_Pin, GPIO_PIN_SET );

	ST7789_Init( 240, 240 );
	ST7789_FillScreen( BLACK );
  	ST7789_SetBL( 100 );

	uint8_t buffer[20];
  
  	for (uint8_t y=0; y<160; y++)
  	{
		memcpy(buffer, &pics[(y)*20], 20);

		for (uint8_t x=0; x<20; x++)
    	{		
		  	for(uint8_t i=0; i<8; i++)
		  	{
				if (buffer[x] & (0x80>>i)) ST7789_DrawPixel(40 + (x*8) + i, y + 20, YELLOW);
			}
		}
  	}

	ST7789_print_7x11( 60, 200, YELLOW, BLACK, 0, ( char * ) "MCUINSIDE.RU" );
 
  	HAL_Delay( 5000 );
	
	ST7789_FillScreen( BLACK );

	// Инициализируем экран и основные надписи.
	MX_OLED_Init();

	// 200Hz.
	HAL_TIM_Base_Start_IT( & htim3 );

	// Запускаем АЦП с DMA.
	HAL_ADC_Start_DMA( & hadc1, ( uint32_t * ) ADC1_RESULT, 4 );

	// Запускаем АЦП2.
	HAL_ADC_Start( & hadc2 );

	// Включаем выход ШИМ и таймер ШИМ.
	HAL_HRTIM_WaveformOutputStart( & hhrtim1, HRTIM_OUTPUT_TA1 | HRTIM_OUTPUT_TA2 );
	
	// Включаем выход ШИМ и таймер ШИМ.
	HAL_HRTIM_WaveformOutputStart( & hhrtim1, HRTIM_OUTPUT_TB1 | HRTIM_OUTPUT_TB2 );

	HAL_HRTIM_WaveformCountStart( & hhrtim1, HRTIM_TIMERID_TIMER_A );
	HAL_HRTIM_WaveformCountStart( & hhrtim1, HRTIM_TIMERID_TIMER_B );

	__HAL_HRTIM_TIMER_ENABLE_IT( & hhrtim1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_TIM_IT_REP );
}


/**
 * \brief   Выполняется периодически в теле основного цикла.
 *
 */
void loop( void )
{
	// Запуск обновления экрана каждые 300 миллисекунд
	if ( OLEDShowCnt > 60 )
	{
		OLEDShowCnt = 0;
		OLEDShow();
	}
}
