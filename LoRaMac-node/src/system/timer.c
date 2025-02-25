#include "timer.h"
#include "battery.h"
#include "board.h"
#include "light.h"
// extern uint8_t light_Num;

static uint32_t sDelayCnt = 0;

static uint32_t s_delay_cnt = 0;

static uint16_t keyDownCnt = 0;

static uint32_t turn_off_count_15s = 1500;
static uint32_t turn_off_count_3s = 300;
bool try_to_turn_off = false;
bool turn_off_time_15s_flag = false;
bool turn_off_time_3s_flag = false;
bool working_flag = false;

uint16_t rand_num = 0;
// static uint16_t keyUpCnt = 0;
// static bool keyUpFlag = false;
volatile bool keyDownFlag = false;
volatile bool it_key_up_flag = false;
volatile uint8_t keyState = 0;

bool low_power_flag = false;
bool wake_flag = false;
uint32_t sleep_time = 0;

volatile uint32_t Stanby_10ms_Cnt = Stanby_10ms_max;
volatile uint32_t SYN_10ms_Cnt = SYN_10ms_max;
volatile bool lightEnable = false; //
volatile uint16_t cnt_10ms = 0, Time_10ms_Cnt = 0, Req_Send_Time_SYN_Frame = 0;
volatile uint8_t LED_Max_On_10ms_Cnt = 6;

/**
 * @description: 10ms��ʱ��
 * @return {*}
 */
void TIM4_Init(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE); // ʱ������
    TIM4_DeInit();
    TIM4_TimeBaseInit(TIM4_Prescaler_256, 156); // 4M/(128*156)=100
    TIM4_ClearFlag(TIM4_FLAG_Update);
    TIM4_ITConfig(TIM4_IT_Update, ENABLE);
    TIM4_Cmd(ENABLE);
}

/**
 * @description: 1s��ʱ��
 * @return {*}
 */
void TIM4_1s_Init(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE); // ʱ������
    TIM4_DeInit();
    TIM4_TimeBaseInit(TIM4_Prescaler_32768, 122 - 1); // 4M/(128*156)=100
    TIM4_ClearFlag(TIM4_FLAG_Update);
    TIM4_ITConfig(TIM4_IT_Update, ENABLE);
    TIM4_Cmd(ENABLE);
}

void Delay_10ms(uint32_t tms)
{
    // Reset_WWDG();
    if (tms == 0) {
        return;
    }
    disableInterrupts();
    sDelayCnt = tms;
    enableInterrupts();

    while (sDelayCnt != 0)
        ;
    // Reset_WWDG();
}

void DelayMs(uint32_t tms)
{
    // 1M/(6*200) =1ms
    uint16_t i;
    if (tms == 0) {
        return;
    }

    while (tms != 0) {
        for (i = 0; i < 770; i++)
            ;
        tms--;
    }
}

void Delay_s(uint32_t ts)
{
    if (ts == 0) {
        return;
    }
    disableInterrupts();
    s_delay_cnt = ts;
    enableInterrupts();

    while (s_delay_cnt != 0)
        ;
}

/**
 * @brief TIM4 Update/Overflow/Trigger Interrupt routine.
 * @param  None
 * @retval None
 */
uint8_t Pre_Key1_Value = 1,
        Pre_Key2_Value = 1, Pre_Key3_Value = 1;
