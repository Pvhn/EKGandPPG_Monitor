/*
 * max030201.h
 *
 *  Created on: Apr 12, 2025
 *      Author: peter
 */

#ifndef INC_MAX030201_H_
#define INC_MAX030201_H_

/*
 * max030102.h
 *
 *  Created on: Apr 12, 2025
 *      Author: peter
 */

#ifndef MAX030102_H_
#define MAX030102_H_

#include <stdint.h>

#define MAX03I2CADDR_W 0xAE // Address for Write
#define MAX03I2CADDR_R 0xAF // Address for Read

/* ========= Register Addresses ================== */
#define REG_ADDR_INTSTAT1       0x00
#define REG_ADDR_INTSTAT2       0x01
#define REG_ADDR_INTEN1         0x02
#define REG_ADDR_INTEN2         0x03
#define REG_ADDR_FIFO_WRITE_PTR 0x04
#define REG_ADDR_OVERFLOW_CTR   0x05
#define REG_ADDR_FIFO_READ_PTR  0x06
#define REG_ADDR_FIFO_DATA      0x07
#define REG_ADDR_FIFO_CONFG     0x08
#define REG_ADDR_MODE_CONFG     0x09
#define REG_ADDR_SPO2_CONFG     0x0A
#define REG_ADDR_REDLEDCURR_CTL 0x0C
#define REG_ADDR_IRLEDCURR_CTL  0x0D
#define REG_ADDR_MULTMODE1_CTL  0x11
#define REG_ADDR_MULTMODE2_CTL  0x12
#define REG_ADDR_TEMPINT_DATA   0x1F
#define REG_ADDR_TEMPFRAC_DATA  0x20
#define REG_ADDR_TEMP_CONFG     0x21

#define MODE_HR 2
#define MODE_SPO2 3
#define MODE_MULTLED 7



typedef struct
{
	uint8_t addr;
	uint8_t data;
}I2CDataType;

typedef union
{
    uint8_t As8BitWord;

    struct
    {
        uint8_t PowerReady      :1;
        uint8_t Reserved1to4    :4;
        uint8_t AmbLightCancOvf :1;
        uint8_t PPGReady        :1;
        uint8_t AlmostFull      :1;
    }AsBits;
}Interrupt1RegType;

typedef union
{
    uint8_t As8BitWord;

    struct
    {
        uint8_t Reserved0       :1;
        uint8_t DieTempReady    :1;
        uint8_t Reserved2to7    :6;

    }AsBits;
}Interrupt2RegType;

typedef union
{
    uint16_t As16BitWord;

    struct
    {
        Interrupt1RegType IntStatus1;
        Interrupt2RegType IntStatus2;
    }As8BitWord;
}InterruptStatRegType;

typedef union
{
    uint16_t As16BitWord;

    struct
    {
        Interrupt1RegType IntStatus1;
        Interrupt2RegType IntStatus2;
    }As8BitWord;

}InterruptEnableRegType;

typedef union
{
    uint8_t As8BitWord;

    struct
    {
        uint8_t FIFOAlmFull  :4; // FIFO Almost Full Config
        uint8_t FIFORollOvEn :1; // FIFO Roll Over Config
        uint8_t SampleAvg    :3; // FIFO Sample Averaging Config
    }AsBits;
}FIFOConfgRegType;

typedef union
{
    uint8_t As8BitWord;

    struct
    {
        uint8_t Mode        :3;
        uint8_t Reserved3   :1;
        uint8_t Reserved4   :1;
        uint8_t Reserved5   :1;
        uint8_t Reset       :1;
        uint8_t Shutdown    :1;
    }AsBits;

}ModeConfigRegType;

typedef union
{
    uint8_t As8BitWord;

    struct
    {
        uint8_t LEDPulseWidthCtl  :2;
        uint8_t SpO2SampleRate    :3;
        uint8_t SpO2ADCRangeCtl   :2;
        uint8_t Reserved7         :1;
    }AsBits;
}SpO2ConfigRegType;

typedef union
{
    uint16_t As16BitWord;

    struct
    {
        uint16_t Slot1       :3;
        uint16_t Reserved3   :1;
        uint16_t Slot2       :3;
        uint16_t Reserved7   :1;
        uint16_t Slot3       :3;
        uint16_t Reserved10  :1;
        uint16_t Slot4       :3;
        uint16_t Reserved15  :1;
    }AsBits;

    struct
	{
    	uint8_t Slot1and2 :8;
    	uint8_t Slot3and4 :8;
	}As8BitWord;

}MultLEDModeCtlRegType;

typedef union
{
    uint16_t As16BitWord;

    struct
    {
        int16_t DieTempInteger     :8;
        uint16_t DieTempFraction   :8;
    }AsBits;

}TempIntFracDataRegType;

typedef union
{
    uint8_t As8BitWord;

    struct
    {
        uint8_t TempEnable  :1;
        uint8_t Reserved    :7;
    }AsBits;
}TempDieCtlRegType;

typedef struct
{
	Interrupt1RegType IntStatus1;
	Interrupt2RegType IntStatus2;
	Interrupt1RegType IntEnable1;
	Interrupt2RegType IntEnable2;
	uint8_t FIFO_WR_PTR;
	uint8_t OVF_COUNTER;
	uint8_t FIFO_RD_PTR;
	uint8_t FIFO_DATA;
	FIFOConfgRegType FIFOConfig;
	ModeConfigRegType ModeConfig;
	SpO2ConfigRegType SpO2Config;
	uint8_t Reserved_0B;
	uint8_t LEDPulseAmpRed;
	uint8_t LEDPulseAmpIR;
	uint8_t Reserved_0E_0F[2];
	MultLEDModeCtlRegType MultModeCtl;
	uint8_t Reserved_13_17[4];
	uint8_t Reserved_18_1E[6];
	TempIntFracDataRegType DieTempReg;
	TempDieCtlRegType DieTempConfg;
	// Remainder of the registers we
	// do not use so will not define to save space
}SensorIOCRegType;

typedef struct
{
	uint32_t Red[4];
	uint32_t IR[4];
	uint32_t Green[4];
	uint8_t head;
	uint8_t tail;
}SensorFIFOBufferType;

/*================== FUNCTION DECLARATIONS =======================
 */

int max030201_init(I2C_HandleTypeDef *handle);

#endif /* MAX030102_H_ */

#endif /* INC_MAX030201_H_ */
