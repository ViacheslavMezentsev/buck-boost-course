# LLR IDs: BBC_SW_LLR_BOARD_TEST_02_02
import gdb

# Последовательность действий (см. описание требования).
try:
    # Подключаемся и делаем сброс.
    gdb.execute( 'cmds connect_both_mcu; set $result = false' )

    # Устанавливаем ограничение по времени.
    gdb.execute( 'break_main_and_set_timeout 5' )

    # Ставим точку останова на функцию Error_Handler().
    gdb.execute( 'break Error_Handler' )

    # Переходим на функцию HAL_UART_Init().
    gdb.execute( 'cmds break HAL_UART_Init; continue' )

    if gdb.selected_frame().name() == 'HAL_UART_Init':

        # Изменяем значение передаваемого параметра и
        # завершаем работу функций HAL_UART_Init() и MX_USART1_UART_Init().
        gdb.execute( 'cmds set huart = NULL; finish; finish' )

        # Проверяем нахождение внутри функции Error_Handler().
        result = gdb.selected_frame().name() == 'Error_Handler'

        # Значение $result: true - тест пройден, false - не пройден.
        gdb.set_convenience_variable( 'result', result )

except: 
    # Обработка исключений: $result = false.
    gdb.set_convenience_variable( 'result', False )

finally:
    # Отключаемся и выходим.
    gdb.execute( 'disconnect_and_quit' )
