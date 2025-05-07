/*
 * UserInterface.c
 *
 *  Created on: May 6, 2025
 *      Author: peter
 */

#include "UserInterface.h"
#include "img_heart.h"
volatile uint8_t mode_tf = 1;
volatile uint8_t mode = PPGMode;
volatile uint8_t ekgPage = 0;
volatile uint8_t pause_screen = false;
static uint8_t pause_pv = false;
uint8_t ekgMode = ekgContinuous;

uint16_t ekgTrace[ILI9341_WIDTH];
uint16_t ppgTrace[ILI9341_WIDTH];

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))


void gfx_DrawGrid_VerticalLine(uint16_t x, uint16_t y1, uint16_t y2);

void gfx_DrawGridLarge(uint8_t full);

uint8_t gfx_DrawHeartBeatAnimate(uint16_t hb, uint8_t valid, uint16_t posx, uint16_t posy)
{
	uint16_t rate = 0;
	const uint16_t HBMAX = 260;
	static uint32_t time_pv = 0;
	uint32_t time_curr = HAL_GetTick();
	static uint8_t toggle_img = 0;

	if (valid)
	{
		// Limit the update rate of the animation so
		// we don't try to anime something stupid
		if (hb < HBMAX)
		{
			if (hb > 0 && hb != 0xDEAD)
			{
				// Calculate the heartbeats per second.
				rate = 60000/hb;
			}
			else
			{
				ILI9341_DrawImage(posx, posy, img_heart_width, img_heart_height, (const uint16_t*)img_heart_off);
				return 0;
			}

		}
		else
		{
			rate = 60000/HBMAX;
		}

		//
		if (time_curr - time_pv > rate/2)
		{
			if(toggle_img == 0)
			{
				ILI9341_DrawImage(posx, posy, img_heart_width, img_heart_height, (const uint16_t*)img_heart_off);
				toggle_img = 1;
			}
			else
			{
				ILI9341_DrawImage(posx, posy, img_heart_width, img_heart_height, (const uint16_t*)img_heart_on);
				toggle_img = 0;
			}
			// Save off time for next run
			time_pv = time_curr;

			return 1;
		}
	}
	else
	{
		ILI9341_DrawImage(posx, posy, img_heart_width, img_heart_height, (const uint16_t*)img_heart_off);
	}


	return 0;
}

void gfx_DrawGridLarge(uint8_t full)
{
	  uint16_t x_start, y_start, width, height, i;

	  // Draw full sized grid
	  if (full)
	  {
		  x_start = 0;
		  y_start = 0;
		  width = ILI9341_WIDTH;
		  height = ILI9341_HEIGHT;
	  }
	  else // Half page grid
	  {
		  x_start = 0;
		  y_start = ILI9341_HEIGHT/2;
		  width = ILI9341_WIDTH;
		  height = ILI9341_HEIGHT;
	  }

	  for (i = y_start; i < ILI9341_HEIGHT; i++)
	  {
		  if (i % 8 == 0)
		  {
			  ILI9341_FillRectangle(0, i, width, 1, ILI9341_MAROON);
		  }
	  }
	  for (i = x_start; i < ILI9341_WIDTH; i++)
	  {
		  if (i % 8 == 0)
		  {
			  ILI9341_FillRectangle(i, y_start, 1, height, ILI9341_MAROON);
		  }
	  }
	  for (i = y_start; i < ILI9341_HEIGHT; i++)
	  {
		  if (i % 40 == 0)
		  {
			  ILI9341_FillRectangle(0, i, width, 2, ILI9341_RED);
		  }
	  }
	  for (i = x_start; i < ILI9341_WIDTH; i++)
	  {
		  if (i % 40 == 0)
		  {
			  ILI9341_FillRectangle(i, y_start, 2, height, ILI9341_RED);
		  }
	  }
}

