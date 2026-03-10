/*
 * autodrive.h
 *
 *  Created on: 2026. 3. 4.
 *      Author: kimsuyeon
 */

#ifndef INC_AUTODRIVE_H_
#define INC_AUTODRIVE_H_

#include "main.h"

extern uint8_t auto_mode;
extern uint8_t current_speed;

void Drive_Control(uint8_t cmd, uint8_t speed);

// 초기화
void AutoDrive_Init(void);

// 주행 판단 실행
void AutoDrive_Run(void);

// 현재 동작 문자열 (디버그용)
const char* AutoDrive_GetActionString(void);

// 속도측정용?

extern uint8_t stop_flag;

#endif /* INC_AUTODRIVE_H_ */
