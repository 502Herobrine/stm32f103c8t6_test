#ifndef __BUZZER_H
#define __BUZZER_H

#include "stm32f1xx_hal.h"

/**
 * @brief 蜂鸣器初始化（使用PWM控制）
 */
void Buzzer_Init(void);

/**
 * @brief 蜂鸣器设置音量（PWM占空比）
 * @param volume: 音量等级 (0-100, 其中0为静音，100为最大)
 */
void Buzzer_SetVolume(uint8_t volume);

/**
 * @brief 蜂鸣器开启
 */
void Buzzer_On(void);

/**
 * @brief 蜂鸣器关闭
 */
void Buzzer_Off(void);

/**
 * @brief 获取蜂鸣器当前音量
 * @retval 当前音量 (0-100)
 */
uint8_t Buzzer_GetVolume(void);

/**
 * @brief 蜂鸣器切换状态
 */
void Buzzer_Toggle(void);

/**
 * @brief 播放SOS信号（一次）
 * @param dit_ms: 短声时长(ms)，通常为100-200ms
 * @param gap_ms: 声音间隔(ms)，通常为200-300ms
 */
void Buzzer_SOS(uint16_t dit_ms, uint16_t gap_ms);

#endif
