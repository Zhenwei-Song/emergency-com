/*
 * @Author: Zhenwei Song zhenwei.song@qq.com
 * @Date: 2024-06-06 16:43:41
 * @LastEditors: Zhenwei Song zhenwei_song@foxmail.com
 * @LastEditTime: 2024-11-21 16:28:51
 * @FilePath: \emergency com 20241114V1.5 cad_test\MiddleWares\radio_manage.c
 * @Description: 仅供学习交流使用
 * Copyright (c) 2024 by Zhenwei Song, All Rights Reserved.
 */

#include "radio_manage.h"

#include "board.h"
#include "radio.h"
#include "timer.h"

uint32_t emer_sleep_time_s = 30; // 默认30s

extern uint8_t keyState;

volatile States_t RadioState = RX;

volatile recv_msg radio_recv_msg_type = EMP;

static send_message_t radio_send_msg;

bool radio_recv_flag = false;
bool message_checked_flag = false;

bool radio_TX_busy = false;

#ifdef USING_CAD
bool cad_finished_flag = false;
bool cad_true_flag = false;
#endif // USING_CAD

uint8_t recv_buffer[RADIO_BUFFER_SIZE];
uint8_t payloadsize;

static RadioEvents_t RadioEvents;
static RadioState_t radio_state;

#define isChannelFree Radio.IsChannelFree(MODEM_LORA, RF_FREQUENCY, RSSI_THRESHOLD, 10)

// Function to be executed on Radio Tx Done event
void OnTxDone(void)
{
    // Radio.Sleep( );
    RF_TX_Over_Flag = 1;
    // RadioState = Goto_LOWPOWER; // 发送完成
    if (is_controller || is_terminal_normal)
        RadioState = RX;
    else
        // RadioState = LOWPOWER; // 用来置空
        RadioState = IDLE;
}

// Function to be executed on Radio Rx Done event
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
    // Radio.Sleep( );
    payloadsize = size;
    memcpy(recv_buffer, payload, payloadsize);
    RadioState = RX; // 接收完成
                     // #endif
    radio_recv_flag = true;
}

// Function executed on Radio Tx Timeout event
void OnTxTimeout(void)
{
    // Radio.Sleep( );
    RadioState = TX_TIMEOUT;
    RF_TX_Over_Flag = 2;
}

// Function executed on Radio Rx Timeout event
void OnRxTimeout(void)
{

    // Radio.Sleep( );
    RadioState = RX_TIMEOUT;
}

// Function executed on Radio Rx Error event
void OnRxError(void)
{
    // Radio.Sleep( );

    RadioState = RX_ERROR;
}

#ifdef USING_CAD
// Function executed on CadDone event
void OnCadDone(bool channelActivityDetected)
{
    cad_finished_flag = true;
    cad_true_flag = false;
    if (channelActivityDetected == true) { // 信道被占用
        cad_true_flag = true;
    }
    else { // 信道空闲,该状态下可发送
        cad_true_flag = false;
    }
}
#endif // USING_CAD

void my_radio_config(void)
{
    // Radio initialization
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;
#ifdef USING_CAD
    RadioEvents.CadDone = OnCadDone;
#endif

    Radio.Init(&RadioEvents);

    Radio.SetChannel(RF_FREQUENCY);

    Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                      LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                      LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                      true, 0, 0, LORA_IQ_INVERSION_ON, 6000);

    Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                      LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                      LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                      0, true, 0, 0, LORA_IQ_INVERSION_ON, true);

    // Radio.Rx( RX_TIMEOUT_VALUE );//进入接收状态
}

/**
 * @description: 应急指令T0
 * @param {uint8_t} cmd
 * @return {*}
 */
static send_message_t cmd_T0_to_payload(uint8_t cmd)
{
    send_message_t radio_message;
    radio_message.buffer[0] = cmd;
    radio_message.buffer[1] = ~radio_message.buffer[0];
    radio_message.payloadsize = RADIO_BUFFER_SIZE;
    return radio_message;
}

/**
 * @description: 应急指令T2
 * @param {uint8_t} cmd
 * @return {*}
 */
static send_message_t cmd_T2_to_payload(uint8_t cmd)
{
    uint8_t temp_cmd;
    send_message_t radio_message;
    temp_cmd = cmd + RADIO_CMD_T2_RANGE_MIN;
    radio_message.buffer[0] = temp_cmd;
    radio_message.buffer[1] = ~radio_message.buffer[0];
    radio_message.payloadsize = RADIO_BUFFER_SIZE;
    return radio_message;
}

