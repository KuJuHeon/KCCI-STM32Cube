#include "main.h"
#include "key.h"

volatile int key = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_12) {
        // 핀 12에 대한 처리
        key = 1;
    }
    else if (GPIO_Pin == GPIO_PIN_13) {
        // 핀 13에 대한 처리
        key = 2;
    }
    else if (GPIO_Pin == GPIO_PIN_14) {
        // 핀 14에 대한 처리
        key = 3;
    }
    else if (GPIO_Pin == GPIO_PIN_15) {
        // 핀 15에 대한 처리
        key = 4;
    }
}