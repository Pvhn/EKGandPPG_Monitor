/*
 * UserInterface.h
 *
 *  Created on: May 6, 2025
 *      Author: peter
 */

#ifndef INC_USERINTERFACE_H_
#define INC_USERINTERFACE_H_

#include "max030201.h"
#include "ppgSensor.h"
#include "ekgSensor.h"
#include "ili9341.h"
#include  <stdbool.h>
#include <stdio.h>

#define NUMOFMODES 3

typedef enum
{
	PPGMode,
	EKGModeContLarge,
	EKGModeContSmall,
	EKGModeSingleLarge,
	EKGModeSingleSmall,
}MonitorModeEnumType;

typedef enum
{
	EKGLargePage,
	EKGLargePageHB,
	EKGSmallPage,
	EKGSmallPageHB,
}EkgPageEnumType;

extern volatile uint8_t mode_tf;
extern volatile uint8_t mode;
extern volatile uint8_t pause_screen;


extern void gfx_DrawPPGPage(void);
extern void gfx_DrawEKGPageLarge(int16_t sample, uint32_t time, uint16_t bpm, uint16_t status);
extern void gfx_DrawEKGPageSmall(int16_t sample, uint32_t time, uint16_t bpm, uint16_t status);
extern void gfx_CheckPauseState(void);

#endif /* INC_USERINTERFACE_H_ */
