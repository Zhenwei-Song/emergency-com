#include "bluetooth.h"
#include "board.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm8l15x_itc.h"
#include "utilities.h"




// #define RST_BT_PORT    GPIOF
// #define RST_BT_PIN     GPIO_Pin_7
// #define LP_BT_PORT    GPIOD
// #define LP_BT_PIN     GPIO_Pin_5

// #define BT_RX_PORT GPIOH
// #define BT_RX_PIN  GPIO_Pin_4
// #define BT_TX_PORT GPIOH
// #define BT_TX_PIN  GPIO_Pin_5

// #define rx_buf_size 0x100      //原0x200
// static uint8_t rx_buf[rx_buf_size] = {0};
// static uint32_t rx_end = 0;
// static uint32_t rx_ptr = 0;

// #define rx_msg_size 0x100      //原0x100
// static uint8_t rx_msg[rx_msg_size] = {0};
// static uint8_t rx_last_data = 0;
// static uint32_t rx_last_frame_header = 0; 

// static uint8_t rx_finding_header = 1;
// static uint8_t rx_current_rx_len = 0;
// static uint8_t rx_this_frame_content_length = 0;
  
// #define tx_buf_size 0x100      //原0x100
// static uint8_t tx_buf[tx_buf_size] = {0};
// static uint32_t tx_ptr = 0;
// static uint32_t tx_len = 0;
// static uint8_t txing = 0;

// static uint8_t current_token = '?';
 
Bluetooth_Persistent* persistent;

//For block reading
#define rx_blocking_buf_size 0x40
static uint8_t rx_blocking_read = 0;
static uint8_t rx_blocking_read_buf[rx_blocking_buf_size] = {0};
static uint8_t rx_blocking_read_ptr = 0; 

RX_BT_State rx_BT_State = NO_MSG;

static uint8_t bluetooth_char_to_hex(uint8_t ch);
static uint8_t bluetooth_hex_to_char(uint8_t hex); 
/*
uint8_t k=0;
void team_msg_handler(uint8_t *str)////
{
    //uint8_t i=0;
    if(strstr((char const*)str, "start")!=NULL)     //开始帧
    {   
        team_Num=bluetooth_char_to_hex(*(str + 12));//
        team_msg[team_Num][0]=0;                    //清零
        team_msg[team_Num][0]=bluetooth_char_to_hex(*(str + 20));     //enable
    }
    else if(strstr((char const*)str, "end;")!=NULL) //结束帧,接收结束
    {
        team_msg[team_Num][0]+=(k<<8);//&0x0000ff00;              //成员个数
        k=0;
        rx_BT_State=TEAM_MSG;
    }
    else                                            //数据帧
    {
        char *token;
        char *next_token = NULL;
        
        token = strtok_r((char *)(str+18), ",;", &next_token);/////
        while (token != NULL) 
        {
            if(strchr(token,'\r')==NULL)       //不存在'\r',不是末尾字符串
            {
                team_msg[team_Num][k+1]=atoi(token);
                k++;
            }   
            token = strtok_r(NULL, ",;", &next_token);
        }
    }
}
*/

#if 0 // SZW修改

void ctl_msg_handler(uint8_t *str)
{
   /* uint8_t i=0;
    char arr[4]={0};
    char *token;
    char *next_token = NULL;
    token = strtok_r((char *)str, ",;", &next_token);
    while (token != NULL) 
    {
        if(strchr(token,'\r')==NULL)   //不存在'\r'
        {
            arr[i++]=atoi(token);
        }
        token = strtok_r(NULL, ",;", &next_token);
    }*/
    team_Num=str[0]&0x0F;
    light_Num=(str[0]>>4)+ ((str[1]>>2)&0x30);
    sound_Num=str[1]&0x3F;
    persistent->team[team_Num-1].Current_Light=light_Num;
    persistent->team[team_Num-1].Current_sound=sound_Num;
    rx_BT_State=CTL_MSG;
}  
void bluetooth_send_raw(uint8_t * str) {
  strcpy((char*)tx_buf, (char const*)str); 
  txing = 1;
  tx_ptr = 0;
  tx_len = strlen((char const*)tx_buf);
  USART_ITConfig(USART2, USART_IT_TXE, ENABLE);  
  while(txing);
}

