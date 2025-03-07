/*
 * @Author: Zhenwei Song zhenwei.song@qq.com
 * @Date: 2024-06-05 10:51:06
 * @LastEditors: Zhenwei Song zhenwei_song@foxmail.com
 * @LastEditTime: 2025-02-20 15:37:10
 * @FilePath: \emergency com for git\MiddleWares\my_uart.c
 * @Description: 仅供学习交流使用
 * Copyright (c) 2024 by Zhenwei Song, All Rights Reserved.
 */
#include "my_uart.h"
#include "battery.h"
#include "board-config.h"
#include "board.h"
#include "field.h"
#include "radio_manage.h"
#include "timer.h"

// bool PCTL_Location_Flag = false;
// bool PCTL_GEO_Flag = false;
// bool PCTL_DMR_Flag = false;
// bool PCTL_HeadBoard_Flag = false;
// bool PCTL_5G_Flag = false;
// bool PCTL_Adhoc_Flag = false;

// uint8_t UartMessage[UART_REC_BUFFER_SIZE] = {0};
uint8_t UART_Command_Num = 0; // 命令

float my_version = MY_VERSION;

uint32_t TIDSlot_time = 29; // 默认30s（29+1）

extern bool it_key_up_flag;
extern uint8_t keyState;
extern volatile bool keyDownFlag;

extern bool low_power_flag;
extern bool wake_flag;
extern uint32_t sleep_time;

#define rx_buf_size 256 // 原0x200
static uint8_t rx_buf[rx_buf_size] = {0};
static uint32_t rx_end = 0;
static uint32_t rx_ptr = 0;

#define rx_msg_size 256 // 原0x100
static uint8_t rx_msg[rx_msg_size] = {0};
static uint8_t rx_last_data = 0;
static uint32_t rx_last_frame_header = 0;

static uint8_t rx_finding_header = 1;
static uint8_t rx_current_rx_len = 0;
static uint8_t rx_this_frame_content_length = 0;

#define tx_buf_size 256 // 原0x100
static uint8_t tx_buf[tx_buf_size] = {0};
static uint32_t tx_ptr = 0;
static uint32_t tx_len = 0;
static uint8_t txing = 0;

// For block reading
#define rx_blocking_buf_size 0x40
static uint8_t rx_blocking_read = 0;
static uint8_t rx_blocking_read_buf[rx_blocking_buf_size] = {0};
static uint8_t rx_blocking_read_ptr = 0;

static void process_complete_frame(uint8_t *frame, uint32_t length);
static void process_p_frame(const uint8_t *frame_data);
static void process_a_frame(const uint8_t *frame_data);
static void process_t_frame(const uint8_t *frame_data);
static void process_c_frame(const uint8_t *frame_data);
static void process_b_frame(const uint8_t *frame_data);

/**
 * @description: 串口初始化
 * @return {*}
 */
