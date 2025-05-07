/*!
 * \file      board-config.h
 *
 * \brief     Board configuration
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    Daniel Jaeckle ( STACKFORCE )
 *
 * \author    Johannes Bruder ( STACKFORCE )
 */
#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#define Master   1        // 1:master, 0:slaver
#define deviceID 99990001 // YYYYXXXX  ������ʼ��ű���Ϊ9999xxxx
// #define     deviceID     19       //YYYYXXXX  �����ʼ��ű���Ϊ0000xxxx~8999xxxx

// Car_Master��������ڸ�װ����������4��������3��ָʾ��
// ����0 ���� ���ػ� ��Դֱ�ӵ��
// ����1 ģʽѡ�����ӻ�������ͬ(PA5�� ģʽѡ�񣬶�Ӧ��ΪPE7 ���
// ����2  ����2s ���䣬��Ӧ����ΪPD7,  (ȡ���ӻ������ƵĿ���), ��Ӧ��Ϊ PE6 �̵�
// ����3 ��ѯ��ǰ״̬��ť�����ӵ�PD6��(ȡ���ӻ���LED��Դ����), ��Ӧ����Ч
// #define  _Car_Master

#include "stm8l15x.h"

#define MY_VERSION 1.9

// #define _debug_without_power
// #define USING_USART3 //调试串口

#define USING_CAD  // 使用信道活动检测来避免数据包碰撞，不启用CAD则使用信号强度进行冲突检测

extern char MY_ID;

// #define _debug_without_power

/*!
 * Defines the time required for the TCXO to wakeup [ms].
 */
#define BOARD_TCXO_WAKEUP_TIME 5 //

/*!
 * Board MCU pins definitions
 */

#define RADIO_SPI_ID           SPI_2

#define RADIO_RESET            PH_0
#define RADIO_MOSI             PI_2
#define RADIO_MISO             PI_3
#define RADIO_SCLK             PI_1
#define RADIO_BUSY             PH_1
#define RADIO_DIO_1            PH_2

#define RADIO_NSS              PG_6 // ��GPIO1������һ��
// #define RADIO_ANT_SWITCH_POWER                      PG_6  //��GPIO0������һ��
#define Radio_Busy             GPIO_ReadInputDataBit(GPIOH, GPIO_Pin_1)

#define KEY1_GPIO_PORT         GPIOF
#define KEY1_GPIO_PIN          GPIO_Pin_0

#ifdef _debug_without_power
#define KEY1 0
#else
#define KEY1 GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_GPIO_PIN)
#endif

#define PCTL_AD_PORT              GPIOG
#define PCTL_AD_PIN               GPIO_Pin_7
#define AD_BAT_CTL_PORT           GPIOG
#define AD_BAT_CTL_PIN            GPIO_Pin_7

#define Chargin_Det               GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_1)

#define Light_PORT                GPIOC /*照明灯*/
#define Light_PIN                 GPIO_Pin_0
#define LED_R_test_PORT           GPIOE
#define LED_R_test_PIN            GPIO_Pin_7 /*need init*/

#define LED_R_Bat_PORT            GPIOE
#define LED_R_Bat_PIN             GPIO_Pin_6
#define LED_G_Bat_PORT            GPIOC
#define LED_G_Bat_PIN             GPIO_Pin_4

// #define LED_G_Bat_PORT          GPIOE
// #define LED_G_Bat_PIN           GPIO_Pin_6
// #define LED_R_Bat_PORT          GPIOC
// #define LED_R_Bat_PIN           GPIO_Pin_4

// #define PCTL_MainBoard_PORT     GPIOD
// #define PCTL_MainBoard_PIN      GPIO_Pin_6 /*need init*/
#define PCTL_MainBoard_PORT       GPIOF
#define PCTL_MainBoard_PIN        GPIO_Pin_5 /*need init*/
#define KEY_3588_PORT             GPIOD
#define KEY_3588_PIN              GPIO_Pin_0
#define ST_3588_PORT              GPIOB
#define ST_3588_PIN               GPIO_Pin_4
#define ST_3588                   GPIO_ReadInputDataBit(ST_3588_PORT, ST_3588_PIN)

/*need init*/
#define PCTL_MCU_PORT             GPIOD
#define PCTL_MCU_PIN              GPIO_Pin_5
#define PCTL_Location_PORT        GPIOD
#define PCTL_Location_PIN         GPIO_Pin_6
#define PCTL_GEO_PORT             GPIOD
#define PCTL_GEO_PIN              GPIO_Pin_4
#define PCTL_DMR_PORT             GPIOF
#define PCTL_DMR_PIN              GPIO_Pin_7
#define PCTL_HeadBoard_PORT       GPIOF
#define PCTL_HeadBoard_PIN        GPIO_Pin_4
#define PCTL_5G_PORT              GPIOF
#define PCTL_5G_PIN               GPIO_Pin_3
#define PCTL_Adhoc_PORT           GPIOF
#define PCTL_Adhoc_PIN            GPIO_Pin_2

