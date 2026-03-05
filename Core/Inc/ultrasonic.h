/*
 * ultrasonic.h
 *
 *  Created on: 2026. 3. 4.
 *      Author: kimsuyeon
 */

#ifndef INC_ULTRASONIC_H_
#define INC_ULTRASONIC_H_

#include "main.h"

// 거리 값 외부 제공
extern uint16_t dist_L;
extern uint16_t dist_C;
extern uint16_t dist_R;

// 초기화
void Ultrasonic_Init(void);

// 논블로킹 트리거 태스크
void Ultrasonic_Task(void);

// IC 콜백 처리 함수
void Ultrasonic_IC_Callback(TIM_HandleTypeDef *htim);

#endif /* INC_ULTRASONIC_H_ */
