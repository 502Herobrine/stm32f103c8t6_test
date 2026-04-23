#include "buzzer.h"

#define BUZZER_PORT GPIOB
#define BUZZER_PIN  GPIO_PIN_13

/* TIM1 PWM参数 */
#define BUZZER_TIMER_INSTANCE   TIM1
#define BUZZER_TIMER_CHANNEL    TIM_CHANNEL_1
#define BUZZER_TIMER_FREQ       1000  /* 1kHz PWM频率 */

static TIM_HandleTypeDef buzzer_tim;
static uint8_t buzzer_volume = 100;  /* 0-100, 表示PWM占空比 */
static uint8_t buzzer_is_on = 0;

void Buzzer_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    /* 使能时钟 */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_TIM1_CLK_ENABLE();

    /* GPIO配置：PB13 -> TIM1_CH1N */
    GPIO_InitStruct.Pin = BUZZER_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(BUZZER_PORT, &GPIO_InitStruct);

    /* TIM1基础配置 */
    /* 72MHz / (72-1) / 1000 = 1kHz */
    buzzer_tim.Instance = BUZZER_TIMER_INSTANCE;
    buzzer_tim.Init.Prescaler = 72 - 1;
    buzzer_tim.Init.CounterMode = TIM_COUNTERMODE_UP;
    buzzer_tim.Init.Period = BUZZER_TIMER_FREQ - 1;
    buzzer_tim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    buzzer_tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&buzzer_tim);

    /* PWM输出配置 */
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;  /* 初始占空比0% (关闭) */
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    HAL_TIM_PWM_ConfigChannel(&buzzer_tim, &sConfigOC, BUZZER_TIMER_CHANNEL);

    /* 启动PWM和互补输出 */
    HAL_TIM_PWM_Start(&buzzer_tim, BUZZER_TIMER_CHANNEL);
    HAL_TIMEx_PWMN_Start(&buzzer_tim, BUZZER_TIMER_CHANNEL);

    buzzer_volume = 100;
    buzzer_is_on = 0;
}

void Buzzer_SetVolume(uint8_t volume)
{
    if (volume > 100)
        volume = 100;
    buzzer_volume = volume;
    
    /* 根据音量设置PWM占空比 */
    if (volume == 0)
    {
        __HAL_TIM_SET_COMPARE(&buzzer_tim, BUZZER_TIMER_CHANNEL, 0);
    }
    else
    {
        uint32_t pulse = (BUZZER_TIMER_FREQ * volume) / 100;
        __HAL_TIM_SET_COMPARE(&buzzer_tim, BUZZER_TIMER_CHANNEL, pulse);
    }
}

void Buzzer_On(void)
{
    buzzer_is_on = 1;
    Buzzer_SetVolume(buzzer_volume);
}

void Buzzer_Off(void)
{
    buzzer_is_on = 0;
    __HAL_TIM_SET_COMPARE(&buzzer_tim, BUZZER_TIMER_CHANNEL, 0);
}

uint8_t Buzzer_GetVolume(void)
{
    return buzzer_volume;
}

void Buzzer_Toggle(void)
{
    if (buzzer_is_on)
        Buzzer_Off();
    else
        Buzzer_On();
}

/**
 * @brief 播放单个声音
 */
static void Buzzer_PlayTone(uint16_t duration_ms)
{
    Buzzer_On();
    HAL_Delay(duration_ms);
    Buzzer_Off();
}

void Buzzer_SOS(uint16_t dit_ms, uint16_t gap_ms)
{
    /* S: 3个短声 */
    Buzzer_PlayTone(dit_ms);
    HAL_Delay(gap_ms);
    Buzzer_PlayTone(dit_ms);
    HAL_Delay(gap_ms);
    Buzzer_PlayTone(dit_ms);
    HAL_Delay(gap_ms * 2);  /* 字母间隔加长 */

    /* O: 3个长声（长声是短声的3倍） */
    Buzzer_PlayTone(dit_ms * 3);
    HAL_Delay(gap_ms);
    Buzzer_PlayTone(dit_ms * 3);
    HAL_Delay(gap_ms);
    Buzzer_PlayTone(dit_ms * 3);
    HAL_Delay(gap_ms * 2);  /* 字母间隔加长 */

    /* S: 3个短声 */
    Buzzer_PlayTone(dit_ms);
    HAL_Delay(gap_ms);
    Buzzer_PlayTone(dit_ms);
    HAL_Delay(gap_ms);
    Buzzer_PlayTone(dit_ms);
}
