#include "light.h"
#include "board-config.h"
#include "stdbool.h"
#include "timer.h"

extern volatile uint16_t time_1s_cnt, time_10s_cnt;
extern volatile bool lightEnable;
extern volatile uint8_t LED_Max_On_10ms_Cnt;

 /* -------------------------------------------------------------------------- */
 /*    LED_R1_Off; LED_R2_Off; LED_G1_Off? LED_G2_Off; LED_B1_On; LED_B2_On;  ??? SZW?? */
 /* -------------------------------------------------------------------------- */

// À¶É«ÂýÉÁ
void ledB_toggle(void)
{
    //LED_R1_Off;
    //LED_R2_Off;
    //LED_G1_Off;
    //LED_G2_Off;
    if (time_1s_cnt == 50) {
        //LED_B1_On;
        //LED_B2_On;
    }
    else if (time_1s_cnt == 50 + LED_Max_On_10ms_Cnt) {
        //LED_B1_Off;
        //LED_B2_Off;
    }
}
// ÂÌÉ«ÂýÉÁ
void ledG_toggle(void)
{
    //LED_R1_Off;
    //LED_R2_Off;
    //LED_B1_Off;
    //LED_B2_Off;
    if (time_1s_cnt == 50) {
        //LED_G1_On;
        //LED_G2_On;
    }
    else if (time_1s_cnt == 50 + LED_Max_On_10ms_Cnt) {
        //LED_G1_Off;
        //LED_G2_Off;
    }
}
// »ÆÉ«ÂýÉÁ
void ledY_toggle(void)
{
    //LED_B1_Off;
    //LED_B2_Off;
    if (time_1s_cnt == 50) {
        //LED_G1_On;
        //LED_G2_On;
        //LED_R1_On;
        //LED_R2_On;
    }
    else if (time_1s_cnt == 50 + LED_Max_On_10ms_Cnt) {
        //LED_R1_Off;
        //LED_R2_Off;
        //LED_G1_Off;
        //LED_G2_Off;
    }
}

// ºìÉ«ÂýÉÁ
void ledR_toggle(void)
{
    //LED_G1_Off;
    //LED_G2_Off;
    //LED_B1_Off;
    //LED_B2_Off;
    if (time_1s_cnt == 50) {
        //LED_R1_On;
        //LED_R2_On;
    }
    else if (time_1s_cnt == 5 + LED_Max_On_10ms_Cnt) {
        //LED_R1_Off;
        //LED_R2_Off;
    }
}

