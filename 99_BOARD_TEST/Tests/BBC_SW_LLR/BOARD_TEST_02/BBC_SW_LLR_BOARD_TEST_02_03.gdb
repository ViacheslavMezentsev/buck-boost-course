# LLR IDs: BBC_SW_LLR_BOARD_TEST_02_03

# Подключаемся и делаем сброс.
connect_both_mcu

# Значение $result: true - тест пройден, false - не пройден.
set $result = false

# Последовательность действий (см. описание требования).

# Устанавливаем ограничение по времени.
break_main_and_set_timeout 5

# Переходим на функцию HAL_UART_MspInit().
break HAL_UART_MspInit
continue

if $_caller_is( "HAL_UART_MspInit", 0 )

    # Проверяем значение поля Instance входного параметра uartHandle.
    set $result = ( uartHandle->Instance == USART1 )

    # Завершаем функцию HAL_UART_MspInit().
    finish
    
    # Проверяем результат вызова макроса __HAL_RCC_USART1_CLK_ENABLE()
    set $result = __HAL_RCC_USART1_IS_CLK_ENABLED()

    # Проверяем результат вызова макроса __HAL_RCC_GPIOB_CLK_ENABLE()
    set $result = $result && __HAL_RCC_GPIOB_IS_CLK_ENABLED()

end

# Выполняем тест и отключаемся.
disconnect_and_quit
