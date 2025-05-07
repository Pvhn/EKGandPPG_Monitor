/*
 * ekgSensor.c
 *
 *  Created on: Apr 30, 2025
 *      Author: peter
 */


#include "ekgSensor.h"
#include "stm32f4xx_hal.h"


#define max(a, b) (((a) > (b)) ? (a) : (b))
#define MAXBPM 280
#define MINBPM 20
#define MAXBPM_MS 60000/MAXBPM
#define MINBPM_MS 60000/MINBPM
#define MAXHBPEAKS 12

static uint16_t numofsamples = 0;
static int16_t ekgSamples[MAXEKGSAMPLES] = {0,};
static uint32_t sampleTime = 0;
static uint16_t LO_pos = 0;
static uint16_t LO_neg = 0;
uint32_t LO_pos_PC = 0;
uint32_t LO_neg_PC = 0;
static const uint32_t LeadsOffPCLimit = 50;

int16_t bpsample = 0;
//int16_t ekgHBSamples[750] = {0,};
volatile static bool newSample = false;

void ekgSensor_storeSample(int16_t sample)
{
	ekgSamples[numofsamples] = sample;
	numofsamples = numofsamples+1;
	if (numofsamples >= MAXEKGSAMPLES)
	{
		numofsamples = 0;
	}

	newSample = true;
	sampleTime = HAL_GetTick();
}


static uint16_t bpm = 0xDEAD;



uint16_t ekgSensor_calcHB(int16_t sample, uint32_t time, uint16_t status)
{
    static uint16_t PeakTime = 0; 		// Counter for samples in a peak
    static uint32_t peakTimes[MAXHBPEAKS] = {0}; // Circular buffer for peak times
    static uint16_t peakIndex = 0; 	// Current index in peakTimes
    static uint16_t peakCount = 0; 	// Number of peaks in window
//    static uint16_t bpm = 0xDEAD;
    const uint16_t THRESH_LO = 2200;
    const uint16_t THRESH_HI = 2300; //Maybe turn this into a moving threshold

	uint32_t totalInterval = 0;
	uint16_t idx = 0;
	uint16_t idx_pv = 0;
	uint32_t interval = 0;
	uint32_t avg_Interval = 0;
	uint16_t newBPM;
	static uint16_t sample_size = 12;
    static uint32_t time_pv = 0;
    static uint8_t init = true;
    static uint16_t status_pv;


    // If fail then reset bpm stats
    if (status != ekgNoFail)
    {
    	bpm = 0xDEAD;
    	peakCount = 0;
    	peakIndex = 0;
    	PeakTime = 0;

    	for (int i = 0; i < MAXHBPEAKS; i++ )
    	{
    		peakTimes[i] = 0;
    	}
    }
    else
    {
        bpsample = sample;

        if(status_pv != ekgNoFail)
        {
        	init = true;
        }

    	if (PeakTime > 0)
    	{
    		PeakTime++;
    		// Check if peak has ended
    		if (sample < THRESH_LO)
    		{
    			// Ensure peak is narrow (less than 100ms)
    			if (PeakTime < (100 / EKGPERIOD))
    			{
    				interval = time - time_pv;
    				if(init == true)
    				{
    					time_pv = time;
    					init = false;
    				}

    				// Valid time between peaks must between 230ms to 3s
    				// Fortunately this is such a wide window that
    				// it might not do very much...
    				if ((interval > MAXBPM_MS) && (interval <= MINBPM_MS) )
    				{
    					// Store peak time in circular buffer
    					// and update time since last valid peak
    					peakTimes[peakIndex] = time;
    					peakIndex = (peakIndex + 1) % sample_size;


    					if (peakCount < sample_size)
    					{
    						peakCount++;
    					}
    					else
    					{
    						if (peakCount > sample_size)
    						{
    							peakCount--;
    						}
    					}

    					// Calculate BPM from intervals in the window
    					if (peakCount >= 2)
    					{
    						for (uint16_t i = 1; i < peakCount; i++)
    						{
    							idx = i;
    							idx_pv = i-1;
    							interval = peakTimes[idx] - peakTimes[idx_pv];
    							totalInterval += interval;
    						}
    						// Calculate average interval and BPM
    						avg_Interval = totalInterval / peakCount;
    						if (avg_Interval > 0)
    						{
        						newBPM = 60000 / avg_Interval;
        						// Limit the output BPM
        						if (newBPM >= MINBPM && newBPM <= MAXBPM)
        						{
        							bpm = newBPM;
        						}
    						}
    					}
    					else
    					{
    						bpm = 0xDEAD;
    					}
    				}
    			}
    			PeakTime = 0;

    			time_pv = time;
    		}
    	}
    	else
    	{
    		// Detect start of a peak
    		if (sample > THRESH_HI)
    		{
    			PeakTime = 1;
    		}
    	}
    }

	return bpm;
}

