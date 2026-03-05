/*
 * ultrasonic.c
 *
 *  Created on: 2026. 3. 4.
 *      Author: kimsuyeon
 */

#include "ultrasonic.h"
#include "tim.h"

// ================= 내부 변수 =================
static uint16_t IC1_L = 0, IC2_L = 0;
static uint16_t IC1_C = 0, IC2_C = 0;
static uint16_t IC1_R = 0, IC2_R = 0;

static uint16_t echo_L = 0;
static uint16_t echo_C = 0;
static uint16_t echo_R = 0;

static uint8_t flag_L = 0;
static uint8_t flag_C = 0;
static uint8_t flag_R = 0;

// ================= 외부 공개 거리 =================
uint16_t dist_L = 0;
uint16_t dist_C = 0;
uint16_t dist_R = 0;

// ================= 상태머신 =================
static uint32_t ultra_prev_time = 0;
static uint8_t ultra_step = 0;

// ================= TRIG 함수 =================
static void TRIG_LEFT(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
    for(volatile int i=0;i<100;i++);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
}

static void TRIG_CENTER(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
    for(volatile int i=0;i<100;i++);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
}

static void TRIG_RIGHT(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
    for(volatile int i=0;i<100;i++);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
}

// ================= 초기화 =================
void Ultrasonic_Init(void)
{
    HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_4);
    HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_3);
    HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_4);
}

// ================= 논블로킹 태스크 =================
void Ultrasonic_Task(void)
{
    if(HAL_GetTick() - ultra_prev_time < 20)
        return;

    ultra_prev_time = HAL_GetTick();

    switch(ultra_step)
    {
        case 0:
            TRIG_LEFT();
            ultra_step = 1;
            break;

        case 1:
            TRIG_CENTER();
            ultra_step = 2;
            break;

        case 2:
            TRIG_RIGHT();
            ultra_step = 0;
            break;
    }
}

// ================= IC 콜백 처리 =================
void Ultrasonic_IC_Callback(TIM_HandleTypeDef *htim)
{
    // LEFT
    if (htim->Instance == TIM3 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
    {
        if (flag_L == 0)
        {
            IC1_L = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
            flag_L = 1;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else
        {
            IC2_L = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
            __HAL_TIM_SET_COUNTER(htim, 0);

            echo_L = (IC2_L > IC1_L) ? (IC2_L - IC1_L) : ((0xFFFF - IC1_L) + IC2_L);
            dist_L = echo_L / 58;

            flag_L = 0;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_RISING);
        }
    }

    // CENTER
    if (htim->Instance == TIM4 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
    {
        if (flag_C == 0)
        {
            IC1_C = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
            flag_C = 1;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else
        {
            IC2_C = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
            __HAL_TIM_SET_COUNTER(htim, 0);

            echo_C = (IC2_C > IC1_C) ? (IC2_C - IC1_C) : ((0xFFFF - IC1_C) + IC2_C);
            dist_C = echo_C / 58;

            flag_C = 0;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_RISING);
        }
    }

    // RIGHT
    if (htim->Instance == TIM4 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
    {
        if (flag_R == 0)
        {
            IC1_R = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
            flag_R = 1;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else
        {
            IC2_R = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
            __HAL_TIM_SET_COUNTER(htim, 0);

            echo_R = (IC2_R > IC1_R) ? (IC2_R - IC1_R) : ((0xFFFF - IC1_R) + IC2_R);
            dist_R = echo_R / 58;

            flag_R = 0;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_RISING);
        }
    }
}
