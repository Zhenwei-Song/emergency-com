
#include "msg.h"
#include "battery.h"
#include "bluetooth.h"
#include "board-config.h"
#include "board.h"
#include "my_uart.h"
#include "radio_manage.h"
#include "sx126x.h"
// #include "sx126x-board.h"

#define WINDOW_VALUE 128 // WWDG
#define COUNTER_INIT 255 // WWDG

extern bool it_key_up_flag;

bool power_off_flag = false;

static void
WWDG_Config(void)
{
    /* WWDG configuration: WWDG is clocked by SYSCLK = 2MHz */
    /* WWDG timeout is equal to 251,9 ms */
    /* Watchdog Window = (COUNTER_INIT - 63) * 1 step
                       = 41 * (12288 / 2Mhz)
                       = 251,9 ms
    */
    /* Non Allowed Window = (COUNTER_INIT - WINDOW_VALUE) * 1 step
                          = (104 - 97) * 1 step
                          =  7 * 1 step
                          =  7 * (12288 / 2Mhz)
                          =  43.008 ms
     */
    /* So the non allowed window starts from 0.0 ms to 43.008 ms
    and the allowed window starts from 43.008 ms to 251,9 ms
    If refresh is done during non allowed window, a reset will occur.
    If refresh is done during allowed window, no reset will occur.
    If the WWDG down counter reaches 63, a reset will occur. */
    WWDG_Init(COUNTER_INIT, WINDOW_VALUE);
}
static void Reset_WWDG(void)
{
    // if ((WWDG_GetCounter() & 0x7F) > WINDOW_VALUE)
    /* Refresh WWDG counter during allowed window so no MCU reset will occur */
    //  WWDG_SetCounter(COUNTER_INIT);
}

/**
 * @description: mainboard 开机子函数 SZW添加
 * @return {*}
 */
static void mainboard_init(void)
{
    PCTL_MainBoard_On;
    PCTL_HeadBoard_On;
    //PCTL_HeadBoard_Flag = true;
    KEY_3588_Low;
    for (int i = 0; i < 100; i++) {
        // if (Bat_Sampled_Over) {
        //     Bat_Sampled_Over = 0;
        //     battery_det();
        //     Flash_Status_LED();
        // }
        Delay_10ms(3);
    }
    KEY_3588_High;
    for (int i = 0; i < 100; i++) {
        // if (Bat_Sampled_Over) {
        //     Bat_Sampled_Over = 0;
        //     battery_det();
        //     Flash_Status_LED();
        // }
        Delay_10ms(3);
    }
}

/**
 * Main application entry point.
 */
