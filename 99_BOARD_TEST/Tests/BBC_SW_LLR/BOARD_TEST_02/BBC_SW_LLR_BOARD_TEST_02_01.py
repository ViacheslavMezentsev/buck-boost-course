# LLR IDs: BPSD_SW_LLR_MFONO_02_01
import gdb

# Подключаемся и делаем сброс.
gdb.execute( 'cmds connect_both_mcu; set $result = false' )

# Последовательность действий (см. описание требования).
try:
    # Устанавливаем ограничение по времени.
    gdb.execute( 'break_main_and_set_timeout 5' )

    # Выполняем функцию MX_USART1_UART_Init().
    gdb.execute( 'cmds break MX_USART1_UART_Init; continue; finish' )

    # Проверяем настройки интерфейса USART после инициализации.
    params = [ 'huart1.Instance == USART1',
        'huart1.Init.BaudRate == 38400',
        'huart1.Init.WordLength == UART_WORDLENGTH_8B',
        'huart1.Init.StopBits == UART_STOPBITS_1',
        'huart1.Init.Parity == UART_PARITY_NONE',
        'huart1.Init.Mode == UART_MODE_TX_RX',
        'huart1.Init.HwFlowCtl == UART_HWCONTROL_NONE',
        'huart1.Init.OverSampling == UART_OVERSAMPLING_16',
        'huart1.Init.OneBitSampling == UART_ONE_BIT_SAMPLE_DISABLE',
        'huart1.AdvancedInit.AdvFeatureInit == UART_ADVFEATURE_NO_INIT' ]

    result = all( map( gdb.parse_and_eval, params ) )

    # Значение $result: true - тест пройден, false - не пройден.
    gdb.set_convenience_variable( 'result', result )

# Обработка исключений: $result = false.
except gdb.error: gdb.set_convenience_variable( 'result', False )

# Отключаемся и выходим.
gdb.execute( 'disconnect_and_quit' )
