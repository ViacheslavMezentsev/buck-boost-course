@echo off
:: Конфигурация: Debug или Release. 
:: Пакет xpack-arm-none-eabi-gcc-13.2.1-1.1 должен быть установлен в папку пользователя.
:: Утилиты cmake и ninja должны быть установлены и доступны из командной строки (можно использовать паектный менеджер scoop).
cmake.exe --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH="%UserProfile%\xpack-arm-none-eabi-gcc-13.2.1-1.1\bin\arm-none-eabi-gcc.exe" -B%~dp0build/Debug -G Ninja
::cmake.exe --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH="%UserProfile%\xpack-arm-none-eabi-gcc-13.2.1-1.1\bin\arm-none-eabi-gcc.exe" -B%~dp0build/Release -G Ninja