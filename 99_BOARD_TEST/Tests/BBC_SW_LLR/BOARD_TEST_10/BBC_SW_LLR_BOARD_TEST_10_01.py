# LLR IDs: BPSD_SW_LLR_MFONO_10_01
import gdb

# Последовательность действий (см. описание требования).
try:
    # Подключаемся и делаем сброс.
    gdb.execute( 'cmds connect_both_mcu; set $result = false' )

    # Устанавливаем ограничение по времени.
    gdb.execute( 'break_main_and_set_timeout 5' )

    # Проверяем вызов функции SystemClock_Config().
    gdb.execute( 'cmds break SystemClock_Config; continue' )

    if gdb.selected_frame().name() == 'SystemClock_Config':

        # Завершаем работу функции.
        gdb.execute( 'finish' )

        result = gdb.parse_and_eval( 'SystemCoreClock' ) == 64000000

        # Значение $result: true - тест пройден, false - не пройден.
        gdb.set_convenience_variable( 'result', result )

except: 
    # Обработка исключений: $result = false.
    gdb.set_convenience_variable( 'result', False )

finally:
    # Отключаемся и выходим.
    gdb.execute( 'disconnect_and_quit' )
