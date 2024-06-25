# buck-boost-course
Демо проекты из buck-boost курса Владимира Мединцева.

Каждый проект собирается при помощи CMake и Ninja. Используются вспомогательные сборочные скрипты stm32-cmake.
Можно использовать HAL-драйверы из папки с проектом или из локального репозитория stm32cubemx.

Для конфигурирования выполните команду в папке с проектом:

cmake.exe --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=<путь к компилятору arm-gcc> -Bbuild/Debug -G Ninja

Для сборки cmake'ом выполните команды:

cd build/Debug
cmake --build .

Для очистки от артефактов сборки выполните команды:

cd build/Debug
cmake --build . --target clean