void bluetooth_wait_for_rx(uint8_t * str) {
  rx_blocking_read_ptr = 0; 
  while(strstr((char const*)rx_blocking_read_buf, (char const*)str) == 0);  
}

void bluetooth_upload_device_name() {
  rx_blocking_read = 1;
  
  bluetooth_send_raw("AT:STR\r\n"); 
  bluetooth_wait_for_rx("AT:OK\r\n");
  
  strcpy((char*)tx_buf, "AT:NAME="); 
  int len = strnlen((char const*)persistent->device_name, 20);
  if(len == 0)
  {
    strcpy(persistent->device_name, "Default Device");
    len = strnlen((char const*)persistent->device_name, 20);
  }
  strncpy((char*)tx_buf + 8, (char const*)persistent->device_name, len);
  strcpy((char*)tx_buf + 8 + len, "\r\n");
  txing = 1;
  tx_ptr = 0;
  tx_len = strlen((char const*)tx_buf);
  USART_ITConfig(USART2, USART_IT_TXE, ENABLE);  
  while(txing); 
  bluetooth_wait_for_rx("Changed Name to:");
  
  bluetooth_send_raw("AT:END\r\n"); 
  bluetooth_wait_for_rx("AT:END\r\n");
  
  rx_blocking_read = 0; 
}

void bluetooth_set_power(uint8_t enable){
  if(!enable)
    RST_BT_On;
  else
    RST_BT_Off;
}

void bluetooth_init(){
   
  //GPIO_Init(RST_BT_PORT, RST_BT_PIN, GPIO_Mode_Out_PP_Low_Fast); 
  GPIO_Init(BT_RX_PORT, BT_RX_PIN, GPIO_Mode_In_PU_No_IT);
  GPIO_Init(BT_TX_PORT, BT_TX_PIN, GPIO_Mode_Out_PP_Low_Fast);
  GPIO_Init(GPIOB,GPIO_Pin_4,GPIO_Mode_In_FL_No_IT);//Link
  GPIO_Init(RST_BT_PORT, RST_BT_PIN, GPIO_Mode_Out_PP_Low_Slow);RST_BT_On;
  GPIO_Init(LP_BT_PORT, LP_BT_PIN, GPIO_Mode_Out_PP_Low_Slow);Set_BT_LP
  bluetooth_set_power(0);
  DelayMs(10); 
  bluetooth_set_power(1);
  Set_BT_Work
//  GPIO_Init(LP_BT_PORT, LP_BT_PIN, GPIO_Mode_Out_PP_Low_Fast);  
//  GPIO_ResetBits(LP_BT_PORT, LP_BT_PIN);
  
  //Initialize UART
  CLK_PeripheralClockConfig(CLK_Peripheral_USART2, ENABLE);
  
//  GPIO_Init(BT_RX_PORT, BT_RX_PIN, GPIO_Mode_In_PU_No_IT);
//  GPIO_Init(BT_TX_PORT, BT_TX_PIN, GPIO_Mode_Out_PP_Low_Fast);
  GPIO_ExternalPullUpConfig(GPIOH, GPIO_Pin_4 | GPIO_Pin_5, ENABLE);
  GPIO_SetBits(BT_TX_PORT, BT_TX_PIN);
  
  USART_DeInit(USART2);  
  USART_Init(USART2, 9600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, (USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx));
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); 
  USART_Cmd(USART2, ENABLE);    
   
  persistent = (Bluetooth_Persistent*) FLASH_DATA_BLUETOOTH_PERSISTENT_ADDRESS;
  
  DelayMs(200);   
  bluetooth_upload_device_name();
}   