// À¶É«½»Ìæ
void ledB_LR_toggle(void)
{
    //LED_R1_Off;
    //LED_R2_Off;
    //LED_G1_Off;
    //LED_G2_Off;
    if (time_1s_cnt == 50) {
        //LED_B2_Off;
        //LED_B1_On;
    }
    else if (time_1s_cnt == 50 + LED_Max_On_10ms_Cnt) {
        //LED_B2_Off;
        //LED_B1_Off;
    }
    else if (time_1s_cnt == 0) {
        //LED_B1_Off;
        //LED_B2_On;
    }
    else if (time_1s_cnt == LED_Max_On_10ms_Cnt) {
        //LED_B2_Off;
        //LED_B1_Off;
    }
}
// ÂÌÉ«½»Ìæ
void ledG_LR_toggle(void)
{
    //LED_R1_Off;
    //LED_R2_Off;
    //LED_B1_Off;
    //LED_B2_Off;
    if (time_1s_cnt == 50) {
        //LED_G2_Off;
        //LED_G1_On;
    }
    else if (time_1s_cnt == 50 + LED_Max_On_10ms_Cnt) {
        //LED_G2_Off;
        //LED_G1_Off;
    }
    else if (time_1s_cnt == 0) {
        //LED_G1_Off;
        //LED_G2_On;
    }
    else if (time_1s_cnt == LED_Max_On_10ms_Cnt) {
        //LED_G2_Off;
        //LED_G1_Off;
    }
}
// »ÆÉ«½»Ìæ
void ledY_LR_toggle(void)
{
    //LED_B1_Off;
    //LED_B2_Off;
    if (time_1s_cnt == 50) {
        //LED_R2_Off;
        //LED_G2_Off;
        //LED_G1_On;
        //LED_R1_On;
    }
    else if (time_1s_cnt == 50 + LED_Max_On_10ms_Cnt) {
        //LED_R2_Off;
        //LED_G2_Off;
        //LED_G1_Off;
        //LED_R1_Off;
    }
    else if (time_1s_cnt == 0) {
        //LED_R1_Off;
        //LED_G1_Off;
        //LED_G2_On;
        //LED_R2_On;
    }
    else if (time_1s_cnt == LED_Max_On_10ms_Cnt) {
        //LED_R2_Off;
        //LED_G2_Off;
        //LED_G1_Off;
        //LED_R1_Off;
    }
}
// ºìÉ«½»Ìæ
void ledR_LR_toggle(void)
{
    //LED_G1_Off;
    //LED_G2_Off;
    //LED_B1_Off;
    //LED_B2_Off;
    if (time_1s_cnt == 50) {
        //LED_R2_Off;
        //LED_R1_On;
    }
    else if (time_1s_cnt == 50 + LED_Max_On_10ms_Cnt) {
        //LED_R2_Off;
        //LED_R1_Off;
    }
    else if (time_1s_cnt == 0) {
        //LED_R1_Off;
        //LED_R2_On;
    }
    else if (time_1s_cnt == LED_Max_On_10ms_Cnt) {
        //LED_R2_Off;
        //LED_R1_Off;
    }
}

