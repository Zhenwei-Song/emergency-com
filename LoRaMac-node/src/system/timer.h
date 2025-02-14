/*
 * @Author: Zhenwei Song zhenwei_song@foxmail.com
 * @Date: 2024-11-11 09:24:52
 * @LastEditors: Zhenwei Song zhenwei_song@foxmail.com
 * @LastEditTime: 2025-01-02 19:16:30
 * @FilePath: \emergency com 20250102V1.8\LoRaMac-node\src\system\timer.h
 * @Description: 仅供学习交流使用
 * Copyright (c) 2024 by Zhenwei Song, All Rights Reserved.
 */
#ifndef __TIMER_H
#define __TIMER_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"
#include <stdbool.h>
#define Stanby_10ms_max 30000 /*5 ����*/
#define SYN_10ms_max 2000     /*ÿ20s����һ��ͬ����Ϣ*/

extern uint16_t rand_num;

extern bool try_to_turn_off;
extern bool turn_off_time_15s_flag;
extern bool turn_off_time_3s_flag;

void TIM4_Init(void);
void TIM4_1s_Init(void);
void DelayMs(uint32_t tms);
void Delay_10ms(uint32_t tms);

void Delay_s(uint32_t ts);

extern volatile uint32_t Stanby_10ms_Cnt;
extern volatile uint32_t SYN_10ms_Cnt;
extern volatile uint32_t SYN_10ms_Cnt;

#endif