uint8_t bluetooth_char_to_hex(uint8_t ch){
  if(ch >= 48 && ch <= 57)
    return ch - 48;
  else if(ch >= 65 && ch <= 70)
    return ch - 55;
  else if(ch >= 97 && ch <= 102)
    return ch - 87;
  return 0;
}

uint8_t bluetooth_hex_to_char(uint8_t hex){
  if(hex <= 9) { //删掉了hex >= 0 && 
    return hex + 48;
  }
  else if(hex >= 10 && hex <= 15) {
    return hex + 55;
  }
  return 48;
}

uint8_t bluetooth_get_msg_checksum(uint8_t* data, uint32_t length){
  int value = 0;
  uint8_t * checksum = data + length - 2;
  value += bluetooth_char_to_hex(checksum[0]) * 16;
  value += bluetooth_char_to_hex(checksum[1]);
  return value;
} 

uint8_t bluetooth_is_msg_valid(uint8_t* data, uint32_t length){
  uint8_t sum = 0;
  for (int i = 2; i < length - 2; ++i){
    sum += data[i];
  } 
  return bluetooth_get_msg_checksum(data, length) == sum;
} 

//Send message, automatically add header, token, checksum and \r\n
void bluetooth_send_message_len(uint8_t * str, int len) {
  
  uint8_t sum = current_token;
  for (int i = 0; i < len; ++i){
    sum += str[i];
  }
  
  int ptr = 0;
  tx_buf[ptr++] = '$';
  tx_buf[ptr++] = len + 3;
  tx_buf[ptr++] = current_token;
  for (int i = 0; i < len; ++i){
    tx_buf[ptr++] = str[i];
  } 
  tx_buf[ptr++] = bluetooth_hex_to_char(sum / 16);
  tx_buf[ptr++] = bluetooth_hex_to_char(sum % 16);  
  
  txing = 1;
  tx_ptr = 0;
  tx_len = ptr;
  USART_ITConfig(USART2, USART_IT_TXE, ENABLE); 
  
  while(txing);
}

void bluetooth_send_message(uint8_t * str) {
  bluetooth_send_message_len(str, strlen((char const*)str));
}

void bluetooth_send_raw_message(uint8_t * str) {
  int len = strlen((char const*)str);
  for (int i = 0; i < len; ++i){
    tx_buf[i] = str[i];
  }
  
  txing = 1;
  tx_ptr = 0;
  tx_len = len; 
  USART_ITConfig(USART2, USART_IT_TXE, ENABLE); 
  
  while(txing);
}
 