void my_uart_init(void)
{
    GPIO_Init(MY_USART_PORT, MY_USART_RxPin, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(MY_USART_PORT, MY_USART_TxPin, GPIO_Mode_Out_PP_Low_Fast);

    /* Enable USART clock */
    CLK_PeripheralClockConfig(MY_USART_CLK, ENABLE);

    /* Configure USART Tx as alternate function push-pull  (software pull up)*/
    GPIO_ExternalPullUpConfig(MY_USART_PORT, MY_USART_TxPin, ENABLE);

    /* Configure USART Rx as alternate function push-pull  (software pull up)*/
    GPIO_ExternalPullUpConfig(MY_USART_PORT, MY_USART_RxPin, ENABLE);

    GPIO_SetBits(MY_USART_PORT, MY_USART_TxPin);

    /* USART configuration */
    USART_DeInit(MY_USART);

    USART_Init(MY_USART, (uint32_t)9600, USART_WordLength_8b, USART_StopBits_1,
               USART_Parity_No, (USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx));
    // USART_ITConfig(USART_MP3,USART_IT_RXNE,ENABLE);

    USART_ITConfig(MY_USART, USART_IT_RXNE, ENABLE);
    USART_Cmd(MY_USART, ENABLE);
}

/**
 * @description: 直接发送*str（未自动添加起始标志和结束符）
 * @param {uint8_t} *str
 * @return {*}
 */
static void my_uart_send_raw(uint8_t *str)
{
    strcpy((char *)tx_buf, (char const *)str);
    txing = 1;
    tx_ptr = 0;
    tx_len = strlen((char const *)tx_buf);
    USART_ITConfig(MY_USART, USART_IT_TXE, ENABLE);
    while (txing)
        ;
}

/**
 * @description: 阻塞等待特定字符出现
 * @param {uint8_t} *str
 * @return {*}
 */
static void my_uart_wait_for_rx(uint8_t *str)
{
    rx_blocking_read_ptr = 0;
    while (strstr((char const *)rx_blocking_read_buf, (char const *)str) == 0)
        ;
}

/**
 * @description: 添加了起始标志和结束符
 * @param {uint8_t} *str
 * @param {int} len
 * @return {*}
 */
static void my_uart_send_message_block(uint8_t *str, int len)
{
    int ptr = 0;
    tx_buf[ptr++] = '$';
    for (int i = 0; i < len; ++i) {
        tx_buf[ptr++] = str[i];
    }
    tx_buf[ptr++] = '#';
    txing = 1;
    tx_ptr = 0;
    tx_len = ptr;
    USART_ITConfig(MY_USART, USART_IT_TXE, ENABLE);

    while (txing)
        ;
}

/**
 * @description: 发送完整数据
 * @param {uint8_t} *str
 * @return {*}
 */
void my_uart_send_full_message(uint8_t *str)
{
    my_uart_send_message_block(str, strlen((char const *)str));
    message_green_led(); // 闪灯
}

/* -------------------------------------------------------------------------- */
/*                                   模块->上位机                                  */
/* -------------------------------------------------------------------------- */

/**
 * @description: 发送P命令
 * @return {*}
 */
void uart_send_P(void)
{
    uint8_t message[14];
#if 0
    message[0] = 'P';
    message[1] = 'H';
    if (PCTL_HeadBoard_Flag)
        message[2] = '1';
    else
        message[2] = '0';
    message[3] = 'A';
    if (PCTL_Adhoc_Flag)
        message[4] = '1';
    else
        message[4] = '0';
    message[5] = 'D';
    if (PCTL_DMR_Flag)
        message[6] = '1';
    else
        message[6] = '0';
    message[7] = 'M';
    if (PCTL_5G_Flag)
        message[8] = '1';
    else
        message[8] = '0';
    message[9] = 'G';
    if (PCTL_GEO_Flag)
        message[10] = '1';
    else
        message[10] = '0';
    message[11] = 'L';
    if (PCTL_Location_Flag)
        message[12] = '1';
    else
        message[12] = '0';
    message[13] = '\0';
#else
    message[0] = 'P';
    message[1] = 'H';
    if (PCTL_Read_HeadBoard)
        message[2] = '1';
    else
        message[2] = '0';
    message[3] = 'A';
    if (PCTL_Read_Adhoc)
        message[4] = '1';
    else
        message[4] = '0';
    message[5] = 'D';
    if (PCTL_Read_DMR)
        message[6] = '1';
    else
        message[6] = '0';
    message[7] = 'M';
    if (PCTL_Read_5G)
        message[8] = '1';
    else
        message[8] = '0';
    message[9] = 'G';
    if (PCTL_Read_GEO)
        message[10] = '1';
    else
        message[10] = '0';
    message[11] = 'L';
    if (PCTL_Read_Location)
        message[12] = '1';
    else
        message[12] = '0';
    message[13] = '\0';
#endif
    my_uart_send_full_message(message);
}

/**
 * @description: 发送V命令
 * @param {float} version
 * @return {*}
 */
void uart_send_V(float version)
{
    uint8_t ver_buffer[10];
    ver_buffer[0] = 'V';
    sprintf((char *)ver_buffer + 1, "%.1f", version);
    my_uart_send_full_message(ver_buffer);
}

/**
 * @description: 发送M命令
 * @return {*}
 */
void uart_send_M(void)
{
    if (is_controller) {
        uint8_t message[] = "MController";
        my_uart_send_full_message(message);
    }
    else {
        uint8_t message[] = "MTerminal";
        my_uart_send_full_message(message);
    }
}
/**
 * @description: 发送L命令
 * @return {*}
 */
void uart_send_L(void)
{
    uint8_t message[6] = {0};
    message[0] = 'L';
    if (Energy_out >= 10) {
        sprintf((char *)message + 1, "%.1f", Energy_out);
        message[5] = 'V';
    }
    else {
        sprintf((char *)message + 1, "%.1f", Energy_out);
        message[4] = 'V';
    }

    my_uart_send_full_message(message);
}

/**
 * @description: 发送自己ID
 * @return {*}
 */
void uart_send_ID(void)
{
    uint8_t message[10];
    if (MY_ID <= 9) {
        sprintf((char *)message, "0");
        sprintf((char *)message + 1, "%d", MY_ID);
    }
    else {
        sprintf((char *)message, "%d", MY_ID);
    }
    my_uart_send_full_message(message);
}

/**
 * @description: 由于收到T0指令，终端向上位机发送指令
 * @param {uint8_t} msg
 * @return {*}
 */
void uart_send_R_T0_tml(uint8_t msg)
{
    uint8_t msg_buf[6];
    if (msg / 10 == 0) {
        msg_buf[0] = 'R';
        msg_buf[1] = '0';
        msg_buf[2] = '0';
        sprintf(msg_buf + 3, "%d", msg);
    }
    else {
        msg_buf[0] = 'R';
        msg_buf[1] = '0';
        sprintf(msg_buf + 2, "%d", msg);
    }
    my_uart_send_full_message(msg_buf);
}

/**
 * @description: 由于收到T2指令，终端向上位机发送指令
 * @param {uint8_t} msg
 * @return {*}
 */
void uart_send_R_T2_tml(uint8_t msg)
{
    uint8_t temp_msg;
    temp_msg = msg - RADIO_CMD_T2_RANGE_MIN;
    uint8_t msg_buf[6];
    if (temp_msg / 10 == 0) {
        msg_buf[0] = 'R';
        msg_buf[1] = '2';
        msg_buf[2] = '0';
        sprintf(msg_buf + 3, "%d", temp_msg);
    }
    else {
        msg_buf[0] = 'R';
        msg_buf[1] = '2';
        sprintf(msg_buf + 2, "%d", temp_msg);
    }
    my_uart_send_full_message(msg_buf);
}

/**
 * @description: 由于收到T2回复，控制器向上位机发送指令
 * @param {uint8_t} msg
 * @return {*}
 */
void uart_send_R_T2_Response_tml(uint8_t msg)
{
    uint8_t temp_msg;
    temp_msg = msg - RADIO_T2_RESPONSE_RANGE_MIN;
    uint8_t msg_buf[6];
    if (temp_msg / 10 == 0) {
        msg_buf[0] = 'R';
        msg_buf[1] = '2';
        msg_buf[2] = '0';
        sprintf(msg_buf + 3, "%d", temp_msg);
    }
    else {
        msg_buf[0] = 'R';
        msg_buf[1] = '2';
        sprintf(msg_buf + 2, "%d", temp_msg);
    }
    my_uart_send_full_message(msg_buf);
}

/**
 * @description: 控制器发送指令
 * @param {uint8_t} msg
 * @return {*}
 */
void uart_send_R_ctler_emer(uint8_t msg)
{
    uint8_t id;
    uint8_t msg_buf[6];
    id = msg - RADIO_MAYDAY_RANGE_MIN;
    if (id / 10 == 0) {
        msg_buf[0] = 'R';
        msg_buf[1] = '1';
        msg_buf[2] = '0';
        sprintf(msg_buf + 3, "%d", id);
    }
    else {
        msg_buf[0] = 'R';
        msg_buf[1] = '1';
        sprintf(msg_buf + 2, "%d", id);
    }
    my_uart_send_full_message(msg_buf);
}

/**
 * @description: 控制器发送指令
 * @param {uint8_t} msg
 * @return {*}
 */
void uart_send_R_ctler_normal(uint8_t msg)
{
    uint8_t id;
    uint8_t msg_buf[6];
    id = msg - RADIO_ID_RANGE_MIN;
    if (id / 10 == 0) {
        msg_buf[0] = 'R';
        msg_buf[1] = '2';
        msg_buf[2] = '0';
        sprintf(msg_buf + 3, "%d", id);
    }
    else {
        msg_buf[0] = 'R';
        msg_buf[1] = '2';
        sprintf(msg_buf + 2, "%d", id);
    }
    my_uart_send_full_message(msg_buf);
}

void uart_send_C_Off(void)
{
    uint8_t message[] = "COFF";
    my_uart_send_full_message(message);
}

void uart_send_C_Reset(void)
{
    uint8_t message[] = "CReset";
    my_uart_send_full_message(message);
}

/**
 * @description: 发送BOK
 * @return {*}
 */
void uart_send_BOK(void)
{
    uint8_t message[] = "BOK";
    my_uart_send_full_message(message);
}

/**
 * @description: 发送BError
 * @return {*}
 */
void uart_send_BError(void)
{
    uint8_t message[] = "BError";
    my_uart_send_full_message(message);
}

/* -------------------------------------------------------------------------- */
/*                                   接收字符串处理                                  */
/* -------------------------------------------------------------------------- */

/**
 * @description: 串口接收数据
 * @return {*}
 */
void uart_message_receive(void)
{
    static bool is_receiving_frame = false;
    static uint8_t frame_buffer[256]; // 设最大帧大小为256字节
    static uint32_t frame_ptr = 0;

    while (rx_ptr != rx_end) {
        uint8_t data = rx_buf[rx_ptr];

        if (data == '$' && !is_receiving_frame) { // 检测到帧起始标志
            is_receiving_frame = true;
            frame_ptr = 0;
            frame_buffer[frame_ptr++] = data; // 保存起始标志
        }

        else if (data == '#' && is_receiving_frame) { // 检测到帧结束符
            frame_buffer[frame_ptr++] = data;         // 保存结束符
            is_receiving_frame = false;
            // 处理完整帧（包含$和#）
            process_complete_frame(frame_buffer, frame_ptr);
            //  重置状态
            frame_ptr = 0;
            memset(frame_buffer, 0, sizeof(frame_buffer));
        }
        // 正在接收帧的数据
        else if (is_receiving_frame) {
            frame_buffer[frame_ptr++] = data;
        }

        rx_ptr = (rx_ptr + 1) % rx_buf_size; // 移至下一个数据
    }
}

/**
 * @description: 判断命令种类
 * @param {uint8_t} *frame
 * @param {uint32_t} length
 * @return {*}
 */
static void process_complete_frame(uint8_t *frame, uint32_t length)
{
    // memcpy(UartMessage, frame, length);
    // UartMessage[length] = '\0';
    bool cmd_valid_flag = true;
    if (length < 3) {
        // printf("Invalid frame \n");
        //  LED_TEST_On;
        //  Delay_10ms(50);
        //  LED_TEST_Off;
        uart_send_BError();
        return;
    }
#ifdef USART_DEBUG
    // my_uart_send_raw(frame);
#endif

    uint8_t cmd = frame[1]; // 获取命令字
    switch (cmd) {
    case 'P':
        UART_Command_Num = 1;
        uint8_t temp_frame[13]; // 数据内容加一个空字符
        memcpy(temp_frame, frame + 2, 12);
        temp_frame[12] = '\0'; // 确保字符串正确终止
        process_p_frame(temp_frame);
        break;
    case 'A':
        UART_Command_Num = 2;
        process_a_frame(frame + 2);
        break;
    case 'M':
        UART_Command_Num = 3;
        break;
    case 'T':
        UART_Command_Num = 4;
        process_t_frame(frame + 2);
        break;
    case 'C':
        UART_Command_Num = 5;
        process_c_frame(frame + 2);
        break;
    case 'B':
        UART_Command_Num = 6;
        break;
    default:
        UART_Command_Num = 0;
        cmd_valid_flag = false;
        uart_send_BError();
#ifdef USART_DEBUG
        uint8_t debug_msg[] = "unknown command";
        my_uart_send_raw(debug_msg);
#endif
        break;
    }
    if (cmd_valid_flag == true) { // 每次模块都会回复上层，所以此处不必闪烁
        // message_green_led();//串口接收到正确指令，闪绿灯
    }
    // memset(UartMessage, 0, UART_REC_BUFFER_SIZE);
}

/**
 * @description: 判断命令p的内容
 * @param {uint8_t} *frame_data
 * @return {*}
 */
static void process_p_frame(const uint8_t *frame_data)
{
    bool error_flag = false;
    if (strlen(frame_data) != 12) { // strlen不算'\0'
        error_flag = true;
#ifdef USART_DEBUG
        uint8_t len = (uint8_t)strlen(frame_data);
        uint8_t debug_msg1[] = "p error len";
        my_uart_send_raw(debug_msg1);
        uint8_t msg_buf[5];
        sprintf(msg_buf, "%d", len);
        my_uart_send_raw((msg_buf));
#endif
    }
    if (frame_data[0] != 'H' || frame_data[2] != 'A' || frame_data[4] != 'D' || frame_data[6] != 'M' || frame_data[8] != 'G' || frame_data[10] != 'L') {
        error_flag = true;
#ifdef USART_DEBUG
        uint8_t debug_msg2[] = "p error form";
        my_uart_send_raw(debug_msg2);
#endif
    }
    for (int i = 1; i < 12; i += 2) {
        if (frame_data[i] == '1' || frame_data[i] == '0') {
        }
        else {
            error_flag = true;
#ifdef USART_DEBUG
            uint8_t debug_msg3[] = "p error num";
            my_uart_send_raw(debug_msg3);
#endif
        }
    }
    if (error_flag == true) {
        uart_send_BError();
    }
    else {
        uint8_t helmet_power = frame_data[1] == '1' ? 1 : 0;
        uint8_t mesh_network_power = frame_data[3] == '1' ? 1 : 0;
        uint8_t walkie_talkie_power = frame_data[5] == '1' ? 1 : 0;
        uint8_t mobile_network_power = frame_data[7] == '1' ? 1 : 0;
        uint8_t satellite_comm_power = frame_data[9] == '1' ? 1 : 0;
        uint8_t location_module_power = frame_data[11] == '1' ? 1 : 0;
        if (helmet_power) {
            PCTL_HeadBoard_On;
            //PCTL_HeadBoard_Flag = true;
        }
        else {
            PCTL_HeadBoard_Off;
            //PCTL_HeadBoard_Flag = false;
        }
        if (mesh_network_power) {
            PCTL_Adhoc_On;
            //PCTL_Adhoc_Flag = true;
        }

        else {
            PCTL_Adhoc_Off;
            //PCTL_Adhoc_Flag = false;
        }

        if (walkie_talkie_power) {
            PCTL_DMR_On;
            //PCTL_DMR_Flag = true;
        }

        else {
            PCTL_DMR_Off;
            //PCTL_DMR_Flag = false;
        }

        if (mobile_network_power) {
            PCTL_5G_On;
            //PCTL_5G_Flag = true;
        }
        else {
            PCTL_5G_Off;
            // PCTL_5G_Flag = false;
        }
        if (satellite_comm_power) {
            PCTL_GEO_On;
            //PCTL_GEO_Flag = true;
        }
        else {
            PCTL_GEO_Off;
            //PCTL_GEO_Flag = false;
        }
        if (location_module_power) {
            PCTL_Location_On;
            //PCTL_Location_Flag = true;
        }
        else {
            PCTL_Location_Off;
            //PCTL_Location_Flag = false;
        }
        uart_send_BOK();
    }
}

/**
 * @description: 判断命令a的内容
 * @param {uint8_t} *frame_data
 * @return {*}
 */
static void process_a_frame(const uint8_t *frame_data)
{
    // #ifdef USART_DEBUG
    //     my_uart_send_raw((uint8_t *) frame_data);
    // #endif
    if (strcmp((const char *)frame_data, (const char *)SoftVersion) == 0) { // 不会比较空字符后的部分
        uart_send_V(my_version);
    }
    // if (strcmp((const char *)frame_data, "SoftVersion") == 0) { // 不会比较空字符后的部分
    //     uart_send_V(my_version);
    // }
    else if (strcmp((const char *)frame_data, (const char *)PowerState) == 0) {
        uart_send_P();
    }
    else if (strcmp((const char *)frame_data, (const char *)Mode) == 0) {
        uart_send_M();
    }
    else if (strcmp((const char *)frame_data, (const char *)PowerLevel) == 0) {
        uart_send_L();
    }
    else if (strcmp((const char *)frame_data, (const char *)ModelID) == 0) {
        uart_send_ID();
    }
    else {
        uart_send_BError();
#ifdef USART_DEBUG
        uint8_t debug_msg1[] = "A error";
        my_uart_send_raw(debug_msg1);
#endif
    }
}

/**
 * @description: 判断命令t的内容
 * @param {uint8_t} *frame_data
 * @return {*}
 */
static void process_t_frame(const uint8_t *frame_data)
{
    bool error_flag = false;
    if (is_controller) { // 仅控制器执行
#ifdef USING_CAD
        if (radio_TX_busy == false && cad_true_flag == false) { // 当前发送空闲
#else
        if (radio_TX_busy == false) { // 当前发送空闲
#endif
            // if (strncmp((const char *)frame_data, "110", 3) == 0) {
            //     if (RadioState == TX) { // 上个指令没发完
            //         error_flag = true;
            //         goto ERROR_FLAG;
            //     }
            //     radio_send_cmd(T_110_CMD); // TODO:自定义，先用来测试
            //     uart_send_BOK();
            // }
            if (frame_data[0] == '2') { // T2??指令
                int command_number = (frame_data[1] - '0') * 10 + (frame_data[2] - '0');
                if (command_number >= 0 && command_number <= 50) {
                    if (RadioState == TX) { // 上个指令没发完
                        error_flag = true;
                        goto ERROR_FLAG;
                    }
                    radio_send_cmd_T2(command_number);
                    uart_send_BOK();
                }
                else {
                    error_flag = true;
                }
            }
            else if (frame_data[0] == '0') {
                int command_number = (frame_data[1] - '0') * 10 + (frame_data[2] - '0');
                if (command_number >= 0 && command_number <= 50) {
                    if (RadioState == TX) { // 上个指令没发完
                        error_flag = true;
                        goto ERROR_FLAG;
                    }
                    radio_send_cmd_T0(command_number);
                    uart_send_BOK();
                }
                else {
                    error_flag = true;
                }
            }
            else {
                error_flag = true;
            }
        }
        else { // 非controller
            error_flag = true;
        }
    }
    else {
        error_flag = true;
    }

ERROR_FLAG:
    if (error_flag == true) {
        uart_send_BError();
    }
}

/**
 * @description: 判断命令c的内容
 * @param {uint8_t} *frame_data
 * @return {*}
 */
static void process_c_frame(const uint8_t *frame_data)
{
    bool error_flag = false;
    if (strcmp((const char *)frame_data, (const char *)Emergency) == 0) {
        is_controller = false;
        is_terminal_normal = false;
        is_terminal_emergency = true;
        if (RadioState == TX) { // 上个指令没发完
            error_flag = true;
            goto ERROR_FLAG;
        }
        radio_send_mayday(MY_ID);
        uart_send_BOK();
        TIM4_DeInit();
        low_power_flag = true;
        TIM4_1s_Init(); // 开启1s定时器

        McuEnterLowPowerStopMode();
        while (1) {
            if (KEY1) { // 按键按下导致的唤醒
                while (KEY1)
                    ;                             // 等待按键弹起
                if (keyState == 3) {              // 长按
                    for (int i = 0; i < 2; i++) { // 绿灯闪烁 2 下
                        LED_G_Bat_On;
                        Delay_s(1);
                        LED_G_Bat_Off;
                        Delay_s(1);
                    }
                    PCTL_MCU_Off;
                    keyState = 0;
                    while (1) // 关机
                        ;
                }
            }
            else { // 定时器导致的唤醒
                if (wake_flag) {
                    wake_flag = false;
                    if (RadioState == TX) { // 上个指令没发完
                        error_flag = true;
                        goto ERROR_FLAG;
                    }
                    radio_send_mayday(MY_ID);
                    radio_state_check(RadioState);
                    sleep_time = emer_sleep_time_s; // 重新开始计时
                }
                PWR_UltraLowPowerCmd(ENABLE);
                // 进入停机模式
                halt();
            }
        }
    }
    else if (strcmp((const char *)frame_data, (const char *)Controller) == 0) {
        is_controller = true;
        is_terminal_normal = false;
        is_terminal_emergency = false;
        uart_send_BOK();
    }
    else if (strcmp((const char *)frame_data, (const char *)Terminal) == 0) {
        is_controller = false;
        is_terminal_normal = true;
        is_terminal_emergency = false;
        uart_send_BOK();
    }
    else if (strncmp((const char *)frame_data, "Sleep", 5) == 0) {
        const char *timeStr = frame_data + 5; // 获取"Sleep"之后的字符串部分
        int time = atoi(timeStr);             // 将字符串转换为整数
        // 确认转换后的数字在1到999之间
        if (time >= 1 && time <= 999) {
            emer_sleep_time_s = time;
            uart_send_BOK();
        }
        else {
            error_flag = true;
        }
    }
    else if (strncmp((const char *)frame_data, "ID", 2) == 0) {
        const char *timeStr = frame_data + 2;
        int time = atoi(timeStr); // 将字符串转换为整数
        // 确认转换后的数字在1到999之间
        if (time >= 0 && time <= 50) {
            MY_ID = time;
            uart_send_BOK();
        }
        else {
            error_flag = true;
        }
    }
    else if (strncmp((const char *)frame_data, "TIDSlot", 7) == 0) {
        const char *timeStr = frame_data + 7;
        int time = atoi(timeStr); // 将字符串转换为整数
        // 确认转换后的数字在1到999之间
        if (time >= 1 && time <= 999) {
            TIDSlot_time = time;
            if (TIDSlot_time == 999) {
                TIDSlot_time = TIDSlot_time - 1;
            }
            uart_send_BOK();
        }
        else {
            error_flag = true;
        }
    }
    /* -------------------------------------------------------------------------- */
    /*                                    CAD测试                                   */
    /* -------------------------------------------------------------------------- */
    // #ifdef USING_CAD
    // else if (strncmp((const char *)frame_data, "CAD", 3) == 0) {
    //     RadioState = START_CAD;
    //     uart_send_BOK();
    // }
    // else if (strncmp((const char *)frame_data, "CADTX", 5) == 0) {
    //     radio_send_cmd_T0_CAD(10);
    //     uart_send_BOK();
    // }
    // else if (strncmp((const char *)frame_data, "SENDTX", 6) == 0) {
    //     radio_send_cmd_T0(10);
    //     uart_send_BOK();
    // }
    // #endif
    else {
        error_flag = true;
    }

ERROR_FLAG:
    if (error_flag == true) {
        uart_send_BError();
    }
}

/**
 * @description: 判断命令b的内容
 * @param {uint8_t} *frame_data
 * @return {*}
 */
static void process_b_frame(const uint8_t *frame_data)
{
    bool error_flag = false;
    if (strcmp((const char *)frame_data, (const char *)BOK) == 0) { // 不会比较空字符后的部分
        // TODO:上位机发来OK
    }
    else if (strcmp((const char *)frame_data, (const char *)BError) == 0) {
        // TODO:上位机发来ERROR
    }
    else {
        error_flag = true;
    }
    if (error_flag == true) {
        uart_send_BError();
    }
}

#ifdef USING_USART3
INTERRUPT_HANDLER(TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQHandler, 21)
{
    if (USART_GetITStatus(MY_USART, USART_IT_TXE) != RESET) {
        // LED_G_Bat_On;
        USART_ClearITPendingBit(MY_USART, USART_IT_TXE);

        // Send via UART cost 16ms, there's no need to delay 20ms.
        USART_SendData8(MY_USART, tx_buf[tx_ptr++]);
        tx_ptr %= tx_buf_size;

        if (tx_ptr == tx_len) {
            USART_ITConfig(MY_USART, USART_IT_TXE, DISABLE);
            txing = 0;
            memset(tx_buf, 0, tx_buf_size);
            // LED_G_Bat_Off;
        }
    }
}

INTERRUPT_HANDLER(TIM3_CC_USART3_RX_IRQHandler, 22)
{
    if (USART_GetITStatus(MY_USART, USART_IT_RXNE) != RESET) {
        uint8_t rx_data = USART_ReceiveData8(MY_USART);

        if (rx_blocking_read) {
            rx_blocking_read_buf[rx_blocking_read_ptr++] = rx_data;
            rx_blocking_read_ptr %= rx_blocking_buf_size;
        }
        else {
            rx_buf[rx_end++] = rx_data;
            rx_end %= rx_buf_size;
        }

        USART_ClearITPendingBit(MY_USART, USART_IT_RXNE);
    }
}

#else

// TIM2_CC_USART2_RX_IRQn: 19
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler, 19)
{
    if (USART_GetITStatus(MY_USART, USART_IT_TXE) != RESET) {
        // LED_G_Bat_On;
        USART_ClearITPendingBit(MY_USART, USART_IT_TXE);

        // Send via UART cost 16ms, there's no need to delay 20ms.
        USART_SendData8(MY_USART, tx_buf[tx_ptr++]);
        tx_ptr %= tx_buf_size;

        if (tx_ptr == tx_len) {
            USART_ITConfig(MY_USART, USART_IT_TXE, DISABLE);
            txing = 0;
            memset(tx_buf, 0, tx_buf_size);
            // LED_G_Bat_Off;
        }
    }
}

// TIM2_CC_USART2_RX_IRQn: 20
INTERRUPT_HANDLER(TIM2_CC_USART2_RX_IRQHandler, 20)
{
    if (USART_GetITStatus(MY_USART, USART_IT_RXNE) != RESET) {
        uint8_t rx_data = USART_ReceiveData8(MY_USART);

        if (rx_blocking_read) {
            rx_blocking_read_buf[rx_blocking_read_ptr++] = rx_data;
            rx_blocking_read_ptr %= rx_blocking_buf_size;
        }
        else {
            rx_buf[rx_end++] = rx_data;
            rx_end %= rx_buf_size;
        }

        USART_ClearITPendingBit(MY_USART, USART_IT_RXNE);
    }
}

#endif