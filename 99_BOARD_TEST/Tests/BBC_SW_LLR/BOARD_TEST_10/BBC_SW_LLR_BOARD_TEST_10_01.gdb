# LLR IDs: BPSD_SW_LLR_MFONO_10_08

# Подключаемся и делаем сброс.
connect_both_mcu

# Значение $result: true - тест пройден, false - не пройден.
set $result = false

# Последовательность действий (см. описание требования).

# Устанавливаем ограничение по времени.
break_main_and_set_timeout 5

# Проверяем вызов функции SystemClock_Config().
break *SystemClock_Config
continue

set $result = ( $pc == & SystemClock_Config )

# Завершаем работу функции.
finish

set $result = $result && ( SystemCoreClock == 64000000 )

# Выполняем тест и отключаемся.
disconnect_and_quit
