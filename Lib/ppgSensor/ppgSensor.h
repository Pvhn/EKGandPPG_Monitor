/*
 * ppgSensor.h
 *
 *  Created on: Apr 29, 2025
 *      Author: peter
 */

#ifndef INC_PPGSENSOR_H_
#define INC_PPGSENSOR_H_

#include "stdint.h"
#include "max030201.h"

extern SensorIOCRegType SensorRegMap;

extern int ppgSensor_Init(void);
extern void ppgSensor_PPGInterrupt(void);
extern void ppgSensor_I2CInterrupt(void);
extern void ppgSensor_calculate_spO2HB(void);
extern int ppgSensor_Shutdown(uint8_t shutdown);
extern int ppgSensor_Reset(void);
#endif /* INC_PPGSENSOR_H_ */
