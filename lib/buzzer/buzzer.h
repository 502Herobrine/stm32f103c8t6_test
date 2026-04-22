#ifndef __BUZZER_H
#define __BUZZER_H

#include "stm32f1xx_hal.h"

/**
 * @brief 蜂鸣器初始化
 */
void Buzzer_Init(void);

/**
 * @brief 蜂鸣器开启（低电平触发）
 */
void Buzzer_On(void);

/**
 * @brief 蜂鸣器关闭（高电平停止）
 */
void Buzzer_Off(void);

/**
 * @brief 获取蜂鸣器当前状态
 * @retval 0: 关闭, 1: 开启
 */
uint8_t Buzzer_GetState(void);

/**
 * @brief 蜂鸣器切换状态（翻转）
 */
void Buzzer_Toggle(void);

#endif
