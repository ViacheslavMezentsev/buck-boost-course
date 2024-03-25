# LLR IDs: BBC_SW_LLR_BOARD_TEST_01_08
import gdb

# Последовательность действий (см. описание требования).
try:
    # Подключаемся и делаем сброс.
    gdb.execute( 'cmds connect_both_mcu; set $result = false' )

    # Устанавливаем ограничение по времени.
    gdb.execute( 'break_main_and_set_timeout 5' )

    # Выполняем функцию MX_ADC2_Init().
    gdb.execute( 'cmds break MX_ADC2_Init; continue; finish' )

    # Проверяем настройки АЦП после инициализации.    
    params = [ 'hadc2.Instance == ADC2',
        'hadc2.Init.ClockPrescaler == ADC_CLOCK_SYNC_PCLK_DIV4',
        'hadc2.Init.Resolution == ADC_RESOLUTION_12B',
        'hadc2.Init.ScanConvMode == ADC_SCAN_DISABLE',
        'hadc2.Init.ContinuousConvMode = ENABLE',
        'hadc2.Init.DiscontinuousConvMode == DISABLE',
        'hadc2.Init.ExternalTrigConvEdge == ADC_EXTERNALTRIGCONVEDGE_NONE',
        'hadc2.Init.ExternalTrigConv == ADC_SOFTWARE_START',
        'hadc2.Init.DataAlign == ADC_DATAALIGN_RIGHT',
        'hadc2.Init.NbrOfConversion == 1',
        'hadc2.Init.DMAContinuousRequests == DISABLE',
        'hadc2.Init.EOCSelection == ADC_EOC_SINGLE_CONV',
        'hadc2.Init.LowPowerAutoWait == DISABLE',
        'hadc2.Init.Overrun == ADC_OVR_DATA_OVERWRITTEN' ]

    result = all( map( gdb.parse_and_eval, params ) )

    # Значение $result: true - тест пройден, false - не пройден.
    gdb.set_convenience_variable( 'result', result )

# Обработка исключений: $result = false.
except Exception as ex:
    gdb.set_convenience_variable( 'result', False )

finally:
    conn = gdb.selected_inferior().connection
    # Отключаемся и выходим.
    gdb.execute( ('quit !$result', 'disconnect_and_quit')[isinstance(conn, gdb.RemoteTargetConnection)] )
