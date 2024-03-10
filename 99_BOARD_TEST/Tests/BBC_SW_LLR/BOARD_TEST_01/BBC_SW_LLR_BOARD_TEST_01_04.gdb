# LLR IDs: BBC_SW_LLR_BOARD_TEST_01_04

# Подключаемся и делаем сброс.
connect_both_mcu

# Значение $result: true - тест пройден, false - не пройден.
set $result = false

# Последовательность действий (см. описание требования).

# Устанавливаем ограничение по времени.
break_main_and_set_timeout 5

# Ставим точку останова на функцию Error_Handler().
break Error_Handler

# Переходим на функцию HAL_ADC_ConfigChannel().
break HAL_ADC_ConfigChannel \
    if ( hadc == & hadc1 ) && ( sConfig->Channel == ADC_CHANNEL_1 )
continue

if $_caller_is( "HAL_ADC_ConfigChannel", 0 )

    # Проверяем параметры функции.
    set $result = sConfig->Channel == ADC_CHANNEL_1 \
        && sConfig->Rank == ADC_REGULAR_RANK_1 \
        && sConfig->SingleDiff == ADC_SINGLE_ENDED \
        && sConfig->SamplingTime == ADC_SAMPLETIME_4CYCLES_5 \
        && sConfig->OffsetNumber == ADC_OFFSET_NONE \
        && sConfig->Offset == 0

    # Принудительно возвращаем значение ошибки.
    return HAL_ERROR
    finish

    # Проверяем нахождение внутри функции Error_Handler().
    set $result = $result && $_caller_is( "Error_Handler", 0 )

end

# Выполняем тест и отключаемся.
disconnect_and_quit