uint32_t ppgMAXInput = 350;
uint32_t ppgMINInput = 200;
void gfx_DrawPPGPage(void)
{
	static int32_t temp_hb_pv = 0;
	static float temp_spO2_pv = 0;
	char str_buffer[40];
	uint16_t xpos_hb = 52;
	uint16_t ypos_hb = 10;
	uint16_t xpos_spO2 = 36;
	uint16_t ypos_spO2 = 62;
	static uint16_t status_pv = 0;

	// This is more of a hack. Need to properly work out the timing
	// transition
	if (mode_tf == 1 && mode == PPGMode)
	{
		ILI9341_FillScreen(ILI9341_BLACK);
		gfx_DrawGridLarge(0); // Draw half page grid
		// Draw on top right corner
		ILI9341_DrawImage(196, 20, img_heart_width, img_heart_height, (const uint16_t*)img_heart_off);

		// Write the BPM and spO2 strings once since they don't change
		ILI9341_WriteString(99, ypos_hb+28, "BPM", Font_11x18, ILI9341_WHITE, ILI9341_BLACK);
		ILI9341_WriteString(87, ypos_spO2+28, "spO2", Font_11x18, ILI9341_WHITE, ILI9341_BLACK);

		mode_tf = 0;
	}

	if (temp_hb_pv != heartRate)
	{
		if (heartRateValid == true)
		{
			sprintf(str_buffer, "%5d", (int) heartRate);
		}
		else
		{
			sprintf(str_buffer, "%s", "Error");
		}
		ILI9341_FillRectangle(xpos_hb, ypos_hb, 16*3, 26, ILI9341_BLACK);
		ILI9341_WriteString(xpos_hb, ypos_hb, str_buffer, Font_16x26, ILI9341_WHITE, ILI9341_BLACK);

	}
	temp_hb_pv = heartRate;

	if(temp_spO2_pv != spO2)
	{
		if (spO2Valid == true)
		{
			if(spO2 >= 100.0)
			{
				sprintf(str_buffer, "%5g%%", spO2);
			}
			else
			{
				sprintf(str_buffer, "%5.2f%%", spO2);
			}
		}
		else
		{
			sprintf(str_buffer, "%s", " Error");
		}

	    ILI9341_FillRectangle(xpos_spO2, ypos_spO2, 16*6, 26, ILI9341_BLACK);
		ILI9341_WriteString(xpos_spO2, ypos_spO2, str_buffer, Font_16x26, ILI9341_WHITE, ILI9341_BLACK);
	}
	temp_spO2_pv = spO2;

	if(ppgSensorFail != ppgSensorNoFail)
	{
		// Im too lazy to figure out how to justify this correctly...
		sprintf(str_buffer, "%s", "Error");
		ILI9341_WriteString(xpos_hb, ypos_hb, str_buffer, Font_16x26, ILI9341_WHITE, ILI9341_BLACK);
		sprintf(str_buffer, "%s", " Error");
		ILI9341_WriteString(xpos_spO2, ypos_spO2, str_buffer, Font_16x26, ILI9341_WHITE, ILI9341_BLACK);

		if(ppgSensorFail == ppgSensorI2CFail)
		{
			sprintf(str_buffer, "Error: No response from I2C.");
			ILI9341_WriteString(0, ILI9341_HEIGHT-20,str_buffer, Font_7x10, ILI9341_WHITE, ILI9341_BLACK);
			sprintf(str_buffer, "Please Reset Sensor.");
			ILI9341_WriteString(0, ILI9341_HEIGHT-10,str_buffer, Font_7x10, ILI9341_WHITE, ILI9341_BLACK);
		}
		else
		{
			sprintf(str_buffer, "Error: No valid pulse detected.");
			ILI9341_WriteString(0, ILI9341_HEIGHT-20,str_buffer, Font_7x10, ILI9341_WHITE, ILI9341_BLACK);
			sprintf(str_buffer, "Try adjust finger placement.");
			ILI9341_WriteString(0, ILI9341_HEIGHT-10,str_buffer, Font_7x10, ILI9341_WHITE, ILI9341_BLACK);
		}
	}
	else
	{
		if (ppgSensorFail == ppgSensorNoFail)
		{
			if (status_pv != ppgSensorNoFail)
			{
				for (int x = 0; x < ILI9341_WIDTH; x++)
				{
					gfx_DrawGrid_VerticalLine(x, ILI9341_HEIGHT-20, ILI9341_HEIGHT);
				}
			}
		}
	}
	status_pv = ppgSensorFail;

	if (heartRateValid == true)
	{
		gfx_DrawHeartBeatAnimate(heartRate, heartRateValid, 196, 20);
	}
}

