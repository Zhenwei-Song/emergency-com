/*
 * @Author: Zhenwei Song zhenwei.song@qq.com
 * @Date: 2024-06-04 15:08:16
 * @LastEditors: Zhenwei Song zhenwei.song@qq.com
 * @LastEditTime: 2024-08-02 14:51:10
 * @FilePath: \emergency com 20240625V1.3-debug\MiddleWares\my_uart.h
 * @Description: 仅供学习交流使用
 * Copyright (c) 2024 by Zhenwei Song, All Rights Reserved.
 */
#ifndef __MY_UART_H
#define __MY_UART_H

#include "board-config.h"
#include "stm8l15x.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// #define USART_DEBUG

#if defined USING_USART3
#define MY_USART USART3
#define MY_USART_PORT GPIOG
#define MY_USART_CLK CLK_Peripheral_USART3
#define MY_USART_RxPin GPIO_Pin_0
#define MY_USART_TxPin GPIO_Pin_1
#elif defined USING_USART0 // 有问题
#define MY_USART USART0
#define MY_USART_PORT GPIOH
#define MY_USART_CLK CLK_Peripheral_USART0
#define MY_USART_RxPin GPIO_Pin_4
#define MY_USART_TxPin GPIO_Pin_5
#else
#define MY_USART USART2
#define MY_USART_PORT GPIOH
#define MY_USART_CLK CLK_Peripheral_USART2
#define MY_USART_RxPin GPIO_Pin_4
#define MY_USART_TxPin GPIO_Pin_5
#endif

// extern uint8_t UartMessage[BUFFER_SIZE];

extern uint32_t TIDSlot_time;

// extern bool PCTL_Location_Flag;
// extern bool PCTL_GEO_Flag;
// extern bool PCTL_DMR_Flag;
// extern bool PCTL_HeadBoard_Flag;
// extern bool PCTL_5G_Flag;
// extern bool PCTL_Adhoc_Flag;

extern uint8_t UART_Command_Num; // 命令

void my_uart_init(void);
void my_uart_send_full_message(uint8_t *str);

void uart_send_P(void);
void uart_send_V(float version);
void uart_send_M(void);
void uart_send_L(void);
void uart_send_R_T0_tml(uint8_t msg);
void uart_send_R_T2_tml(uint8_t msg);
void uart_send_R_T2_Response_tml(uint8_t msg);
void uart_send_R_ctler_emer(uint8_t msg);
void uart_send_R_ctler_normal(uint8_t msg);
void uart_send_C_Off(void);
void uart_send_C_Reset(void);
void uart_send_BOK(void);
void uart_send_BError(void);

void uart_message_receive(void);

#endif
