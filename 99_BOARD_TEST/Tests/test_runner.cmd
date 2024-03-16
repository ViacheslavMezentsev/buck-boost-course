@echo off
:: Вариант 1. Запуск сервера, запуск клиента, выполнение теста, завершение клиента и сервера
::( start /min "" JLinkGDBServerCL.exe -device ${config:device} -if swd -port 2334 -swoport 2335 -telnetport 2336 -select usb=XXXXXXXXX -nogui -noir -novd -nosinglerun -silent ) ^
::    && ( arm-none-eabi-gdb.exe --iex "set $mcu=2" --quiet --symbols=%1 --command=%script% --batch-silent 2> nul )

set gdbscript=%~f2.gdb
set pyscript=%~f2.py
set gdbclient=arm-none-eabi-gdb.exe
set gdbinit=%~dp0.gdbinit
set gdbrunning=%~dp0BOARD_TEST_RUNNING.gdb

:: Ищем gdb-клиент с поддержкой python.
for %%n in (11.3.1-1.1, 12.2.1-1.1, 12.2.1-1.2, 12.3.1-1.1, 12.3.1-1.2, 13.2.1-1.1) do (
    if exist "%UserProfile%\xpack-arm-none-eabi-gcc-%%n\bin\arm-none-eabi-gdb-py3.exe" (
        set gdbclient="%UserProfile%\xpack-arm-none-eabi-gcc-%%n\bin\arm-none-eabi-gdb-py3.exe"
    ) 
)

:: Выбираем тип скрипта: .gdb или .py.
if "%~3"=="" (
    for %%f in (%gdbscript% %pyscript%) do if exist %%f set script=%%f
) else (
    set script=%~f2%3
)

:: Проверяем существование gdb-клиента, elf-файла и скрипта.
for %%f in (%gdbclient% %1 %script% %gdbinit% %gdbrunning%) do if not exist %%f ( echo File not found: "%%f" ) && exit 1

goto :test

:: Проверка значения кода выхода.
:ExitOnError
if %errorlevel% neq 0 ( exit %errorlevel% )
goto:eof

:: Вариант 2.
:: Запуск сервера(-ов) отдельно:
:: JLinkGDBServerCL.exe -device ${config:device} -if swd -port 2331 -swoport 2332 -telnetport 2333 -select usb=${config:jlink-masterid} -nogui -noir -novd -nosinglerun -silent -nohalt
:: JLinkGDBServerCL.exe -device ${config:device} -if swd -port 2334 -swoport 2335 -telnetport 2336 -select usb=${config:jlink-slaveid} -nogui -noir -novd -nosinglerun -silent -nohalt
:test

:: Перед тестом нужно привести стенд в начальное состояние.
::for %%n in (1,2) do (
::    %gdbclient% -iex "set auto-load safe-path /" -ix "%gdbinit%" --ex "set $mcu=%%n" --command=%gdbrunning% ^
::        --quiet --symbols=%1 --batch-silent 2> nul
::    call :ExitOnError
::)
for %%n in (Master,Slave) do (
    ((echo r & echo h & echo g & echo q) > %~dp0flash.jlink) ^
        && (JLink.exe -device STM32F334C8 -if SWD -speed 4000 -NoGui 1 -USB %%n -CommandFile %~dp0flash.jlink) 1> nul
    call :ExitOnError
)

:: Запуск клиента и теста. Поток ошибок перенаправляем в nul.
for %%n in (1,2) do (
     %gdbclient% -iex "set auto-load safe-path /" -ix "%gdbinit%" --ex "set $mcu=%%n" --ex "source %script%" ^
        --quiet --symbols=%1 --batch-silent 2> nul
    call :ExitOnError
)
