/*
 * @Author: Zhenwei Song zhenwei.song@qq.com
 * @Date: 2024-06-03 09:38:54
 * @LastEditors: Zhenwei Song zhenwei.song@qq.com
 * @LastEditTime: 2024-06-13 11:53:47
 * @FilePath: \emergency com 20240504V1.1\MiddleWares\key.h
 * @Description: 仅供学习交流使用
 * Copyright (c) 2024 by Zhenwei Song, All Rights Reserved.
 */
#ifndef __KEY_H
#define __KEY_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

void Key_Detected(void);

BitStatus KEY_Down(GPIO_TypeDef *GPIOx, GPIO_Pin_TypeDef GPIO_Pin); // 用于判断哪个按键被按下
void EXIT_KEY_Init(void);

#endif