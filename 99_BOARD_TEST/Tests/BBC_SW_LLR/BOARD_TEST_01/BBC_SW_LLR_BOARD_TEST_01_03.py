# LLR IDs: BBC_SW_LLR_BOARD_TEST_01_03
import gdb

# Последовательность действий (см. описание требования).
try:
    # Подключаемся и делаем сброс.
    gdb.execute( 'cmds connect_both_mcu; set $result = false' )

    # Устанавливаем ограничение по времени.
    gdb.execute( 'break_main_and_set_timeout 5' )

    # Ставим точку останова на функцию Error_Handler().
    gdb.execute( 'break Error_Handler' )

    # Переходим на функцию HAL_ADCEx_MultiModeConfigChannel().
    gdb.execute( 'break HAL_ADCEx_MultiModeConfigChannel '
        'if ( hadc == & hadc1 ) && ( multimode->Mode == ADC_MODE_INDEPENDENT )' )
    gdb.execute( 'continue' )

    if gdb.selected_frame().name() == 'HAL_ADCEx_MultiModeConfigChannel':

        # Проверяем параметры функции.
        params = [ 'hadc == & hadc1', 'multimode->Mode == ADC_MODE_INDEPENDENT' ]

        result = all( map( gdb.parse_and_eval, params ) )

        # Принудительно возвращаем значение ошибки.
        gdb.execute( 'cmds return HAL_ERROR; finish' )

        # Проверяем нахождение внутри функции Error_Handler().
        result = result and gdb.selected_frame().name() == 'Error_Handler'

        # Значение $result: true - тест пройден, false - не пройден.
        gdb.set_convenience_variable( 'result', result )

except: 
    # Обработка исключений: $result = false.
    gdb.set_convenience_variable( 'result', False )

finally:
    # Отключаемся и выходим.
    gdb.execute( 'disconnect_and_quit' )