// À¶É«±¬ÉÁ
void ledB_Fast_toggle(void)
{
    //LED_R1_Off;
    //LED_R2_Off;
    //LED_G1_Off;
    //LED_G2_Off;
    if (time_1s_cnt == 0 || time_1s_cnt == 10 || time_1s_cnt == 20 || time_1s_cnt == 30) {
        //LED_B1_On;
        //LED_B2_On;
    }
    if (time_1s_cnt == LED_Max_On_10ms_Cnt || time_1s_cnt == 10 + LED_Max_On_10ms_Cnt || time_1s_cnt == 20 + LED_Max_On_10ms_Cnt || time_1s_cnt == 30 + LED_Max_On_10ms_Cnt) {
        //LED_B1_Off;
        //LED_B2_Off;
    }
}
// ÂÌÉ«±¬ÉÁ
void ledG_Fast_toggle(void)
{
    //LED_R1_Off;
    //LED_R2_Off;
    //LED_B1_Off;
    //LED_B2_Off;
    if (time_1s_cnt == 0 || time_1s_cnt == 10 || time_1s_cnt == 20 || time_1s_cnt == 30) {
        //LED_G1_On;
        //LED_G2_On;
    }
    if (time_1s_cnt == LED_Max_On_10ms_Cnt || time_1s_cnt == 10 + LED_Max_On_10ms_Cnt || time_1s_cnt == 20 + LED_Max_On_10ms_Cnt || time_1s_cnt == 30 + LED_Max_On_10ms_Cnt) {
        //LED_G1_Off;
        //LED_G2_Off;
    }
}
// »ÆÉ«±¬ÉÁ
void ledY_Fast_toggle(void)
{
    //LED_B1_Off;
    //LED_B2_Off;
    if (time_1s_cnt == 0 || time_1s_cnt == 10 || time_1s_cnt == 20 || time_1s_cnt == 30) {
        //LED_G1_On;
        //LED_G2_On;
        //LED_R1_On;
        //LED_R2_On;
    }
    if (time_1s_cnt == LED_Max_On_10ms_Cnt || time_1s_cnt == 10 + LED_Max_On_10ms_Cnt || time_1s_cnt == 20 + LED_Max_On_10ms_Cnt || time_1s_cnt == 30 + LED_Max_On_10ms_Cnt) {
        //LED_R1_Off;
        //LED_R2_Off;
        //LED_G1_Off;
        //LED_G2_Off;
    }
}
// ºìÉ«±¬ÉÁ
void ledR_Fast_toggle(void)
{
    //LED_G1_Off;
    //LED_G2_Off;
    //LED_B1_Off;
    //LED_B2_Off;
    if (time_1s_cnt == 0 || time_1s_cnt == 10 || time_1s_cnt == 20 || time_1s_cnt == 30) {
        //LED_R1_On;
        //LED_R2_On;
    }
    if (time_1s_cnt == LED_Max_On_10ms_Cnt || time_1s_cnt == 10 + LED_Max_On_10ms_Cnt || time_1s_cnt == 20 + LED_Max_On_10ms_Cnt || time_1s_cnt == 30 + LED_Max_On_10ms_Cnt) {
        //LED_R1_Off;
        //LED_R2_Off;
    }
}
// ÂÌ»Æ
void ledGY_toggle(void)
{
    //LED_B1_Off;
    //LED_B2_Off;
    if (time_1s_cnt == 50) {
        //LED_R1_Off;
        //LED_R2_Off;
        //LED_G1_On;
        //LED_G2_On;
    }
    else if (time_1s_cnt == 50 + LED_Max_On_10ms_Cnt) {
        //LED_R1_Off;
        //LED_R2_Off;
        //LED_G1_Off;
        //LED_G2_Off;
    }
    else if (time_1s_cnt == 0) {
        //LED_R1_On;
        //LED_G1_On;
        //LED_R2_On;
        //LED_G2_On;
    }
    else if (time_1s_cnt == LED_Max_On_10ms_Cnt) {
        //LED_R1_Off;
        //LED_R2_Off;
        //LED_G1_Off;
        //LED_G2_Off;
    }
}
// À¶»Æ
void ledBY_toggle(void)
{
    if (time_1s_cnt == 50) {
        //LED_R1_Off;
        //LED_G1_Off;
        //LED_R2_Off;
        //LED_G2_Off;
        //LED_B1_On;
        //LED_B2_On;
    }
    else if (time_1s_cnt == 50 + LED_Max_On_10ms_Cnt) {
        //LED_R1_Off;
        //LED_G1_Off;
        //LED_R2_Off;
        //LED_G2_Off;
        //LED_B1_Off;
        //LED_B2_Off;
    }
    else if (time_1s_cnt == 0) {
        //LED_B1_Off;
        //LED_B2_Off;
        //LED_R1_On;
        //LED_G1_On;
        //LED_R2_On;
        //LED_G2_On;
    }
    else if (time_1s_cnt == LED_Max_On_10ms_Cnt) {
        //LED_R1_Off;
        //LED_G1_Off;
        //LED_R2_Off;
        //LED_G2_Off;
        //LED_B1_Off;
        //LED_B2_Off;
    }
}
// ºì»Æ
void ledRY_toggle(void)
{
    //LED_B1_Off;
    //LED_B2_Off;
    if (time_1s_cnt == 50) {
        //LED_G1_Off;
        //LED_G2_Off;
        //LED_R1_On;
        //LED_R2_On;
    }
    else if (time_1s_cnt == 50 + LED_Max_On_10ms_Cnt) {
        //LED_G1_Off;
        //LED_G2_Off;
        //LED_R1_Off;
        //LED_R2_Off;
    }
    else if (time_1s_cnt == 0) {
        //LED_G1_On;
        //LED_G2_On;
        //LED_R1_On;
        //LED_R2_On;
    }
    else if (time_1s_cnt == LED_Max_On_10ms_Cnt) {
        //LED_G1_Off;
        //LED_G2_Off;
        //LED_R1_Off;
        //LED_R2_Off;
    }
}
// ºìÀ¶
// ºìÀ¶
void ledRB_toggle(void)
{
    //LED_G1_Off;
    //LED_G2_Off;
    if (time_1s_cnt == 0 || time_1s_cnt == 10 || time_1s_cnt == 20 || time_1s_cnt == 30) {
        //LED_R1_Off;
        //LED_R2_Off;
        //LED_B1_Off;
        //LED_B2_On;
    }
    else if (time_1s_cnt == LED_Max_On_10ms_Cnt || time_1s_cnt == 10 + LED_Max_On_10ms_Cnt || time_1s_cnt == 20 + LED_Max_On_10ms_Cnt || time_1s_cnt == 30 + LED_Max_On_10ms_Cnt) {
        //LED_R1_Off;
        //LED_R2_Off;
        //LED_B1_Off;
        //LED_B2_Off;
    }
    else if (time_1s_cnt == 50 || time_1s_cnt == 60 || time_1s_cnt == 70 || time_1s_cnt == 80) {
        //LED_B1_Off;
        //LED_B2_Off;
        //LED_R1_On;
        //LED_R2_Off;
    }
    else if (time_1s_cnt == 50 + LED_Max_On_10ms_Cnt || time_1s_cnt == 60 + LED_Max_On_10ms_Cnt || time_1s_cnt == 70 + LED_Max_On_10ms_Cnt || time_1s_cnt == 80 + LED_Max_On_10ms_Cnt) {
        //LED_B1_Off;
        //LED_B2_Off;
        //LED_R1_Off;
        //LED_R2_Off;
    }
}
//
void all_light_Off(void)
{
    // lightEnable=false;
    //LED_G1_Off;
    //LED_G2_Off;
    //LED_B1_Off;
    //LED_B2_Off;
    //LED_R1_Off;
    //LED_R2_Off;
}
//
void all_power_Off(void)
{
    // PCTL_MP3_Off; //SZW??
    // RST_BT_Off;//SZW??
    // Set_BT_LP;//SZW??
    LED_R_Bat_Off;
    LED_G_Bat_Off;
#ifndef _Car_Master
    // PCTL_LED_Off;//SZW??
    Light_Off;
#endif
}
//
#if 0 //SZW??
void all_power_on(void)
{
    PCTL_MCU_On;
    // PCTL_Location_On;
    PCTL_GEO_On;
    PCTL_DMR_On;
    PCTL_HeadBoard_On;
    PCTL_5G_On;
    PCTL_Adhoc_On;
    PCTL_MainBoard_On;
    DelayMs(250);
    LED_R_Bat_On;
    DelayMs(250);
    LED_R_Bat_Off;
    DelayMs(250);
    LED_R_Bat_On;
    DelayMs(250);
    LED_R_Bat_Off; // ¿ª»úÌáÊ¾
}
#else //SZW?????RED LED????
void all_power_on(void)
{
    PCTL_MCU_On;
    // PCTL_Location_On;
    PCTL_GEO_On;
    PCTL_DMR_On;
    PCTL_HeadBoard_On;
    PCTL_5G_On;
    PCTL_Adhoc_On;
    PCTL_MainBoard_On;
}
#endif // SZW??
//
void Select_light(uint8_t lightnum)
{
    switch (lightnum) {
    case 0:
        ledRB_toggle();
        break;
    case 1:
        ledG_toggle();
        break;
    case 2:
        ledB_toggle();
        break;
    case 3:
        ledY_toggle();
        break;
    case 4:
        ledR_toggle();
        break;
    case 5:
        ledG_Fast_toggle();
        break;
    case 6:
        ledB_Fast_toggle();
        break;
    case 7:
        ledY_Fast_toggle();
        break;
    case 8:
        ledR_Fast_toggle();
        break;
    case 9:
        ledG_LR_toggle();
        break;
    case 10:
        ledB_LR_toggle();
        break;
    case 11:
        ledY_LR_toggle();
        break;
    case 12:
        ledR_LR_toggle();
        break;
    case 13:
        ledGY_toggle();
        break;
    case 14:
        ledBY_toggle();
        break;
    case 15:
        ledRY_toggle();
        break;
    default:
        all_light_Off();
        break;
    }
}
