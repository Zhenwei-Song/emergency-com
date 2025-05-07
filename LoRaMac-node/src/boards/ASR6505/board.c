/*!
 * \file      board.c
 *
 * \brief     Target board general functions implementation
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
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#include "gpio.h"
#include "light.h"
#include "spi.h"
#include "timer.h"
#include <stdio.h>
// #include "rtc-board.h"
#include "N910x_mp3.h"
#include "board.h"
#include "gpio-board.h"
#include "my_uart.h"
#include "radio.h"
#include "sx126x-board.h"

/* -------------------------------------------------------------------------- */
/*                                  �����ϵ�Ϊ����ģʽ                                 */
/* -------------------------------------------------------------------------- */
bool is_controller = false;
bool is_terminal_normal = true;
bool is_terminal_emergency = false;

volatile bool McuStopFlag = true; //
volatile bool McuInitialized = false;

extern bool it_key_up_flag;
extern uint8_t keyState;
extern volatile bool keyDownFlag;

static void SystemClockConfig(void);
static void SystemClockReConfig(void);

static void open_Check(void);

char MY_ID = 1;

bool led_busy_flag = false;
/**
 * @description: �����̵���˸�����ڴ��ں�����ͨ�ţ����պͷ��;���˸��1s��˸2��
 * @return {*}
 */
void message_green_led(void)
{
    led_busy_flag = true;
    LED_R_Bat_Off;
    LED_G_Bat_On;
    Delay_10ms(33);
    LED_G_Bat_Off;
    Delay_10ms(33);
    LED_G_Bat_On;
    Delay_10ms(33);
    LED_G_Bat_Off;
    led_busy_flag = false;
}

void power_on_led(void)
{
    led_busy_flag = true;
    for (int i = 0; i < 3; i++) { // �̺콻�����˸ 3 ��
        LED_R_Bat_Off;
        LED_G_Bat_On;
        Delay_10ms(20);
        LED_G_Bat_Off;
        LED_R_Bat_On;
        Delay_10ms(20);
    }
    LED_R_Bat_Off;
    led_busy_flag = false;
}

void power_off_led(void)
{
    led_busy_flag = true;
    for (int i = 0; i < 2; i++) { // �̺콻�����˸ 2 ��
        LED_R_Bat_Off;
        LED_G_Bat_On;
        Delay_10ms(20);
        LED_G_Bat_Off;
        LED_R_Bat_On;
        Delay_10ms(20);
    }
    LED_R_Bat_Off;
    led_busy_flag = false;
}

uint8_t GetBoardPowerSource(void)
{
    return USB_POWER;
    // return BATTERY_POWER;
}

void BoardDisableIrq(void)
{
    disableInterrupts();
}

void BoardEnableIrq(void)
{
    enableInterrupts();
}

void BoardInitPeriph(void)
{
}

void BoardInitMcu(void)
{
    SystemClockConfig();
    // CFG->GCR |= CFG_GCR_SWD;//����swim��
    // DisableUnusedPeripherals();
    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE); //
    CLK_PeripheralClockConfig(CLK_Peripheral_SPI2, ENABLE); //
    // CLK_PeripheralClockConfig(CLK_Peripheral_USART2,ENABLE);//
    // CLK_PeripheralClockConfig(CLK_Peripheral_USART3,ENABLE);//
    CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE); //
    // CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, ENABLE); //

    BoardUnusedIoInit();
    Output_Pin_Init();

    SX126xIoInit(); // LORA

    disableInterrupts(); // ������ʼ��
                         // GPIO_Init(KEY1_GPIO_PORT, KEY1_GPIO_PIN, GPIO_Mode_In_PU_IT);
    EXTI_SetPinSensitivity((EXTI_Pin_TypeDef)EXTI_Pin_0, EXTI_Trigger_Falling);
    enableInterrupts();

    TIM4_Init(); // ��ʱ����ʼ��
