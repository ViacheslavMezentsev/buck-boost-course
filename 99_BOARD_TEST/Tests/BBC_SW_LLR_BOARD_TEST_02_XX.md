# BBC_SW_LLR_BOARD_TEST_02_XX

## BBC_SW_LLR_BOARD_TEST_02_01

```text
Функция MX_CAN1_Init() должна заполнить поля структуры данных hcan1 типа CAN_HandleTypeDef согласно таблице ниже
```

Покрытие исходного кода

- [Core/Src/main.c#L155](https://gitlab.air-burg.ru/okb-private/debn/firmware_mcu/main/27714302.30163-02/-/blob/d80cc3e4a444fa38a1e5ffae98c212f020012176/Core/Src/main.c#L155)
- [Core/Src/can.c#L30-52](https://gitlab.air-burg.ru/okb-private/debn/firmware_mcu/main/27714302.30163-02/-/blob/d80cc3e4a444fa38a1e5ffae98c212f020012176/Core/Src/can.c#L30-52)

---

## BBC_SW_LLR_BOARD_TEST_02_02

```text
Если результат вызова функции HAL_CAN_Init() с параметром равным указателю на структуру данных hcan1 не равен HAL_OK, то функция MX_CAN1_Init() должна вызвать функцию обработки ошибок Error_Handler().
```

Покрытие исходного кода

- [Core/Src/main.c#L323-324](https://gitlab.air-burg.ru/okb-private/debn/firmware_mcu/main/27714302.30163-02/-/blob/d80cc3e4a444fa38a1e5ffae98c212f020012176/Core/Src/main.c#L323-324)
- [Core/Src/can.c#L52-55](https://gitlab.air-burg.ru/okb-private/debn/firmware_mcu/main/27714302.30163-02/-/blob/d80cc3e4a444fa38a1e5ffae98c212f020012176/Core/Src/can.c#L52-55)
