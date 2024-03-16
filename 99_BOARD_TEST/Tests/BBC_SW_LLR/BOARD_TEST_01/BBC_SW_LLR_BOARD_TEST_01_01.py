# LLR IDs: BBC_SW_LLR_BOARD_TEST_01_01
import gdb

# Последовательность действий (см. описание требования).
try:
    # Подключаемся и делаем сброс.
    gdb.execute( 'cmds connect_both_mcu; set $result = false' )

    # Устанавливаем ограничение по времени.
    gdb.execute( 'break_main_and_set_timeout 5' )

    # Выполняем функцию MX_ADC1_Init().
    gdb.execute( 'cmds break MX_ADC1_Init; continue; finish' )

    # Проверяем настройки АЦП после инициализации.    
    params = [ 'hadc1.Instance == ADC1',
        'hadc1.Init.ClockPrescaler == ADC_CLOCK_SYNC_PCLK_DIV4',
        'hadc1.Init.Resolution == ADC_RESOLUTION_12B',
        'hadc1.Init.ScanConvMode == ADC_SCAN_ENABLE',
        'hadc1.Init.ContinuousConvMode == DISABLE',
        'hadc1.Init.DiscontinuousConvMode == DISABLE',
        'hadc1.Init.ExternalTrigConvEdge == ADC_EXTERNALTRIGCONVEDGE_RISING',
        'hadc1.Init.ExternalTrigConv == ADC_EXTERNALTRIGCONVHRTIM_TRG1',
        'hadc1.Init.DataAlign == ADC_DATAALIGN_RIGHT',
        'hadc1.Init.NbrOfConversion == 4',
        'hadc1.Init.DMAContinuousRequests == ENABLE',
        'hadc1.Init.EOCSelection == ADC_EOC_SEQ_CONV',
        'hadc1.Init.LowPowerAutoWait == DISABLE',
        'hadc1.Init.Overrun == ADC_OVR_DATA_OVERWRITTEN' ]

    result = all( map( gdb.parse_and_eval, params ) )

    # Значение $result: true - тест пройден, false - не пройден.
    gdb.set_convenience_variable( 'result', result )

except: 
    # Обработка исключений: $result = false.
    gdb.set_convenience_variable( 'result', False )

finally:
    # Отключаемся и выходим.
    gdb.execute( 'disconnect_and_quit' )
