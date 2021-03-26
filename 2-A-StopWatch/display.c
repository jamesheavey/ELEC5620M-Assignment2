/*
 * display.c
 *
 *  Created on: 25 Mar 2021
 *      Author: James
 */

#include "display.h"

// Wrapper for driver function to send initialisation command to LCD
void LCD_init()
{
	LT24_initialise(0xFF200060,0xFF200080);
}

// Wrapper for driver function to clear LCD display
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

// Function to display the split time at a given location
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

// Function to draw vertically oscillating, colour changing message on the LCD.
void LCD_draw_moving_msg(int x, int y, int colour)
{
	// draw split number
	LT24_drawVertMovingChar(LCD_msg[0], colourMap[colour%32], LT24_BLACK, x, y_range[y%12], 5, 8, 6, 10);			// T
	LT24_drawVertMovingChar(LCD_msg[1], colourMap[(colour+1)%32], LT24_BLACK, x + 48, y_range[(y+1)%12], 5, 8, 6, 10);		// I
	LT24_drawVertMovingChar(LCD_msg[2], colourMap[(colour+2)%32], LT24_BLACK, x + 2*48, y_range[(y+2)%12], 5, 8, 6, 10);	// M
	LT24_drawVertMovingChar(LCD_msg[3], colourMap[(colour+3)%32], LT24_BLACK, x + 3*48, y_range[(y+3)%12], 5, 8, 6, 10);	// E
	LT24_drawVertMovingChar(LCD_msg[4], colourMap[(colour+4)%32], LT24_BLACK, x + 4*48, y_range[(y+4)%12], 5, 8, 6, 10);	// R
}

// Function to update individual timer units at specified location.
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

// Function to display a rotating message on the Seven segment displays
void SevenSeg_display_msg(int i)
{
	int j;

	for (j = 0; j < 6; j++){
		DE1SoC_SevenSeg_SetSingleLetter(5-j,SevenSeg_msg[(i+j)%27]);
	}
}
