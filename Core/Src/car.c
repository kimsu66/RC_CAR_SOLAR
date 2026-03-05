/*
 * car.c
 *
 *  Created on: Feb 24, 2026
 *      Author: lenovo
 */



#include "car.h"


void Car_Init(void)
{
  /* 안전하게 정지 상태로 시작 */
  Direction_Set(DIR_STOP);
  Speed_StopAll();

  /* PWM 출력 시작 (Speed_Init 이후 호출되어야 함) */
  Speed_Start();
}

void Car_Move(car_state_t state, speed_state_t speed)
{
  switch (state)
  {
    case CAR_STOP:
      Direction_Set(DIR_STOP);
      Speed_StopAll();
      break;

    case CAR_FRONT:
      Direction_Set(DIR_FRONT);
      Speed_SetStateBoth(speed, speed);
      break;

    case CAR_BACK:
      Direction_Set(DIR_BACK);
      Speed_SetStateBoth(speed, speed);
      break;

    /* 좌/우 회전: 좌 50 / 우 0, 좌 0 / 우 50 */
    case CAR_LEFT:
      Direction_Set(DIR_FRONT);
      Speed_SetStateBoth(SPD_STOP, SPD_50);
      break;

    case CAR_RIGHT:
      Direction_Set(DIR_FRONT);
      Speed_SetStateBoth(SPD_50, SPD_STOP);
      break;

    /* 대각 전진: 50/speed, speed/50 */
    case CAR_LEFTFRONT:
      Direction_Set(DIR_FRONT);
      Speed_SetStateBoth(SPD_50, speed);
      break;

    case CAR_RIGHTFRONT:
      Direction_Set(DIR_FRONT);
      Speed_SetStateBoth(speed, SPD_50);
      break;

    /* 대각 후진 */
    case CAR_LEFTBACK:
      Direction_Set(DIR_BACK);
      Speed_SetStateBoth(SPD_50, speed);
      break;

    case CAR_RIGHTBACK:
      Direction_Set(DIR_BACK);
      Speed_SetStateBoth(speed, SPD_50);
      break;

    default:
      Direction_Set(DIR_STOP);
      Speed_StopAll();
      break;
  }
}

void Car_Stop(void)
{
  Direction_Set(DIR_STOP);
  Speed_StopAll();
}



/// Added
void Car_MovePercent(car_state_t state, uint8_t percent)
{
    if (percent > 100) percent = 100;

    switch (state)
    {
        case CAR_STOP:
            Direction_Set(DIR_STOP);
            Speed_StopAll();
            break;

        case CAR_FRONT:
            Direction_Set(DIR_FRONT);
            Speed_SetPercentBoth(percent, percent);
            break;

        case CAR_BACK:
            Direction_Set(DIR_BACK);
            Speed_SetPercentBoth(percent, percent);
            break;

        /* 좌/우 회전: 좌 50 / 우 0, 좌 0 / 우 50 */
        case CAR_LEFT:
            Direction_Set(DIR_FRONT);
            Speed_SetPercentBoth(0, 50);
            break;

        case CAR_RIGHT:
            Direction_Set(DIR_FRONT);
            Speed_SetPercentBoth(50, 0);
            break;

        /* 대각 전진: 50/percent, percent/50 */
        case CAR_LEFTFRONT:
            Direction_Set(DIR_FRONT);
            Speed_SetPercentBoth(50, percent);
            break;

        case CAR_RIGHTFRONT:
            Direction_Set(DIR_FRONT);
            Speed_SetPercentBoth(percent, 50);
            break;

        /* 대각 후진 */
        case CAR_LEFTBACK:
            Direction_Set(DIR_BACK);
            Speed_SetPercentBoth(50, percent);
            break;

        case CAR_RIGHTBACK:
            Direction_Set(DIR_BACK);
            Speed_SetPercentBoth(percent, 50);
            break;

        default:
            Direction_Set(DIR_STOP);
            Speed_StopAll();
            break;
    }
}

void Car_test(void)
{

		  // 1) 정지
		  Car_Stop();
		  HAL_Delay(700);

		  // 2) 전진 (30/50/70/100 순서)
		  Car_Move(CAR_FRONT, SPD_30);
		  HAL_Delay(1000);
		  Car_Move(CAR_FRONT, SPD_50);
		  HAL_Delay(1000);
		  Car_Move(CAR_FRONT, SPD_70);
		  HAL_Delay(1000);
		  Car_Move(CAR_FRONT, SPD_100);
		  HAL_Delay(1200);

		  Car_Stop();
		  HAL_Delay(700);

		  // 3) 후진
		  Car_Move(CAR_BACK, SPD_50);
		  HAL_Delay(1500);

		  Car_Stop();
		  HAL_Delay(700);

		  // 4) 제자리(또는 단순) 좌/우 회전 (요구대로 50/0, 0/50)
		  Car_Move(CAR_LEFT, SPD_70);   // speed 인자는 무시되지만 시그니처 맞춰 넣음
		  HAL_Delay(900);
		  Car_Stop();
		  HAL_Delay(400);

		  Car_Move(CAR_RIGHT, SPD_70);
		  HAL_Delay(900);
		  Car_Stop();
		  HAL_Delay(700);

		  // 5) 대각 전진
		  Car_Move(CAR_LEFTFRONT, SPD_70);   // 좌 50 / 우 70
		  HAL_Delay(1200);
		  Car_Stop();
		  HAL_Delay(400);

		  Car_Move(CAR_RIGHTFRONT, SPD_70);  // 좌 70 / 우 50
		  HAL_Delay(1200);
		  Car_Stop();
		  HAL_Delay(700);

		  // 6) 대각 후진
		  Car_Move(CAR_LEFTBACK, SPD_70);
		  HAL_Delay(1200);
		  Car_Stop();
		  HAL_Delay(400);

		  Car_Move(CAR_RIGHTBACK, SPD_70);
		  HAL_Delay(1200);

		  // 루프 끝 정지
		  Car_Stop();
		  HAL_Delay(1500);

}




