# LLR IDs: BBC_SW_LLR_BOARD_TEST_03_03
import gdb

# Подключаемся и делаем сброс.
gdb.execute( 'cmds connect_both_mcu; set $result = false' )

# Последовательность действий (см. описание требования).
try:
    # Устанавливаем ограничение по времени.
    gdb.execute( 'break_main_and_set_timeout 5' )

    # Переходим на функцию HAL_SPI_MspInit().
    gdb.execute( 'cmds break HAL_SPI_MspInit; continue' )

    if gdb.selected_frame().name() == 'HAL_SPI_MspInit':

        # Проверяем значение поля Instance входного параметра spiHandle.
        result = gdb.parse_and_eval( 'spiHandle->Instance == SPI1' )

        # Завершаем функцию HAL_SPI_MspInit().
        gdb.execute( 'finish' )
        
        # Проверяем результат вызова макросов __HAL_RCC_SPI1_CLK_ENABLE() и
        # __HAL_RCC_GPIOA_CLK_ENABLE().
        items = [ '__HAL_RCC_SPI1_IS_CLK_ENABLED()',
            '__HAL_RCC_GPIOA_IS_CLK_ENABLED()' ]

        result = result and all( map( gdb.parse_and_eval, items ) )

        # Значение $result: true - тест пройден, false - не пройден.
        gdb.set_convenience_variable( 'result', result )

# Обработка исключений: $result = false.
except gdb.error: gdb.set_convenience_variable( 'result', False )

# Отключаемся и выходим.
gdb.execute( 'disconnect_and_quit' )