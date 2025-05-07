/*
 * @Author: Zhenwei Song zhenwei.song@qq.com
 * @Date: 2024-06-03 09:38:54
 * @LastEditors: Zhenwei Song zhenwei_song@foxmail.com
 * @LastEditTime: 2025-05-07 16:02:38
 * @FilePath: \emergency com for git\MiddleWares\key.c
 * @Description: 仅供学习交流使用
 * Copyright (c) 2024 by Zhenwei Song, All Rights Reserved.
 */
#include "key.h"
#include "board-config.h"
#include "board.h"
// #include "stm815x_exti.h"
// #include "stm815x_gpio.h"
// volatile uint8_t keyState=0;

extern bool keyDownFlag;
extern bool it_key_up_flag;
extern uint8_t keyState;

/**
 *在外部中断中调用
 */
void Key_Detected(void)
{
    if (KEY1 == 1) {
        keyDownFlag = true;
    }
    else if (KEY1 == 0) {
        it_key_up_flag = true;
    }
}

void EXIT_KEY_Init(void)
{
    // disableInterrupts();
    GPIO_DeInit(KEY1_GPIO_PORT);
    GPIO_Init(KEY1_GPIO_PORT, KEY1_GPIO_PIN, GPIO_Mode_In_PU_IT);
    // EXIT_DeInit();
    EXTI_SetPinSensitivity((EXTI_Pin_TypeDef)KEY1_GPIO_PIN, EXTI_Trigger_Falling); // 设置外部中断触发方式为下降沿触发
    // enableInterrupts();//使能总中断
}

// static void Delay(uint32_t temp)
// {
//     for (; temp != 0; temp--)
//         ;
// }

// void KEY_Down(GPIO_TypeDef *GPIOx, GPIO_Pin_TypeDef GPIO_Pin)
// {
//     /*检测是否有按键按下 */
//     if (GPIO_ReadInputPin(GPIOx, GPIO_Pin) == 0) {
//         /*延时消抖*/
//         Delay(10000);
//         if (GPIO_ReadInputPin(GPIOx, GPIO_Pin) == 0) {
//             /*等待按键释放 */
//             while (GPIO_ReadInputPin(GPIOx, GPIO_Pin) == 0)
//                 ;
//             keyDownFlag = true;
//         }
//         else {
//         }
//     }
// }
