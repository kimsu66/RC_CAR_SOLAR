/*
 * temp.c
 *
 *  Created on: 2026. 3. 5.
 *      Author: kimsuyeon
 */


#include "temp.h"
#include <stdio.h>

#define TEMP_WARNING_THRESHOLD  50  // 50도 이상 시 감속 시작
#define TEMP_DANGER_THRESHOLD   70  // 70도 이상 시 즉각 위험

#include "adc.h"

uint16_t Temp_ReadADC(void)
{
//    return adc_buf[0];   // Rank1 = ADC_CHANNEL_6 = PA6
	return adcValue[0];
}

//// ADC 센서
//uint16_t Temp_ReadADC(void)
//{
//    HAL_ADC_Start(&hadc1);
//    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
//
//    uint16_t adc = HAL_ADC_GetValue(&hadc1);
//
//    HAL_ADC_Stop(&hadc1);
//
//    return adc;
//}

/* ln 근사 */
static int32_t ln_approx(int32_t x)
{
    return (2000 * (x - 1000)) / (x + 1000);
}

// 3. 온도 계산
int16_t Temp_CalcCelsius(uint16_t adc)
{
		// mV 계산
    int32_t vout = (adc * VREF_mV) / ADC_MAX;

    // 저항 계산
    // Vout = 3.3 * Rntc / (Rntc + 10k)
    int32_t r_ntc = (R_FIXED * vout) / (VREF_mV - vout);

    // ln(R/R0) 계산 (×1000 스케일)
    int32_t ratio = (r_ntc * 1000) / R0;

    // Beta 식 계산 (Kelvin ×100)
    int32_t ln_val = ln_approx(ratio);

    // 위 식 float 없이 변형
    int32_t tempK = (T0 * BETA) / (BETA + ln_val);

    // °C 변환
    int16_t tempC = (tempK - 27315) / 100;

    return tempC;

}

// 5. UART 디버그
void Temp_DebugPrint(uint16_t adc, int16_t temp, uint8_t speed)
{
    uint32_t voltage = (adc * 3300) / 4095;

    printf("ADC: %d\r\n", adc);

    printf("Voltage: %lu.%03lu V\r\n",
           voltage / 1000,
           voltage % 1000);

    printf("Temp: %d C\r\n", temp);

    printf("Speed: %d %%\r\n", speed);

    printf("===========\r\n");
}

// 온도 레벨 판정 함수
TempLevel_t Temp_GetLevel(int16_t temp)
{
    if (temp >= TEMP_DANGER_THRESHOLD)  return TEMP_DANGER;
    if (temp >= TEMP_WARNING_THRESHOLD) return TEMP_WARNING;
    return TEMP_SAFE;
}

// Gas_TaskPPM과 동일하게 레벨을 반환하는 함수
TempLevel_t Temp_TaskLevel(void)
{
    uint16_t adc = Temp_ReadADC();
    int16_t temp = Temp_CalcCelsius(adc);

    // 현재 온도에 따른 레벨 확인
    TempLevel_t level = Temp_GetLevel(temp);

    // 디버그 출력
//    printf("Temp ADC: %d | Temp: %d C | Status: ", adc, temp);
//    switch(level) {
//        case TEMP_SAFE:    printf("[SAFE]\r\n"); break;
//        case TEMP_WARNING: printf("[WARNING - HIGH TEMP]\r\n"); break;
//        case TEMP_DANGER:  printf("[! DANGER - OVERHEAT !]\r\n"); break;
//    }

    return level;
}