void gfx_DrawGrid_VerticalLine(uint16_t x, uint16_t y1, uint16_t y2)
{
	uint16_t temp = 0;
	uint16_t y = 0;

	if (y1 > y2)
	{
	  temp = y2;
	  y2 = y1;
	  y1 = temp;
	}

	if (x % 40 == 0)
	{
		ILI9341_FillRectangle(x, y1, 2, y2-y1+1, ILI9341_RED);
	}
	else if (x % 8 == 0)
	{
		ILI9341_FillRectangle(x, y1, 1, y2-y1+1, ILI9341_MAROON);
	}
	else
	{
		ILI9341_FillRectangle(x, y1, 1, y2-y1+1, ILI9341_BLACK);
		for (y = y1; y < y2; y++)
		{
			if (y % 40 == 0)
			{
//				ILI9341_DrawPixel(x,y, ILI9341_RED);
				ILI9341_FillRectangle(x, y, 1, 2, ILI9341_RED);
			}

			if (y % 8 == 0)
			{
				ILI9341_DrawPixel(x,y, ILI9341_MAROON);

			}
		}
	}

}

void gfx_DrawEKGPageLarge(int16_t sample, uint32_t time, uint16_t bpm, uint16_t status)
{
	uint8_t update = 0;
	char str_buffer[40];

	static uint16_t ypos_pv;
	static uint16_t xpos = 0;
	static uint16_t ytracepos_pv = 0;
	uint16_t ypos = 0;
	uint16_t xpos_text = 0;
	static uint16_t status_pv = 0;
	static uint32_t tick_pv = 0;
	uint32_t tick = HAL_GetTick();


	// Must check for mode due to timing issues.
	// Ensures we don't double draw
	if(mode_tf == 1 && mode == EKGModeContLarge)
	{
		// Clear the screen and draw grid
		ILI9341_FillScreen(ILI9341_BLACK);
		gfx_DrawGridLarge(1); // Draw Full sized grid
		mode_tf = 0;
		xpos = 0;
	}

	if(pause_screen == false ||
		((pause_screen == true) && (xpos < ILI9341_WIDTH-1)))
	{
		if (status == ekgNoFail)
		{
			if (status_pv != ekgNoFail)
			{
				for (int x = 0; x < ILI9341_WIDTH*3/4; x++)
				{
					gfx_DrawGrid_VerticalLine(x, ILI9341_HEIGHT-20, ILI9341_HEIGHT);
				}
			}

			ypos = ILI9341_HEIGHT-(sample/15);

			gfx_DrawGrid_VerticalLine(xpos, ytracepos_pv, ekgTrace[xpos]);

			if (ypos > ypos_pv)
			{
				ILI9341_FillRectangle(xpos, ypos_pv, 1, ypos-ypos_pv+1, ILI9341_WHITE);
			}
			else
			{
				ILI9341_FillRectangle(xpos, ypos, 1, ypos_pv-ypos+1, ILI9341_WHITE);
			}
			ypos_pv = ypos;

			ytracepos_pv = ekgTrace[xpos];
			ekgTrace[xpos] = ypos;

			if(bpm != 0xDEAD && bpm > 0)
			{
				update = gfx_DrawHeartBeatAnimate(bpm, 1 , ILI9341_WIDTH-img_heart_width-8, ILI9341_HEIGHT - img_heart_height);

				// So we don't have to constantly write the nunmber
				if (update)
				{
					sprintf(str_buffer, "%d", (int) bpm);
					if (bpm < 100)
					{
						xpos_text = ILI9341_WIDTH-(img_heart_width/2)-18;
					}
					else
					{
						xpos_text = ILI9341_WIDTH-(img_heart_width/2)-24;
					}

					ILI9341_WriteString(xpos_text, ILI9341_HEIGHT - (img_heart_height/2)-10, str_buffer, Font_11x18, ILI9341_WHITE, ILI9341_RED2);
				}
			}
			else
			{
				if (tick - tick_pv > 500)
				{
					update = gfx_DrawHeartBeatAnimate(bpm, 0 , ILI9341_WIDTH-img_heart_width-8, ILI9341_HEIGHT - img_heart_height);

					sprintf(str_buffer, "??");
					xpos_text = ILI9341_WIDTH-(img_heart_width/2)-24;
					ILI9341_WriteString(xpos_text, ILI9341_HEIGHT - (img_heart_height/2)-10, str_buffer, Font_11x18, ILI9341_WHITE, ILI9341_RED2);
					tick_pv = tick;
				}
			}
		}
		else
		{
			if(status_pv == ekgNoFail)
			{
				gfx_DrawHeartBeatAnimate(bpm, 0 , ILI9341_WIDTH-img_heart_width-8, ILI9341_HEIGHT - img_heart_height);

				sprintf(str_buffer, "Err");
				xpos_text = ILI9341_WIDTH-(img_heart_width/2)-24;
				ILI9341_WriteString(xpos_text, ILI9341_HEIGHT - (img_heart_height/2)-10, str_buffer, Font_11x18, ILI9341_WHITE, ILI9341_RED2);

				if(status == ekgPosLeadOff)
				{
					sprintf(str_buffer, "Error: Pos(+) Lead not detected");
				}
				else if (status == ekgNegLeadOff)
				{
					sprintf(str_buffer, "Error: Neg(-) Lead not detected");
				}
				ILI9341_WriteString(0, ILI9341_HEIGHT-20,str_buffer, Font_7x10, ILI9341_WHITE, ILI9341_BLACK);
				sprintf(str_buffer, "Check connection");
				ILI9341_WriteString(0, ILI9341_HEIGHT-10,str_buffer, Font_7x10, ILI9341_WHITE, ILI9341_BLACK);
			}

			ypos = ILI9341_HEIGHT/2;

			gfx_DrawGrid_VerticalLine(xpos, ytracepos_pv, ekgTrace[xpos]);

			if (xpos > 0)
			{
				ILI9341_DrawPixel(xpos-1, ypos, ILI9341_BLUE);
			}
			ILI9341_FillRectangle(xpos, ypos, 4, 1, ILI9341_WHITE);

			ypos_pv = ypos;

			ytracepos_pv = ekgTrace[xpos];
			ekgTrace[xpos] = ypos;
		}

		xpos = (xpos + 1) % ILI9341_WIDTH;
		status_pv = status;

		if(pause_screen == true)
		{
			if (xpos < ILI9341_WIDTH-1)
			{
				if(pause_pv == false)
				{
					sprintf(str_buffer, "Pausing Screen...");
					ILI9341_WriteString(0, ILI9341_HEIGHT-10,str_buffer, Font_7x10, ILI9341_WHITE, ILI9341_BLACK);
				}
			}
			else
			{
				sprintf(str_buffer, "Screen Paused    ");
				ILI9341_WriteString(0, ILI9341_HEIGHT-10,str_buffer, Font_7x10, ILI9341_WHITE, ILI9341_BLACK);
			}
		}
		else
		{
			if(pause_pv == true)
			{
				for (int x = 0; x < ILI9341_WIDTH/2; x++)
				{
					gfx_DrawGrid_VerticalLine(x, ILI9341_HEIGHT-10, ILI9341_HEIGHT);
				}
			}
		}
	}
}

