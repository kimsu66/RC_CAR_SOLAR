/*
 * ina219.c
 *
 *  Created on: 2026. 3. 10.
 *      Author: kimsuyeon
 */


#include "ina219.h"

// 32V, 2A 범위 설정 기준 (0.1 Ohm 션트 저항 가정)
uint8_t INA219_Init(INA219_t *dev, I2C_HandleTypeDef *hi2c) {
    dev->hi2c = hi2c;
    uint8_t data[2];

    uint16_t calValue = 4096;
    data[0] = (uint8_t)(calValue >> 8);
    data[1] = (uint8_t)(calValue & 0xFF);
    if (HAL_I2C_Mem_Write(dev->hi2c, INA219_ADDR, REG_CALIBRATION, 1, data, 2, 100) != HAL_OK) {
            return HAL_ERROR;
    }

    uint16_t config = 0x399F;
		data[0] = (uint8_t)(config >> 8);
		data[1] = (uint8_t)(config & 0xFF);
		HAL_I2C_Mem_Write(dev->hi2c, INA219_ADDR, REG_CONFIG, 1, data, 2, 100);

    return HAL_OK;
}

int32_t INA219_ReadBusVoltage_mV(INA219_t *dev) {
    uint8_t data[2];
    // I2C3를 통해 전압 레지스터 읽기
    if (HAL_I2C_Mem_Read(dev->hi2c, INA219_ADDR, REG_BUS_VOLT, 1, data, 2, 100) != HAL_OK) return 0;

    int16_t raw = (int16_t)((data[0] << 8) | data[1]);
    raw >>= 2; // LSB 3비트는 사용 안함

    // 1 LSB = 4mV
    return (int32_t)raw * 4;

}

int32_t INA219_ReadCurrent_mA(INA219_t *dev) {
    uint8_t data[2];
    // I2C3를 통해 전류 레지스터 읽기
    if (HAL_I2C_Mem_Read(dev->hi2c, INA219_ADDR, REG_CURRENT, 1, data, 2, 100) != HAL_OK) return 0;

    int16_t raw = (int16_t)((data[0] << 8) | data[1]);

    // 1 LSB = 0.1mA (Calibration 4096일 때)
    // 정수 mA 단위를 위해 10으로 나눔
    return (int32_t)raw / 10;
}
