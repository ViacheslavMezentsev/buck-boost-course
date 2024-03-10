# LLR IDs: BBC_SW_LLR_BOARD_TEST_02_02

# Подключаемся и делаем сброс.
connect_both_mcu

# Значение $result: true - тест пройден, false - не пройден.
set $result = false

# Последовательность действий (см. описание требования).

# Устанавливаем ограничение по времени.
break_main_and_set_timeout 5

# Ставим точку останова на функцию Error_Handler().
break Error_Handler

# Переходим на функцию HAL_UART_Init().
break HAL_UART_Init
continue

if $_caller_is( "HAL_UART_Init", 0 )

    # Изменяем значение передаваемого параметра.
    set huart = NULL

    # Завершаем работу функций HAL_UART_Init() и MX_USART1_UART_Init().
    finish
    finish

    # Проверяем нахождение внутри функции Error_Handler().
    set $result = $_caller_is( "Error_Handler", 0 )

end  

# Выполняем тест и отключаемся.
disconnect_and_quit
