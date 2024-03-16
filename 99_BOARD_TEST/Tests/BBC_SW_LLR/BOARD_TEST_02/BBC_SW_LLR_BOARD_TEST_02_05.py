# LLR IDs: BBC_SW_LLR_BOARD_TEST_02_05
import gdb

# Последовательность действий (см. описание требования).
try:
    # Подключаемся и делаем сброс.
    gdb.execute( 'cmds connect_both_mcu; set $result = false' )

    # Проверяем наличие символа.
    if gdb.lookup_global_symbol( 'HAL_UART_MspDeInit' ) is None:
        gdb.execute( 'disconnect_and_quit' )

    # Устанавливаем ограничение по времени.
    gdb.execute( 'break_main_and_set_timeout 5' )

    # Переходим на функцию HAL_UART_MspDeInit().
    gdb.execute( 'break HAL_UART_MspDeInit if ( uartHandle->Instance == USART1 )' )
    gdb.execute( 'continue' )

    # Переходим на функцию HAL_GPIO_DeInit().
    gdb.execute( 'cmds break HAL_GPIO_DeInit; continue' )

    # Проверяем результат вызова макроса __HAL_RCC_USART1_CLK_DISABLE()
    result = gdb.parse_and_eval( '__HAL_RCC_USART1_IS_CLK_DISABLED()' )

    # Проверяем значения параметров функции HAL_GPIO_DeInit():    
    # - первый равен GPIOB;
    # - второй равен значению выражения GPIO_PIN_6|GPIO_PIN_7.
    params = [ 'GPIOx == GPIOB', 'GPIO_Pin == ( GPIO_PIN_6 | GPIO_PIN_7 )' ]
    
    result = result and all( map( gdb.parse_and_eval, params ) )

    # Значение $result: true - тест пройден, false - не пройден.
    gdb.set_convenience_variable( 'result', result )
    gdb.execute( 'finish' )

except: 
    # Обработка исключений: $result = false.
    gdb.set_convenience_variable( 'result', False )

finally:
    # Отключаемся и выходим.
    gdb.execute( 'disconnect_and_quit' )
