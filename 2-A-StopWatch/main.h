/*
 * main.h
 *
 *  Created on: Mar 19, 2021
 *      Author: 		James
 *      SID: 			201198933
 *      Affiliation: 	University of Leeds
 *
 * DESCRIPTION:
 * This file includes all the functions
 * required to operate a timer on the
 * DE1-SoC board. Drivers are included
 * from the UoL ELEC5620 Resources Repository.
 *
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "HPS_Watchdog/HPS_Watchdog.h"
#include "HPS_PrivateTimer/HPS_PrivateTimer.h"
#include "DE1SoC_SevenSeg/DE1SoC_SevenSeg.h"
#include "DE1SoC_LT24/DE1SoC_LT24.h"
#include "BasicFont/BasicFont.h"

// ADDRESS DEFINITIONS
volatile unsigned int *key_ptr = (unsigned int *) 0xFF200050;  // key buttons base address
volatile unsigned int *LED_ptr = (unsigned int *) 0xFF200000;  // LEDs base address

// CONSTANT DEFINITIONS
const unsigned int scaler = 200 - 1;
const unsigned int period = 225000000/(scaler+1); 			   // A9 Private timer freq. = 225MHz

const unsigned int 	TIMER_SIZE 	= 4;						   // hundredths, seconds, minutes, hours
const unsigned int 	LED_MAX 	= 1024;                        // 2^10 as there are 10 LEDs

// TYPE DEFINITIONS
typedef void (*TaskFunction)(unsigned int*);


/*
 *	INIT LCD & TIMER FUNCTIONS
 */

// Initialises A9 Private Timer
// Loads in max timer value, sets control bits
void init_timer()
{
	Timer_initialise(0xFFFEC600);
	Timer_setLoad(0xFFFFFFFF);
	Timer_setControl(scaler, 0, 1, 1);
}

// Sends command to initialise LCD
void init_lcd()
{
	LT24_initialise(0xFF200060,0xFF200080);
	HPS_ResetWatchdog();
}


/*
 * 	LCD FUNCTIONS
 */

// Clears the display, writes starting timer structure
// i.e. '00:00:00.00'
void reset_lcd()
{
	// clear display
	LT24_clearDisplay(LT24_BLACK);

	LT24_drawCharDoubleDec(0, LT24_WHITE, LT24_BLACK, 20 , 20, 5, 8, 3);
	LT24_drawCharDoubleDec(0, LT24_WHITE, LT24_BLACK, 80 , 20, 5, 8, 3);
	LT24_drawCharDoubleDec(0, LT24_WHITE, LT24_BLACK, 140 , 20, 5, 8, 3);
	LT24_drawCharDoubleDec(0, LT24_WHITE, LT24_BLACK, 200 , 25, 5, 8, 2);

	LT24_drawChar(BF_fontMap[26], LT24_WHITE, LT24_BLACK, 65, 20, 5, 8, 3);
	LT24_drawChar(BF_fontMap[26], LT24_WHITE, LT24_BLACK, 125, 20, 5, 8, 3);
	LT24_drawChar(BF_fontMap[14], LT24_WHITE, LT24_BLACK, 185, 25, 5, 8, 2);
}

// Used for one time write of instantaneous timeValues when split button pressed.
// Split display location determined my splitNum value
void draw_split(unsigned int timeValues[], int x, int y, int scale, int splitNum)
{
	// draw split number
	LT24_drawCharDoubleDec(splitNum, LT24_WHITE, LT24_BLACK, x-2*(5*scale)-30, y, 5, 8, scale);
	LT24_drawChar(BF_fontMap[29], LT24_WHITE, LT24_BLACK, x-(5*scale) -10, y, 5, 8, scale);

	// draw timer values
	LT24_drawCharDoubleDec(timeValues[3], LT24_WHITE, LT24_BLACK, x , y, 5, 8, scale);
	LT24_drawCharDoubleDec(timeValues[2], LT24_WHITE, LT24_BLACK, x +2*(2*5*scale), y, 5, 8, scale);
	LT24_drawCharDoubleDec(timeValues[1], LT24_WHITE, LT24_BLACK, x +4*(2*5*scale), y, 5, 8, scale);
	LT24_drawCharDoubleDec(timeValues[0], LT24_WHITE, LT24_BLACK, x +6*(2*5*scale), (y+8*scale/2), 5, 8, scale/2);

	// timer timer markings ':' and '.'
	LT24_drawChar(BF_fontMap[26], LT24_WHITE, LT24_BLACK, x +1.5*(2*5*scale), y, 5, 8, scale);
	LT24_drawChar(BF_fontMap[26], LT24_WHITE, LT24_BLACK, x +3.5*(2*5*scale), y, 5, 8, scale);
	LT24_drawChar(BF_fontMap[14], LT24_WHITE, LT24_BLACK, x +5.5*(2*5*scale), (y+8*scale/2), 5, 8, scale/2);
}


