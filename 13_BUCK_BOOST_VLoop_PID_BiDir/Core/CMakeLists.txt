cmake_minimum_required(VERSION 3.19)

target_include_directories(${PROJECT_NAME} PRIVATE Inc )

target_sources(${PROJECT_NAME} PRIVATE
    Src/stm32f3xx_hal_msp.c
    Src/stm32f3xx_it.c
    Src/adc.c
    Src/dma.c
    Src/gpio.c
    Src/hrtim.c
    Src/tim.c
    Src/spi.c
    Src/usart.c
    Src/main.c
)