#ifdef _debug_without_power
    McuStopFlag = false;
#else
    if (KEY1 == 0) // ��������ʧ��
    {
        LED_R_Bat_Off;
        LED_G_Bat_Off;
        PCTL_MCU_Off;
        McuStopFlag = true;
        LED_TEST_Off;
        while (1)
            ; // �ػ�
    }
    open_Check();
    PCTL_MCU_On;

#endif

    LED_TEST_On;

    SpiInit(&SX126x.Spi, RADIO_SPI_ID, RADIO_MOSI, RADIO_MISO, RADIO_SCLK, NC /*RADIO_NSS*/);
}

/*
 * @description: �����������
 * @return {*}
 */
void open_Check(void)
{
    u8 temp;
    if (KEY1 > 0) {    // ��������
        while (KEY1) { // �ȴ���������
        };
        if (keyState == 3) { // ��⵽��������,�Ұ�������3s������
            power_on_led();
            // PCTL_MCU_On;
            McuStopFlag = false;
            keyState = 0;
            // all_power_on();

            // Radio.SetRxDutyCycle(9600,54400);       //150ms , 850ms
            // Radio.Rx(RX_TIMEOUT_VALUE); // �������״̬
        }
        else { // ��������ʧ��
            LED_R_Bat_Off;
            LED_G_Bat_Off;
            PCTL_MCU_Off;
            McuStopFlag = true;
            LED_TEST_Off;
            while (1)
                ; // �ػ�
        }
    }
}

void BoardResetMcu(void)
{
    BoardDisableIrq();
}

void BoardDeInitMcu(void)
{
    // COMDeInit();
    SpiDeInit(&SX126x.Spi);
    SX126xIoDeInit();
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, DISABLE);
}

uint32_t BoardGetRandomSeed(void)
{
    return 0;
}

#define ID1 (0x4926)
#define ID2 (0x492A)
#define ID3 (0x492E)

void BoardGetUniqueId(uint8_t *id)
{
    id[7] = ((*(uint32_t *)ID1) + (*(uint32_t *)ID3)) >> 24;
    id[6] = ((*(uint32_t *)ID1) + (*(uint32_t *)ID3)) >> 16;
    id[5] = ((*(uint32_t *)ID1) + (*(uint32_t *)ID3)) >> 8;
    id[4] = ((*(uint32_t *)ID1) + (*(uint32_t *)ID3));
    id[3] = ((*(uint32_t *)ID2)) >> 24;
    id[2] = ((*(uint32_t *)ID2)) >> 16;
    id[1] = ((*(uint32_t *)ID2)) >> 8;
    id[0] = ((*(uint32_t *)ID2));
}

uint16_t BoardBatteryMeasureVolage(void)
{
    return 0;
}

uint32_t BoardGetBatteryVoltage(void)
{
    return 0;
}

uint8_t BoardGetBatteryLevel(void)
{
    return 0;
}
/*
void BoardUnusedIoInit( void )
{
    GPIO_Init(GPIOA, GPIO_Pin_All, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOB, GPIO_Pin_All, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOC, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOD, GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_5|GPIO_Pin_6, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOE, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_7, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOF, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_5|GPIO_Pin_3|GPIO_Pin_7, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOG, GPIO_Pin_All, GPIO_Mode_In_PU_No_IT);
    //GPIO_Init(GPIOG, GPIO_Pin_7, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOD, GPIO_Pin_0, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOH, GPIO_Pin_All, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOI, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3, GPIO_Mode_In_PU_No_IT);
}
*/
void BoardUnusedIoInit(void)
{
    GPIO_Init(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOB, GPIO_Pin_All, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOC, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7, GPIO_Mode_In_PU_No_IT);
    // #ifndef _Car_Master
    GPIO_Init(GPIOD, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_7, GPIO_Mode_In_PU_No_IT); // 3467
    // #else
    //     GPIO_Init(GPIOD, GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7, GPIO_Mode_In_PU_No_IT);//3467
    // #endif
    GPIO_Init(GPIOE, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5, GPIO_Mode_In_PU_No_IT); // 3567
    GPIO_Init(GPIOF, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Mode_In_FL_No_IT);                                                     // charge st
    GPIO_Init(GPIOF, GPIO_Pin_6, GPIO_Mode_In_PU_No_IT);                                                                  // 46
    GPIO_Init(GPIOG, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOH, GPIO_Pin_All, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOI, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3, GPIO_Mode_In_PU_No_IT);
}