/*
 *	INTRO FUNCTION
 */

// Function to introduce the timer on the Seven Segment displays
void intro()
{
	unsigned int lastIncrTime [3] = {0};
	unsigned int introPeriods [32] = {period/5, period/10};
	unsigned int y[12] = { 106,116,126,136,146,156,166,156,146,136,126,116};
	unsigned int msg[27] = {0xF,0xF,0x9,0xA,0xF,0xB,0xC,0xD,0xE,0x6,0x8,0xF,0xF,0xF,0x0,0x1,0x2,0xF,0x3,0x4,0xF,0x5,0x6,0x6,0x7,0x8,0xF};
	unsigned short colourMap[32] = {0x0008,0x012C,0x0271,0x0396,0x04DB,0x061F,0x157C,0x2CFA,
									0x4457,0x5BD4,0x7351,0x8AAE,0xA22B,0xB9A8,0xD105,0xE082,
									0xF800,0xF8C0,0xF980,0xFA40,0xFB00,0xFBC0,0xFC80,0xFD40,
									0xFE00,0xDD41,0xBC82,0x9BC3,0x7B04,0x5A45,0x3986,0x18C7};

	int n=1; int i=0; int j; int k=0; int x=6;

	Timer_setLoad(0xFFFFFFFF);
	LT24_clearDisplay(LT24_BLACK);

	// if function called on recursion, catch the key press
	while (*key_ptr & 0x1) {HPS_ResetWatchdog();};

	while (!(*key_ptr & 0x1)) {

		if ((lastIncrTime[0] - Timer_readValue()) >= introPeriods[0]) {
			*LED_ptr = (n | 512/n);

			n = (n*2)%511;
			i++;

			lastIncrTime[0] = lastIncrTime[0] - introPeriods[0];
		}

		if ((lastIncrTime[1] - Timer_readValue()) >= introPeriods[1]){

			LT24_drawVertMovingChar(BF_fontMap[52], colourMap[k%32], LT24_BLACK, x, y[k%12], 5, 8, 6, 10);  // T
			LT24_drawVertMovingChar(BF_fontMap[41], colourMap[k%32], LT24_BLACK, x + 48, y[(k+1)%12], 5, 8, 6, 10);  // I
			LT24_drawVertMovingChar(BF_fontMap[45], colourMap[k%32], LT24_BLACK, x + 2*48, y[(k+2)%12], 5, 8, 6, 10);  // M
			LT24_drawVertMovingChar(BF_fontMap[37], colourMap[k%32], LT24_BLACK, x + 3*48, y[(k+3)%12], 5, 8, 6, 10);  // E
			LT24_drawVertMovingChar(BF_fontMap[50], colourMap[k%32], LT24_BLACK, x + 4*48, y[(k+4)%12], 5, 8, 6, 10);  // R

			k++;

			lastIncrTime[1] = lastIncrTime[1] - introPeriods[1];
		}

		for (j = 0; j<6; j++){
			DE1SoC_SevenSeg_SetSingleLetter(5-j,msg[(i+j)%27]);
		}

		HPS_ResetWatchdog();
	};

	*LED_ptr = 0;

	while (*key_ptr & 0x1) {HPS_ResetWatchdog();};
}


/*
 * BUTTON FUNCTIONS
 */

// Function to pause the timer on button press
void pause()
{
	// disable the timer by setting enable bit to 0
	Timer_setControl(scaler, 0, 1, 0);

	// poll key presses until the pause key is pressed again
	while (*key_ptr & 0x4) {HPS_ResetWatchdog();};
	while (!(*key_ptr & 0x4)){HPS_ResetWatchdog();};
	while (*key_ptr & 0x4) {HPS_ResetWatchdog();};

	// re-enable timer
	Timer_setControl(scaler, 0, 1, 1);
}

