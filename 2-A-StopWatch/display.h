/*
 * display.h
 *
 *  Created on: 25 Mar 2021
 *      Author: James
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_


#include "DE1SoC_LT24/DE1SoC_LT24.h"
#include "BasicFont/BasicFont.h"
#include "DE1SoC_SevenSeg/DE1SoC_SevenSeg.h"

unsigned int SevenSeg_msg[27] = {0xF,0xF,0x9,0xA,0xF,0xB,0xC,0xD,0xE,0x6,0x8,0xF,0xF,0xF,0x0,0x1,0x2,0xF,0x3,0x4,0xF,0x5,0x6,0x6,0x7,0x8,0xF};
signed char* LCD_msg[5]= {BF_fontMap[52], BF_fontMap[41], BF_fontMap[45], BF_fontMap[37], BF_fontMap[50]};
unsigned int y[12] = {106,116,126,136,146,156,166,156,146,136,126,116};

// colour map imported from '3-1-Mandelbrot' code, supplied by UoL
unsigned short colourMap[32] = {0x0008,0x012C,0x0271,0x0396,0x04DB,0x061F,0x157C,0x2CFA,
				0x4457,0x5BD4,0x7351,0x8AAE,0xA22B,0xB9A8,0xD105,0xE082,
				0xF800,0xF8C0,0xF980,0xFA40,0xFB00,0xFBC0,0xFC80,0xFD40,
				0xFE00,0xDD41,0xBC82,0x9BC3,0x7B04,0x5A45,0x3986,0x18C7};

/*
 *	LCD FUNCTIONS
 */

void LCD_reset( void );

// Clears the display, writes starting timer structure
// i.e. '00:00:00.00'
void LCD_preset( void );

// Used for one time write of instantaneous timeValues when split button pressed.
// Split display location determined my splitNum value
void LCD_draw_split(unsigned int timeValues[], int x, int y, int scale, int splitNum);

void LCD_draw_moving_msg(int x, int j, int k);

void LCD_clear(int y);

void LCD_draw_unit(int timeValue, int x, int y, int scale);

/*
 *	SEVEN-SEGEMNT FUNCTIONS
 */

// used to set all the seven segment displays on reset or mode toggle
void SevenSeg_set(unsigned int timeValues[], bool mode);

void SevenSeg_display_unit(int display, unsigned int timeValue);

void SevenSeg_display_character(int display, unsigned int character);

#endif /* DISPLAY_H_ */
