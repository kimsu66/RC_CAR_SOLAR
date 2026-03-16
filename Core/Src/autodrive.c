/*
 * autodrive.c
 *
 *  Created on: 2026. 3. 4.
 *      Author: kimsuyeon
 */

#include "autodrive.h"
#include "ultrasonic.h"
#include "car.h"
#include <string.h>

// ================= 내부 상태 =================

typedef enum
{
    ACT_STOP = 0,
    ACT_FRONT,
    ACT_BACK,
    ACT_LEFT,
    ACT_RIGHT
} action_t;

static action_t current_action = ACT_STOP;
static char act_str[10];

// ================= 기준 값 =================

#define BACK_DIST       10
#define LRTH            15
#define CRITICAL_DIST   50
#define TURN_THRESHOLD  20

// ================= 초기화 =================

uint8_t auto_mode = 0;
uint8_t current_speed = 80;

uint8_t stop_flag = 1;


void Drive_Control(uint8_t cmd, uint8_t speed) {
    switch (cmd) {
        case 'w': auto_mode = 0; Car_MovePercent(CAR_FRONT, speed); stop_flag = 0; break;
        case 's': auto_mode = 0; Car_MovePercent(CAR_BACK, speed); stop_flag = 0; break;
        case 'a': auto_mode = 0; Car_MovePercent(CAR_LEFT, speed); stop_flag = 0; break;
        case 'd': auto_mode = 0; Car_MovePercent(CAR_RIGHT, speed); stop_flag = 0; break;
        case 'q': auto_mode = 1; break;
        default:  auto_mode = 0; Car_Stop(); stop_flag = 1; break;
    }
}

void AutoDrive_Init(void)
{
    current_action = ACT_STOP;
    strcpy(act_str, "STOP");
}

// ================= 주행 로직 =================

void AutoDrive_Run(uint8_t current_speed)
{

    action_t next_action = current_action;

    // 1. 완전 막힘 → BACK
    if(dist_C < BACK_DIST && (dist_L < BACK_DIST || dist_R < BACK_DIST))
    {
        next_action = ACT_BACK;
    }
    // 2. 좌우 좁음 → 피하기
    else if(dist_L < LRTH)
    {
        next_action = ACT_RIGHT;
    }
    else if(dist_R < LRTH)
    {
        next_action = ACT_LEFT;
    }
    // 3. 앞 여유 → FRONT
    else if(dist_C >= CRITICAL_DIST)
    {
        next_action = ACT_FRONT;
    }
    // 4. 좌우 차이 판단
    else
    {
        if(dist_L > dist_R + TURN_THRESHOLD)
            next_action = ACT_LEFT;
        else if(dist_R > dist_L + TURN_THRESHOLD)
            next_action = ACT_RIGHT;
        else
        {
            next_action = (dist_L > dist_R) ? ACT_LEFT : ACT_RIGHT;
        }
    }

    current_action = next_action;

    // ================= 실제 동작 =================

    switch(current_action)
    {
        case ACT_FRONT:
            Car_MovePercent(CAR_FRONT, current_speed);
            stop_flag = 0;
            strcpy(act_str, "FRONT");
            break;

        case ACT_BACK:
            Car_MovePercent(CAR_BACK, current_speed);
            stop_flag = 0;
            strcpy(act_str, "BACK");
            break;

        case ACT_LEFT:
            Car_MovePercent(CAR_LEFT, current_speed);
            stop_flag = 0;
            strcpy(act_str, "LEFT");
            break;

        case ACT_RIGHT:
            Car_MovePercent(CAR_RIGHT, current_speed);
            stop_flag = 0;
            strcpy(act_str, "RIGHT");
            break;

        default:
            Car_Stop();
            stop_flag = 1;
            strcpy(act_str, "STOP");
            break;
    }
}

// ================= 문자열 getter =================

const char* AutoDrive_GetActionString(void)
{
    return act_str;
}
