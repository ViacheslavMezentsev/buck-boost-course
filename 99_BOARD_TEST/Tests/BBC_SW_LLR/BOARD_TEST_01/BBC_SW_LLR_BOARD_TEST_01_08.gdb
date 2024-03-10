# LLR IDs: BBC_SW_LLR_BOARD_TEST_01_08

# Подключаемся и делаем сброс.
connect_both_mcu

# Значение $result: true - тест пройден, false - не пройден.
set $result = false

# Последовательность действий (см. описание требования).

# Устанавливаем ограничение по времени.
break_main_and_set_timeout 5

# Выполняем функцию MX_ADC2_Init().
break MX_ADC2_Init
continue
finish

# Проверяем настройки АЦП после инициализации.    
set $result = ( hadc2.Instance == ADC2 )

set $init = hadc2.Init

set $result = $result \
    && $init.ClockPrescaler == ADC_CLOCK_SYNC_PCLK_DIV4 \
    && $init.Resolution == ADC_RESOLUTION_12B \
    && $init.ScanConvMode == ADC_SCAN_DISABLE \
    && $init.ContinuousConvMode == ENABLE \
    && $init.DiscontinuousConvMode == DISABLE \
    && $init.ExternalTrigConvEdge == ADC_EXTERNALTRIGCONVEDGE_NONE \
    && $init.ExternalTrigConv == ADC_SOFTWARE_START \
    && $init.DataAlign == ADC_DATAALIGN_RIGHT \
    && $init.NbrOfConversion == 1 \
    && $init.DMAContinuousRequests == DISABLE \
    && $init.EOCSelection == ADC_EOC_SINGLE_CONV \
    && $init.LowPowerAutoWait == DISABLE \
    && $init.Overrun == ADC_OVR_DATA_OVERWRITTEN

# Выполняем тест и отключаемся.
disconnect_and_quit
