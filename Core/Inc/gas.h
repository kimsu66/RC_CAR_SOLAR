/*
 * gas.h
 *
 *  Created on: 2026. 3. 6.
 *      Author: kimsuyeon
 */

#ifndef INC_GAS_H_
#define INC_GAS_H_

#include "main.h"
#include <stdint.h>


typedef enum
{
    GAS_SAFE = 0,
    GAS_WARNING,
    GAS_DANGER

} GasLevel_t;

GasLevel_t Gas_TaskPPM(void);


#endif /* INC_GAS_H_ */
