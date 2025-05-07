/*
 * ppgSensor.c
 *
 *  Created on: Apr 29, 2025
 *      Author: peter
 */

#include "ppgSensor.h"
#include "spO2_algorithm.h"

#define NUMSAMPLES FreqS

// Allocate a pointer containing
// the local memory map of the sensor
SensorIOCRegType SensorRegMap = {0,};

uint8_t ppgDataReady = 0;
volatile uint8_t ppgBusy = 0;
SensorSampleDataType ppgSampleData[NUMSAMPLES] = {0,};
SensorDataBufferType ppgDataBuffer = {0,};

Interrupt1RegType ppgInterrupt = {0,};
int32_t heartRate = 0;
int32_t heartRates[4] = {0,};
int32_t heartRate_pv = 0.0f;
float spO2 = 0.0f;
float spO2_pv = 0.0f;
uint32_t spO2Valid_PC = 0;
uint32_t heartRateValid_PC = 0;
int8_t spO2Valid = 0;
int8_t heartRateValid = 0;
uint8_t ppgSensorFail = ppgSensorNoFail;
uint8_t tempdata[4] = {0,};
uint8_t ppgSensorAlive = 0;

int ppgSensor_Init(void)
{
	int error = 0;
	Interrupt1RegType intstatus = {0,};
	uint8_t *data;

	error = max030201_CheckDevice();
	if (error)
	{
		// TBD Error Handle
	}

	// Reset the sensor
	SensorRegMap.ModeConfig.AsBits.Reset = 1;
	data = &(SensorRegMap.ModeConfig.As8BitWord);
	error = max030201_writeReg(REG_ADDR_MODE_CONFG, data, 1);
	SensorRegMap.ModeConfig.AsBits.Reset = 0;
	error = max030201_writeReg(REG_ADDR_MODE_CONFG, data, 1);

	// Clear ram copy of memory map
	data = &(SensorRegMap.IntStatus1.As8BitWord);
	// Eventually change this so 0x12 is not hardcoded
	for (int i = 0; i< 0x12; i++)
	{
		*data = 0;
		data++; // Increment the pointer.
	}

	// Initialize Registers.
	SensorRegMap.IntEnable1.AsBits.PPGReady = 1;
	SensorRegMap.IntEnable2.As8BitWord = 0x00;
	SensorRegMap.FIFOConfig.AsBits.FIFORollOvEn = 1;
	SensorRegMap.FIFOConfig.AsBits.SampleAvg = 0x02; // Avg of 4 samples per FIFO
	SensorRegMap.FIFOConfig.AsBits.FIFOAlmFull = 0x07;
	SensorRegMap.ModeConfig.AsBits.Mode = MODE_SPO2;
	SensorRegMap.SpO2Config.As8BitWord = 0x27;
	SensorRegMap.LEDPulseAmpRed = 60;
	SensorRegMap.LEDPulseAmpIR = 60;
	SensorRegMap.MultModeCtlS1S2.As8BitWord = 0x0;
	SensorRegMap.MultModeCtlS3S4.As8BitWord = 0x0;

	error = max030201_readReg(REG_ADDR_INTSTAT1, (uint8_t *) &intstatus, 1);

	// Write to Interrupt Register
	data = &(SensorRegMap.IntEnable1.As8BitWord);
	error = max030201_writeReg(REG_ADDR_INTEN1, data, 2);

	// Clear FIFO Register
	data = &(SensorRegMap.FIFO_WR_PTR);
	error = max030201_writeReg(REG_ADDR_FIFO_RD_PTR, data, 3);

	// Write Remaining Configuration values starting from FIFO Config
	data = &(SensorRegMap.FIFOConfig.As8BitWord);
	error =  max030201_writeReg(REG_ADDR_FIFO_CONFG, data, 10);

	return error;
}

