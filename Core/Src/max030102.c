/*
 * max030102.c
 *
 *  Created on: Apr 12, 2025
 *      Author: peter
 */
#include "stm32f4xx_hal.h"
#include "max030201.h"


// Allocate a pointer containing
// the local memory map of the sensor
SensorIOCRegType SensorRegMap;

SensorIOCRegType SensorRegMapRead;

int max030201_writeRegMask(I2C_HandleTypeDef *handle, const uint8_t regAddr, uint8_t data, uint8_t mask)
{
	uint8_t *dataBuffer;

	// Read the register first
	HAL_I2C_Mem_Read(handle, MAX03I2CADDR_R, regAddr, 1, dataBuffer, 1, HAL_MAX_DELAY);

	// Mask out what we want then write
	*dataBuffer = (*dataBuffer) & mask;
	*dataBuffer = (*dataBuffer) | data;
	HAL_I2C_Mem_Write(handle, MAX03I2CADDR_W, regAddr, 1, dataBuffer,1, HAL_MAX_DELAY);

	return 0;
}

int max030201_writeReg(I2C_HandleTypeDef *handle, const uint8_t regAddr, const uint8_t data)
{
	uint8_t dataBuffer[2] = {regAddr, data};

	HAL_I2C_Master_Transmit(handle, MAX03I2CADDR_W, dataBuffer, 2, HAL_MAX_DELAY);

	// TBD ADD ERROR CHECKING
	return 0;
}

int max030201_writeReg_IT(I2C_HandleTypeDef *handle, const uint8_t regAddr, const uint8_t data)
{
	uint8_t dataBuffer[2] = {regAddr, data};

	HAL_I2C_Master_Transmit_IT(handle, MAX03I2CADDR_W, dataBuffer, 2);

	// TBD ADD ERROR CHECKING
	return 0;
}

int max030201_readReg(I2C_HandleTypeDef *handle, const uint8_t regAddr, uint8_t *pData, uint8_t bytes)
{
	HAL_I2C_Mem_Read(handle, MAX03I2CADDR_W, regAddr, 1, pData, bytes, HAL_MAX_DELAY);
	// TBD ADD ERROR CHECKING
	return 0;
}

/* =======================================================
 * Function Name: max030201_readReg_IT
 * =======================================================
 * Parameters: regAddr, *pData, bytes
 * Return: None
 * Description: Non-Blocking (Interrupt) routine for
 * reading data from a register.
 * Data is stored in the buffer pointer provided.
 * =======================================================
 */
int max030201_readReg_IT(I2C_HandleTypeDef *handle, const uint8_t regAddr, uint8_t *pData, uint8_t bytes)
{

	HAL_I2C_Mem_Read_IT(handle, MAX03I2CADDR_W, regAddr, 1, pData, bytes);
	return 0;
}

int max030201_init(I2C_HandleTypeDef *handle)
{
	int error = 0;
	SensorRegMap.FIFOConfig.AsBits.FIFORollOvEn = 1;
	SensorRegMap.FIFOConfig.AsBits.SampleAvg = 0;
	SensorRegMap.ModeConfig.AsBits.Mode = MODE_MULTLED;
	SensorRegMap.SpO2Config.As8BitWord = 0x00;

	uint8_t dataBuffer[10] =
	{
		0x7F,
		0x07,
		0x27,
		0x00,
		0xFF,
		0xFF,
		0x00,
		0x00,
		0x21,
		0x3,
	};

	error = HAL_I2C_Mem_Write(handle, MAX03I2CADDR_W, REG_ADDR_FIFO_CONFG, I2C_MEMADD_SIZE_8BIT,
			dataBuffer, 10, HAL_MAX_DELAY);

	dataBuffer[0] = 0x0000;
	dataBuffer[1] = 0x0000;
	dataBuffer[2] = 0x0000;
	error = HAL_I2C_Mem_Write(handle, MAX03I2CADDR_W, REG_ADDR_FIFO_WRITE_PTR, I2C_MEMADD_SIZE_8BIT,
			dataBuffer, 3, HAL_MAX_DELAY);

	return error;
}
