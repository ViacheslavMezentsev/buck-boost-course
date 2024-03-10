# LLR IDs: BBC_SW_LLR_BOARD_TEST_02_05

# Подключаемся и делаем сброс.
connect_both_mcu

# Значение $result: true - тест пройден, false - не пройден.
set $result = false

# Последовательность действий (см. описание требования).

# Устанавливаем ограничение по времени.
break_main_and_set_timeout 5

# Быстрая проверка наличия функции.
if & HAL_UART_MspDeInit == NULL
    disconnect_and_quit
end

# Переходим на функцию HAL_UART_MspDeInit().
break HAL_UART_MspDeInit if ( uartHandle->Instance == USART1 )
continue

# Переходим на функцию HAL_GPIO_DeInit().
break HAL_GPIO_DeInit
continue

# Проверяем:
# - результат вызова макроса __HAL_RCC_USART1_CLK_DISABLE()
# - значения параметров функции HAL_GPIO_DeInit():
#  - первый равен GPIOA;
#  - второй равен значению выражения GPIO_PIN_6|GPIO_PIN_7.
if $_caller_is( "HAL_GPIO_DeInit", 0 )

    set $result = __HAL_RCC_USART1_IS_CLK_DISABLED() \
        && GPIOx == GPIOB \
        && GPIO_Pin == ( GPIO_PIN_6 | GPIO_PIN_7 )
   
    finish

end

# Выполняем тест и отключаемся.
disconnect_and_quit