// Function to print split value to LCD on button press
void split(unsigned int timeValues[], int *splitNum)
{
	signed char	clear [1] 	= {0x1};					   // used to clear LCD splits
	// if spits extend of screen, clear the splits and start again at the top
	if ((*splitNum % 10) == 0) { LT24_drawChar(clear, LT24_BLACK,  LT24_BLACK, 0 , 60, 1, 1, 260); }

	// draw the splits at the specified location
	draw_split(timeValues, 80, 60+25*(*splitNum % 10), 2, *splitNum + 1);

	// increment splitNum address value
	*splitNum = *splitNum + 1;

	// poll key press, wait for it to be released
	while (*key_ptr & 0x2) {HPS_ResetWatchdog();};
}

// Function to toggle hour mode
void mode_toggle(bool* mode)
{
	while (*key_ptr & 0x8) {HPS_ResetWatchdog();};
	*mode = !(*mode);
}


/*
 * 	TIMER INCREMENT FUNCTIONS
 */

// Increment hundredths timer value, display values
void hundredths(unsigned int* timeValue)
{
	*timeValue = (*timeValue +1)% 100;

	// update LCD display with current time value
	LT24_drawCharDoubleDec(*timeValue, LT24_WHITE, LT24_BLACK, 200 , 25, 5, 8, 2);
}

// Increment seconds timer value, display values
void seconds(unsigned int* timeValue)
{
	*timeValue = (*timeValue +1)% 60;

	// update LCD display with current time value
	LT24_drawCharDoubleDec(*timeValue, LT24_WHITE, LT24_BLACK, 140 , 20, 5, 8, 3);
}

// Increment minutes timer value, display values
void minutes(unsigned int* timeValue)
{
	*timeValue = (*timeValue +1)% 60;

	// update LCD display with current time value
	LT24_drawCharDoubleDec(*timeValue, LT24_WHITE, LT24_BLACK, 80 , 20, 5, 8, 3);
}

// Increment hours timer value, display values
void hours(unsigned int* timeValue)
{
	*timeValue = (*timeValue +1)% 24;

	// update LCD display with current time value
	LT24_drawCharDoubleDec(*timeValue, LT24_WHITE, LT24_BLACK, 20 , 20, 5, 8, 3);
}


/*
 * 	MAIN TIMER FUNCTION
 */

void timer()
{
	unsigned int lastIncrTime[TIMER_SIZE] = {0}; 										   	// all timers start incrementing immediately
	unsigned int timeValues[TIMER_SIZE] = {0}; 											   	// all time values initialised to 0
	const unsigned int incrPeriod[TIMER_SIZE] = {period/100,period,period*60,period*3600}; 	// set the increment period for all timer units
	TaskFunction taskFunctions[TIMER_SIZE] = {&hundredths,&seconds,&minutes,&hours};		// define task function struct to call increment functions when required

	bool mode = false; int splitNum = 0; int i;

	intro();

	reset_lcd();

	Timer_setLoad(0xFFFFFFFF);  // reset timer before main loop

	/* Main Run Loop */
	while(1) {
		// poll key 1
		if (*key_ptr & 0x1) { timer(); }  							// use recursion to restart timer

		// poll key 2
		if (*key_ptr & 0x2) { split(timeValues, &splitNum); }

		// poll key 3
		if (*key_ptr & 0x4) { pause(); }

		// poll key 4
		if ((timeValues[3] >= 1)) { mode = true; }					// if it has been more than 1 hour, force hour mode,
		else if (*key_ptr & 0x8) { mode_toggle(&mode); }			// else toggle hour mode

		// if elapsed time is greater than any specified unit period,
		// increment the associated timeValue with task scheduler
		for (i = 0; i < TIMER_SIZE; i++) {
			if ((lastIncrTime[i] - Timer_readValue()) >= incrPeriod[i]) {
				taskFunctions[i](&timeValues[i]);
				lastIncrTime[i] = lastIncrTime[i] - incrPeriod[i];
			}
			// LEDs fill up every second then reset back to 0
			*LED_ptr =  ~((signed int) -1 << (timeValues[0]/10)+1);
		}

		// update 7Segment display with new time values
		DE1SoC_SevenSeg_SetDoubleDec(0,timeValues[0+mode]);
		DE1SoC_SevenSeg_SetDoubleDec(2,timeValues[1+mode]);
		DE1SoC_SevenSeg_SetDoubleDec(4,timeValues[2+mode]);

		// check timer interrupt flag, reset if required
		Timer_clearInterrupt();
		// reset system watchdog
		HPS_ResetWatchdog();
	}
}

#endif /* MAIN_H_ */
