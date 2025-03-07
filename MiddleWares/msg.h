/*
 * @Author: Zhenwei Song zhenwei.song@qq.com
 * @Date: 2024-06-03 09:38:54
 * @LastEditors: Zhenwei Song zhenwei.song@qq.com
 * @LastEditTime: 2024-06-12 10:37:11
 * @FilePath: \emergency com 20240504V1.1\MiddleWares\msg.h
 * @Description: 仅供学习交流使用
 * Copyright (c) 2024 by Zhenwei Song, All Rights Reserved.
 */
#ifndef __MSG_H
#define __MSG_H

#include "N910x_mp3.h"
#include "bluetooth.h"
#include "board.h"
#include "light.h"
#include "radio.h"
#include "timer.h"
#include "utilities.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// #define WAKEUP_PREAMBLE_LENGTH                      88

// #define RX_TIMEOUT_VALUE                            6000
#define BUFFER_SIZE 0x80 // Define the payload size here

#define RXTIME 12800    // RXTIME*15.625us
#define SLEEPTIME 51200 // SLEEPTIME*15.625us

void MsgLoop(void);

// void keyDetectedIrq(void);
// void Key_Detected(void);
//  extern uint8_t buffer[BUFFER_SIZE];
//  extern uint8_t payLoadSize;
//  extern volatile States_t RadioState;

// extern bool McuStopFlag;

extern bool keyDownFlag;
extern uint8_t keyState;

extern uint8_t Wireless_SYN_Flag; // �ӻ�����ͬ���Ϻ󣬸ñ�־λΪ1.
extern bool lightEnable;
extern volatile uint16_t time_1s_cnt, time_10s_cnt, Req_Send_Time_SYN_Frame;

extern void Reset_WWDG(void);
extern void WWDG_Config(void);

#endif
