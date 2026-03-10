/*
 * ina219.h
 *
 *  Created on: 2026. 3. 10.
 *      Author: kimsuyeon
 */

#ifndef INC_INA219_H_
#define INC_INA219_H_

#include "main.h"

// INA219 I2C 주소 (A0, A1이 GND일 때 0x40 -> Left Shift 1 = 0x80)
#define INA219_ADDR         (0x40 << 1)

// 레지스터 주소
#define REG_CONFIG          0x00
#define REG_BUS_VOLT        0x02
#define REG_CURRENT         0x04
#define REG_CALIBRATION     0x05

typedef struct {
    I2C_HandleTypeDef *hi2c;
} INA219_t;

uint8_t INA219_Init(INA219_t *dev, I2C_HandleTypeDef *hi2c);
int32_t INA219_ReadBusVoltage_mV(INA219_t *dev);
int32_t INA219_ReadCurrent_mA(INA219_t *dev);

#endif /* INC_INA219_H_ */
