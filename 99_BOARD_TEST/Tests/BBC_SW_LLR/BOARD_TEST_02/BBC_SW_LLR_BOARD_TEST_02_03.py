# LLR IDs: BBC_SW_LLR_BOARD_TEST_02_03
import gdb

# Последовательность действий (см. описание требования).
try:
    # Подключаемся и делаем сброс.
    gdb.execute( 'cmds connect_both_mcu; set $result = false' )

    # Устанавливаем ограничение по времени.
    gdb.execute( 'break_main_and_set_timeout 5' )

    # Переходим на функцию HAL_UART_MspInit().
    gdb.execute( 'cmds break HAL_UART_MspInit; continue' )

    if gdb.selected_frame().name() == 'HAL_UART_MspInit':

        # Проверяем значение поля Instance входного параметра uartHandle.
        result = gdb.parse_and_eval( 'uartHandle->Instance == USART1' )

        # Завершаем функцию HAL_UART_MspInit().
        gdb.execute( 'finish' )
        
        # Проверяем результат вызова макросов __HAL_RCC_USART1_CLK_ENABLE() и
        # __HAL_RCC_GPIOA_CLK_ENABLE().
        items = [ '__HAL_RCC_USART1_IS_CLK_ENABLED()',
            '__HAL_RCC_GPIOB_IS_CLK_ENABLED()' ]

        result = result and all( map( gdb.parse_and_eval, items ) )

        # Значение $result: true - тест пройден, false - не пройден.
        gdb.set_convenience_variable( 'result', result )

except: 
    # Обработка исключений: $result = false.
    gdb.set_convenience_variable( 'result', False )

finally:
    # Отключаемся и выходим.
    gdb.execute( 'disconnect_and_quit' )