/**
 * @description: 求救信号
 * @param {uint8_t} my_id
 * @return {*}
 */
static send_message_t mayday_payload(uint8_t my_id)
{
    uint8_t temp_id;
    send_message_t radio_message;
    temp_id = my_id + RADIO_MAYDAY_RANGE_MIN; // 加101
    radio_message.buffer[0] = temp_id;
    radio_message.buffer[1] = ~radio_message.buffer[0];
    radio_message.payloadsize = RADIO_BUFFER_SIZE;
    return radio_message;
}

/**
 * @description: 自身ID广播
 * @param {uint8_t} my_id
 * @return {*}
 */
static send_message_t my_id_payload(uint8_t my_id)
{
    uint8_t temp_id;
    send_message_t radio_message;
    temp_id = my_id + RADIO_ID_RANGE_MIN; // 加200
    radio_message.buffer[0] = temp_id;
    radio_message.buffer[1] = ~radio_message.buffer[0];
    radio_message.payloadsize = RADIO_BUFFER_SIZE;
    return radio_message;
}

/**
 * @description: T2回复
 * @param {uint8_t} my_id
 * @return {*}
 */
static send_message_t my_id_payload_T2(uint8_t my_id)
{
    uint8_t temp_id;
    send_message_t radio_message;
    temp_id = my_id + RADIO_T2_RESPONSE_RANGE_MIN; // 加200
    radio_message.buffer[0] = temp_id;
    radio_message.buffer[1] = ~radio_message.buffer[0];
    radio_message.payloadsize = RADIO_BUFFER_SIZE;
    return radio_message;
}

/**
 * @description: Lora发送应急指令T0
 * @param {uint8_t} cmd
 * @return {*}
 */
void radio_send_cmd_T0(uint8_t cmd)
{
    radio_send_msg = cmd_T0_to_payload(cmd);
    RadioState = TX;
}

/**
 * @description: Lora发送应急指令T2
 * @param {uint8_t} cmd
 * @return {*}
 */
void radio_send_cmd_T2(uint8_t cmd)
{
    radio_send_msg = cmd_T2_to_payload(cmd);
    RadioState = TX;
}

/**
 * @description: Lora发送求救信号
 * @param {uint8_t} my_id
 * @return {*}
 */
void radio_send_mayday(uint8_t my_id)
{
    radio_send_msg = mayday_payload(my_id);
    RadioState = TX;
}

/**
 * @description: Lora广播自身ID
 * @param {uint8_t} my_id
 * @return {*}
 */
void radio_send_my_id(uint8_t my_id)
{
    radio_send_msg = my_id_payload(my_id);
    RadioState = TX;
}

/**
 * @description: 终端收到T2指令，Lora回复自身ID
 * @param {uint8_t} my_id
 * @return {*}
 */
void radio_response_T2(uint8_t my_id)
{
    radio_send_msg = my_id_payload_T2(my_id);
    RadioState = TX;
}

