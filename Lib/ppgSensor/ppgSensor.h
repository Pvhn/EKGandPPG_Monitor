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

typedef enum
{
	ppgSensorNoFail,
	ppgSensorDataFail,
	ppgSensorI2CFail,
}ppgSensorFailEnumType;

extern SensorIOCRegType SensorRegMap;

extern float spO2;
extern int32_t heartRate;
extern int8_t spO2Valid;
extern int8_t heartRateValid;
extern uint8_t ppgSensorFail;
extern SensorDataBufferType ppgDataBuffer;
extern uint8_t ppgSensorAlive;

extern int ppgSensor_Init(void);
extern void ppgSensor_PPGInterrupt(void);
extern void ppgSensor_I2CInterrupt(void);
extern void ppgSensor_calculate_spO2HB(void);
extern int ppgSensor_Shutdown(uint8_t shutdown);
extern int ppgSensor_Reset(void);
extern void ppgSensor_SetFailure(uint8_t failtype);
#endif /* INC_PPGSENSOR_H_ */
