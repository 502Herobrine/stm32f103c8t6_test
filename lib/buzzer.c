#include "buzzer.h"

#define BUZZER_PORT GPIOB
#define BUZZER_PIN  GPIO_PIN_13

static uint8_t buzzer_state = 0;  /* 0: OFF, 1: ON */

void Buzzer_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = BUZZER_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(BUZZER_PORT, &GPIO_InitStruct);

    buzzer_state = 1;  /* 初始化为开启状态 */
}

void Buzzer_On(void)
{
    HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);  /* 低电平触发 */
    buzzer_state = 1;
}

void Buzzer_Off(void)
{
    HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET);  /* 高电平停止 */
    buzzer_state = 0;
}

uint8_t Buzzer_GetState(void)
{
    return buzzer_state;
}

void Buzzer_Toggle(void)
{
    HAL_GPIO_TogglePin(BUZZER_PORT, BUZZER_PIN);
    buzzer_state = !buzzer_state;
}
