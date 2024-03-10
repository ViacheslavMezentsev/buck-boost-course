# LLR IDs:BBC_SW_LLR_BOARD_TEST_02_04

# Подключаемся и делаем сброс.
connect_both_mcu

# Значение $result: true - тест пройден, false - не пройден.
set $result = false

# Последовательность действий (см. описание требования).

# Устанавливаем ограничение по времени.
break_main_and_set_timeout 5

break HAL_UART_MspInit
continue
break HAL_GPIO_Init
continue

# Проверяем значения параметров функции HAL_GPIO_Init():    
# - первый равен GPIOA:
set $result = ( GPIOx == GPIOB )

# - второй равен адресу структуры типа GPIO_InitTypeDef с заполненными полями:
set $result = $result \
    && GPIO_Init->Pin == ( GPIO_PIN_6 | GPIO_PIN_7 ) \
    && GPIO_Init->Mode == GPIO_MODE_AF_PP \
    && GPIO_Init->Pull == GPIO_NOPULL \
    && GPIO_Init->Speed == GPIO_SPEED_FREQ_HIGH \
    && GPIO_Init->Alternate == GPIO_AF7_USART1

# Выполняем тест и отключаемся.
disconnect_and_quit
