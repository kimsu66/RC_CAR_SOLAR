/*
 * car.c
 *
 *  Created on: 2026. 2. 27.
 *      Author: kimsuyeon
 */

#include <car2.h>
#include "stdio.h"

/* ================= LEFT Motor (IN1, IN2) ================= */

void Left_Forward(void)
{
    HAL_GPIO_WritePin(LEFT_IN1_GPIO, LEFT_IN1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LEFT_IN2_GPIO, LEFT_IN2_PIN, GPIO_PIN_SET);
}

void Left_Reverse(void)
{
    HAL_GPIO_WritePin(LEFT_IN1_GPIO, LEFT_IN1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LEFT_IN2_GPIO, LEFT_IN2_PIN, GPIO_PIN_RESET);
}

void Left_Brake(void)
{
    HAL_GPIO_WritePin(LEFT_IN1_GPIO, LEFT_IN1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LEFT_IN2_GPIO, LEFT_IN2_PIN, GPIO_PIN_RESET);
}


void Left_SetSpeed(uint8_t percent)
{
    if(percent > 100) percent = 100;

    TIM1->CCR1 = (TIM1->ARR * percent) / 100;
}

/* ================= RIGHT Motor (IN3, IN4) ================= */

void Right_Forward(void)
{
    HAL_GPIO_WritePin(RIGHT_IN3_GPIO, RIGHT_IN3_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RIGHT_IN4_GPIO, RIGHT_IN4_PIN, GPIO_PIN_RESET);
}

void Right_Reverse(void)
{
    HAL_GPIO_WritePin(RIGHT_IN3_GPIO, RIGHT_IN3_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RIGHT_IN4_GPIO, RIGHT_IN4_PIN, GPIO_PIN_SET);
}

void Right_Brake(void)
{
    HAL_GPIO_WritePin(RIGHT_IN3_GPIO, RIGHT_IN3_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RIGHT_IN4_GPIO, RIGHT_IN4_PIN, GPIO_PIN_RESET);
}

void Right_SetSpeed(uint8_t percent)
{
    if(percent > 100) percent = 100;

    TIM1->CCR2 = (TIM1->ARR * percent) / 100;
}

/* ================= Motor function ================= */
void Car_SetSpeed(uint8_t percent)
{
	  Left_SetSpeed(percent);
	  Right_SetSpeed(percent);
}

void Car_SetForward()
{
    Left_Forward();
    Right_Forward();
}

void Car_SetReverse()
{
    Left_Reverse();
    Right_Reverse();
}

void Car_Forward(uint8_t percent)
{
    Left_Forward();
    Right_Forward();

    Left_SetSpeed(percent);
    Right_SetSpeed(percent);
}

void Car_Reverse(uint8_t percent)
{
    Left_Reverse();
    Right_Reverse();

    Left_SetSpeed(percent);
    Right_SetSpeed(percent);
}

void Car_Left_Turn(uint8_t percent)
{
    Left_Forward();
    Right_Forward();

    Left_SetSpeed(percent / 3);
    Right_SetSpeed(percent);
}

void Car_Right_Turn(uint8_t percent)
{
    Left_Forward();
    Right_Forward();

    Right_SetSpeed(percent / 3);
    Left_SetSpeed(percent);
}

void Car_Right_Reverse(uint8_t percent)
{
    Left_Reverse();
    Right_Reverse();

    Left_SetSpeed(percent);
    Right_SetSpeed(percent / 3);
}

void Car_Left_Reverse(uint8_t percent)
{
    Left_Reverse();
    Right_Reverse();

    Right_SetSpeed(percent);
    Left_SetSpeed(percent / 3);
}

void Car_Stop2(void)
{
    Left_SetSpeed(0);
    Right_SetSpeed(0);
}


// joystick ADC
uint16_t adc_val[3];
uint16_t adc_y;
uint16_t adc_x2;
uint16_t adc_y2;