void MsgLoop(void)
{
    uint8_t temp_buffer[RADIO_BUFFER_SIZE] = {255, 255};
    // TIM4_Init(); // 定时器初始化
    //  asm("rim");
    my_radio_config(); // 配置radio
    my_uart_init();
    battery_init();   // 里面初始化了ADC
    mainboard_init(); // mainboard开机

    // LED_R_Bat_On;
    // Delay_10ms(50);
    // LED_R_Bat_Off; // 上电提示
    // LED_TEST_Off;
    uart_send_V(MY_VERSION); // 串口打印当前版本
    while (1) {
        if (!McuStopFlag) {

            FLASH_Unlock(FLASH_MemType_Data); // 开机后解锁
            // USART_MP3_Init();                 // 串口初始化，管脚：TX G1;RX G0
            Radio.Rx(RX_TIMEOUT_VALUE);
                working_flag = true;
            while (!McuStopFlag) {
                /* -------------------------------------------------------------------------- */
                /*                                     测试                                     */
                /* -------------------------------------------------------------------------- */
                // LED_TEST_On;
                // Delay_10ms(50);
                // LED_TEST_Off;
                // Delay_10ms(5);
                // uart_send_BOK();
                // Delay_10ms(5);
                // uart_send_BError();
                // LED_R_Bat_On;
                // radio_send_cmd_T0(0X01); //CMD
                // Delay_10ms(100);
                // LED_R_Bat_Off;
                // Key_Detected();
                /* -------------------------------------------------------------------------- */
                /*                                    电量显示                                    */
                /* -------------------------------------------------------------------------- */
                if (Bat_Sampled_Over) {
                    Bat_Sampled_Over = 0;
                    battery_det();
                    // if (led_busy_flag == false)
                    //     Flash_Status_LED();
                    if (bat_power_off_flag == true) { // 电量低，自动关机
                        PCTL_Adhoc_Off;               // 关闭所有电源
                        PCTL_5G_Off;
                        PCTL_HeadBoard_Off;
                        PCTL_DMR_Off;
                        PCTL_GEO_Off;
                        PCTL_Location_Off;
                        LED_R_Bat_Off;
                        LED_G_Bat_Off;
                        PCTL_MainBoard_Off;

                        // PCTL_Location_Flag = false;
                        // PCTL_GEO_Flag = false;
                        // PCTL_DMR_Flag = false;
                        // PCTL_HeadBoard_Flag = false;
                        // PCTL_5G_Flag = false;
                        // PCTL_Adhoc_Flag = false;

                        // it_key_up_flag = false;
                        power_off_led();
                        PCTL_MCU_Off;
                        keyState = 0;
                        LED_TEST_Off;
                        while (1)
                            ; // 关机
                    }
                }
                /* -------------------------------------------------------------------------- */
                /*                                    按键状态                                    */
                /* -------------------------------------------------------------------------- */
                switch (keyState) {
                case 1: // 单击
                    keyState = 0;
                    break;
                // case 2: //连击
                case 3: // 长按
                    // LED_G_Bat_On;
                    // //Delay_10ms(50);
                    // LED_G_Bat_Off;
                    // Delay_10ms(50);

                    while (KEY1)
                        ; // 等待按键弹起
                    uart_send_C_Off();
                    // Delay_10ms(1000); // 延时10s
                    KEY_3588_Low;
                    try_to_turn_off = true;
                    while (power_off_flag == false) {
                        if (ST_3588 == 0) {
                            while (turn_off_time_3s_flag == false)
                                ;             // 延时3s
                            if (ST_3588 == 0) // 3s后仍是低电平，立即关机
                                power_off_flag = true;
                        }
                        if (turn_off_time_15s_flag == true) // 15s强制关机
                            power_off_flag = true;
                    }

                    if (power_off_flag == true) {
                        PCTL_Adhoc_Off; // 关闭所有电源
                        PCTL_5G_Off;
                        PCTL_HeadBoard_Off;
                        PCTL_DMR_Off;
                        PCTL_GEO_Off;
                        PCTL_Location_Off;
                        LED_R_Bat_Off;
                        LED_G_Bat_Off;
                        PCTL_MainBoard_Off;

                        // PCTL_Location_Flag = false;
                        // PCTL_GEO_Flag = false;
                        // PCTL_DMR_Flag = false;
                        // PCTL_HeadBoard_Flag = false;
                        // PCTL_5G_Flag = false;
                        // PCTL_Adhoc_Flag = false;

                        // it_key_up_flag = false;
                        power_off_led();
                        PCTL_MCU_Off;
                        keyState = 0;
                        LED_TEST_Off;
                        while (1)
                            ; // 关机
                    }

                    break;
                default: //
                    // keyState = 0;
                    break;
                }
                /* -------------------------------------------------------------------------- */
                /*                                  串口接收数据查询                                  */
                /* -------------------------------------------------------------------------- */
                if (is_controller || is_terminal_normal) {
                    uart_message_receive();
                }
                /* -------------------------------------------------------------------------- */
                /*                                   RADIO控制                                  */
                /* -------------------------------------------------------------------------- */
                radio_state_check(RadioState);

                if (message_checked_flag) { // 接收到数据
                    // Delay_10ms(50);
                    //  memcpy(temp_buffer, recv_buffer, RADIO_BUFFER_SIZE);

                    // if (strcmp(recv_buffer, temp_buffer) != 0) {
                    if (recv_buffer[0] != 0xff) {
                        switch (radio_recv_msg_type) {
                        case CMD_T0:
                            if (is_terminal_normal == true) {
                                // uart_send_R_tml(temp_buffer[1]);
                                uart_send_R_T0_tml(recv_buffer[0]);
                                // LED_R_Bat_Off; //仅测试接收使用
                                // LED_G_Bat_On; // 用于接受测试
                                // Delay_10ms(30);
                                // LED_G_Bat_Off;
                            }
                            radio_recv_msg_type = EMP;
                            break;
                        case CMD_T2:
                            if (is_terminal_normal == true) {
                                uart_send_R_T2_tml(recv_buffer[0]);
                                Delay_10ms(100);
                                Delay_10ms(rand_num % (TIDSlot_time * 100)); // 最大TIDSlot_time s
                                radio_response_T2(MY_ID);
                            }
                            radio_recv_msg_type = EMP;
                            break;
                        // case CMD_110:
                        //     if (is_terminal_normal == true) {
                        //         Delay_10ms(100);
                        //         Delay_10ms(rand_num % 2900); // 最大29000ms，29s
                        //         radio_send_my_id(MY_ID);
                        //     }
                        //     radio_recv_msg_type = EMP;
                        //     break;
                        case MAYDAY:
                            if (is_controller == true)
                                // uart_send_R_ctler_emer(temp_buffer[1]);
                                uart_send_R_ctler_emer(recv_buffer[0]);
                            radio_recv_msg_type = EMP;
                            break;
                        case ID:
                            if (is_controller == true)
                                // uart_send_R_ctler_normal(temp_buffer[1]);
                                uart_send_R_ctler_normal(recv_buffer[0]);
                            radio_recv_msg_type = EMP;
                            break;
                        case T2_RESPONSE:
                            if (is_controller == true)
                                // uart_send_R_ctler_normal(temp_buffer[1]);
                                uart_send_R_T2_Response_tml(recv_buffer[0]);
                            radio_recv_msg_type = EMP;
                            break;
                        default:
                            radio_recv_msg_type = EMP;
                            break;
                        }
                    }
                    radio_recv_msg_type = EMP;
                    memset(recv_buffer, 255, RADIO_BUFFER_SIZE);
                    message_checked_flag = false;
                }
            }
        }
    }
}

// void Flash_WriteTeamNumber(void)
// {
//     FLASH_Unlock(FLASH_MemType_Data);
//     // FLASH_ProgramByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS + TeamNumAddr_offset*sizeof(uint8_t), (uint8_t)true);
//     FLASH_ProgramByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS + (TeamNumAddr_offset + 1) * sizeof(uint8_t), (uint8_t)team_Num);
//     FLASH_ProgramByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS + (TeamNumAddr_offset + 2) * sizeof(uint8_t), (uint8_t)teamEnable);
//     FLASH_Lock(FLASH_MemType_Data);
// }

// void Flash_ReadTeamNumber(void)
// {
//     FLASH_Unlock(FLASH_MemType_Data);

//     team_Num = FLASH_ReadByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS + (TeamNumAddr_offset + 1) * sizeof(uint8_t));
//     teamEnable = FLASH_ReadByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS + (TeamNumAddr_offset + 2) * sizeof(uint8_t));

//     FLASH_Lock(FLASH_MemType_Data);
// }
