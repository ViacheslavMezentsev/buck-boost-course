# LLR IDs: BBC_SW_LLR_BOARD_TEST_01_02
import gdb

# Подключаемся и делаем сброс.
gdb.execute( 'cmds connect_both_mcu; set $result = false' )

# Последовательность действий (см. описание требования).
try:
    # Устанавливаем ограничение по времени.
    gdb.execute( 'break_main_and_set_timeout 5' )

    # Ставим точку останова на функцию Error_Handler().
    gdb.execute( 'break Error_Handler' )

    # Переходим на функцию HAL_ADC_Init().
    gdb.execute( 'break HAL_ADC_Init if hadc == & hadc1' )
    gdb.execute( 'continue' )
    
    if gdb.selected_frame().name() == 'HAL_ADC_Init':

        # Изменяем значение передаваемого параметра.
        gdb.execute( 'set hadc = NULL' )

        # Завершаем работу функций HAL_ADC_Init() и MX_ADC1_Init().
        gdb.execute( 'cmds finish; finish' )

        # Проверяем нахождение внутри функции Error_Handler().
        result = gdb.selected_frame().name() == 'Error_Handler'

        # Значение $result: true - тест пройден, false - не пройден.
        gdb.set_convenience_variable( 'result', result )

# Обработка исключений: $result = false.
except gdb.error: gdb.set_convenience_variable( 'result', False )

# Отключаемся и выходим.
gdb.execute( 'disconnect_and_quit' )