void gfx_DrawEKGPageSmall(int16_t sample, uint32_t time, uint16_t bpm, uint16_t status)
{
	uint8_t update = 0;
	char str_buffer[40];

	static uint16_t ypos_pv;
	static uint16_t xpos = 0;
	static uint16_t yoffset = 0;
	int16_t ypos = 0;
	uint16_t xpos_text = 0;
	static uint16_t status_pv = 0;
	static uint32_t tick_pv = 0;
	uint32_t tick = HAL_GetTick();
	uint16_t xpos_scale = 0;
	static const uint16_t imgheartxpos = ILI9341_WIDTH-img_heart_width-8;

	// Must check for mode due to timing issues.
	// Ensures we don't double draw
	if(mode_tf == 1 && mode == EKGModeContSmall)
	{
		// Clear the screen and draw grid
		ILI9341_FillScreen(ILI9341_BLACK);

		for (int x = 0; x < ILI9341_WIDTH*4; x++)
		{
			if (x % 4 == 0)
			{
				if (x*4 % EKGFREQ == 0)
				{
					ILI9341_FillRectangle(x/4, 0, 1, ILI9341_HEIGHT, ILI9341_RED);
				}
			}
		}

		ILI9341_FillRectangle(0, ILI9341_HEIGHT/4, ILI9341_WIDTH, 1, ILI9341_RED);
		ILI9341_FillRectangle(0, ILI9341_HEIGHT*2/4, ILI9341_WIDTH, 1, ILI9341_RED);
		ILI9341_FillRectangle(0, ILI9341_HEIGHT*3/4, ILI9341_WIDTH, 1, ILI9341_RED);


		yoffset = 0;
		mode_tf = 0;
		xpos = 0;
	}

	xpos_scale = xpos/4;

	if ((pause_screen == false) ||
	  ((pause_screen == true) && (xpos_scale < ILI9341_WIDTH-1)))
	{
		if(xpos == 0)
		{
			yoffset = (yoffset+ ILI9341_HEIGHT/4);
			if (yoffset >= ILI9341_HEIGHT)
			{
				yoffset = 0;
			}
		}

		// Draw Grid
		if (xpos % 4 == 0)
		{
			if (xpos*4 % EKGFREQ == 0)
			{
				if ( yoffset == 120)
				{
					ILI9341_FillRectangle(xpos_scale, yoffset, 1, ILI9341_HEIGHT/4 - 10, ILI9341_RED);
				}
				else if (yoffset == 180)
				{
					if(xpos_scale < imgheartxpos)
					{
						// Only draw if no fail so we don't overwrite the error message
						if(status ==ekgNoFail)
						{
							ILI9341_FillRectangle(xpos_scale, yoffset, 1, ILI9341_HEIGHT/4, ILI9341_RED);
						}
						else
						{
							ILI9341_FillRectangle(xpos_scale, yoffset, 1, ILI9341_HEIGHT/4-20, ILI9341_RED);
						}
					}
				}
				else
				{
					ILI9341_FillRectangle(xpos_scale, yoffset, 1, ILI9341_HEIGHT/4, ILI9341_RED);
				}
			}
			else
			{
				if ( yoffset == 120)
				{
					ILI9341_FillRectangle(xpos_scale, yoffset, 1, ILI9341_HEIGHT/4 - 10, ILI9341_BLACK);
				}
				else if (yoffset == 180)
				{

					if(xpos_scale < imgheartxpos)
					{
						if (status == ekgNoFail)
						{
							ILI9341_FillRectangle(xpos_scale, yoffset, 1, ILI9341_HEIGHT/4, ILI9341_BLACK);
						}
						else
						{
							ILI9341_FillRectangle(xpos_scale, yoffset, 1, ILI9341_HEIGHT/4-20, ILI9341_BLACK);
						}
					}
				}
				else
				{
					ILI9341_FillRectangle(xpos_scale, yoffset, 1, ILI9341_HEIGHT/4, ILI9341_BLACK);
				}
			}
		}

		if (status == ekgNoFail)
		{
			if (status_pv != ekgNoFail)
			{
				for (int x = 0; x < ILI9341_WIDTH*3/4; x++)
				{
					gfx_DrawGrid_VerticalLine(x, ILI9341_HEIGHT-20, ILI9341_HEIGHT);
				}
			}
			// Scale everything down to make sure it is within the limits
			ypos = (ILI9341_HEIGHT/4)-(sample/50);


			if (yoffset != 180)
			{
				if (ypos >= ypos_pv)
				{
					ILI9341_FillRectangle(xpos_scale, ypos_pv+yoffset, 1, ypos-ypos_pv+1, ILI9341_WHITE);
				}
				else
				{
					ILI9341_FillRectangle(xpos_scale, ypos-yoffset, 1, ypos_pv-ypos+1, ILI9341_WHITE);
				}
			}
			else
			{
				if (xpos_scale < imgheartxpos)
				{
					if (ypos >= ypos_pv)
					{
						ILI9341_FillRectangle(xpos_scale, ypos_pv+yoffset, 1, ypos-ypos_pv+1, ILI9341_WHITE);
					}
					else
					{
						ILI9341_FillRectangle(xpos_scale, ypos-yoffset, 1, ypos_pv-ypos+1, ILI9341_WHITE);
					}
				}
			}

			ypos_pv = ypos;

			ILI9341_DrawPixel(0, yoffset, ILI9341_RED);

			if(bpm != 0xDEAD && bpm > 0)
			{
				update = gfx_DrawHeartBeatAnimate(bpm, 1 , ILI9341_WIDTH-img_heart_width-8, ILI9341_HEIGHT - img_heart_height);

				// So we don't have to constantly write the nunmber
				if (update)
				{
					sprintf(str_buffer, "%d", (int) bpm);
					if (bpm < 100)
					{
						xpos_text = ILI9341_WIDTH-(img_heart_width/2)-18;
					}
					else
					{
						xpos_text = ILI9341_WIDTH-(img_heart_width/2)-24;
					}

					ILI9341_WriteString(xpos_text, ILI9341_HEIGHT - (img_heart_height/2)-10, str_buffer, Font_11x18, ILI9341_WHITE, ILI9341_RED2);
				}
			}
			else
			{
				if (tick - tick_pv > 500)
				{
					update = gfx_DrawHeartBeatAnimate(bpm, 0 , ILI9341_WIDTH-img_heart_width-8, ILI9341_HEIGHT - img_heart_height);

					sprintf(str_buffer, "??");
					xpos_text = ILI9341_WIDTH-(img_heart_width/2)-24;
					ILI9341_WriteString(xpos_text, ILI9341_HEIGHT - (img_heart_height/2)-10, str_buffer, Font_11x18, ILI9341_WHITE, ILI9341_RED2);
					tick_pv = tick;
				}
			}
		}
		else
		{
			if(status_pv == ekgNoFail)
			{
				gfx_DrawHeartBeatAnimate(bpm, 0 , ILI9341_WIDTH-img_heart_width-8, ILI9341_HEIGHT - img_heart_height);

				sprintf(str_buffer, "Err");
				xpos_text = ILI9341_WIDTH-(img_heart_width/2)-24;
				ILI9341_WriteString(xpos_text, ILI9341_HEIGHT - (img_heart_height/2)-10, str_buffer, Font_11x18, ILI9341_WHITE, ILI9341_RED2);

				if(status == ekgPosLeadOff)
				{
					sprintf(str_buffer, "Error: Pos(+) Lead not detected");
				}
				else if (status == ekgNegLeadOff)
				{
					sprintf(str_buffer, "Error: Neg(-) Lead not detected");
				}
				ILI9341_WriteString(0, ILI9341_HEIGHT-20,str_buffer, Font_7x10, ILI9341_WHITE, ILI9341_BLACK);
				sprintf(str_buffer, "Check connection");
				ILI9341_WriteString(0, ILI9341_HEIGHT-10,str_buffer, Font_7x10, ILI9341_WHITE, ILI9341_BLACK);
			}


			if (yoffset != 180)
			{
				if (xpos_scale > 0)
				{
					ILI9341_DrawPixel(xpos_scale-1, yoffset + (ILI9341_HEIGHT/8), ILI9341_BLUE);
				}
				ILI9341_FillRectangle(xpos_scale, yoffset + (ILI9341_HEIGHT/8), 4, 1, ILI9341_WHITE);
			}
			else
			{
				if (xpos_scale < imgheartxpos)
				{
					if (xpos_scale > 0)
					{
						ILI9341_DrawPixel(xpos_scale-1, yoffset + (ILI9341_HEIGHT/8), ILI9341_BLUE);
					}
					ILI9341_FillRectangle(xpos_scale, yoffset + (ILI9341_HEIGHT/8), 4, 1, ILI9341_WHITE);
				}
			}
		}

		xpos = (xpos + 1) % (ILI9341_WIDTH*4);
		status_pv = status;

		if(pause_screen == true)
		{

			if (xpos_scale+1 < ILI9341_WIDTH-1)
			{
				if(pause_pv == false)
				{
					sprintf(str_buffer, "Pausing Screen...");
					ILI9341_WriteString(0, ILI9341_HEIGHT-10,str_buffer, Font_7x10, ILI9341_WHITE, ILI9341_BLACK);
				}
			}
			else
			{
				sprintf(str_buffer, "Screen Paused    ");
				ILI9341_WriteString(0, ILI9341_HEIGHT-10,str_buffer, Font_7x10, ILI9341_WHITE, ILI9341_BLACK);
			}
		}
		else
		{
			if(pause_pv == true)
			{
				for (int x = 0; x < ILI9341_WIDTH/2; x++)
				{
					if(x % 4 == 0)
					{
						if (xpos*4 % EKGFREQ == 0)
						{
							ILI9341_FillRectangle(x, ILI9341_HEIGHT-10, 1, ILI9341_HEIGHT, ILI9341_RED);
						}
						else
						{
							ILI9341_FillRectangle(x, ILI9341_HEIGHT-10, 1, ILI9341_HEIGHT, ILI9341_BLACK);
						}
					}
				}
			}
		}
	}
}

void gfx_CheckPauseState(void)
{
	if(pause_pv == true && pause_screen == false)
	{
		pause_pv = false;
	}
	else
	{
		if(pause_pv == false && pause_screen == true)
		{
			pause_pv = true;
		}
	}
}
