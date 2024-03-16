# LLR IDs: BBC_SW_LLR_BOARD_TEST_03_01
import gdb

# Последовательность действий (см. описание требования).
try:
    # Подключаемся и делаем сброс.
    gdb.execute( 'cmds connect_both_mcu; set $result = false' )

    # Устанавливаем ограничение по времени.
    gdb.execute( 'break_main_and_set_timeout 5' )

    # Выполняем функцию MX_SPI1_Init().
    gdb.execute( 'cmds break MX_SPI1_Init; continue; finish' )

    # Проверяем настройки АЦП после инициализации.    
    params = [ 'hspi1.Instance == SPI1',
        'hspi1.Init.Mode == SPI_MODE_MASTER',
        'hspi1.Init.Direction == SPI_DIRECTION_2LINES',
        'hspi1.Init.DataSize == SPI_DATASIZE_8BIT',
        'hspi1.Init.CLKPolarity == SPI_POLARITY_HIGH',
        'hspi1.Init.CLKPhase == SPI_PHASE_2EDGE',
        'hspi1.Init.NSS == SPI_NSS_SOFT',
        'hspi1.Init.BaudRatePrescaler == SPI_BAUDRATEPRESCALER_4',
        'hspi1.Init.FirstBit == SPI_FIRSTBIT_MSB',
        'hspi1.Init.TIMode == SPI_TIMODE_DISABLE',
        'hspi1.Init.CRCCalculation == SPI_CRCCALCULATION_DISABLE',
        'hspi1.Init.CRCPolynomial == 7',
        'hspi1.Init.CRCLength == SPI_CRC_LENGTH_DATASIZE',
        'hspi1.Init.NSSPMode == SPI_NSS_PULSE_DISABLE' ]

    result = all( map( gdb.parse_and_eval, params ) )

    # Значение $result: true - тест пройден, false - не пройден.
    gdb.set_convenience_variable( 'result', result )

except: 
    # Обработка исключений: $result = false.
    gdb.set_convenience_variable( 'result', False )

finally:
    # Отключаемся и выходим.
    gdb.execute( 'disconnect_and_quit' )
