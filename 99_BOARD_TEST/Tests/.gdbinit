# Настройки терминала.
set suppress-cli-notifications on
set confirm off

macro define false 0
macro define true !false

# Подключает и запускает MCU (только для J-Link).
define connect_and_run_mcu

    if $arg0 == 1
        target remote localhost:2331
    end

    if $arg0 == 2
        target remote localhost:2334
    end

    # Протестировано для J-Link OB.
    monitor reset

    break main
    continue

    monitor go

end


# Подключает только к MCU1.
define connect_mcu1_only

    if $mcu != 1        
        exit ( $mcu != 2 )
    end

    # Запускаем MCU2.
    connect_and_run_mcu 2

    # Подключаемся к MCU1.
    target remote localhost:2331

    # Выполняем сброс.
    monitor reset

end


# Подключает только к MCU2.
define connect_mcu2_only

    if $mcu != 2
        exit ( $mcu != 1 )
    end

    # Запускаем MCU1.
    connect_and_run_mcu 1

    # Подключаемся к MCU2.
    target remote localhost:2334

    # Выполняем сброс.
    monitor reset

end


# Подключает к разным MCU.
define connect_both_mcu

    if $mcu == 1
        connect_and_run_mcu 2

        # Подключаемся к MCU1.
        target remote localhost:2331
    else
        if $mcu == 2
            connect_and_run_mcu 1
            
            # Подключаемся к MCU1.
            target remote localhost:2334
        else
            exit 1
        end
    end

    # Выполняем сброс удалённого микроконтроллера.
    monitor reset

end


# Выполняет тест и отключает от MCU.
define test_and_disconnect

    # Значение $result: true - тест пройден, false - не пройден.
    set $result = false

    # Выполняем тест.
    test

    # Завершаем соединение.
    disconnect

    # Формируем код выхода, обратный значению $result.
    quit !$result

end


# Отключает от MCU и завершает работу.
define disconnect_and_quit

    # Завершаем соединение.
    disconnect

    # Формируем код выхода, обратный значению $result.
    quit !$result

end


# Выполняет переход на main().
define break_main_and_continue

    # Переходим на функцию main().
    break main
    continue
   
end


# Ставит точку останова на main() и ограничивает время теста.
define break_main_and_set_timeout

    # Переходим на функцию main().
    break main
    continue

    # Сохраняем текущее значение таймера HAL.
    set $uwTick = uwTick

    # Ставим точку останова по длительности исполнения (таймаут).
    watch uwTick > $uwTick + $arg0
    commands
    silent
    set $result = false
    disconnect_and_quit
    end    
end


# Отключает сторожевой таймер IWDG.
define iwdg_disable

    break HAL_IWDG_Init
    continue
    return HAL_OK
    finish
    
end

# multiple commands
# https://stackoverflow.com/questions/1262639/multiple-commands-in-gdb-separated-by-some-sort-of-delimiter
python
import gdb

class Cmds(gdb.Command):
  """run multiple commands separated by ';'"""
  def __init__(self):
    gdb.Command.__init__( self, "cmds", gdb.COMMAND_DATA, gdb.COMPLETE_SYMBOL, True, )

  def invoke(self, arg, from_tty):
    for fragment in arg.split(';'):
      # from_tty is passed in from invoke.
      # These commands should be considered interactive if the command
      # that invoked them is interactive.
      # to_string is false. We just want to write the output of the commands, not capture it.
      gdb.execute(fragment, from_tty=from_tty, to_string=False)

Cmds()
end
