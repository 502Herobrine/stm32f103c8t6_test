#include "servo_control.h"

#define SERVO_TIMER_INSTANCE TIM2
#define SERVO_TIMER_CHANNEL  TIM_CHANNEL_1
#define SERVO_PORT           GPIOA
#define SERVO_PIN            GPIO_PIN_0

/* PWM 参数定义 */
#define SERVO_FREQ           50          /* 50Hz */
#define SERVO_PERIOD         20000       /* 20ms = 20000us */
#define SERVO_MIN_PULSE      500         /* 最小脉宽 0.5ms */
#define SERVO_MAX_PULSE      2500        /* 最大脉宽 2.5ms */
#define SERVO_CENTER_PULSE   1500        /* 中位 1.5ms */

static uint16_t servo_current_pulse = SERVO_CENTER_PULSE;
static TIM_HandleTypeDef htim2;

/**
 * @brief 初始化 TIM2 PWM 输出
 */
void Servo_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    /* 使能时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_TIM2_CLK_ENABLE();

    /* GPIO 配置：PA0 -> TIM2_CH1 */
    GPIO_InitStruct.Pin = SERVO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SERVO_PORT, &GPIO_InitStruct);

    /* TIM2 基础配置 */
    /* 72MHz / (72-1) = 1MHz，即 1us per tick */
    htim2.Instance = SERVO_TIMER_INSTANCE;
    htim2.Init.Prescaler = 72 - 1;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = SERVO_PERIOD - 1;  /* 20ms */
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&htim2);

    /* PWM 输出配置 */
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = SERVO_CENTER_PULSE;  /* 初始脉宽 1.5ms (停止) */
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, SERVO_TIMER_CHANNEL);

    /* 启动 PWM 输出 */
    HAL_TIM_PWM_Start(&htim2, SERVO_TIMER_CHANNEL);

    servo_current_pulse = SERVO_CENTER_PULSE;
}

/**
 * @brief 设置舵机脉宽
 */
void Servo_SetPulse(uint16_t pulse)
{
    /* 限制脉宽范围 */
    if (pulse < SERVO_MIN_PULSE)
        pulse = SERVO_MIN_PULSE;
    if (pulse > SERVO_MAX_PULSE)
        pulse = SERVO_MAX_PULSE;

    servo_current_pulse = pulse;

    /* 更新 CCR1 寄存器 */
    __HAL_TIM_SET_COMPARE(&htim2, SERVO_TIMER_CHANNEL, pulse);
}

/**
 * @brief 舵机持续正转
 * speed: 1-100 (50为中速, 100为最快正转, 1为最慢)
 */
void Servo_Forward(uint8_t speed)
{
    uint16_t pulse;

    /* 限制速度范围 */
    if (speed > 100)
        speed = 100;
    if (speed < 1)
        speed = 1;

    /* 计算脉宽：1500 + (speed/100) * 1000 */
    /* speed=50时，pulse=2000 (全速正转) */
    /* speed=100时，pulse=2500 (超满度) */
    pulse = SERVO_CENTER_PULSE + (speed * 10);  /* 1500 + speed*10 */

    Servo_SetPulse(pulse);
}

/**
 * @brief 舵机持续反转
 * speed: 1-100 (50为中速, 100为最快反转, 1为最慢)
 */
void Servo_Reverse(uint8_t speed)
{
    uint16_t pulse;

    /* 限制速度范围 */
    if (speed > 100)
        speed = 100;
    if (speed < 1)
        speed = 1;

    /* 计算脉宽：1500 - (speed/100) * 1000 */
    /* speed=50时，pulse=1000 (全速反转) */
    /* speed=100时，pulse=500 (超满度) */
    pulse = SERVO_CENTER_PULSE - (speed * 10);  /* 1500 - speed*10 */

    Servo_SetPulse(pulse);
}

/**
 * @brief 舵机停止
 */
void Servo_Stop(void)
{
    Servo_SetPulse(SERVO_CENTER_PULSE);  /* 1500us - 停止 */
}

/**
 * @brief 获取当前脉宽
 */
uint16_t Servo_GetPulse(void)
{
    return servo_current_pulse;
}