/*
void BoardAllIoInit( void )
{

    GPIO_Init(GPIOA, GPIO_Pin_All, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOB, GPIO_Pin_All, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOC, GPIO_Pin_All, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOD, GPIO_Pin_All, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOE, GPIO_Pin_All, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOF, GPIO_Pin_All, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOG, GPIO_Pin_All, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOH, GPIO_Pin_All, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOI, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3, GPIO_Mode_In_PU_No_IT);

}
*/
void DisableUnusedPeripherals(void)
{
    //    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1,DISABLE);
    //    //CLK_PeripheralClockConfig(CLK_Peripheral_TIM4,DISABLE);//
    //    CLK_PeripheralClockConfig(CLK_Peripheral_USART2,DISABLE);//
    //    CLK_PeripheralClockConfig(CLK_Peripheral_USART3,DISABLE);//
    CLK_PeripheralClockConfig(CLK_Peripheral_RTC, DISABLE);  //
    CLK_PeripheralClockConfig(CLK_Peripheral_SPI2, DISABLE); //
}

#if 0 // SZW�޸�
void Output_Pin_Init(void)
{ 
    
    GPIO_Init(PCTL_MCU_PORT, PCTL_MCU_PIN, GPIO_Mode_Out_PP_Low_Slow);PCTL_MCU_On;   
    GPIO_Init(PCTL_MainBoard_PORT, PCTL_MainBoard_PIN, GPIO_Mode_Out_PP_Low_Slow);PCTL_MainBoard_On; //
    GPIO_Init(KEY_3588_PORT, KEY_3588_PIN, GPIO_Mode_Out_PP_Low_Slow);KEY_3588_High;
    
    GPIO_Init(PCTL_Location_PORT, PCTL_Location_PIN, GPIO_Mode_Out_PP_Low_Slow);PCTL_Location_On ;
    GPIO_Init(PCTL_GEO_PORT, PCTL_GEO_PIN, GPIO_Mode_Out_PP_Low_Slow);PCTL_GEO_On;
    GPIO_Init(PCTL_DMR_PORT, PCTL_DMR_PIN, GPIO_Mode_Out_PP_Low_Slow);PCTL_DMR_On;
    GPIO_Init(PCTL_HeadBoard_PORT, PCTL_HeadBoard_PIN, GPIO_Mode_Out_PP_Low_Slow);PCTL_HeadBoard_On;
    GPIO_Init(PCTL_5G_PORT, PCTL_5G_PIN, GPIO_Mode_Out_PP_Low_Slow);PCTL_5G_On; 

    GPIO_Init(PCTL_Adhoc_PORT, PCTL_Adhoc_PIN, GPIO_Mode_Out_PP_Low_Slow);PCTL_Adhoc_On;
//    GPIO_Init(PCTL_AD_PORT, PCTL_AD_PIN, GPIO_Mode_Out_PP_Low_Slow);PCTL_AD_Off;
//    GPIO_Init(GPIOC, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Slow);AD_BAT_On;
//    GPIO_Init(AD_BAT_CTL_PORT, AD_BAT_CTL_PIN, GPIO_Mode_Out_PP_Low_Slow);AD_BAT_On;
    GPIO_Init(LED_R_Bat_PORT, LED_R_Bat_PIN, GPIO_Mode_Out_PP_Low_Slow);LED_R_Bat_On;
    GPIO_Init(LED_G_Bat_PORT, LED_G_Bat_PIN, GPIO_Mode_Out_PP_Low_Slow);LED_G_Bat_Off;
    // GPIO_Init(LED_R_test_PORT, LED_R_test_PIN, GPIO_Mode_Out_PP_Low_Slow);LED_R_test_On; //SZW�޸�
}
#else // PCTL_MCU_On ��������Դ���ƹܽ�Ϊ��
void Output_Pin_Init(void)
{

    GPIO_Init(PCTL_MCU_PORT, PCTL_MCU_PIN, GPIO_Mode_Out_PP_Low_Slow);
    PCTL_MCU_On;
    GPIO_Init(PCTL_MainBoard_PORT, PCTL_MainBoard_PIN, GPIO_Mode_Out_PP_Low_Slow);
    // PCTL_MainBoard_Off; //
    PCTL_MainBoard_On; //
    GPIO_Init(KEY_3588_PORT, KEY_3588_PIN, GPIO_Mode_Out_PP_Low_Slow);
    // KEY_3588_High; 3588������λ������û�ϵ�ǰ�����ߵ�ƽ

    GPIO_Init(PCTL_Location_PORT, PCTL_Location_PIN, GPIO_Mode_Out_PP_Low_Slow);
    // PCTL_Location_Off;
    PCTL_Location_On;
    GPIO_Init(PCTL_GEO_PORT, PCTL_GEO_PIN, GPIO_Mode_Out_PP_Low_Slow);
    // PCTL_GEO_Off;
    PCTL_GEO_On;
    GPIO_Init(PCTL_DMR_PORT, PCTL_DMR_PIN, GPIO_Mode_Out_PP_Low_Slow);
    // PCTL_DMR_Off;
    PCTL_DMR_On;
    GPIO_Init(PCTL_HeadBoard_PORT, PCTL_HeadBoard_PIN, GPIO_Mode_Out_PP_Low_Slow);
    // PCTL_HeadBoard_Off;
    PCTL_HeadBoard_On;
    GPIO_Init(PCTL_5G_PORT, PCTL_5G_PIN, GPIO_Mode_Out_PP_Low_Slow);
    // PCTL_5G_Off;
    PCTL_5G_On;
    GPIO_Init(PCTL_Adhoc_PORT, PCTL_Adhoc_PIN, GPIO_Mode_Out_PP_Low_Slow);
    // PCTL_Adhoc_Off;
    PCTL_Adhoc_On;
    //    GPIO_Init(PCTL_AD_PORT, PCTL_AD_PIN, GPIO_Mode_Out_PP_Low_Slow);PCTL_AD_Off;
    //    GPIO_Init(GPIOC, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Slow);AD_BAT_On;
    //    GPIO_Init(AD_BAT_CTL_PORT, AD_BAT_CTL_PIN, GPIO_Mode_Out_PP_Low_Slow);AD_BAT_On;
    GPIO_Init(LED_R_Bat_PORT, LED_R_Bat_PIN, GPIO_Mode_Out_PP_Low_Slow);
    LED_R_Bat_Off;
    GPIO_Init(LED_G_Bat_PORT, LED_G_Bat_PIN, GPIO_Mode_Out_PP_Low_Slow);
    LED_G_Bat_Off;
    GPIO_Init(LED_R_test_PORT, LED_R_test_PIN, GPIO_Mode_Out_PP_Low_Slow);
    LED_TEST_On;
    GPIO_Init(Light_PORT, Light_PIN, GPIO_Mode_Out_PP_Low_Slow);
    Light_On;
    Light_Off;
}
#endif // SZW�޸�

