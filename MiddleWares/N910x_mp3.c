#include "N910x_mp3.h"
#include "timer.h"
#include "board-config.h"
/**********************************************
   MP3������Ŀ˳�� 
   01 -- 
   02 -- 
   03 -- 
   04 -- 
   05 -- 
**********************************************/

/***********��ѯ����״̬*************
���ͣ�04 FB 01 00 00 
���գ�04 FB 02 00 ������״̬���롿SM 
���صĲ���״̬���뺬�壺00 ��ֹͣ 01 ������ 02 ����ͣ
**************************************/

void Ask_Play_Status(void)
{
  Delay_10ms(28);
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0x04);
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0xFB);
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0x01);
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0x00);
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0x00);
}

/***********����*************
���ͣ�04 FB 01 01 01 
���գ�04 FB 02 00 ������״̬���롿SM 
���صĲ���״̬���뺬�壺00 ��ֹͣ 01 ������ 02 ����ͣ
**************************************/
void Start_Play(void)
{
 // sDelayMs(250);
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0x04);
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0xFB);
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0x01);
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0x01);
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0x01);
}

/***********ֹͣ*************
���ͣ�04 FB 01 03 03 
���գ�04 FB 02 00 ������״̬���롿SM 
���صĲ���״̬���뺬�壺00 ��ֹͣ 01 ������ 02 ����ͣ
**************************************/
void Stop_Play(void)
{  
  Delay_10ms(25);
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0x04);
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0xFB);
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0x01);
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0x03);
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0x03);
}

/***********ָ����Ŀ*************
���ͣ�04 FB 03 06 ��Ŀ��λ ��Ŀ��λ SM
���գ���
��ע����Ŀһ���иߵ�2���ֽڱ�ʾ�������6�ף����� 00 06.
ѡ����Զ���ʼ����
**************************************/
uint8_t Pre_MP3_Num=0;
uint8_t Select_MP3(uint16_t Num)
{
  if(Pre_MP3_Num==Num)//��������ͬ�����β�����
  {
    // if(MP3_Buzy==0) //������ //MP3_Buzyȱʧ
    // {
    //   Pre_MP3_Num=Num;
    //   return 0;
    // }
  }
  Pre_MP3_Num=Num;
  uint8_t Mp3_TX_Data[7]={0x04, 0xFB, 0x03, 0x06, 0x00, 0x00, 0x00};
  uint16_t Msum;
//  uint8_t ttt=0;
  
  Mp3_TX_Data[4]=Num>>8;
  Mp3_TX_Data[5]=Num&0xff;
  Msum=0x08+Mp3_TX_Data[4]+Mp3_TX_Data[5];
  Mp3_TX_Data[6]=Msum&0xff;
  USART_SendBuf(USART_MP3,Mp3_TX_Data,7);
//  Mp3_TX_Data_i=0;
//  USART_ITConfig(USART_MP3, USART_IT_TXE, ENABLE);//ʹ��USART2�ķ����ж� 
  /*
  
  //sDelayMs(280);
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0x04);
  Msum=0x04;
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0xFB);
  Msum+=0xFB;
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0x03);
  Msum+=0x03;
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0x06);
  Msum+=0x06;
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, Num>>8);
  Msum+=(Num>>8);
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, Num&0xff);
  Msum+=(Num&0xff); 
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, Msum&0xff);
  
  //sDelayMs(100);
  Received_mp3_acho=0;*/
  /*Ask_MP3_Num();//ȷ��MP3״̬
  i=300;
  while(--i)
  {
    sDelayMs(2);
    if(Received_mp3_acho)
    {
      Received_mp3_acho=0;
      if((Mp3_RX_Data[5]!=Num))//�Ѿ���ʼ���Ÿ���Ŀ��
        ttt=1;//error
      break;
    }
   }
  if(i==0)
    ttt=1; //error
  else
    ttt=0; //right
  if(ttt==1)
    sDelayMs(500);//�����⣬��ʱ4s
  */
//  return ttt;
  return 1;
}


/*******************��ѯ��ǰ��Ŀ*************
���ͣ�04 FB 01 0E 0E
���գ�04 FB 03 0E ��Ŀ��λ ��Ŀ��λ SM
********************************************/
void Ask_MP3_Num(void)
{
  Delay_10ms(28);
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0x04);
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0xFB);
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0x01);
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0x0E);
  while(USART_GetFlagStatus(USART_MP3, USART_FLAG_TC) == RESET); 
  USART_SendData8(USART_MP3, 0x0E);
  
}



void USART_MP3_Init(void)
{
  
    /* Enable USART clock */
    CLK_PeripheralClockConfig(USART_MP3_CLK, ENABLE);
    
    /* Configure USART Tx as alternate function push-pull  (software pull up)*/
    GPIO_ExternalPullUpConfig(USART_MP3_PORT, USART_MP3_TxPin, ENABLE);
    
    /* Configure USART Rx as alternate function push-pull  (software pull up)*/
    GPIO_ExternalPullUpConfig(USART_MP3_PORT, USART_MP3_RxPin, ENABLE);
    
    /* USART configuration */
    USART_Init(USART_MP3, (uint32_t)9600,USART_WordLength_8b, USART_StopBits_1,
                USART_Parity_No, (USART_Mode_TypeDef)(USART_Mode_Tx|USART_Mode_Rx));
    //USART_ITConfig(USART_MP3,USART_IT_RXNE,ENABLE);
    USART_Cmd(USART_MP3,ENABLE);
    USART_ClearFlag(USART_MP3 , USART_FLAG_TC);
}

void USART_SendBuf(USART_TypeDef* USARTx, uint8_t* Buf, uint16_t TxLen)
{
    uint16_t i;
    
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    
    for(i = 0; i < TxLen; i++)
    {
      USART_SendData8(USARTx, *Buf++);
      while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    }
}
