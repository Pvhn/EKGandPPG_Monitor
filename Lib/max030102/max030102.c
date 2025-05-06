/*
 * max030102.c
 *
 *  Created on: Apr 12, 2025
 *      Author: peter
 */

#include "max030201.h"

int max030201_writeRegMask(const uint8_t regAddr, uint8_t data, uint8_t mask)
{
	uint8_t dataBuffer;
	int error = 0;
	// Read the register first
	HAL_I2C_Mem_Read(&h_I2Cbus, MAX03I2CADDR_R, regAddr, 1, &dataBuffer, 1, HAL_MAX_DELAY);

	// Mask out what we want then write
	dataBuffer = (dataBuffer) & mask;
	dataBuffer = (dataBuffer) | data;
	HAL_I2C_Mem_Write(&h_I2Cbus, MAX03I2CADDR_W, regAddr, 1, &dataBuffer,1, HAL_MAX_DELAY);

	return error;
}

int max030201_writeReg(const uint8_t regAddr, uint8_t *pData, uint8_t bytes)
{
	int error = 0;
	HAL_I2C_Mem_Write(&h_I2Cbus, MAX03I2CADDR_W, regAddr, 1, pData, bytes, HAL_MAX_DELAY);
	// TBD ADD ERROR CHECKING
	return error;
}

int max030201_writeReg_IT(const uint8_t regAddr, uint8_t *pData, uint8_t bytes)
{
	int error = 0;
	HAL_I2C_Mem_Write_IT(&h_I2Cbus, MAX03I2CADDR_W, regAddr, 1, pData, bytes);
	// TBD ADD ERROR CHECKING
	return error;
}

int max030201_readReg(const uint8_t regAddr, uint8_t *pData, uint8_t bytes)
{
	int error = 0;
	HAL_I2C_Mem_Read(&h_I2Cbus, MAX03I2CADDR_W, regAddr, 1, pData, bytes, HAL_MAX_DELAY);
	// TBD ADD ERROR CHECKING
	return error;
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
int max030201_readReg_IT(const uint8_t regAddr, uint8_t *pData, uint8_t bytes)
{
	int error = 0;
	HAL_I2C_Mem_Read_IT(&h_I2Cbus, MAX03I2CADDR_W, regAddr, 1, pData, bytes);
	return error;
}

int max030201_CheckDevice(void)
{
	int error = 0;
	// Check for device. Wait 250ms and try again if no response.
	error = HAL_I2C_IsDeviceReady(&h_I2Cbus, MAX03I2CADDR_W,4, 250);
	return error;
}





