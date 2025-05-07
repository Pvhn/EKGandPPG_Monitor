
#ifndef __ILI9341_H__
#define __ILI9341_H__

#include "fonts.h"
#include <stdbool.h>

#define ILI9341_MADCTL_MY  0x80
#define ILI9341_MADCTL_MX  0x40
#define ILI9341_MADCTL_MV  0x20
#define ILI9341_MADCTL_ML  0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH  0x04

/*** Redefine if necessary ***/
#define ILI9341_SPI_PORT hspi1
extern SPI_HandleTypeDef ILI9341_SPI_PORT;

#define ILI9341_CS_Pin        GPIO_PIN_0
#define ILI9341_CS_GPIO_Port  GPIOB
#define ILI9341_RES_Pin       GPIO_PIN_1
#define ILI9341_RES_GPIO_Port GPIOB
#define ILI9341_DC_Pin        GPIO_PIN_2
#define ILI9341_DC_GPIO_Port  GPIOB
#define ILI9341_LED_Pin        GPIO_PIN_8
#define ILI9341_LED_GPIO_Port  GPIOE

// default orientation
//#define ILI9341_WIDTH  240
//#define ILI9341_HEIGHT 320
//#define ILI9341_ROTATION (ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR)

// rotate right

#define ILI9341_WIDTH  320
#define ILI9341_HEIGHT 240
#define ILI9341_ROTATION (ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR)


// rotate left

//#define ILI9341_WIDTH  320
//#define ILI9341_HEIGHT 240
//#define ILI9341_ROTATION (ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR)


// upside down
/*
#define ILI9341_WIDTH  240
#define ILI9341_HEIGHT 320
#define ILI9341_ROTATION (ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR)
*/

/****************************/

#define ILI9341_TFTWIDTH 240  // ILI9341 max TFT width
#define ILI9341_TFTHEIGHT 320 // ILI9341 max TFT height

#define ILI9341_NOP 0x00     // No-op register
#define ILI9341_SWRESET 0x01 // Software reset register
#define ILI9341_RDDID 0x04   // Read display identification information
#define ILI9341_RDDST 0x09   // Read Display Status

#define ILI9341_SLPIN 0x10  // Enter Sleep Mode
#define ILI9341_SLPOUT 0x11 // Sleep Out
#define ILI9341_PTLON 0x12  // Partial Mode ON
#define ILI9341_NORON 0x13  // Normal Display Mode ON

#define ILI9341_RDMODE 0x0A     // Read Display Power Mode
#define ILI9341_RDMADCTL 0x0B   // Read Display MADCTL
#define ILI9341_RDPIXFMT 0x0C   // Read Display Pixel Format
#define ILI9341_RDIMGFMT 0x0D   // Read Display Image Format
#define ILI9341_RDSELFDIAG 0x0F // Read Display Self-Diagnostic Result

#define ILI9341_INVOFF 0x20   // Display Inversion OFF
#define ILI9341_INVON 0x21    // Display Inversion ON
#define ILI9341_GAMMASET 0x26 // Gamma Set
#define ILI9341_DISPOFF 0x28  // Display OFF
#define ILI9341_DISPON 0x29   // Display ON

#define ILI9341_CASET 0x2A // Column Address Set
#define ILI9341_PASET 0x2B // Page Address Set
#define ILI9341_RAMWR 0x2C // Memory Write
#define ILI9341_RAMRD 0x2E // Memory Read

#define ILI9341_PTLAR 0x30    // Partial Area
#define ILI9341_VSCRDEF 0x33  // Vertical Scrolling Definition
#define ILI9341_MADCTL 0x36   // Memory Access Control
#define ILI9341_VSCRSADD 0x37 // Vertical Scrolling Start Address
#define ILI9341_PIXFMT 0x3A   // COLMOD: Pixel Format Set

#define ILI9341_FRMCTR1 0xB1 // Frame Rate Control (In Normal Mode/Full Colors)
#define ILI9341_FRMCTR2 0xB2 // Frame Rate Control (In Idle Mode/8 colors)
#define ILI9341_FRMCTR3 0xB3 // Frame Rate control (In Partial Mode/Full Colors)
#define ILI9341_INVCTR 0xB4  // Display Inversion Control
#define ILI9341_DFUNCTR 0xB6 // Display Function Control

#define ILI9341_PWCTR1 0xC0 // Power Control 1
#define ILI9341_PWCTR2 0xC1 // Power Control 2
#define ILI9341_PWCTR3 0xC2 // Power Control 3
#define ILI9341_PWCTR4 0xC3 // Power Control 4
#define ILI9341_PWCTR5 0xC4 // Power Control 5
#define ILI9341_VMCTR1 0xC5 // VCOM Control 1
#define ILI9341_VMCTR2 0xC7 // VCOM Control 2
#define ILI9331_PWRCTRA 0xCB // Power Control A
#define ILI9331_PWRCTRB 0xCF // Power Control B

#define ILI9341_RDID1 0xDA // Read ID 1
#define ILI9341_RDID2 0xDB // Read ID 2
#define ILI9341_RDID3 0xDC // Read ID 3
#define ILI9341_RDID4 0xDD // Read ID 4

#define ILI9341_GMCTRP1 0xE0 // Positive Gamma Correction
#define ILI9341_GMCTRN1 0xE1 // Negative Gamma Correction
#define ILI9341_DRVTIMCTRA 0xE8 // Driver Timing Control A
#define ILI9341_DRVTIMCTRB 0xEA // Driver Timing Control B
#define ILI9341_PWRSEQCTR 0xED // Power Sequence Control

#define ILI9341_3GAMMA 0xF2 // 3-Gamma Control
#define ILI9341_PUMPRATCTR 0xF7 // Pump Ratio Control

// Color definitions
#define	ILI9341_BLACK   0x0000
#define	ILI9341_BLUE    0x001F
#define	ILI9341_RED     0xF800
#define ILI9341_MAROON  0x8000
#define	ILI9341_GREEN   0x07E0
#define ILI9341_CYAN    0x07FF
#define ILI9341_MAGENTA 0xF81F
#define ILI9341_YELLOW  0xFFE0
#define ILI9341_WHITE   0xFFFF
#define ILI9341_RED2    0xC963
#define ILI9341_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

void ILI9341_Unselect();

void ILI9341_Init(void);
void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ILI9341_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor);
void ILI9341_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ILI9341_FillScreen(uint16_t color);
void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);
void ILI9341_InvertColors(bool invert);
void ILI9341_Backlight(bool on);

#endif // __ILI9341_H__