void bluetooth_message_handler(uint8_t* data, uint32_t length) {
  
  if((uint8_t *)strstr((char const*)data, "com:establish;") == (data + 3)) {
      current_token = data[2];
      bluetooth_send_message("com:success;");
  }
  else if((uint8_t *)strstr((char const*)data, "ask:team,") == (data + 3)) {//查询编队信息
      if(data[2] != current_token)
        return;
      uint8_t teamNumber = *(data + 12) - '0'; //0-9
      uint8_t ret_message[100] = {0};
      memcpy(ret_message, "resp:team,", 10);
      ret_message[10] = teamNumber + '0';
      ret_message[11] = ',';
      if(persistent->team[teamNumber].length > 10)
        persistent->team[teamNumber].length = 0;
      int data_len = 2 + persistent->team[teamNumber].length * 7;
      memcpy(ret_message + 12, &persistent->team[teamNumber], data_len); 
      ret_message[12 + data_len] = ';';
      bluetooth_send_message_len(ret_message, (int)(13 + data_len)); 
  }
  else if((uint8_t *)strstr((char const*)data, "send:team,") == (data + 3)) {  //下发编队信息
      if(data[2] != current_token)
        return;
      uint8_t teamNumber = *(data + 13) - '0';
      Bluetooth_Team* remote_team = (Bluetooth_Team*)(data + 15);
      if(remote_team->length > 10) {
        bluetooth_send_message("com:error;"); 
        return;
      }
      int cpy_len = 2 + remote_team->length * 7;
      if(data[15 + cpy_len] != ';') {
        bluetooth_send_message("com:error;"); 
      }
      else {
        memcpy(&persistent->team[teamNumber], remote_team, cpy_len);  
        
        msg_to_payload((uint8_t *)remote_team);   
        RF_TX_Over_Flag=0;       
        Radio.Send( buffer, payLoadSize );
        while(RF_TX_Over_Flag==0);
         // Radio.IrqProcess( );         //Process Radio IRQ
        RF_TX_Over_Flag=0;
        Radio.Send( buffer, payLoadSize );
        while(RF_TX_Over_Flag==0);          
         // Radio.IrqProcess( );         //Process Radio IRQ
        RadioState = Goto_LOWPOWER;     
        bluetooth_send_message("com:success;");  
      }
  }
  else if((uint8_t *)strstr((char const*)data, "ask:enteam;") == (data + 3))  //查询启用的编队
  {  
      if(data[2] != current_token)
        return;
      uint8_t ret_msg[100] = {0};
      memcpy(ret_msg, "resp:enteam", 11);
      int ret_p = 11; 
      for(int i = 0;i<10;++i)
      {
        if((persistent->team[i].id_enable >> 7) & 0x1) 
        {
          ret_msg[ret_p++] = ',';
          ret_msg[ret_p++] = '0' + i;
        }
      }
      ret_msg[ret_p++] = ';'; 
      bluetooth_send_message_len(ret_msg, ret_p);  
  }
  else if((uint8_t *)strstr((char const*)data, "ask:entrans;") == (data + 3))  //透传查询启用编队当前的设置
  {  
      if(data[2] != current_token)
        return;
      uint8_t ret_msg[100] = {0};
      memcpy(ret_msg, "resp:entrans,", 13);
      int ret_p = 13; 
      for(int i = 0;i<10;++i)
      {
        if((persistent->team[i].id_enable >> 7) & 0x1) 
        {
          ret_msg[ret_p++] = i;
          ret_msg[ret_p++] = persistent->team[i].Current_Light;
          ret_msg[ret_p++] = persistent->team[i].Current_sound;
        }
      } 
      bluetooth_send_message_len(ret_msg, ret_p);  
  }
  else if((uint8_t *)strstr((char const*)data, "ask:key;") == (data + 3)) 
  {  
      if(data[2] != current_token)
        return;
      uint8_t ret_message[100] = {0};
      memcpy(ret_message, "resp:key,", 9);  
      memcpy(ret_message + 9, &persistent->hotkey, 21); 
      ret_message[30] = ';';
      bluetooth_send_message_len(ret_message, 31); //without ; 
  }
  else if((uint8_t *)strstr((char const*)data, "send:key,") == (data + 3)) { //下发快捷键
      if(data[2] != current_token)
        return;
      if(data[33] != ';') {
        bluetooth_send_message("com:error;"); 
      }
      else {
        memcpy(&persistent->hotkey, data + 12, 21);  
        bluetooth_send_message("com:success;");  
        Refresh_hotkey();
        //rx_BT_State=KEY_MSG;
      } 
  } 
  else if((uint8_t *)strstr((char const*)data, "send:ctrl,") == (data + 3)) { //透传
      if(data[2] != current_token)
        return;
      ctl_msg_handler(data+13);//
      ctlmsg_to_payload();//包含了主机的时间
      RF_TX_Over_Flag=0;       
      Radio.Send( buffer, payLoadSize );
      while(RF_TX_Over_Flag==0);
        //Radio.IrqProcess( );         //Process Radio IRQ
      RF_TX_Over_Flag=0;       
      ctlmsg_to_payload();//包含了主机的时间
      Radio.Send( buffer, payLoadSize );
      while(RF_TX_Over_Flag==0);
        //Radio.IrqProcess( );         //Process Radio IRQ
      bluetooth_send_message("com:success;");  
      memset1(rx_msg, 0, rx_msg_size);
      //rx_BT_State=KEY_MSG;
  }
  else if((uint8_t *)strstr((char const*)data, "send:name,") == (data + 3)) {  
    if(data[2] != current_token)
      return;
    //Recommended to process the following code in main loop, not interrupt.
    for(int i = 0; i < 20; ++i) {
      uint8_t char_val = data[i + 13];
      if(char_val == ';') {
        persistent->device_name[i] = '\0';
        break;
      }
      persistent->device_name[i] = char_val;
    }
    persistent->device_name[20] = '\0'; 
    bluetooth_upload_device_name();
    bluetooth_send_message("com:success;");
  }
}

