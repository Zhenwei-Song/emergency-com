#ifndef __N910X_MP3_H
#define __N910X_MP3_H

#include "stm8l15x.h"

#define USART_MP3                   USART3
#define USART_MP3_PORT              GPIOG
#define USART_MP3_CLK               CLK_Peripheral_USART3
#define USART_MP3_RxPin             GPIO_Pin_0
#define USART_MP3_TxPin             GPIO_Pin_1

void Ask_Play_Status(void);
void Start_Play(void);
void Stop_Play(void);
uint8_t Select_MP3(uint16_t Num);
void Ask_MP3_Num(void);
void USART_MP3_Init(void);
void USART_SendBuf(USART_TypeDef* USARTx, uint8_t* Buf, uint16_t TxLen);

#endif 