void PowerDown_AD_UART_Init(void) // ��ģ�����ӵ�IO����Ҫ��������
{

    ADC_VrefintCmd(DISABLE);                                 // ʹ���ڲ��ο���ѹ
    ADC_Cmd(ADC1, DISABLE);                                  // ADCʹ��
    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, DISABLE); // ����ADC1ʱ��

    //  USART_Cmd(USART_MP3,DISABLE);
    //  CLK_PeripheralClockConfig(USART_MP3_CLK, DISABLE);
    //  GPIO_Init(GPIOG, GPIO_Pin_0, GPIO_Mode_Out_OD_Low_Slow);
    //  GPIO_Init(GPIOG, GPIO_Pin_1, GPIO_Mode_Out_OD_Low_Slow);

    USART_Cmd(USART2, DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_USART2, DISABLE);
    GPIO_Init(GPIOH, GPIO_Pin_4, GPIO_Mode_In_FL_No_IT);
    GPIO_Init(GPIOH, GPIO_Pin_5, GPIO_Mode_In_FL_No_IT);
    GPIO_Init(GPIOB, GPIO_Pin_4, GPIO_Mode_In_FL_No_IT); // Link
    GPIO_Init(KEY_3588_PORT, KEY_3588_PIN, GPIO_Mode_In_FL_No_IT);
    GPIO_Init(ST_3588_PORT, ST_3588_PIN, GPIO_Mode_In_FL_No_IT);
    //  RST_BT_Off;
    //  Set_BT_LP;
    // RST_BT_On;

    //  GPIO_Init(GPIOG, GPIO_Pin_7, GPIO_Mode_In_FL_No_IT);
}
void SystemClockConfig(void)
{
    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_4);

