/* vim: set ai et ts=4 sw=4: */
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_spi.h"
#include "ili9341.h"
#include  <stdint.h>

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

static void ILI9341_Select() {
    HAL_GPIO_WritePin(ILI9341_CS_GPIO_Port, ILI9341_CS_Pin, GPIO_PIN_RESET);
}

void ILI9341_Unselect() {
    HAL_GPIO_WritePin(ILI9341_CS_GPIO_Port, ILI9341_CS_Pin, GPIO_PIN_SET);
}

static void ILI9341_Reset() {
    HAL_GPIO_WritePin(ILI9341_RES_GPIO_Port, ILI9341_RES_Pin, GPIO_PIN_RESET);
    HAL_Delay(20);
    HAL_GPIO_WritePin(ILI9341_RES_GPIO_Port, ILI9341_RES_Pin, GPIO_PIN_SET);
    HAL_Delay(20);
}

static void SPI_WriteByte (uint8_t tx_byte)
{
	if ((SPI1->CR1 & SPI_CR1_SPE) == 0)
	{
		LL_SPI_Enable(SPI1);
	}

	// This is an awful way of doing it LOL
	// I should add a time out here eventually
	while((SPI1->SR & SPI_SR_TXE) != SPI_SR_TXE);

	SPI1->DR = tx_byte;
}

static void ILI9341_WriteCommand(uint8_t cmd)
{

    HAL_GPIO_WritePin(ILI9341_DC_GPIO_Port, ILI9341_DC_Pin, GPIO_PIN_RESET);
    SPI_WriteByte(cmd);
//    error = HAL_SPI_Transmit(&ILI9341_SPI_PORT, &cmd, 1, HAL_MAX_DELAY);

}

static void ILI9341_WriteData(const uint8_t* buff, size_t buff_size)
{

	HAL_GPIO_WritePin(ILI9341_DC_GPIO_Port, ILI9341_DC_Pin, GPIO_PIN_SET);

    // split data in small chunks because HAL can't send more then 64K at once
    while(buff_size > 0) {
        uint16_t chunk_size = buff_size > 32768 ? 32768 : buff_size;
        for (int i = 0; i< chunk_size; i++)
        {
        	SPI_WriteByte(buff[i]);
        }
//        error = HAL_SPI_Transmit(&ILI9341_SPI_PORT, buff, chunk_size, HAL_MAX_DELAY);
        buff += chunk_size;
        buff_size -= chunk_size;
    }
}

static void ILI9341_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    // column address set
    ILI9341_WriteCommand(0x2A); // CASET
    {
        uint8_t data[] = { (x0 >> 8) & 0xFF, x0 & 0xFF, (x1 >> 8) & 0xFF, x1 & 0xFF };
        ILI9341_WriteData(data, sizeof(data));
    }

    // row address set
    ILI9341_WriteCommand(0x2B); // RASET
    {
        uint8_t data[] = { (y0 >> 8) & 0xFF, y0 & 0xFF, (y1 >> 8) & 0xFF, y1 & 0xFF };
        ILI9341_WriteData(data, sizeof(data));
    }

    // write to RAM
    ILI9341_WriteCommand(0x2C); // RAMWR
}

static const uint8_t initcmdlist[] = {
	ILI9331_PWRCTRA, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
	ILI9331_PWRCTRB, 3, 0x00, 0xC1, 0x30,
	ILI9341_DRVTIMCTRA, 3, 0x85, 0x00, 0x78,
	ILI9341_DRVTIMCTRB, 2, 0x00, 0x00,
	ILI9341_PWRSEQCTR, 4, 0x64, 0x03, 0x12, 0x81,
	ILI9341_PUMPRATCTR, 1, 0x20,
	ILI9341_PWCTR1  , 1, 0x23,             // Power control VRH[5:0]
	ILI9341_PWCTR2  , 1, 0x10,             // Power control SAP[2:0];BT[3:0]
	ILI9341_VMCTR1  , 2, 0x3e, 0x28,       // VCM control
	ILI9341_VMCTR2  , 1, 0x86,             // VCM control2
	ILI9341_MADCTL  , 1, 0x48,             // Memory Access Control
	ILI9341_VSCRSADD, 1, 0x00,             // Vertical scroll zero
	ILI9341_PIXFMT  , 1, 0x55,
	ILI9341_FRMCTR1 , 2, 0x00, 0x18,
	ILI9341_DFUNCTR , 3, 0x08, 0x82, 0x27, // Display Function Control
	ILI9341_3GAMMA, 1, 0x00,                 // 3Gamma Function Disable
	ILI9341_GAMMASET , 1, 0x01,             // Gamma curve selected
	ILI9341_GMCTRP1 , 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, // Set Gamma
		0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
	ILI9341_GMCTRN1 , 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, // Set Gamma
		0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
	ILI9341_SLPOUT  , 0x80,                // Exit Sleep
	ILI9341_DISPON  , 0x80,                // Display on
	0x00                                   // End of list
};

