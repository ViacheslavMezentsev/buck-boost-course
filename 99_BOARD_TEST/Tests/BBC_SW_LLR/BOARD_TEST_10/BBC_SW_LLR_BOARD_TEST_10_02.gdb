# LLR IDs: BPSD_SW_LLR_MFONO_10_56

# Подключаемся и делаем сброс.
connect_both_mcu

# Значение $result: true - тест пройден, false - не пройден.
set $result = false

# Последовательность действий (см. описание требования).

break_main_and_continue

# Ставим точку останова на функцию Error_Handler().
break Error_Handler
#break 'main.c':177

# Переходим на функцию HAL_RCC_OscConfig().
break HAL_RCC_OscConfig
continue

if $_caller_is( "HAL_RCC_OscConfig", 0 )

    # Формируем признак ошибки.
    set RCC_OscInitStruct = NULL

    # Завершаем работу функций HAL_RCC_OscConfig() и SystemClock_Config().
    continue

    # Проверяем наличие бесконечного цикла.
    set $result = ( 0xE7FE == * ( uint16_t * ) $pc )

end

# Выполняем тест и отключаемся.
disconnect_and_quit
