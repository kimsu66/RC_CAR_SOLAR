/*
 * temp.h
 *
 *  Created on: 2026. 3. 5.
 *      Author: kimsuyeon
 */

#ifndef INC_TEMP_H_
#define INC_TEMP_H_

#include "main.h"
#include <stdint.h>

#define ADC_MAX     4095
#define VREF_mV     3300

#define R_FIXED     10000
#define R0          10000
#define BETA        3434
#define T0          29815

extern ADC_HandleTypeDef hadc1;

typedef enum {
    TEMP_SAFE = 0,
    TEMP_WARNING,
    TEMP_DANGER
} TempLevel_t;

TempLevel_t Temp_TaskLevel(void);


#endif /* INC_TEMP_H_ */
