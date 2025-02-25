/*
 * @Author: Zhenwei Song zhenwei.song@qq.com
 * @Date: 2024-06-03 09:38:54
 * @LastEditors: Zhenwei Song zhenwei.song@qq.com
 * @LastEditTime: 2024-06-04 14:52:53
 * @FilePath: \emergency com 20240504V1.1\MiddleWares\battery.h
 * @Description: 仅供学习交流使用
 * Copyright (c) 2024 by Zhenwei Song, All Rights Reserved.
 */
#ifndef __BAT_H
#define __BAT_H
#include "stm8l15x.h"
#include <stdbool.h>

#define BAT_100_TH 410
#define BAT_75_TH 383
#define BAT_50_TH 365
#define BAT_25_TH 350
#define BAT_10_TH 310

#define BAT_CORRECTED 0.2

extern uint8_t Energy_Level;
extern float Energy_out;
extern uint8_t System_Low_Power_Flag;
extern uint32_t Bat_int, Bat_AD_Value;
extern uint8_t Bat_Volt_int_cnt;
extern uint8_t Bat_Value_Cnt, Bat_Sampled_Over;
extern bool bat_power_off_flag;
uint16_t get_battery_reading(void);
void battery_init(void);
void battery_det(void);
extern void Flash_Status_LED(void);
#endif