uint8_t Key1_Value = 1, Key2_Value = 1, Key3_Value = 1;
uint16_t Cnt_Key2_Pressed = 0;
uint16_t Cnt_led_flash = 0;
extern uint8_t First_AD_Flag;
INTERRUPT_HANDLER(TIM4_UPD_OVF_TRG_IRQHandler, 25)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
    TIM4_ClearITPendingBit(TIM4_IT_Update);
    rand_num = rand_num + 1;
    /* -------------------------------------------------------------------------- */
    /*                                    ����ģʽ                                    */
    /* -------------------------------------------------------------------------- */
    if (low_power_flag == true) { // 1s����һ��

        Light_Off;

        if (sleep_time > 0) {
            sleep_time--;
        }
        if (sleep_time == 0) {
            wake_flag = true;
        }

        if (s_delay_cnt > 0) {
            s_delay_cnt--;
        }

        // if (keyDownFlag)         // �������������¼�
        {
            // if (KEY2 == RESET)     // ������������ ��KEY2�޸�ΪKEY1,KEY1Ϊ�������� SZW�޸�
            if (KEY1) // ������������
            {
                keyDownCnt++;

                if (keyDownCnt >= 3) // �������µ�3000ms���жϳ���ʱ����������ɳ�����־
                {
                    keyState = 3; // ����(���п���)

                    // keyDownFlag = false; // ״̬��λ
                    keyDownCnt = 0;
                }
            }
            else
                keyDownCnt = 0;
        }
    }
    /* -------------------------------------------------------------------------- */
    /*                                    �ǽ���ģʽ                                   */
    /* -------------------------------------------------------------------------- */
    else { // 10ms����
        Cnt_led_flash++;
        switch (Cnt_led_flash) {
        case 1:
            Light_On;
            break;
        case 5:
            Light_Off;
            break;
        case 200:
            Cnt_led_flash = 0;
            break;
        default:
            break;
        }
        if (Stanby_10ms_Cnt > 0)
            Stanby_10ms_Cnt--;
        if (SYN_10ms_Cnt > 0)
            SYN_10ms_Cnt--;
        // battery_sample_filter();
        // if(First_AD_Flag==2)
        {
            Bat_int += ADC_GetConversionValue(ADC1);
            ADC_SoftwareStartConv(ADC1); // �������ת��
            Bat_Value_Cnt++;
            if (Bat_Value_Cnt == 64) {
                Bat_AD_Value = Bat_int;
                Bat_Value_Cnt = 0;
                Bat_int = 0;
                Bat_Sampled_Over = 1;
            }
        }
        // while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));//�ȴ�ת������
        // ADC_ClearFlag(ADC1, ADC_FLAG_EOC);//�����Ӧ��־

        if (sDelayCnt > 0) {
            sDelayCnt--;
        }

        if (try_to_turn_off == true) {
            turn_off_count_15s--;
            turn_off_count_3s--;
            if (turn_off_count_15s == 0)
                turn_off_time_15s_flag = true;
            if (turn_off_count_3s == 0)
                turn_off_time_3s_flag = true;
        }

        //  if (KEY1) // �������������¼�
        {
            // if (KEY2 == RESET)     // ������������ ��KEY2�޸�ΪKEY1,KEY1Ϊ�������� SZW�޸�
            if (KEY1) // ������������
            {
                keyDownCnt++;

                if (keyDownCnt >= 300) // �������µ�3000ms���жϳ���ʱ����������ɳ�����־
                {
                    keyState = 3; // ����(���п���)

                    // keyDownFlag = false; // ״̬��λ
                    keyDownCnt = 0;
                    // keyUpCnt = 0;
                    // keyUpFlag = false;
                }
            }
            // else // ����̧��
            // {

            //     if (keyDownCnt > 3) // ����ʱ�����30ms����Ч
            //     {

            //         keyUpFlag = true; // ����̧�𰴼������ɰ���̧���־

            //         // �����ϴε���ʱ����30~1000ms֮�䣬����Ϊ���������¼�
            //         if (keyUpCnt > 3 && keyUpCnt < 100) {
            //             keyState = 2; // ����
            //             keyUpFlag = false;
            //         }
            //     }
            //     else // ��������ʱ��С��30ms������
            //     {
            //         keyUpFlag = false;
            //         keyState = 0;
            //     }

            //     keyDownFlag = false;
            //     keyDownCnt = 0;
            //     keyUpCnt = 0;
            // }
        }

        // if (keyUpFlag) // ����̧�������������������1000ms
        // {

        //     keyUpCnt++;

        //     if (keyUpCnt > 100) {
        //         keyState = 1; // ����

        //         keyDownFlag = false;
        //         keyDownCnt = 0;
        //         keyUpFlag = false;
        //         keyUpCnt = 0;
        //     }
        // }

        Time_10ms_Cnt++;
        // if (Time_10ms_Cnt == SYN_10ms_max) // ÿSYN_10ms_max�㲥һ��ͬ����Ϣ
        // {
        //     Time_10ms_Cnt = 0;
        //     Req_Send_Time_SYN_Frame = 1;
        // }
        if (Time_10ms_Cnt == 1000) {
            Time_10ms_Cnt = 0;
        }
        cnt_10ms = Time_10ms_Cnt % 100;

        // Select_light(light_Num);
        if (!McuStopFlag) {
            if (led_busy_flag == false && working_flag == true)
                Flash_Status_LED();
            // Flash_Status_LED();
            //  if (lightEnable) {
            //      switch (light_Num) {
            //      case 0:
            //          ledRB_toggle();
            //          break;
            //      case 1:
            //          ledG_toggle();
            //          break;
            //      case 2:
            //          ledB_toggle();
            //          break;
            //      case 3:
            //          ledY_toggle();
            //          break;
            //      case 4:
            //          ledR_toggle();
            //          break;
            //      case 5:
            //          ledG_Fast_toggle();
            //          break;
            //      case 6:
            //          ledB_Fast_toggle();
            //          break;
            //      case 7:
            //          ledY_Fast_toggle();
            //          break;
            //      case 8:
            //          ledR_Fast_toggle();
            //          break;
            //      case 9:
            //          ledG_LR_toggle();
            //          break;
            //      case 10:
            //          ledB_LR_toggle();
            //          break;
            //      case 11:
            //          ledY_LR_toggle();
            //          break;
            //      case 12:
            //          ledR_LR_toggle();
            //          break;
            //      case 13:
            //          ledGY_toggle();
            //          break;
            //      case 14:
            //          ledBY_toggle();
            //          break;
            //      case 15:
            //          ledRY_toggle();
            //          break;
            //      default:
            //          all_light_Off();
            //          break;
            //      }
            //  }
            //  else {
            //      // all_light_Off();
            //      lightEnable = false;
            //      // LED_G1_Off;LED_G2_Off;
            //      // LED_B1_Off;LED_B2_Off;
            //      // LED_R1_Off;LED_R2_Off;
            //  }
        }
    }
}
