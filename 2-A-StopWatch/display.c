/*
 * display.c
 *
 *  Created on: 25 Mar 2021
 *      Author: James
 */

#include "display.h"

// Sends command to initialise LCD
void LCD_init()
{
	LT24_initialise(0xFF200060,0xFF200080);
}

void LCD_reset()
{
	LT24_clearDisplay(LT24_BLACK);
}

// Clears the LCD display, writes starting timer structure
// i.e. '00:00:00.00'
void LCD_preset()
{
	// clear display
	LCD_reset();

	LT24_drawDoubleDec(0, LT24_WHITE, LT24_BLACK, 20 , 20, 5, 8, 3);
	LT24_drawDoubleDec(0, LT24_WHITE, LT24_BLACK, 80 , 20, 5, 8, 3);
	LT24_drawDoubleDec(0, LT24_WHITE, LT24_BLACK, 140 , 20, 5, 8, 3);
	LT24_drawDoubleDec(0, LT24_WHITE, LT24_BLACK, 200 , 25, 5, 8, 2);

	LT24_drawChar(BF_fontMap[26], LT24_WHITE, LT24_BLACK, 65, 20, 5, 8, 3);
	LT24_drawChar(BF_fontMap[26], LT24_WHITE, LT24_BLACK, 125, 20, 5, 8, 3);
	LT24_drawChar(BF_fontMap[14], LT24_WHITE, LT24_BLACK, 185, 25, 5, 8, 2);
}

// Used for one time write of instantaneous timeValues when split button pressed.
// Split display location determined my splitNum value
void LCD_draw_split(unsigned int timeValues[], int x, int y, int scale, int splitNum)
{
	// draw split number
	LT24_drawDoubleDec(splitNum, LT24_WHITE, LT24_BLACK, x-2*(5*scale)-30, y, 5, 8, scale);
	LT24_drawChar(BF_fontMap[29], LT24_WHITE, LT24_BLACK, x-(5*scale) -10, y, 5, 8, scale);

	// draw timer values
	LT24_drawDoubleDec(timeValues[3], LT24_WHITE, LT24_BLACK, x , y, 5, 8, scale);
	LT24_drawDoubleDec(timeValues[2], LT24_WHITE, LT24_BLACK, x +2*(2*5*scale), y, 5, 8, scale);
	LT24_drawDoubleDec(timeValues[1], LT24_WHITE, LT24_BLACK, x +4*(2*5*scale), y, 5, 8, scale);
	LT24_drawDoubleDec(timeValues[0], LT24_WHITE, LT24_BLACK, x +6*(2*5*scale), (y+8*scale/2), 5, 8, scale/2);

	// timer timer markings ':' and '.'
	LT24_drawChar(BF_fontMap[26], LT24_WHITE, LT24_BLACK, x +1.5*(2*5*scale), y, 5, 8, scale);
	LT24_drawChar(BF_fontMap[26], LT24_WHITE, LT24_BLACK, x +3.5*(2*5*scale), y, 5, 8, scale);
	LT24_drawChar(BF_fontMap[14], LT24_WHITE, LT24_BLACK, x +5.5*(2*5*scale), (y+8*scale/2), 5, 8, scale/2);
}

void LCD_draw_moving_msg(int x, int j, int k)
{
	// draw split number
	LT24_drawVertMovingChar(LCD_msg[0], colourMap[k%32], LT24_BLACK, x, y[j%12], 5, 8, 6, 10);			// T
	LT24_drawVertMovingChar(LCD_msg[1], colourMap[(k+1)%32], LT24_BLACK, x + 48, y[(j+1)%12], 5, 8, 6, 10);		// I
	LT24_drawVertMovingChar(LCD_msg[2], colourMap[(k+2)%32], LT24_BLACK, x + 2*48, y[(j+2)%12], 5, 8, 6, 10);	// M
	LT24_drawVertMovingChar(LCD_msg[3], colourMap[(k+3)%32], LT24_BLACK, x + 3*48, y[(j+3)%12], 5, 8, 6, 10);	// E
	LT24_drawVertMovingChar(LCD_msg[4], colourMap[(k+4)%32], LT24_BLACK, x + 4*48, y[(j+4)%12], 5, 8, 6, 10);	// R
}

void LCD_clear(int y)
{
	LT24_drawSquare(LT24_BLACK, 0, y, 240, 320-y);
}

void LCD_draw_unit(int timeValue, int x, int y, int scale)
{
	LT24_drawDoubleDec(timeValue, LT24_WHITE, LT24_BLACK, x, y, 5, 8, scale);
}

// Sets all seven segments simultaneously
void SevenSeg_set(unsigned int timeValues[], bool mode)
{
	DE1SoC_SevenSeg_SetDoubleDec(0, timeValues[0 + mode]);
	DE1SoC_SevenSeg_SetDoubleDec(2, timeValues[1 + mode]);
	DE1SoC_SevenSeg_SetDoubleDec(4, timeValues[2 + mode]);
}

void SevenSeg_display_msg(int i)
{
	int j;

	for (j = 0; j < 6; j++){
		DE1SoC_SevenSeg_SetSingleLetter(5-j,SevenSeg_msg[(i+j)%27]);
	}
}
