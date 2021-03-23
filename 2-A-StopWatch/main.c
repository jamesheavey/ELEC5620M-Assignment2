/*
 * main.c
 *
 *  Created on: Mar 17, 2021
 *      Author:		James Heavey
 *      SID:			201198933
 *      Affiliation:	University of Leeds
 *
 * DESCRIPTION:
 * This code implements a functional a timer, operating on the
 * DE1-SoC development board. The A9 private timer is used to measure
 * elapsed time and trigger unit increments that are then displayed both
 * on the seven segment displays and the LT24 LCD. Push buttons can
 * be used to pause, split, reset and change from 'MM:SS:FF' mode to 'HH:MM:SS' mode.
 *
 */

#include "main.h"
#include "time.h"
#include "stdio.h"

// Initialises A9 Private Timer
// Loads in max timer value, sets control bits
void init_timer()
{
	Timer_initialise(0xFFFEC600);
	Timer_setLoad(0xFFFFFFFF);
	Timer_setControl(SCALER, 0, 1, 1);
}

// Sends command to initialise LCD
void init_lcd()
{
	LT24_initialise(0xFF200060,0xFF200080);
	HPS_ResetWatchdog();
}


// Clears the display, writes starting timer structure
// i.e. '00:00:00.00'
void reset_lcd()
{
	// clear display
	LT24_clearDisplay(LT24_BLACK);

	LT24_drawDoubleDec(0, LT24_WHITE, LT24_BLACK, 20 , 20, 5, 8, 3);
	LT24_drawDoubleDec(0, LT24_WHITE, LT24_BLACK, 80 , 20, 5, 8, 3);
	LT24_drawDoubleDec(0, LT24_WHITE, LT24_BLACK, 140 , 20, 5, 8, 3);
	LT24_drawDoubleDec(0, LT24_WHITE, LT24_BLACK, 200 , 25, 5, 8, 2);

	LT24_drawChar(BF_fontMap[26], LT24_WHITE, LT24_BLACK, 65, 20, 5, 8, 3);
	LT24_drawChar(BF_fontMap[26], LT24_WHITE, LT24_BLACK, 125, 20, 5, 8, 3);
	LT24_drawChar(BF_fontMap[14], LT24_WHITE, LT24_BLACK, 185, 25, 5, 8, 2);
}


// Sets all seven segments simultaneously
void set_7seg(unsigned int timeValues[], bool mode)
{
	DE1SoC_SevenSeg_SetDoubleDec(0, timeValues[0 + mode]);
	DE1SoC_SevenSeg_SetDoubleDec(2, timeValues[1 + mode]);
	DE1SoC_SevenSeg_SetDoubleDec(4, timeValues[2 + mode]);
}


// Used for one time write of instantaneous timeValues when split button pressed.
// Split display location determined my splitNum value
void draw_split(unsigned int timeValues[], int x, int y, int scale, int splitNum)
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


// Function to pause the timer on button press
void pause()
{
	// disable the timer by setting enable bit to 0
	Timer_setControl(SCALER, 0, 1, 0);

	// clear edge capture flags
	*key_ptr = 0xF;

	// poll key presses until the pause key is pressed again
	while (!(*key_ptr & 0x4)){HPS_ResetWatchdog();}

	// clear edge capture flags
	*key_ptr = 0xF;

	// re-enable timer
	Timer_setControl(SCALER, 0, 1, 1);
}


// Function to print split value to LCD on button press
void split(unsigned int timeValues[], int *splitNum)
{
	signed char	clear [1] 	= {0x1};					   // used to clear LCD splits
	// if spits extend off screen, clear the splits and start again at the top
	if ((*splitNum % 10) == 0) { LT24_drawChar(clear, LT24_BLACK,  LT24_BLACK, 0, 60, 1, 1, 260); }

	// draw the splits at the specified location
	draw_split(timeValues, 80, 60+25*(*splitNum % 10), 2, *splitNum + 1);

	// increment splitNum address value
	*splitNum += 1;

	// clear edge capture flags
	*key_ptr = 0xF;
}