void Motor_Control(int16_t left, int16_t right)
{
    int16_t orig_left = left;
    int16_t orig_right = right;

    // ===== LEFT =====
    if(left == 0)
    {
        Left_Brake();
    }
    else if(left > 0)
    {
        Left_Forward();
    }
    else
    {
        Left_Reverse();
        left = -left;
    }

    // ===== RIGHT =====
    if(right == 0)
    {
        Right_Brake();
    }
    else if(right > 0)
    {
        Right_Forward();
    }
    else
    {
        Right_Reverse();
        right = -right;
    }

    if(left > 100) left = 100;
    if(right > 100) right = 100;

    Left_SetSpeed(left);
    Right_SetSpeed(right);

    // ===== UART 출력 =====
    if(orig_left == 0 && orig_right == 0)
    {
        printf("STOP | L:0 R:0\r\n");
    }
    else
    {
        printf("L:%d R:%d\r\n", orig_left, orig_right);
    }
}

void Motor_Control2(){
	  	  	if (adc_y > CENTER + DEADZONE)
	  	  	{
	  	  	    if (adc_y > CENTER + STEP_BOUND)
	  	  	    {
	  	  	    	printf("FORWARD 2 |");
	  	  	    	Car_Forward(SPEED_LV2);
	  	  	    }
	  	  	    else
	  	  	    {
	  	  	    	printf("FORWARD 1\r\n");
	  	  	    	Car_Forward(SPEED_LV1);
	  	  	    }
	  	  	}
	  	  	else if (adc_y < CENTER - DEADZONE)
	  	  	{
	  	  	    if (adc_y < CENTER - STEP_BOUND)
	  	  	    {
	  	  	    	printf("BACKWARD 2\r\n");
	  	  	    	Car_Reverse(SPEED_LV2);
	  	  	    }
	  	  	    else
	  	  	    {
	  	  	    	printf("BACKWARD 1\r\n");
	  	  	    	Car_Reverse(SPEED_LV1);
	  	  	    }
	  	  	}
	  	  	else
	  	  	{
	  	  	    printf("STOP\r\n");
	  	  	    Car_Stop2();

	  	  	}

}

void carwhileinside(){
	// ADC
//	  	    HAL_ADC_Start(&hadc1);
//
//	  	  	// Y1
//	  	  	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
//	  	  	adc_val[0] = HAL_ADC_GetValue(&hadc1);
//	  	  	adc_y = adc_val[0];
//	  	  	printf(" Y1 :%4d \r\n", adc_val[0]);
//
//
//	  	  	// X2
//	  	  	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
//	  	  	adc_val[1] = HAL_ADC_GetValue(&hadc1);
//	  	  	adc_x2 = adc_val[1];
//	  	  	printf(" X2:%4d \r\n", adc_val[1]);
//
//	  	  	HAL_ADC_Stop(&hadc1);
//
//
//	  	  	y = adc_y - CENTER;
//	  	  	x = adc_x2 - CENTER;
//
//	  	  	if (abs(y) < DEADZONE) y = 0;
//	  	  	if (abs(x) < DEADZONE) x = 0;

	//  	  	baseSpeed = y / 20;   // 전진/후진 속도
	//  	  	steering  = x / 40;   // 조향 강도

	//  	  	leftSpeed  = baseSpeed + steering;
	//  	  	rightSpeed = baseSpeed - steering;

	//  	  	Motor_Control(leftSpeed, rightSpeed);

	//  	  	if (adc_y > CENTER + DEADZONE)
	//  	  	{
	//  	  	    if (adc_y > CENTER + STEP_BOUND)
	//  	  	    {
	//  	  	    	printf("FORWARD \r\n");
	//  	  	    	Car_Forward(SPEED_LV2);
	//  	  	    }
	//  	  	    else
	//  	  	    {
	//  	  	    	printf("FORWARD 1\r\n");
	//  	  	    	Car_Forward(SPEED_LV1);
	//  	  	    }
	//  	  	}
	//  	  	else if (adc_y < CENTER - DEADZONE)
	//  	  	{
	//  	  	    if (adc_y < CENTER - STEP_BOUND)
	//  	  	    {
	//  	  	    	printf("BACKWARD 2\r\n");
	//  	  	    	Car_Reverse(SPEED_LV2);
	//  	  	    }
	//  	  	    else
	//  	  	    {
	//  	  	    	printf("BACKWARD 1\r\n");
	//  	  	    	Car_Reverse(SPEED_LV1);
	//  	  	    }
	//  	  	}
	//  	  	else
	//  	  	{
	//  	  	    printf("STOP\r\n");
	//  	  	    Car_Stop();
	//
	//  	  	}
}
