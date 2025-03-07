#include "battery.h"
#include "board-config.h"
#include "timer.h"
#include <stm8l15x.h>

uint8_t Bat_Volt_int_cnt = 0;
uint8_t Bat_Value_Cnt = 0, Bat_Sampled_Over = 0;
double Volt_Bat1 = 4.2;
uint32_t Bat_AD_Value = 262000;
uint32_t Bat_int = 0;
uint8_t Energy_Level = 0;
float Energy_out = 0;
uint8_t System_Low_Power_Flag = 1, Low_Power_Cnt = 0;
uint8_t First_AD_Flag = 0;
bool bat_power_off_flag = false;
void battery_init(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE); // 开启ADC1时钟
    GPIO_Init(GPIOC, GPIO_Pin_2, GPIO_Mode_In_FL_No_IT);
    ADC_Init(ADC1, ADC_ConversionMode_Single, ADC_Resolution_12Bit, ADC_Prescaler_1);
    ADC_ChannelCmd(ADC1, ADC_Channel_6, ENABLE);
    ADC_VrefintCmd(ENABLE); // 使能内部参考电压
    ADC_Cmd(ADC1, ENABLE);  // ADC使能
    Bat_Value_Cnt = 0;
    Bat_int = 0;
    Bat_Sampled_Over = 0;
    Volt_Bat1 = 4.2;
    GPIO_Init(AD_BAT_CTL_PORT, AD_BAT_CTL_PIN, GPIO_Mode_Out_PP_Low_Fast);
    AD_BAT_On;
    Delay_10ms(2);
    ADC_SoftwareStartConv(ADC1); // 开启软件转换
    Delay_10ms(1);
    First_AD_Flag = 1;
}
uint16_t get_battery_reading(void)
{
    ADC_SoftwareStartConv(ADC1); // 开启软件转换
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
        ;                                // 等待转换结束
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);   // 清除对应标志
    return ADC_GetConversionValue(ADC1); // 获取转换值
}

void battery_sample_filter(void)
{
    /* Bat_int+=ADC_GetConversionValue(ADC1);
     Bat_Value_Cnt++;
     if(Bat_Value_Cnt==64)
     {
       Bat_AD_Value=Bat_int>>6;
       Bat_Value_Cnt=0;Bat_int=0;
     }*/
}

// /**
//  * @description: led显示电量，SZW添加
//  * @return {*}
//  */
// void battery_led(void)
// {
//     if (Energy_Level >= 2) { // 电量大于50
//         LED_R_Bat_On;
//         Delay_10ms(100);
//         LED_R_Bat_Off;
//     }
//     else if (Energy_Level >= 1) { // 电量大于25
//     }
//     else if (Energy_Level >= 0) { // 电量大于10
//     }
//     else if (System_Low_Power_Flag == 2) { // 电量低于10
//     }
// }

void battery_det(void)
{
    if (First_AD_Flag == 1) {
        Volt_Bat1 = 4.2;
        First_AD_Flag = 2;
    }
    else {
        Volt_Bat1 = Bat_AD_Value * 0.00262395523268; // 3.3/3*239/39 /(4096*64)* 100*1.0204
    }
    Energy_out = Volt_Bat1 * 3 / 100;
    Energy_out = Energy_out + BAT_CORRECTED;
    Energy_Level = 5;
    if (Energy_out >= 12) {
        Energy_Level = 4;
        System_Low_Power_Flag = 1;
        Low_Power_Cnt = 0;
    }
    else if (Energy_out >= 11.5) {
        Energy_Level = 3;
        System_Low_Power_Flag = 1;
        Low_Power_Cnt = 0;
    }
    else if (Energy_out >= 11.0) {
        Energy_Level = 2;
        System_Low_Power_Flag = 1;
        Low_Power_Cnt = 0;
    }
    else if (Energy_out >= 10.5) {
        Energy_Level = 1;
        System_Low_Power_Flag = 1;
        Low_Power_Cnt = 0;
    }
    else if (Energy_out >= 10) {
        Energy_Level = 0;
        System_Low_Power_Flag = 1;
        Low_Power_Cnt = 0;
    }
    else {
        Low_Power_Cnt++;
        Energy_Level = 0;
        if (Low_Power_Cnt >= 3)
            System_Low_Power_Flag = 2;
    }
    // if (Volt_Bat1 >= BAT_100_TH) {
    //     Energy_Level = 4;
    //     System_Low_Power_Flag = 1;
    //     Low_Power_Cnt = 0;
    // }
    // else if (Volt_Bat1 >= BAT_75_TH) {
    //     Energy_Level = 3;
    //     System_Low_Power_Flag = 1;
    //     Low_Power_Cnt = 0;
    // }
    // else if (Volt_Bat1 >= BAT_50_TH) {
    //     Energy_Level = 2;
    //     System_Low_Power_Flag = 1;
    //     Low_Power_Cnt = 0;
    // }
    // else if (Volt_Bat1 >= BAT_25_TH) {
    //     Energy_Level = 1;
    //     System_Low_Power_Flag = 1;
    //     Low_Power_Cnt = 0;
    // }
    // else if (Volt_Bat1 >= BAT_10_TH) {
    //     Energy_Level = 0;
    //     System_Low_Power_Flag = 1;
    //     Low_Power_Cnt = 0;
    // }
    // else {
    //     Low_Power_Cnt++;
    //     Energy_Level = 0;
    //     if (Low_Power_Cnt >= 3)
    //         System_Low_Power_Flag = 2;
    // }
    ADC_SoftwareStartConv(ADC1); // 开启软件转换
}