//uint16_t ekgSensor_calcHB(int16_t sample, uint32_t time, uint16_t status)
//{
//
//    static uint16_t ekg_pv = 0; 	// Previous smoothed ECG value
//    static uint16_t PeakTime = 0; 		// Counter for samples in a peak
//    static uint32_t peakTimes[MAXEKG_HBSAMP] = {0}; // Circular buffer for peak times
//    static uint16_t peakIndex = 0; 	// Current index in peakTimes
//    static uint16_t peakCount = 0; 	// Number of peaks in window
//    static uint16_t bpm = 60;
//    const uint16_t THRESH_LO = 2600-10;
//    const uint16_t THRESH_HI = 2600;
//
//    validPeaks = 0;
//    totalInterval = 0;
//    idx = 0;
//    idx_pv = 0;
//    interval = 0;
//    avg_Interval = 0;
//    static uint32_t time_pv = 0;
//    static uint8_t init = true;
//
//
//
//    // If fail then reset bpm stats
//    if (status != ekgNoFail)
//    {
//    	bpm = 0xDEAD;
//    	peakCount = 0;
//    	peakIndex = 0;
//    	PeakTime = 0;
//
//    	for (int i = 0; i < MAXHBPEAKS; i++ )
//    	{
//    		peakTimes[i] = 0;
//    	}
//
//    }
//    else
//    {
//        bpsample = sample;
//
//    	if (PeakTime > 0)
//    	{
//    		PeakTime++;
//    		// Check if peak has ended
//    		if (sample < THRESH_LO)
//    		{
//    			// Ensure peak is narrow (less than 100ms)
//    			if (PeakTime < (100 / EKGPERIOD))
//    			{
//    				interval = time - time_pv;
//    				if(init == true)
//    				{
//    					time_pv = time;
//    					init = false;
//    				}
//
//    				// Valid time between peaks must between 230ms to 3s
//    				// Fortunately this is such a wide window that
//    				// it might not do very much...
//    				if ((interval > MAXBPM_MS) && (interval <= MINBPM_MS) )
//    				{
//    					// Store peak time in circular buffer
//    					// and update time since last valid peak
//    					peakTimes[peakIndex] = time;
//    					peakIndex = (peakIndex + 1) % sample_size;
//    					time_pv = time;
//
//    					if (peakCount < sample_size)
//    					{
//    						peakCount++;
//    					}
//    					else
//    					{
//    						if (peakCount > sample_size)
//    						{
//    							peakCount--;
//    						}
//    					}
//
//    					// Calculate BPM from intervals in the window
//    					if (peakCount >= 2)
//    					{
//    						for (uint16_t i = 1; i < peakCount; i++)
//    						{
//    							idx = i;
//    							idx_pv = i-1;
//    							interval = peakTimes[idx] - peakTimes[idx_pv];
//    							totalInterval += interval;
//    						}
//    						// Calculate average interval and BPM
//    						avg_Interval = totalInterval / peakCount;
//    						if (avg_Interval > 0)
//    						{
//        						newBPM = 60000 / avg_Interval;
//        						// Limit the output BPM
//        						if (newBPM >= MINBPM && newBPM <= MAXBPM)
//        						{
//        							// Smooth BPM
//        								bpm = (bpm * 7 + newBPM) / 8;
//        //							bpm = newBPM;
//        						}
//    						}
//    					}
//    					else
//    					{
//    						bpm = 0xDEAD;
//    					}
//    				}
//    			}
//    			PeakTime = 0;
//    		}
//    	}
//    	else
//    	{
//    		// Detect start of a peak
//    		if (sample > THRESH_HI)
//    		{
//    			PeakTime = 1;
//    		}
//    	}
//    	ekg_pv = sample;
//    }
//
//	return bpm;
//}


int16_t ekgSensor_GetCurrSample(void)
{
	return ekgSamples[numofsamples];
}

uint32_t ekgSensor_GetCurrSampTime(void)
{
	return sampleTime;
}

bool ekgSensor_CheckNewSampleFlag(void)
{
	return newSample;
}

uint16_t ekgSensor_GetStatus(void)
{

	LO_pos = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1);
	LO_neg = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2);

	// NOTE, the Leads Off Detection Pin sometimes oscillates
	// so we try to use a persistence counter to eventually fail the sensor.

	if (LO_pos == 1)
	{
		if(LO_pos_PC < LeadsOffPCLimit)
		{
			LO_pos_PC = LO_pos_PC +1;
			return ekgPosLeadOff;
		}
		else
		{
			return ekgFail;
		}
	}
	else if (LO_neg == 1)
	{
		if(LO_neg_PC < LeadsOffPCLimit)
		{
			LO_neg_PC = LO_neg_PC +1;
			return ekgNegLeadOff;
		}
		else
		{
			return ekgFail;
		}
	}
	else
	{
		if (LO_pos_PC > 0)
		{
			LO_pos_PC = LO_pos_PC - 1;
			return ekgPosLeadOff;
		}
		else if (LO_neg_PC > 0)
		{
			LO_neg_PC--;
			return ekgNegLeadOff;
		}
		else
		{
			return ekgNoFail;
		}
	}
}



void ekgSensor_ClearNewSampleFlag(void)
{
	newSample = false;
}



