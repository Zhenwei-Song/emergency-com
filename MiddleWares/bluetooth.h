#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include <stdint.h>
#include "radio.h"
#include "msg.h"
typedef enum
{
    KEY_MSG,
    TEAM_MSG,
    CTL_MSG,
    NO_MSG
}RX_BT_State; 

typedef struct
{
    uint8_t id_enable;//id: 1-10  bit7 enable
    uint8_t length; //0 - 10，该编队起始和结束段 的段数
    uint8_t segments[7*10]; //一个编队，最多允许10段编号 
    uint8_t Current_Light;//当前该编队的 灯光工作模式，bit0--6 light, 127不工作
    uint8_t Current_sound;//当前该编队的 声音工作模式，bit0--6 , 127不工作
} Bluetooth_Team;

typedef struct
{ 
    uint8_t teamid;//1-10,  0 not configured
    uint8_t light_enable[10]; //bit7 enable, bit0-6 light
    uint8_t sound[10];
} Bluetooth_Hotkey; 

typedef struct
{
    uint8_t device_name[24];
    Bluetooth_Team team[10];
    Bluetooth_Hotkey hotkey; 
} Bluetooth_Persistent;

//Location in EEPROM
#define FLASH_DATA_BLUETOOTH_PERSISTENT_ADDRESS FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS
#define FLASH_DATA_BLUETOOTH_PERSISTENT_SIZE sizeof(Bluetooth_Persistent) /*24+740+21=785 byte */

void bluetooth_init();
void bluetooth_main_loop(); 
void bluetooth_send_message_len(uint8_t* msg, int len);
void bluetooth_send_success();
void bluetooth_send_error();
void ctl_msg_handler(uint8_t *str);
extern RX_BT_State rx_BT_State;

extern Bluetooth_Persistent* persistent;

#endif