/**
 * @description: 电量状态显示，大于75，红灯常亮；大于50，一秒闪烁一次；大于25，一秒两次；20以下，一秒三次
 * @return {*}
 */
void Flash_Status_LED(void)
{
    switch (Energy_Level) {
    case 4: //>12
        LED_R_Bat_On;
        break;
    case 3: // 1s 闪烁1次,>11.5
        if (time_4s_cnt < 100)
            LED_R_Bat_On;
        else
            LED_R_Bat_Off;
        break;
    case 2: // 1s 闪烁2次,>11.0
        if (time_4s_cnt < 25)
            LED_R_Bat_On;
        else if (time_4s_cnt < 50)
            LED_R_Bat_Off;
        else if (time_4s_cnt < 75)
            LED_R_Bat_On;
        else
            LED_R_Bat_Off;
        break;
    case 1: // 1s 闪烁3次,>10.5
        if (time_4s_cnt < 16)
            LED_R_Bat_On;
        else if (time_4s_cnt < 32)
            LED_R_Bat_Off;
        else if (time_4s_cnt < 48)
            LED_R_Bat_On;
        else if (time_4s_cnt < 64)
            LED_R_Bat_Off;
        else if (time_4s_cnt < 80)
            LED_R_Bat_On;
        else
            LED_R_Bat_Off;
        break;
    case 0: // 1s 闪烁4次,>10
        if (time_4s_cnt < 12)
            LED_R_Bat_On;
        else if (time_4s_cnt < 25)
            LED_R_Bat_Off;
        else if (time_4s_cnt < 37)
            LED_R_Bat_On;
        else if (time_4s_cnt < 50)
            LED_R_Bat_Off;
        else if (time_4s_cnt < 62)
            LED_R_Bat_On;
        else if (time_4s_cnt < 75)
            LED_R_Bat_Off;
        else if (time_4s_cnt < 87)
            LED_R_Bat_On;
        else
            LED_R_Bat_Off;
        break;
    default: // 1s 闪烁5次,<10
        // if (time_4s_cnt < 10)
        //     LED_R_Bat_On;
        // else if (time_4s_cnt < 20)
        //     LED_R_Bat_Off;
        // else if (time_4s_cnt < 30)
        //     LED_R_Bat_On;
        // else if (time_4s_cnt < 40)
        //     LED_R_Bat_Off;
        // else if (time_4s_cnt < 50)
        //     LED_R_Bat_On;
        // else if (time_4s_cnt < 60)
        //     LED_R_Bat_Off;
        // else if (time_4s_cnt < 70)
        //     LED_R_Bat_On;
        // else if (time_4s_cnt < 80)
        //     LED_R_Bat_Off;
        // else if (time_4s_cnt < 90)
        //     LED_R_Bat_On;
        // else
        //     LED_R_Bat_Off;
        // break;
        bat_power_off_flag = true;
        break;
    }
}