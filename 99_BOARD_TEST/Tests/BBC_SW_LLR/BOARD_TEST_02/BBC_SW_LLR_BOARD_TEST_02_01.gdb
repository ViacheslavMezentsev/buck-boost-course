# LLR IDs: BBC_SW_LLR_BOARD_TEST_02_01

# Подключаемся и делаем сброс.
connect_both_mcu

# Значение $result: true - тест пройден, false - не пройден.
set $result = false

# Последовательность действий (см. описание требования).

# Устанавливаем ограничение по времени.
break_main_and_set_timeout 5

# Выполняем функцию MX_USART1_UART_Init().
break MX_USART1_UART_Init
continue
finish

# Проверяем настройки интерфейса USART после инициализации.    
set $result = ( huart1.Instance == USART1 )

set $init = huart1.Init

set $result = $result \
    && $init.BaudRate == 38400 \
    && $init.WordLength == UART_WORDLENGTH_8B \
    && $init.StopBits == UART_STOPBITS_1 \
    && $init.Parity == UART_PARITY_NONE \
    && $init.Mode == UART_MODE_TX_RX \
    && $init.HwFlowCtl == UART_HWCONTROL_NONE \
    && $init.OverSampling == UART_OVERSAMPLING_16 \
    && $init.OneBitSampling == UART_ONE_BIT_SAMPLE_DISABLE \
    && huart1.AdvancedInit.AdvFeatureInit == UART_ADVFEATURE_NO_INIT

# Выполняем тест и отключаемся.
disconnect_and_quit
