# LLR IDs: BBC_SW_LLR_BOARD_TEST_01_03

# Подключаемся и делаем сброс.
connect_both_mcu

# Значение $result: true - тест пройден, false - не пройден.
set $result = false

# Последовательность действий (см. описание требования).

# Устанавливаем ограничение по времени.
break_main_and_set_timeout 5

# Ставим точку останова на функцию Error_Handler().
break Error_Handler

# Переходим на функцию HAL_ADCEx_MultiModeConfigChannel().
break HAL_ADCEx_MultiModeConfigChannel \
    if ( hadc == & hadc1 ) && ( multimode->Mode == ADC_MODE_INDEPENDENT )
continue

if $_caller_is( "HAL_ADCEx_MultiModeConfigChannel", 0 )

    # Проверяем параметры функции.
    set $result = hadc == & hadc1 \
        && multimode->Mode == ADC_MODE_INDEPENDENT

    # Принудительно возвращаем значение ошибки.
    return HAL_ERROR
    finish

    # Проверяем нахождение внутри функции Error_Handler().
    set $result = $result && $_caller_is( "Error_Handler", 0 )

end

# Выполняем тест и отключаемся.
disconnect_and_quit
