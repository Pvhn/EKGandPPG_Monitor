/*
 * ekgSensor.c
 *
 *  Created on: Apr 30, 2025
 *      Author: peter
 */

#include <stdint.h>
#include "ekgSensor.h"

#define max(a, b) (((a) > (b)) ? (a) : (b))

uint16_t numofsamples = 0;

int16_t ekgSamples[MAXEKGSAMPLES] = {0,};
int16_t ekgHBSamples[2000] = {0,};

const uint16_t maxBPM = 240;
const uint16_t peakThresh = 2500;

void ekgSensor_storeSample(int16_t sample)
{
	ekgSamples[numofsamples] = sample;
	numofsamples = numofsamples+1;
	if (numofsamples >= MAXEKGSAMPLES)
	{
		numofsamples = 0;
	}
}

uint16_t ekgSensor_calcHB(void)
{

	// Heartbeat calculation is a simple peak-peak counter.
	int16_t peak_pv = 0;
	int16_t tempval = 0;


	// max number of peaks per sample and min time between
	// peaks. Used for error detection
	const uint16_t maxnumpeaks = maxBPM/60;
	const uint16_t mintime = 1/maxnumpeaks;
	uint16_t peakTime[2] = {0,};
	uint16_t numofpeaks = 0;
	uint16_t peakATime = 0;
	uint16_t peakBTime = 0;


	// Calculate based on a sample size of 4 seconds.
	// We only need to identify atleast two peaks
	for(int i = 0; i < EKGFREQ*4; i++)
	{
		if (ekgHBSamples[i] > peakThresh)
		{

			peak_pv = ekgHBSamples[i];

			numofpeaks++;
		}
	}

}