// Function to toggle hour mode
void mode_toggle(bool* mode)
{
	*mode = !(*mode);
	// clear edge capture flags
	*key_ptr = 0xF;
}


// Increment hundredths timer value, display values
void hundredths(unsigned int* timeValue, bool mode)
{
	*timeValue = (*timeValue +1)% 100;

	// update 7Segment display with current time values
	DE1SoC_SevenSeg_SetDoubleDec(0 - 2*mode, *timeValue);

	// update LCD display with current time value
	LT24_drawDoubleDec(*timeValue, LT24_WHITE, LT24_BLACK, 200, 25, 5, 8, 2);
}


// Increment seconds timer value, display values
void seconds(unsigned int* timeValue, bool mode)
{
	*timeValue = (*timeValue +1)% 60;

	// update 7Segment display with current time values
	DE1SoC_SevenSeg_SetDoubleDec(2 - 2*mode, *timeValue);

	// update LCD display with current time value
	LT24_drawDoubleDec(*timeValue, LT24_WHITE, LT24_BLACK, 140, 20, 5, 8, 3);
}


// Increment minutes timer value, display values
void minutes(unsigned int* timeValue, bool mode)
{
	*timeValue = (*timeValue +1)% 60;

	// update 7Segment display with current time values
	DE1SoC_SevenSeg_SetDoubleDec(4 - 2*mode, *timeValue);

	// update LCD display with current time value
	LT24_drawDoubleDec(*timeValue, LT24_WHITE, LT24_BLACK, 80, 20, 5, 8, 3);
}


// Increment hours timer value, display values
void hours(unsigned int* timeValue, bool mode)
{
	*timeValue = (*timeValue +1)% 24;

	// update 7Segment display with current time values
	DE1SoC_SevenSeg_SetDoubleDec(6 - 2*mode, *timeValue);

	// update LCD display with current time value
	LT24_drawDoubleDec(*timeValue, LT24_WHITE, LT24_BLACK, 20, 20, 5, 8, 3);
}


// Function to introduce the timer on the Seven Segment displays
void intro()
{
	unsigned int lastIncrTime [3] = {0};
	unsigned int introPeriods [3] = {PERIOD/5, PERIOD/20, PERIOD/50};
	unsigned int y[12] = { 106,116,126,136,146,156,166,156,146,136,126,116};
	unsigned int msg[27] = {0xF,0xF,0x9,0xA,0xF,0xB,0xC,0xD,0xE,0x6,0x8,0xF,0xF,0xF,0x0,0x1,0x2,0xF,0x3,0x4,0xF,0x5,0x6,0x6,0x7,0x8,0xF};

	// colour map imported from '3-1-Mandelbrot' code, supplied by UoL
	unsigned short colourMap[32] = {0x0008,0x012C,0x0271,0x0396,0x04DB,0x061F,0x157C,0x2CFA,
									0x4457,0x5BD4,0x7351,0x8AAE,0xA22B,0xB9A8,0xD105,0xE082,
									0xF800,0xF8C0,0xF980,0xFA40,0xFB00,0xFBC0,0xFC80,0xFD40,
									0xFE00,0xDD41,0xBC82,0x9BC3,0x7B04,0x5A45,0x3986,0x18C7};

	int n=1; int i=0; int j; int k=0; int x=6; int z=0;

	// clear edge capture flags, on recursion reset
	*key_ptr = 0xF;

	Timer_setLoad(0xFFFFFFFF);
	LT24_clearDisplay(LT24_BLACK);

	while (!(*key_ptr & 0x1)) {

		// LED and seven seg update
		if ((lastIncrTime[0] - Timer_readValue()) >= introPeriods[0]) {
			*LED_ptr = (n | 512/n);

			n = (n*2)%511;
			i++;

			lastIncrTime[0] -= introPeriods[0];
		}

		// LCD position update
		if ((lastIncrTime[1] - Timer_readValue()) >= introPeriods[1]){
			k++;
			lastIncrTime[1] -= introPeriods[1];
		}

		// LCD colour update and print
		if ((lastIncrTime[2] - Timer_readValue()) >= introPeriods[2]){
			LT24_drawVertMovingChar(BF_fontMap[52], colourMap[z%32], LT24_BLACK, x, y[k%12], 5, 8, 6, 10);  // T
			LT24_drawVertMovingChar(BF_fontMap[41], colourMap[z%32], LT24_BLACK, x + 48, y[(k+1)%12], 5, 8, 6, 10);  // I
			LT24_drawVertMovingChar(BF_fontMap[45], colourMap[z%32], LT24_BLACK, x + 2*48, y[(k+2)%12], 5, 8, 6, 10);  // M
			LT24_drawVertMovingChar(BF_fontMap[37], colourMap[z%32], LT24_BLACK, x + 3*48, y[(k+3)%12], 5, 8, 6, 10);  // E
			LT24_drawVertMovingChar(BF_fontMap[50], colourMap[z%32], LT24_BLACK, x + 4*48, y[(k+4)%12], 5, 8, 6, 10);  // R

			z++;

			lastIncrTime[2] -= introPeriods[2];
		}

		for (j = 0; j < 6; j++){
			DE1SoC_SevenSeg_SetSingleLetter(5-j,msg[(i+j)%27]);
		}

		HPS_ResetWatchdog();
	};

	// clear LEDs
	*LED_ptr = 0;
	// clear edge capture flags
	*key_ptr = 0xF;
	// enter timer loop
	stopwatch();
}


