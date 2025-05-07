/*
 * ekgSensor.h
 *
 *  Created on: Apr 30, 2025
 *      Author: peter
 */

#ifndef INC_EKGSENSOR_H_
#define INC_EKGSENSOR_H_

#include <stdint.h>
#include <stdbool.h>

#define EKGFREQ 200 //Sampling Frequency (Hz)
#define EKGPERIOD 5 // Sampling Period
#define MAXDURATION 6
#define MAXEKGSAMPLES (EKGFREQ*MAXDURATION)

void ekgSensor_storeSample(int16_t sample);
uint16_t ekgSensor_calcHB(int16_t sample, uint32_t time, uint16_t status);
int16_t ekgSensor_GetCurrSample(void);
uint32_t ekgSensor_GetCurrSampTime(void);
bool ekgSensor_CheckNewSampleFlag(void);
void ekgSensor_ClearNewSampleFlag(void);
uint16_t ekgSensor_GetStatus(void);

typedef enum
{
	ekgNoFail,
	ekgPosLeadOff,
	ekgNegLeadOff,
	ekgFail
}ekgSensorStatusEnumType;

typedef enum
{
	ekgContinuous,
	ekgSingle,
}ekgModeEnumType;

#endif /* INC_EKGSENSOR_H_ */
