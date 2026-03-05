/*
 * temp.c
 *
 *  Created on: 2026. 3. 2.
 *      Author: kimsuyeon
 */

#include "temp.h"
#include <string.h>
#include <stdio.h>

uint16_t read_adc()
{
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    return HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
}

// ln(x) 근사용 정수 함수
// x는 1~100 범위에서 사용 (스케일된 값)
int32_t ln_approx(int32_t x)
{
    // ln(x) ≈ 2*(x-1)/(x+1)
    return (2000 * (x - 1000)) / (x + 1000);
}

int16_t get_temperature()
{
    uint16_t adc = read_adc();

    // mV 계산
    int32_t vout = (adc * VREF_mV) / ADC_MAX;

    // 저항 계산
    // Vout = 3.3 * Rntc / (Rntc + 10k)
    int32_t r_ntc = (R_FIXED * vout) / (VREF_mV - vout);

    // ln(R/R0) 계산 (×1000 스케일)
    int32_t ratio = (r_ntc * 1000) / R0;
    int32_t ln_val = ln_approx(ratio);

    // Beta 식 계산 (Kelvin ×100)
    int32_t tempK = 1 / ( (1.0 / T0) + (ln_val / (BETA * 1000.0)) );

    // 위 식 float 없이 변형 ↓
    tempK = (T0 * BETA) / (BETA + ln_val);

    // °C 변환
    int16_t tempC = (tempK - 27315) / 100;

    return tempC;
}

void print_temperature()
{
    int16_t temp = get_temperature();

    char msg[50];
    sprintf(msg, "Temp: %d C\r\n", temp);
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
}

int update_speed_by_temp(int temp, int current_speed, int base_speed)
{
    const int min_speed  = 30;

    if (temp > 50)
    {
        if (current_speed > min_speed)
        {
            current_speed -= 5;
            if (current_speed < min_speed)
                current_speed = min_speed;
        }
    }
    else
    {
        if (current_speed < base_speed)
        {
            current_speed += 5;
            if (current_speed > base_speed)
                current_speed = base_speed;
        }
    }

    return current_speed;
}
