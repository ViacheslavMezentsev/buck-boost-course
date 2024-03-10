# LLR IDs:BBC_SW_LLR_BOARD_TEST_02_04
import gdb

# Подключаемся и делаем сброс.
gdb.execute( 'cmds connect_both_mcu; set $result = false' )

# Последовательность действий (см. описание требования).
try:
    # Устанавливаем ограничение по времени.
    gdb.execute( 'break_main_and_set_timeout 5' )

    # Переходим на функцию HAL_GPIO_Init().
    for func in [ 'HAL_SPI_MspInit', 'HAL_GPIO_Init' ]:
        gdb.execute( 'cmds break ' + func + '; continue' )

    params = [ 'GPIOx == GPIOA',
        'GPIO_Init->Pin == ( GPIO_PIN_5 | GPIO_PIN_7 )',
        'GPIO_Init->Mode == GPIO_MODE_AF_PP',
        'GPIO_Init->Pull == GPIO_NOPULL',
        'GPIO_Init->Speed == GPIO_SPEED_FREQ_HIGH',
        'GPIO_Init->Alternate == GPIO_AF5_SPI1' ]

    result = all( map( gdb.parse_and_eval, params ) )

    # Значение $result: true - тест пройден, false - не пройден.
    gdb.set_convenience_variable( 'result', result )

# Обработка исключений: $result = false.
except gdb.error: gdb.set_convenience_variable( 'result', False )

# Отключаемся и выходим.
gdb.execute( 'disconnect_and_quit' )