////////////////////
#if 1
    // Enable LSE
    CLK_LSEConfig(CLK_LSE_ON);
    // Wait for LSE clock to be ready
    while (CLK_GetFlagStatus(CLK_FLAG_LSERDY) == RESET)
        ;

    // Select LSE (32.768 KHz) as RTC clock source
    CLK_RTCClockConfig(CLK_RTCCLKSource_LSE, CLK_RTCCLKDiv_1);
#else
    CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);
#endif

    CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);
}

void SystemClockReConfig(void)
{
    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
}

// ����Ҫ����
void McuEnterLowPowerStopMode(void)
{

    // if (Radio.GetStatus() != RF_IDLE)
    // {
    //     return;
    // }

    // Radio.Standby();
    // DelayMs(5);
    // Radio.Sleep();

    // Ҫ���ڷ��ͽ����ź�

    FLASH_Lock(FLASH_MemType_Data);

    // all_light_Off();
    PowerDown_AD_UART_Init();
    // all_power_Off();

    PCTL_Adhoc_Off; // �ر����е�Դ
    PCTL_5G_Off;
    PCTL_HeadBoard_Off;
    PCTL_DMR_Off;
    PCTL_GEO_Off;
    PCTL_Location_Off;
    PCTL_MainBoard_Off;
    LED_R_Bat_Off;
    LED_G_Bat_Off;
    LED_TEST_Off;
    Light_Off;

    // PCTL_Location_Flag = false;
    // PCTL_GEO_Flag = false;
    // PCTL_DMR_Flag = false;
    // PCTL_HeadBoard_Flag = false;
    // PCTL_5G_Flag = false;
    // PCTL_Adhoc_Flag = false;

    //    GPIO_Init(GPIOH, GPIO_Pin_4, GPIO_Mode_Out_OD_Low_Slow);
    //    GPIO_Init(GPIOH, GPIO_Pin_5, GPIO_Mode_Out_OD_Low_Slow);
    //    GPIO_Init(GPIOB,GPIO_Pin_4,GPIO_Mode_Out_OD_Low_Slow);//Link
    //    GPIO_Init(RST_BT_PORT,RST_BT_PIN,GPIO_Mode_Out_OD_Low_Slow);
    //    GPIO_Init(LP_BT_PORT,LP_BT_PIN,GPIO_Mode_Out_OD_Low_Slow);
    ///////*����ûӰ��*//////////////////////////////////////////////////////////////
    //    ADC_VrefintCmd(DISABLE); //ʹ���ڲ��ο���ѹ
    //    ADC_Cmd(ADC1,DISABLE);//ADCʹ��
    //    //TIM4_DeInit();
    //    USART_DeInit(USART2);USART_DeInit(USART3);
    //    ADC_DeInit(ADC1);
    // RTC_DeInit();
    // DisableUnusedPeripherals();//�ر�����ʱ��
    /////////*�ܽ�����*//////////////////////////////////////////////////////////////////
    //     GPIO_Init(GPIOA,GPIO_Pin_1|GPIO_Pin_4, GPIO_Mode_In_PU_No_IT);
    //     GPIO_Init(GPIOA,GPIO_Pin_6, GPIO_Mode_Out_PP_Low_Slow);//TXEN

    //     GPIO_Init(GPIOC,GPIO_Pin_0|GPIO_Pin_2|GPIO_Pin_3, GPIO_Mode_In_PU_No_IT);

    // //    GPIO_Init(GPIOG,GPIO_Pin_0|GPIO_Pin_1,GPIO_Mode_In_FL_No_IT);//USART3,Ӱ��ܴ�200uA
    // //    GPIO_Init(GPIOH,GPIO_Pin_4|GPIO_Pin_5,GPIO_Mode_In_PU_No_IT);//USART2��Ӱ��ܴ�

    //     GPIO_Init(GPIOH,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2,GPIO_Mode_In_FL_No_IT);//

    //     GPIO_Init(GPIOG,GPIO_Pin_6,GPIO_Mode_In_PU_No_IT);//NSS
    //     GPIO_Init(GPIOI,GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3,GPIO_Mode_In_FL_No_IT);//

    //     GPIO_Init(LED_R_Bat_PORT, LED_R_Bat_PIN, GPIO_Mode_Out_PP_Low_Slow);
    //     GPIO_Init(LED_G_Bat_PORT, LED_G_Bat_PIN, GPIO_Mode_Out_PP_Low_Slow);
    //     LED_R_Bat_Off;
    //     LED_G_Bat_Off;

    //     disableInterrupts();
    //     GPIO_Init(GPIOA, GPIO_Pin_5, GPIO_Mode_In_PU_IT);
    //     GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_In_PU_IT);
    //     EXTI_SetPinSensitivity((EXTI_Pin_TypeDef)EXTI_Pin_5,EXTI_Trigger_Falling);
    //     EXTI_SetPinSensitivity((EXTI_Pin_TypeDef)EXTI_Pin_1,EXTI_Trigger_Rising);

    //     GPIO_Init(KEY1_GPIO_PORT, KEY1_GPIO_PIN, GPIO_Mode_In_PU_IT);
    //     EXTI_SetPinSensitivity((EXTI_Pin_TypeDef)EXTI_Pin_0, EXTI_Trigger_Falling);

    //     enableInterrupts();

    // �����������������볬�͹���ģʽ
    //PWR_UltraLowPowerCmd(ENABLE);
    // ����ͣ��ģʽ
    //halt();
}

/**
 * @description: RTCʱ�����ã����ڵ͹���ģʽ�����Ի���
 * @return {*}
 */
void RTC_Config(void)
{
    CLK_LSICmd(ENABLE);                                        // ʹ��LSI
    CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1); // RTCʱ��ԴLSI, 1��Ƶ=38K
    while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET)
        ; // �ȴ�LSI����
    RTC_WakeUpCmd(DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE); // RTCʱ��
    RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);  // 38K/16=2.375k=0.421ms
    RTC_ITConfig(RTC_IT_WUT, ENABLE);                      // �����ж�
    RTC_SetWakeUpCounter(2375 * 2 * 5);                    // 2375*0.421=1S����  5ss
                                                           // ITC_SetSoftwarePriority(RTC_CSSLSE_IRQn, ITC_PriorityLevel_3);// ���ȼ�
    enableInterrupts();
}
