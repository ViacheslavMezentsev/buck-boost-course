cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 17)

# Семейство и модель мк.
set(MCU STM32F334C8T)
string(SUBSTRING ${MCU} 5 2 MCU_FAMILY)
string(SUBSTRING ${MCU} 5 6 MCU_MODEL)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../modules/stm32-cmake/cmake/stm32_gcc.cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../tools")

# В качестве имени проекта используем имя папки (см. tasks.json и launch.json).
get_filename_component(BASE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(PROJECT_NAME ${BASE_FOLDER})

include(stm32_utils)
#include(fetch_svd)
#fetch_svd(${MCU})

project(${PROJECT_NAME} C CXX ASM)

# Разворачиваем окружение сборки (HAL, CMSIS, and CMSIS-SVD).
# Набор переменных для использования библиотеки Cube из локального репозитория.
#set(CUBE_FW "V1.11.4")
#set(STM32_CUBE_${MCU_FAMILY}_PATH   "$ENV{USERPROFILE}/STM32Cube/Repository/STM32Cube_FW_${MCU_FAMILY}_${CUBE_FW}/Drivers")
#set(STM32_CMSIS_PATH                "${STM32_CUBE_${MCU_FAMILY}_PATH}/CMSIS")
#set(STM32_CMSIS_${MCU_FAMILY}_PATH  "${STM32_CMSIS_PATH}/Device/ST/STM32${MCU_FAMILY}xx")
#set(STM32_HAL_${MCU_FAMILY}_PATH    "${STM32_CUBE_${MCU_FAMILY}_PATH}/STM32${MCU_FAMILY}xx_HAL_Driver")

# Набор переменных для использования локальной библиотеки Cube.
set(STM32_CMSIS_PATH                "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS")
set(STM32_CMSIS_${MCU_FAMILY}_PATH  "${STM32_CMSIS_PATH}/Device/ST/STM32${MCU_FAMILY}xx")
set(STM32_HAL_${MCU_FAMILY}_PATH    "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32${MCU_FAMILY}xx_HAL_Driver")

# Использование локальных файлов.
set(CMSIS_${MCU_FAMILY}_SYSTEM                 "${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/system_stm32f3xx.c")
set(CMSIS_${MCU_FAMILY}_F334x8_STARTUP         "${CMAKE_CURRENT_SOURCE_DIR}/Core/Startup/startup_stm32f334x8.s")

stm32_get_chip_type(${MCU_FAMILY} ${MCU_MODEL} MCU_TYPE)
stm32_fetch_cmsis(${MCU_FAMILY})
stm32_fetch_hal(${MCU_FAMILY})

find_package(CMSIS COMPONENTS ${MCU} REQUIRED)
find_package(HAL COMPONENTS STM32${MCU_FAMILY} REQUIRED)

add_executable(${PROJECT_NAME})

# Подключаем необходимые библиотеки.
add_subdirectory(Core)
add_subdirectory(st7789)
add_subdirectory(User)

target_link_libraries(${PROJECT_NAME}    
    HAL::STM32::${MCU_FAMILY}::RCCEx
    HAL::STM32::${MCU_FAMILY}::GPIO
    HAL::STM32::${MCU_FAMILY}::SPI
    HAL::STM32::${MCU_FAMILY}::ADCEx
    HAL::STM32::${MCU_FAMILY}::UART
	HAL::STM32::${MCU_FAMILY}::TIMEx
	HAL::STM32::${MCU_FAMILY}::HRTIM
    HAL::STM32::${MCU_FAMILY}::DMA
    HAL::STM32::${MCU_FAMILY}::CORTEX    
    CMSIS::STM32::${MCU_TYPE}
    STM32::Nano
    STM32::NoSys
)

target_compile_definitions(${PROJECT_NAME} PRIVATE USE_HAL_DRIVER STM32F334x8)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:DEBUG>:-O2 -gdwarf-2 -gstrict-dwarf>
    $<$<CONFIG:RELEASE>:-O2>
    $<$<COMPILE_LANGUAGE:CXX>:-fpermissive -fno-exceptions -fno-rtti -fno-unwind-tables -fno-threadsafe-statics -fno-use-cxa-atexit -fstrict-enums>
)

target_link_options(${PROJECT_NAME} PRIVATE LINKER:--no-warn-rwx-segments,--print-memory-usage)

# Подключаем конфигурационный файл компоновщика.
stm32_add_linker_script(${PROJECT_NAME} PRIVATE ${MCU}x_FLASH.ld)

stm32_print_size_of_target(${PROJECT_NAME})
stm32_generate_srec_file(${PROJECT_NAME})
stm32_generate_binary_file(${PROJECT_NAME})
stm32_generate_hex_file(${PROJECT_NAME})
stm32_generate_lss_file(${PROJECT_NAME})
