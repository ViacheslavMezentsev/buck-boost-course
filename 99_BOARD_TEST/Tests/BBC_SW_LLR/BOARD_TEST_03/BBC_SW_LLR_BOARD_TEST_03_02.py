# LLR IDs: BBC_SW_LLR_BOARD_TEST_03_02
import gdb

# Последовательность действий (см. описание требования).
try:
    # Подключаемся и делаем сброс.
    gdb.execute( 'cmds connect_both_mcu; set $result = false' )

    # Устанавливаем ограничение по времени.
    gdb.execute( 'break_main_and_set_timeout 5' )

    # Ставим точку останова на функцию Error_Handler().
    gdb.execute( 'break Error_Handler' )

    # Переходим на функцию HAL_SPI_Init().
    gdb.execute( 'break HAL_SPI_Init if hspi == & hspi1' )
    gdb.execute( 'continue' )
    
    if gdb.selected_frame().name() == 'HAL_SPI_Init':

        # Изменяем значение передаваемого параметра.
        gdb.execute( 'set hspi = NULL' )

        # Завершаем работу функций HAL_SPI_Init() и MX_SPI1_Init().
        gdb.execute( 'cmds finish; finish' )

        # Проверяем нахождение внутри функции Error_Handler().
        result = gdb.selected_frame().name() == 'Error_Handler'

        # Значение $result: true - тест пройден, false - не пройден.
        gdb.set_convenience_variable( 'result', result )

except: 
    # Обработка исключений: $result = false.
    gdb.set_convenience_variable( 'result', False )

finally:
    # Отключаемся и выходим.
    gdb.execute( 'disconnect_and_quit' )
