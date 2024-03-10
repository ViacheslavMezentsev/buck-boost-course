# LLR IDs: BBC_SW_LLR_BOARD_TEST_03_02

# Подключаемся и делаем сброс.
connect_both_mcu

# Значение $result: true - тест пройден, false - не пройден.
set $result = false

# Последовательность действий (см. описание требования).

# Устанавливаем ограничение по времени.
break_main_and_set_timeout 5

# Ставим точку останова на функцию Error_Handler().
break Error_Handler

# Переходим на функцию HAL_SPI_Init().
break HAL_SPI_Init if ( hspi == & hspi1 )
continue

if $_caller_is( "HAL_SPI_Init", 0 )

    # Изменяем значение передаваемого параметра.
    set hspi = NULL

    # Завершаем работу функций HAL_ADC_Init() и MX_ADC1_Init().
    finish
    finish

    # Проверяем нахождение внутри функции Error_Handler().
    set $result = $_caller_is( "Error_Handler", 0 )

end

# Выполняем тест и отключаемся.
disconnect_and_quit
