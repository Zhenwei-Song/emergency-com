/*
 * @Author: Zhenwei Song zhenwei.song@qq.com
 * @Date: 2024-06-06 16:43:51
 * @LastEditors: Zhenwei Song zhenwei_song@foxmail.com
 * @LastEditTime: 2024-11-21 10:01:11
 * @FilePath: \emergency com 20241114V1.5 cad_test\MiddleWares\radio_manage.h
 * @Description: 仅供学习交流使用
 * Copyright (c) 2024 by Zhenwei Song, All Rights Reserved.
 */
#ifndef _RADIO_MANAGE_H
#define _RADIO_MANAGE_H

#include "stm8l15x.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "board-config.h"

#define RADIO_BUFFER_SIZE 2

#if 1                            // 修改前
#define RF_FREQUENCY 460000000   // Hz
#define TX_OUTPUT_POWER 20       // dBm
#define LORA_BANDWIDTH 0         // [0: 125 kHz,1: 250 kHz, 2: 500 kHz,3: Reserved]
#define LORA_SPREADING_FACTOR 12 // [SF7..SF12]
#define LORA_CODINGRATE 4        // [1: 4/5, 2: 4/6,3: 4/7,4: 4/8]
#define LORA_PREAMBLE_LENGTH 5   // Same for Tx and Rx��Ӳ���Զ�����4.25��ǰ����
#define LORA_SYMBOL_TIMEOUT 0    // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON false
#else                            // 修改后
#define RF_FREQUENCY 460000000   // Hz
#define TX_OUTPUT_POWER 20       // dBm
#define LORA_BANDWIDTH 2         // [0: 125 kHz,1: 250 kHz, 2: 500 kHz,3: Reserved]
#define LORA_SPREADING_FACTOR 12 // [SF7..SF12]
#define LORA_CODINGRATE 4        // [1: 4/5, 2: 4/6,3: 4/7,4: 4/8]
#define LORA_PREAMBLE_LENGTH 8   // Same for Tx and Rx��Ӳ���Զ�����4.25��ǰ����
#define LORA_SYMBOL_TIMEOUT 100  // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON false
#endif
#define RX_TIMEOUT_VALUE 6000

#define RE_TX_MAX_TIME 500 //*10ms

#define T_110_CMD 0xfb
#define T_003_CMD 0x03

extern bool message_checked_flag;
extern bool radio_TX_busy;

#ifdef USING_CAD
extern bool cad_true_flag;
#else
#define RSSI_THRESHOLD -80
#endif // USING_CAD

typedef enum {
    Goto_LOWPOWER,
    LOWPOWER,
    RX,
    RX_TIMEOUT,
    RX_ERROR,
    TX,
    TX_TIMEOUT,
    IDLE

} States_t;

typedef enum {
    TEMP_EMP,
    CMD_T0,
    CMD_T2,
    T2_RESPONSE,
    MAYDAY,
    ID,
    // CMD_110,
    CMD_003,
    EMP
} recv_msg;

#define RADIO_CMD_T0_RANGE_MIN 0x00
#define RADIO_CMD_T0_RANGE_MAX 0x32
#define RADIO_CMD_T2_RANGE_MIN 0x33
#define RADIO_CMD_T2_RANGE_MAX 0x64
#define RADIO_MAYDAY_RANGE_MIN 0x65
#define RADIO_MAYDAY_RANGE_MAX 0x97
#define RADIO_T2_RESPONSE_RANGE_MIN 0x98
#define RADIO_T2_RESPONSE_RANGE_MAX 0xca
#define RADIO_ID_RANGE_MIN 0xcb
#define RADIO_ID_RANGE_MAX 0xfd

#define RADIO_BUFFER_SIZE 2

typedef struct send_message {
    uint8_t buffer[RADIO_BUFFER_SIZE];
    uint8_t payloadsize;
} send_message_t, *send_message_pt;

// extern uint8_t buffer[RADIO_BUFFER_SIZE];
// extern uint8_t payLoadSize;

extern uint32_t emer_sleep_time_s;

extern bool radio_recv_flag;
extern uint8_t recv_buffer[RADIO_BUFFER_SIZE];
extern volatile States_t RadioState;
extern volatile recv_msg radio_recv_msg_type;

void OnTxDone(void);
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);
void OnTxTimeout(void);
void OnRxTimeout(void);
void OnRxError(void);
void OnCadDone(bool channelActivityDetected);

void my_radio_config(void);
void radio_send_cmd_T0(uint8_t cmd);
void radio_send_cmd_T2(uint8_t cmd);
void radio_send_mayday(uint8_t my_id);
void radio_send_my_id(uint8_t my_id);

#endif // _RADIO_MANAGE_H