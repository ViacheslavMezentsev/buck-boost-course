#include "main.h"
#include "st7789.h"

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
	
	ST7789_DrawLine( 12, 14, 190, 190, RGB565( 200, 0, 200 ) );
}


/**
 * \brief   Выполняется периодически в теле основного цикла.
 *
 */
void loop( void )
{
}


/**
 * \brief   Callback-функция периодического таймера SysTick.
 *
 */
void HAL_SYSTICK_Callback( void )
{
}