void bluetooth_send_success()
{
    bluetooth_send_message("com:success;");
}

void bluetooth_send_error()
{
    bluetooth_send_message("com:error;"); 
}

//Process message in main loop
void bluetooth_main_loop() 
{ 
    while(rx_ptr != rx_end) 
    {
        uint8_t data = rx_buf[rx_ptr];
      
        //寻找帧头
        if(rx_finding_header)
        {
          //帧头
          if(data == '$') 
          {
              rx_last_frame_header = rx_ptr;
          } 
          //本帧长度
          if(rx_last_data == '$')
          { 
              rx_this_frame_content_length = data;
              rx_finding_header = 0;
              rx_current_rx_len = 0;
          } 
          rx_ptr++;
          rx_ptr %= rx_buf_size;
        }
        else
        {
          rx_current_rx_len++; 
          rx_ptr++;
          rx_ptr %= rx_buf_size;
          if(rx_current_rx_len == rx_this_frame_content_length) 
          {
              rx_finding_header = 1;
              //Copy for consistency
              uint32_t copy_ptr = rx_last_frame_header;
              uint32_t msg_ptr = 0;
              while (copy_ptr != rx_ptr)
              {
                  rx_msg[msg_ptr++] = rx_buf[copy_ptr++];
                  copy_ptr %= rx_buf_size;
              } 
            
              //Check header and length
              if(rx_msg[0] == '$' && msg_ptr > 6)
              { 
                //Check checksum
                if(bluetooth_is_msg_valid(rx_msg, msg_ptr))
                { 
                    bluetooth_message_handler(rx_msg, msg_ptr);
                    Stanby_10ms_Cnt=Stanby_10ms_max;
                } 
              }
          }
        }
        rx_last_data = data;        
    }
}

//TIM2_CC_USART2_RX_IRQn: 19
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler, 19)
{ 
  if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET){ 
    USART_ClearITPendingBit(USART2, USART_IT_TXE);
    
    //Send via UART cost 16ms, there's no need to delay 20ms. 
    USART_SendData8(USART2, tx_buf[tx_ptr++]); 
    tx_ptr %= tx_buf_size;
      
    if(tx_ptr == tx_len) {
      USART_ITConfig(USART2, USART_IT_TXE, DISABLE); 
      txing = 0;
    }
  }
}

//TIM2_CC_USART2_RX_IRQn: 20
INTERRUPT_HANDLER(TIM2_CC_USART2_RX_IRQHandler, 20)
{ 
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){
    uint8_t rx_data = USART_ReceiveData8(USART2);  
    
    if(rx_blocking_read) {
      rx_blocking_read_buf[rx_blocking_read_ptr++] = rx_data;
      rx_blocking_read_ptr %= rx_blocking_buf_size;
    } else {
      rx_buf[rx_end++] = rx_data;
      rx_end %= rx_buf_size;  
    }
    
    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
  }
}

#endif
