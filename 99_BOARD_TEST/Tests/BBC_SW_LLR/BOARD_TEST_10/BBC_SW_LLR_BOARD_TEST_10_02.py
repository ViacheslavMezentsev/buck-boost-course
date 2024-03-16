# LLR IDs: BPSD_SW_LLR_MFONO_10_02
import gdb
import re

# Последовательность действий (см. описание требования).
try:
    # Подключаемся и делаем сброс.
    gdb.execute( 'cmds connect_both_mcu; set $result = false' )

    # Устанавливаем ограничение по времени.
    gdb.execute( 'break_main_and_continue' )

    # FIXME: Нужно поставить точку останова на инструкцию 'b.n'.
    # Ставим точку останова на функцию Error_Handler().
    gdb.execute( 'break Error_Handler' )
    #gdb.execute( 'break \'main.c\':177' )

    # Переходим на функцию HAL_RCC_OscConfig().
    gdb.execute( 'cmds break HAL_RCC_OscConfig; continue' )

    if gdb.selected_frame().name() == 'HAL_RCC_OscConfig':

        # Формируем признак ошибки и завершаем работу функций 
        # HAL_RCC_OscConfig() и SystemClock_Config().
        gdb.execute( 'cmds set RCC_OscInitStruct = NULL; continue' )

        # Проверяем наличие бесконечного цикла.
        # Ниже эквивалент операции: * (uint16_t *) $pc == 0xE7FE.
        frame = gdb.selected_frame()
        instruction = frame.architecture().disassemble( frame.pc() )[0]

        if instruction['asm'].startswith( 'b.n' ):

            # Узнаём адрес перехода.
            match = re.search( r'0[xX][0-9a-fA-F]+', instruction['asm'] )

            # Сравниваем с текущим адресом.
            result = hex( frame.pc() ) == match[0] if match else False

            # Значение $result: true - тест пройден, false - не пройден.
            gdb.set_convenience_variable( 'result', result )

except: 
    # Обработка исключений: $result = false.
    gdb.set_convenience_variable( 'result', False )

finally:
    # Отключаемся и выходим.
    gdb.execute( 'disconnect_and_quit' )
