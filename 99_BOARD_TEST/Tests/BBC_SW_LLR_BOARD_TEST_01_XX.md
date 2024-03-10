# BBC_SW_LLR_BOARD_TEST_01_XX

## BBC_SW_LLR_BOARD_TEST_01_01

```text
Функция MX_ADC1_Init() должна заполнить поля структуры данных hadc1 тип ADC_HandleTypeDef согласно таблице ниже...
```

---

## BBC_SW_LLR_BOARD_TEST_01_02

```text
Если результат вызова функции HAL_ADC1_Init() с параметром равным адресу структуру данных hadc1 не равен HAL_OK, то функция HAL_ADC_Init() должна вызвать функцию обработки ошибок Error_Handler().
```

---

## BBC_SW_LLR_BOARD_TEST_01_03

```text
Если результат вызова функции HAL_ADCEx_MultiModeConfigChannel() с параметрами:
- первым параметром равным адресу структуры данных hadc1,
- вторым параметром равным адресу структуры типа ADC_MultiModeTypeDef с заполненными полями согласно таблице, не равен HAL_OK, то функция HAL_ADCEx_MultiModeConfigChannel() должна вызвать функцию обработки ошибок Error_Handler().
```

---

## BBC_SW_LLR_BOARD_TEST_01_04

```text
Если результат вызова функции HAL_ADC_ConfigChannel() с параметрами:
- первым параметром равным адресу структуры данных hadc1,
- вторым параметром равным адресу структуры типа ADC_ChannelConfTypeDef с заполненными полями согласно таблице, не равен HAL_OK, то функция HAL_ADC1_Init() должна вызвать функцию обработки ошибок Error_Handler().
```
