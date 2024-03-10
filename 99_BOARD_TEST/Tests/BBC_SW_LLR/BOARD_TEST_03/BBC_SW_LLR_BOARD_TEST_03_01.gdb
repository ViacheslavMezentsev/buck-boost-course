# LLR IDs: BBC_SW_LLR_BOARD_TEST_03_01

# Подключаемся и делаем сброс.
connect_both_mcu

# Значение $result: true - тест пройден, false - не пройден.
set $result = false

# Последовательность действий (см. описание требования).

# Устанавливаем ограничение по времени.
break_main_and_set_timeout 5

# Выполняем функцию MX_SPI1_Init().
break MX_SPI1_Init
continue
finish

# Проверяем настройки АЦП после инициализации.    
set $result = ( hspi1.Instance == SPI1 )

set $init = hspi1.Init

set $result = $result \
    && $init.Mode == SPI_MODE_MASTER \
    && $init.Direction == SPI_DIRECTION_2LINES \
    && $init.DataSize == SPI_DATASIZE_8BIT \
    && $init.CLKPolarity == SPI_POLARITY_HIGH \
    && $init.CLKPhase == SPI_PHASE_2EDGE \
    && $init.NSS == SPI_NSS_SOFT \
    && $init.BaudRatePrescaler == SPI_BAUDRATEPRESCALER_4 \
    && $init.FirstBit == SPI_FIRSTBIT_MSB \
    && $init.TIMode == SPI_TIMODE_DISABLE \
    && $init.CRCCalculation == SPI_CRCCALCULATION_DISABLE \
    && $init.CRCPolynomial == 7 \
    && $init.CRCLength == SPI_CRC_LENGTH_DATASIZE \
    && $init.NSSPMode == SPI_NSS_PULSE_DISABLE

# Выполняем тест и отключаемся.
disconnect_and_quit