void radio_check_received_message(uint8_t *rx_buffer)
{
    // radio_recv_flag = 1;
    if (RADIO_CMD_T0_RANGE_MIN <= rx_buffer[0] && rx_buffer[0] <= RADIO_CMD_T0_RANGE_MAX) {
        radio_recv_msg_type = CMD_T0;
    }
    else if (RADIO_CMD_T2_RANGE_MIN <= rx_buffer[0] && rx_buffer[0] <= RADIO_CMD_T2_RANGE_MAX) {
        radio_recv_msg_type = CMD_T2;
    }
    else if (RADIO_MAYDAY_RANGE_MIN <= rx_buffer[0] && rx_buffer[0] <= RADIO_MAYDAY_RANGE_MAX) {
        radio_recv_msg_type = MAYDAY;
    }
    else if (RADIO_ID_RANGE_MIN <= rx_buffer[0] && rx_buffer[0] <= RADIO_ID_RANGE_MAX) {
        radio_recv_msg_type = ID;
    }
    else if (RADIO_T2_RESPONSE_RANGE_MIN <= rx_buffer[0] && rx_buffer[0] <= RADIO_T2_RESPONSE_RANGE_MAX) {
        radio_recv_msg_type = T2_RESPONSE;
    }
    // else if (T_110_CMD == rx_buffer[0]) {
    //     radio_recv_msg_type = CMD_110;
    // }
    else { // 无法识别
        // radio_recv_flag = 0;
    TODO:
    }
}
#ifdef USING_CAD
void radio_state_check(States_t Radiostate)
{
    if (is_controller == true) { // 控制器
        switch (Radiostate)      //
        {
        case RX:
            // radio_recv_flag = 1;
            if (radio_recv_flag == true) {
                radio_recv_flag = false;
                radio_check_received_message(recv_buffer);
                message_checked_flag = true;
            }
            // Delay_10ms(1);
            // Radio.Rx(RX_TIMEOUT_VALUE); // 进入接收状态
            // RadioState = Goto_LOWPOWER;
            // RadioState = RX;
            break;
        case RX_TIMEOUT:
            break;
        case RX_ERROR:
            Radio.Rx(RX_TIMEOUT_VALUE); // 进入接收状态
            RadioState = RX;
            // RadioState = Goto_LOWPOWER;
            break;
        case TX: // 发射间隔最快1s，lora特性
        case TX_TIMEOUT:
            cad_finished_flag = false;
            Radio.Standby();
            Radio.StartCad();
            while (cad_finished_flag == false)
                ;
            if (cad_true_flag == false) {
                // 信道空闲，可以发送消息
                RF_TX_Over_Flag = 0;
                LED_G_Bat_On;
                Radio.Send(radio_send_msg.buffer, radio_send_msg.payloadsize);
                while (RF_TX_Over_Flag == 0)
                    ;
                Delay_10ms(10);
                LED_G_Bat_Off;
                Radio.Rx(RX_TIMEOUT_VALUE);
                RadioState = RX;
                radio_TX_busy = false;
            }
            else {
                // 信道忙碌，需要等待后重试
                radio_TX_busy = true;
                Delay_10ms(rand_num % RE_TX_MAX_TIME); // 延迟再尝试
                RadioState = TX;
            }
            break;
        default:
            Radio.Rx(RX_TIMEOUT_VALUE); // 进入接收状态
            RadioState = RX;
            // RadioState = LOWPOWER;
            break;
        }
    }
    else if (is_terminal_normal == true) { // 非紧急终端
        switch (Radiostate)                //
        {
        case RX:
            // radio_recv_flag = 1;
            if (radio_recv_flag == true) {
                radio_recv_flag = false;
                radio_check_received_message(recv_buffer);
                message_checked_flag = true;
            }
            // Delay_10ms(1);
            // Radio.Rx(RX_TIMEOUT_VALUE); // 进入接收状态
            // Radio.StartCad();
            // Delay_10ms(7);
            // RadioState = START_CAD;
            break;
        case RX_TIMEOUT:
        case RX_ERROR:
            Radio.Rx(RX_TIMEOUT_VALUE); // 进入接收状态
            RadioState = RX;
            break;
        case TX:
        case TX_TIMEOUT:
            cad_finished_flag = false;
            Radio.Standby();
            Radio.StartCad();
            while (cad_finished_flag == false)
                ;
            if (cad_true_flag == false) {
                // 信道空闲，可以发送消息
                RF_TX_Over_Flag = 0;
                LED_G_Bat_On;
                Radio.Send(radio_send_msg.buffer, radio_send_msg.payloadsize);
                while (RF_TX_Over_Flag == 0)
                    ;
                Delay_10ms(10);
                LED_G_Bat_Off;
                Radio.Rx(RX_TIMEOUT_VALUE); // 进入接收状态
                RadioState = RX;
                radio_TX_busy = false;
            }
            else {
                // 信道忙碌，可能需要等待后重试
                radio_TX_busy = true;
                Delay_10ms(rand_num % RE_TX_MAX_TIME); // 延迟再尝试
                RadioState = TX;
            }
            break;
        default:
            Radio.Rx(RX_TIMEOUT_VALUE); // 进入接收状态
            RadioState = RX;
            break;
        }
    }
    else { // 紧急终端
        switch (Radiostate) {
        case LOWPOWER:
            break;
        case TX:
        case TX_TIMEOUT:
        default:
            RF_TX_Over_Flag = 0;
            LED_G_Bat_On;
            Radio.Send(radio_send_msg.buffer, radio_send_msg.payloadsize);
            while (RF_TX_Over_Flag == 0)
                ;
            Delay_10ms(10);
            // Radio.Send(radio_send_msg.buffer, radio_send_msg.payloadsize);
            // while (RF_TX_Over_Flag == 0)
            //     ;
            LED_G_Bat_Off;
            // DelayMs(emer_sleep_time_ms);
            // radio_send_mayday(MY_ID);
            break;
        }
    }
}
#else
void radio_state_check(States_t Radiostate)
{
    if (is_controller == true) { // 控制器
        switch (Radiostate)      //
        {
        case RX:
            // radio_recv_flag = 1;
            if (radio_recv_flag == true) {
                radio_recv_flag = false;
                radio_check_received_message(recv_buffer);
                message_checked_flag = true;
            }
            // Delay_10ms(1);
            // Radio.Rx(RX_TIMEOUT_VALUE); // 进入接收状态
            // RadioState = Goto_LOWPOWER;
            // RadioState = RX;
            break;
        case RX_TIMEOUT:
            break;
        case RX_ERROR:
            Radio.Rx(RX_TIMEOUT_VALUE); // 进入接收状态
            RadioState = RX;
            // RadioState = Goto_LOWPOWER;
            break;
        case TX: // 发射间隔最快1s，lora特性
        case TX_TIMEOUT:
            if (isChannelFree) {
                // 信道空闲，可以发送消息
                RF_TX_Over_Flag = 0;
                LED_G_Bat_On;
                Radio.Send(radio_send_msg.buffer, radio_send_msg.payloadsize);
                while (RF_TX_Over_Flag == 0)
                    ;
                Delay_10ms(10);
                LED_G_Bat_Off;
                Radio.Rx(RX_TIMEOUT_VALUE);
                RadioState = RX;
                radio_TX_busy = false;
            }
            else {
                // 信道忙碌，需要等待后重试
                radio_TX_busy = true;
                Delay_10ms(rand_num % RE_TX_MAX_TIME); // 延迟再尝试
                RadioState = TX;
            }
            break;
        // case Goto_LOWPOWER:
        //     RadioState = LOWPOWER;
        //     break;
        // case LOWPOWER:
        //     Radio.Standby();
        //     break;
        default:
            Radio.Rx(RX_TIMEOUT_VALUE); // 进入接收状态
            RadioState = RX;
            // RadioState = LOWPOWER;
            break;
        }
    }
    else if (is_terminal_normal == true) { // 非紧急终端
        switch (Radiostate)                //
        {
        case RX:
            if (radio_recv_flag == true) {
                radio_recv_flag = false;
                radio_check_received_message(recv_buffer);
                message_checked_flag = true;
            }
            // Radio.Rx(RX_TIMEOUT_VALUE); // 进入接收状态
            // RadioState = RX;
            break;
        case RX_TIMEOUT:
        case RX_ERROR:
            Radio.Rx(RX_TIMEOUT_VALUE); // 进入接收状态
            RadioState = RX;
            break;
        case TX:
        case TX_TIMEOUT:
            if (isChannelFree) {
                // 信道空闲，可以发送消息
                RF_TX_Over_Flag = 0;
                LED_G_Bat_On;
                Radio.Send(radio_send_msg.buffer, radio_send_msg.payloadsize);
                while (RF_TX_Over_Flag == 0)
                    ;
                Delay_10ms(10);
                LED_G_Bat_Off;
                Radio.Rx(RX_TIMEOUT_VALUE);
                RadioState = RX;
                radio_TX_busy = false;
            }
            else {
                // 信道忙碌，可能需要等待后重试
                radio_TX_busy = true;
                Delay_10ms(rand_num % RE_TX_MAX_TIME); // 延迟再尝试
                RadioState = TX;
            }
            break;
        default:
            Radio.Rx(RX_TIMEOUT_VALUE); // 进入接收状态
            RadioState = RX;
            break;
        }
    }
    else { // 紧急终端
        switch (Radiostate) {
        case LOWPOWER:
            break;
        case TX:
        case TX_TIMEOUT:
        default:
            RF_TX_Over_Flag = 0;
            LED_G_Bat_On;
            Radio.Send(radio_send_msg.buffer, radio_send_msg.payloadsize);
            while (RF_TX_Over_Flag == 0)
                ;
            Delay_10ms(10);
            // Radio.Send(radio_send_msg.buffer, radio_send_msg.payloadsize);
            // while (RF_TX_Over_Flag == 0)
            //     ;
            LED_G_Bat_Off;
            // DelayMs(emer_sleep_time_ms);
            // radio_send_mayday(MY_ID);
            break;
        }
    }
}
#endif