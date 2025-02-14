/*
 * @Author: Zhenwei Song zhenwei.song@qq.com
 * @Date: 2024-06-03 09:38:53
 * @LastEditors: Zhenwei Song zhenwei.song@qq.com
 * @LastEditTime: 2024-06-16 18:56:02
 * @FilePath: \emergency com 20240504V1.1\Applications\LORA_LED\main.c
 * @Description: 仅供学习交流使用
 * Copyright (c) 2024 by Zhenwei Song, All Rights Reserved.
 */
/*!
 * \file      main.c
 *
 * \brief
 *
 * \copyright 
 *
 * \code
 * REGION_CN470
 * \author
 *
 */

// #include "stm8l15x.h"
#include "msg.h"
// #include "timer.h"
#include "board.h"
// #include "bluetooth.h"
#include "N910x_mp3.h"

/**
 * Main application entry point.
 */
int main(void)
{
    BoardInitMcu();
    MsgLoop();
}

/*****************************END OF FILE******************************/