void ppgSensor_calculate_spO2HB(void)
{
	if(ppgDataReady == 1)
	{
		uint16_t i = 0;

		// Shift all samples left 25
		for(i = 25; i<100; i++)
		{
			ppgDataBuffer.Red[i-25] = ppgDataBuffer.Red[i];
			ppgDataBuffer.IR[i-25] = ppgDataBuffer.IR[i];
		}

		// Process the 25 new samples
		SampletoDataType temp;
		for(i = 75; i < 100; i++)
		{
			temp.AsBytes.Byte0 = ppgSampleData[i-75].Red[2];
			temp.AsBytes.Byte1 = ppgSampleData[i-75].Red[1];
			temp.AsBytes.Byte2 = ppgSampleData[i-75].Red[0];
			ppgDataBuffer.Red[i] = (temp.As32BitWord & 0x3FFFF);

			temp.AsBytes.Byte0 = ppgSampleData[i-75].IR[2];
			temp.AsBytes.Byte1 = ppgSampleData[i-75].IR[1];
			temp.AsBytes.Byte2 = ppgSampleData[i-75].IR[0];
			ppgDataBuffer.IR[i] = (temp.As32BitWord & 0x3FFFF);
		}

		int32_t heartRateTemp = 0;
		float spO2Temp = 0.0f;
		maxim_heart_rate_and_oxygen_saturation(ppgDataBuffer.IR, BUFFER_SIZE, ppgDataBuffer.Red, &spO2Temp, &spO2Valid, &heartRateTemp, &heartRateValid);

		if (spO2Valid == 0)
		{
			// Use last good value and increment persistence counter
			spO2 = spO2_pv;
			spO2Valid_PC = spO2Valid_PC +1;
		}
		else
		{
			// Limit spO2 to 100.
			if (spO2Temp > 100.0f)
			{
				spO2Temp = 100.0;
			}
			// Set new value and clear persistence counter
			spO2 = spO2Temp;
			spO2Valid_PC = 0;
			spO2_pv = spO2;
		}

		if (heartRateValid == 0)
		{
			// Use last good value and increment persistence counter
			heartRate = heartRate_pv;
			heartRateValid_PC = heartRateValid_PC +1;
		}
		else
		{
			// Set new value and clear persistence counter
			heartRates[3] = heartRateTemp;
			heartRateValid_PC = 0;
			heartRate = (heartRates[0]+heartRates[1]+heartRates[2]+heartRates[3])/4;
			heartRate_pv = heartRate;
			heartRates[0] = heartRates[1];
			heartRates[1] = heartRates[2];
			heartRates[2] = heartRates[3];
		}

		// If either PC is > 4 then indicate failure and stop sampling
		if (spO2Valid_PC > 4 || heartRateValid_PC > 4)
		{
			ppgSensorFail = ppgSensorDataFail;
			spO2 = -1;
			heartRate = -1;
		}
		else
		{
			ppgSensorFail = ppgSensorNoFail;
		}
		ppgDataReady = 0;
	}
}

void ppgSensor_PPGInterrupt(void)
{
	int8_t numofsamples = 0;
	uint8_t data[3] = {0,};
	uint8_t readptr = 0;
	uint8_t writeptr = 0;

	if (ppgBusy != 1)
	{
		// Risky. Should convert this to interrupts instead
		max030201_readReg(REG_ADDR_INTSTAT1,(uint8_t *) &ppgInterrupt, 1);

		if (ppgInterrupt.AsBits.PPGReady == 1)
		{
			max030201_readReg(REG_ADDR_FIFO_WR_PTR, data, 3);

			writeptr = data[0];
			readptr = data[2];
			numofsamples = writeptr-readptr;
			// Account for wrap-around. Although this should
			if (numofsamples < 0)
			{
				numofsamples = numofsamples + 32;
			}

			// Check if we have 25 samples, if so initiate i2c read
			if (numofsamples == NUMSAMPLES)
			{
				// Initiate I2C Read of the 25 data samples.
				max030201_readReg_IT(REG_ADDR_FIFO_DATA, (uint8_t *) ppgSampleData, NUMSAMPLES*6);

				// Set busy flag while data dumps
				ppgBusy = 1;
			}
		}
	}

	// ppgSensor "heartbeat" used to detect loss of communication with device
	ppgSensorAlive = ppgSensorAlive+1;
}

void ppgSensor_I2CInterrupt(void)
{
	// Perform this action only if ppgBusy flag was set
	// i.e. ppgBusy is only set only when 25 samples are ready.
	if(ppgBusy == 1)
	{
		ppgDataReady = 1;

		// Reset FIFO Read, FIFO Write, and clear interrupts
		max030201_writeReg_IT(REG_ADDR_FIFO_WR_PTR, tempdata, 3);
		max030201_readReg_IT(REG_ADDR_INTSTAT1,&tempdata[3],1);

		// Clear busy flag to allow next set of sample collection.
		ppgBusy = 0;
	}
}

int ppgSensor_Shutdown(uint8_t shutdown)
{
	int error = 0;
	// Set the mode to shutdown in the ram copy
	if (shutdown)
	{
		SensorRegMap.ModeConfig.AsBits.Shutdown = 1;
	}
	else
	{
		SensorRegMap.ModeConfig.AsBits.Shutdown = 0;
	}

	error = max030201_writeReg_IT(REG_ADDR_MODE_CONFG, &SensorRegMap.ModeConfig.As8BitWord, 1);
	return error;
}

int ppgSensor_Reset(void)
{
	int error = 0;

	// Run initialization to restart device
	error = ppgSensor_Init();
	ppgDataReady = 0;

	// Should we reset the buffers here?
	return error;
}

void ppgSensor_SetFailure(uint8_t failtype)
{
	ppgSensorFail = failtype;
	heartRateValid = 0;
	spO2Valid = 0;

}