void ILI9341_Init() {

    ILI9341_Unselect();
    ILI9341_Reset();

    ILI9341_Select();

    uint8_t cmd, x, numArgs;
    const uint8_t *addr = initcmdlist;
    while ((cmd = pgm_read_byte(addr++)) > 0) {
      x = pgm_read_byte(addr++);
      numArgs = x & 0x7F;

	  ILI9341_WriteCommand(cmd);
	  ILI9341_WriteData(addr, numArgs);

      addr += numArgs;
      if (x & 0x80)
        HAL_Delay(150);
    }

    // MADCTL
    ILI9341_WriteCommand(0x36);
    {
        uint8_t data[] = { ILI9341_ROTATION };
        ILI9341_WriteData(data, sizeof(data));
    }

    ILI9341_Unselect();

    ILI9341_Backlight(1);
}

void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT))
        return;

    ILI9341_Select();

    ILI9341_SetAddressWindow(x, y, x+1, y+1);
    uint8_t data[] = { color >> 8, color & 0xFF };
    ILI9341_WriteData(data, sizeof(data));

    ILI9341_Unselect();
}

static void ILI9341_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor) {
    uint32_t i, b, j;

    ILI9341_SetAddressWindow(x, y, x+font.width-1, y+font.height-1);

    for(i = 0; i < font.height; i++)
    {
        b = font.data[(ch - 32) * font.height + i];
        for(j = 0; j < font.width; j++)
        {
            if((b << j) & 0x8000)
            {
                uint8_t data[] = { color >> 8, color & 0xFF };
                ILI9341_WriteData(data, sizeof(data));
            }
            else
            {
                uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
                ILI9341_WriteData(data, sizeof(data));
            }
        }
    }
}

void ILI9341_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor) {
    ILI9341_Select();

    while(*str)
    {
        if(x + font.width >= ILI9341_WIDTH) {
            x = 0;
            y += font.height;
            if(y + font.height >= ILI9341_HEIGHT) {
                break;
            }

            if(*str == ' ') {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }

        ILI9341_WriteChar(x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }

    ILI9341_Unselect();
}

void ILI9341_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    // clipping
    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) return;
    if((x + w - 1) >= ILI9341_WIDTH) w = ILI9341_WIDTH - x;
    if((y + h - 1) >= ILI9341_HEIGHT) h = ILI9341_HEIGHT - y;

    ILI9341_Select();
    ILI9341_SetAddressWindow(x, y, x+w-1, y+h-1);

    uint8_t data[] = { color >> 8, color & 0xFF };
    HAL_GPIO_WritePin(ILI9341_DC_GPIO_Port, ILI9341_DC_Pin, GPIO_PIN_SET);
    for(y = h; y > 0; y--) {
        for(x = w; x > 0; x--) {
        	SPI_WriteByte(data[0]);
        	SPI_WriteByte(data[1]);
        }
    }

    ILI9341_Unselect();
}

void ILI9341_FillScreen(uint16_t color) {
    ILI9341_FillRectangle(0, 0, ILI9341_WIDTH, ILI9341_HEIGHT, color);
}

void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {
    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) return;
    if((x + w - 1) >= ILI9341_WIDTH) return;
    if((y + h - 1) >= ILI9341_HEIGHT) return;

    ILI9341_Select();
    ILI9341_SetAddressWindow(x, y, x+w-1, y+h-1);
    ILI9341_WriteData((uint8_t*)data, sizeof(uint16_t)*w*h);
    ILI9341_Unselect();
}

void ILI9341_InvertColors(bool invert) {
    ILI9341_Select();
    ILI9341_WriteCommand(invert ? 0x21 /* INVON */ : 0x20 /* INVOFF */);
    ILI9341_Unselect();
}

void ILI9341_Backlight(bool on)
{
	HAL_GPIO_WritePin(ILI9341_LED_GPIO_Port, ILI9341_LED_Pin, on);
}

