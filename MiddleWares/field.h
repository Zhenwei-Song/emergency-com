/*
 * @Author: Zhenwei Song zhenwei.song@qq.com
 * @Date: 2024-06-05 15:49:20
 * @LastEditors: Zhenwei Song zhenwei_song@foxmail.com
 * @LastEditTime: 2024-12-09 15:51:47
 * @FilePath: \emergency com 20241209V1.7\MiddleWares\field.h
 * @Description: 仅供学习交流使用
 * Copyright (c) 2024 by Zhenwei Song, All Rights Reserved.
 */
#ifndef _FIELD_H
#define _FIELD_H

#include "stm8l15x.h"

const uint8_t SoftVersion[20] = "SoftVersion#"; // 后面会自动添加空字符
const uint8_t PowerState[20] = "PowerState#";
const uint8_t Mode[20] = "Mode#";
const uint8_t PowerLevel[20] = "PowerLevel#";
const uint8_t ModelID[20] = "ModelID#";

const uint8_t OFF[20] = "OFF";
const uint8_t Reset[20] = "Reset";

const uint8_t Emergency[20] = "Emergency#";
const uint8_t Controller[20] = "Controller#";
const uint8_t Terminal[20] = "Terminal#";

const uint8_t BOK[20] = "BOK#";
const uint8_t BError[20] = "BError#";
#endif