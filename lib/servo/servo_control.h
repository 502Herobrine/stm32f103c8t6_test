#ifndef __SERVO_CONTROL_H
#define __SERVO_CONTROL_H

#include "stm32f1xx_hal.h"

/**
 * @brief 舵机初始化
 * 使用 TIM2_CH1 (PA0) 输出 50Hz PWM 信号
 * PWM脉宽范围: 500-2500 (对应 0-180度 或 正转-反转)
 */
void Servo_Init(void);

/**
 * @brief 设置舵机角度或转速
 * @param pulse: PWM脉宽 (单位: us)
 *   - 1500: 中位（停止）
 *   - 1000: 全速反转
 *   - 2000: 全速正转
 *   - 范围: 500-2500
 */
void Servo_SetPulse(uint16_t pulse);

/**
 * @brief 舵机持续正转
 * @param speed: 转速 (1-100, 其中50为中速)
 */
void Servo_Forward(uint8_t speed);

/**
 * @brief 舵机持续反转
 * @param speed: 转速 (1-100, 其中50为中速)
 */
void Servo_Reverse(uint8_t speed);

/**
 * @brief 舵机停止
 */
void Servo_Stop(void);

/**
 * @brief 获取舵机当前脉宽
 * @retval 当前 PWM 脉宽值
 */
uint16_t Servo_GetPulse(void);

#endif
