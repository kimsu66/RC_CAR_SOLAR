/*
 * autodrive.h
 *
 *  Created on: 2026. 3. 4.
 *      Author: kimsuyeon
 */

#ifndef INC_AUTODRIVE_H_
#define INC_AUTODRIVE_H_

#include "main.h"

// 초기화
void AutoDrive_Init(void);

// 주행 판단 실행
void AutoDrive_Run(void);

// 현재 동작 문자열 (디버그용)
const char* AutoDrive_GetActionString(void);

#endif /* INC_AUTODRIVE_H_ */