// ��ɫ����ָʾ�ƣ�����--��������50%��1s��˸һ�� ��ѹ����30%��1s��˸2�Σ���������10%��1s��˸3�Σ���������10%
// ��ɫָʾ�ƣ� ��˸һ�α�ʾһ�����߷�����߽���
#define AD_BAT_Off                GPIO_ResetBits(AD_BAT_CTL_PORT, AD_BAT_CTL_PIN)
#define AD_BAT_On                 GPIO_SetBits(AD_BAT_CTL_PORT, AD_BAT_CTL_PIN)

#define Light_On                  GPIO_SetBits(Light_PORT, Light_PIN)
#define Light_Off                 GPIO_ResetBits(Light_PORT, Light_PIN)
#define LED_TEST_On               GPIO_ResetBits(LED_R_test_PORT, LED_R_test_PIN)
#define LED_TEST_Off              GPIO_SetBits(LED_R_test_PORT, LED_R_test_PIN)

#define LED_R_Bat_Off             GPIO_ResetBits(LED_R_Bat_PORT, LED_R_Bat_PIN)
#define LED_R_Bat_On              GPIO_SetBits(LED_R_Bat_PORT, LED_R_Bat_PIN)
#define LED_G_Bat_Off             GPIO_ResetBits(LED_G_Bat_PORT, LED_G_Bat_PIN)
#define LED_G_Bat_On              GPIO_SetBits(LED_G_Bat_PORT, LED_G_Bat_PIN)
#define Green_Work_LED_Out_Status GPIO_ReadOutputDataBit(LED_G_Bat_PORT, LED_G_Bat_PIN)

#define PCTL_MainBoard_On         GPIO_SetBits(PCTL_MainBoard_PORT, PCTL_MainBoard_PIN)
#define PCTL_MainBoard_Off        GPIO_ResetBits(PCTL_MainBoard_PORT, PCTL_MainBoard_PIN)
#define KEY_3588_High             GPIO_SetBits(KEY_3588_PORT, KEY_3588_PIN)
#define KEY_3588_Low              GPIO_ResetBits(KEY_3588_PORT, KEY_3588_PIN)

#define PCTL_MCU_On               GPIO_SetBits(PCTL_MCU_PORT, PCTL_MCU_PIN)
#define PCTL_MCU_Off              GPIO_ResetBits(PCTL_MCU_PORT, PCTL_MCU_PIN)

// #define Set_BT_LP      {GPIO_ResetBits(LP_BT_PORT,LP_BT_PIN); DelayMs(2);GPIO_SetBits(LP_BT_PORT,LP_BT_PIN);}
// #define Set_BT_Work    {GPIO_SetBits(LP_BT_PORT,LP_BT_PIN); DelayMs(2);GPIO_ResetBits(LP_BT_PORT,LP_BT_PIN);}

#define PCTL_Location_On          GPIO_SetBits(PCTL_Location_PORT, PCTL_Location_PIN)
#define PCTL_Location_Off         GPIO_ResetBits(PCTL_Location_PORT, PCTL_Location_PIN)
#define PCTL_GEO_On               GPIO_SetBits(PCTL_GEO_PORT, PCTL_GEO_PIN)
#define PCTL_GEO_Off              GPIO_ResetBits(PCTL_GEO_PORT, PCTL_GEO_PIN)
#define PCTL_DMR_On               GPIO_SetBits(PCTL_DMR_PORT, PCTL_DMR_PIN)
#define PCTL_DMR_Off              GPIO_ResetBits(PCTL_DMR_PORT, PCTL_DMR_PIN)
#define PCTL_HeadBoard_On         GPIO_SetBits(PCTL_HeadBoard_PORT, PCTL_HeadBoard_PIN)
#define PCTL_HeadBoard_Off        GPIO_ResetBits(PCTL_HeadBoard_PORT, PCTL_HeadBoard_PIN)
#define PCTL_5G_On                GPIO_SetBits(PCTL_5G_PORT, PCTL_5G_PIN)
#define PCTL_5G_Off               GPIO_ResetBits(PCTL_5G_PORT, PCTL_5G_PIN)
#define PCTL_Adhoc_On             GPIO_SetBits(PCTL_Adhoc_PORT, PCTL_Adhoc_PIN)
#define PCTL_Adhoc_Off            GPIO_ResetBits(PCTL_Adhoc_PORT, PCTL_Adhoc_PIN)

#define PCTL_Read_Location        GPIO_ReadOutputDataBit(PCTL_Location_PORT, PCTL_Location_PIN)
#define PCTL_Read_GEO             GPIO_ReadOutputDataBit(PCTL_GEO_PORT, PCTL_GEO_PIN)
#define PCTL_Read_DMR             GPIO_ReadOutputDataBit(PCTL_DMR_PORT, PCTL_DMR_PIN)
#define PCTL_Read_HeadBoard       GPIO_ReadOutputDataBit(PCTL_HeadBoard_PORT, PCTL_HeadBoard_PIN)
#define PCTL_Read_5G              GPIO_ReadOutputDataBit(PCTL_5G_PORT, PCTL_5G_PIN)
#define PCTL_Read_Adhoc           GPIO_ReadOutputDataBit(PCTL_Adhoc_PORT, PCTL_Adhoc_PIN)
/* -------------------------------------------------------------------------- */
/*                                 ��ʱ���ӣ�����������  SZW�޸�                              */
/* -------------------------------------------------------------------------- */

#endif // __BOARD_CONFIG_H__
