/*
 * car.h
 *
 *  Created on: 2026. 2. 27.
 *      Author: kimsuyeon
 */

#ifndef INC_CAR2_H_
#define INC_CAR2_H_

#define CENTER 2050
#define DEADZONE 200
#define STEP_BOUND 1800

#define SPEED_LV1 40
#define SPEED_LV2 80

#include "stm32f4xx_hal.h"

/* ================= Left Motor (IN1, IN2) ================= */
#define LEFT_IN1_GPIO GPIOB
#define LEFT_IN1_PIN  GPIO_PIN_13

#define LEFT_IN2_GPIO GPIOB
#define LEFT_IN2_PIN  GPIO_PIN_14

/* ================= Right Motor (IN3, IN4) ================= */
#define RIGHT_IN3_GPIO  GPIOB
#define RIGHT_IN3_PIN   GPIO_PIN_15

#define RIGHT_IN4_GPIO  GPIOB
#define RIGHT_IN4_PIN   GPIO_PIN_1

void Left_Forward(void);
void Left_Reverse(void);
void Left_Brake(void);
void Left_SetSpeed(uint8_t percent);

void Right_Forward(void);
void Right_Reverse(void);
void Right_Brake(void);
void Right_SetSpeed(uint8_t percent);

void Car_SetSpeed(uint8_t percent);
void Car_SetForward();
void Car_SetReverse();

void Car_Forward(uint8_t percent);
void Car_Reverse(uint8_t percent);
void Car_Left_Turn(uint8_t percent);
void Car_Right_Turn(uint8_t percent);
void Car_Right_Reverse(uint8_t percent);
void Car_Left_Reverse(uint8_t percent);
void Car_Stop2(void);

#endif /* INC_CAR2_H_ */
