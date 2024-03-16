# LLR IDs: BBC_SW_LLR_BOARD_TEST_01_05
import gdb

# Последовательность действий (см. описание требования).
try:
    # Подключаемся и делаем сброс.
    gdb.execute( 'cmds connect_both_mcu; set $result = false' )

    # Устанавливаем ограничение по времени.
    gdb.execute( 'break_main_and_set_timeout 5' )

    # Ставим точку останова на функцию Error_Handler().
    gdb.execute( 'break Error_Handler' )

    # Переходим на функцию HAL_ADC_ConfigChannel().
    gdb.execute( 'break HAL_ADC_ConfigChannel '
        'if ( hadc == & hadc1 ) && ( sConfig->Channel == ADC_CHANNEL_2 )' )
    gdb.execute( 'continue' )

    if gdb.selected_frame().name() == 'HAL_ADC_ConfigChannel':

        # Проверяем параметры функции.
        params = [ 'sConfig->Channel == ADC_CHANNEL_2',
            'sConfig->Rank == ADC_REGULAR_RANK_2',
            'sConfig->SingleDiff == ADC_SINGLE_ENDED',
            'sConfig->SamplingTime == ADC_SAMPLETIME_4CYCLES_5',
            'sConfig->OffsetNumber == ADC_OFFSET_NONE',
            'sConfig->Offset == 0' ]

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