void stopwatch()
{
	unsigned int lastIncrTime[TIMER_SIZE] = {0}; 										   	// all timers start incrementing immediately
	unsigned int timeValues[TIMER_SIZE] = {0}; 											   	// all time values initialised to 0
	const unsigned int incrPeriod[TIMER_SIZE] = {PERIOD/100,PERIOD,PERIOD*60,PERIOD*3600}; 	// set the increment period for all timer units
	TaskFunction taskFunctions[TIMER_SIZE] = {&hundredths,&seconds,&minutes,&hours};		// define task function struct to call increment functions when required

	bool mode = false; int splitNum = 0; int i;

//	// TESTING
//	double t_elapsed;
//	clock_t test_timer;

	reset_lcd();

	set_7seg(timeValues, mode); // Initialise to '00 00 00'

	Timer_setLoad(0xFFFFFFFF);  // reset timer before main loop

//	test_timer = clock();    // Begin test clock

	/* Main Run Loop */
	while(1) {
		// poll key 1
		if (*key_ptr & 0x1) { intro(); }  	// use recursion to restart timer

		// poll key 2
		if (*key_ptr & 0x2) { split(timeValues, &splitNum); }

		// poll key 3
		if (*key_ptr & 0x4) { pause(); }

		// poll key 4
		if (timeValues[3] >= 1) { mode = true; set_7seg(timeValues, mode); }					// if it has been more than 1 hour, force hour mode,
		else if (*key_ptr & 0x8) { mode_toggle(&mode); set_7seg(timeValues, mode); }			// else toggle hour mode

		// if elapsed time is greater than any specified unit period,
		// increment the associated timeValue with task scheduler
		for (i = 0; i < TIMER_SIZE; i++) {
			if ((lastIncrTime[i] - Timer_readValue()) >= incrPeriod[i]) {
				taskFunctions[i](&timeValues[i], mode);
				lastIncrTime[i] -= incrPeriod[i];

//				// TESTING
//				if (i == 2) {
//					t_elapsed = ((double)(clock() - test_timer)/CLOCKS_PER_SEC);
//					printf("%s: Private timer value = %d. Time.h value = %f. \n", "MINUTES",timeValues[i]*60,t_elapsed);
//				}
			}
		}

		// LEDs fill up every second then reset back to 0
		*LED_ptr =  ~((signed int) -1 << (timeValues[0]/10)+1);

		// check timer interrupt flag, reset if required
		Timer_clearInterrupt();
		// reset system watchdog
		HPS_ResetWatchdog();
	}
}


//Main Function
int main(void) {
	// initialise LCD
	init_lcd();

	// initialise Timer
	init_timer();

	// begin introduction loop
	intro();
}
