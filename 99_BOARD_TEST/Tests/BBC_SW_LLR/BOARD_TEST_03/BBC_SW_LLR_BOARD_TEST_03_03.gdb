# LLR IDs: BBC_SW_LLR_BOARD_TEST_03_03

# Подключаемся и делаем сброс.
connect_both_mcu

# Значение $result: true - тест пройден, false - не пройден.
set $result = false

# Последовательность действий (см. описание требования).

# Устанавливаем ограничение по времени.
break_main_and_set_timeout 5

# Переходим на функцию HAL_SPI_MspInit().
break HAL_SPI_MspInit
continue

if $_caller_is( "HAL_SPI_MspInit", 0 )

    # Проверяем значение поля Instance входного параметра spiHandle.
    set $result = ( spiHandle->Instance == SPI1 )

    # Завершаем функцию HAL_SPI_MspInit().
    finish
    
    # Проверяем результат вызова макроса __HAL_RCC_SPI1_CLK_ENABLE().
    set $result = __HAL_RCC_SPI1_IS_CLK_ENABLED()

    # Проверяем результат вызова макроса __HAL_RCC_GPIOA_CLK_ENABLE().
    set $result = $result && __HAL_RCC_GPIOA_IS_CLK_ENABLED()

end

# Выполняем тест и отключаемся.
disconnect_and_quit
