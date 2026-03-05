/*
 * temp.h
 *
 *  Created on: 2026. 3. 2.
 *      Author: kimsuyeon
 */

#ifndef INC_TEMP_H_
#define INC_TEMP_H_

#include "main.h"
#include "stdio.h"
#include "stdint.h"

#define ADC_MAX     4095
#define VREF_mV     3300

#define R_FIXED     10000   // 10k 기준 저항
#define R0          10000   // 25°C에서 NTC 저항
#define BETA        3434		// datasheet 확인
#define T0          29815   // 25°C = 298.15K (×100)

extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;

uint16_t read_adc();
int32_t ln_approx(int32_t x);
int16_t get_temperature();
void print_temperature();

int update_speed_by_temp(int temp, int current_speed, int base_speed);

#endif /* INC_TEMP_H_ */
