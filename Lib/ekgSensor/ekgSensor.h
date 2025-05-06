/*
 * ekgSensor.h
 *
 *  Created on: Apr 30, 2025
 *      Author: peter
 */

#ifndef INC_EKGSENSOR_H_
#define INC_EKGSENSOR_H_

#define EKGFREQ 500 //Sampling Frequency (Hz)
#define MAXDURATION 6
#define MAXEKGSAMPLES (EKGFREQ*MAXDURATION)

extern uint16_t numofsamples;

extern int16_t ekgSamples[MAXEKGSAMPLES];

extern void ekgSensor_storeSample(int16_t sample);


#endif /* INC_EKGSENSOR_H_ */
