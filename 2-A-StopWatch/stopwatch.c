/*
 * stopwatch.c
 *
 *  Created on: 25 Mar 2021
 *      Author: James
 */

#include "stopwatch.h"


// Initialises A9 Private Timer
// Loads in max timer value, sets control bits
void Timer_init()
{
	Timer_initialise(0xFFFEC600);
	Timer_setLoad(0xFFFFFFFF);
	Timer_setControl(SCALER, 0, 1, 1);
}


// Function to print split value to LCD on button press
void split(unsigned int timeValues[], unsigned int splitValues[][TIMER_SIZE], int *splitNum)
{
	int i, j, k;

	// increment splitNum
	*splitNum += 1;

	// set split limit at 10 (max number that can fit on LCD screen at once)
	if(*splitNum >= SPLIT_MAX){ k = SPLIT_MAX; }

	else{ k = *splitNum; }

	// move all previously saved splits down one index, losing last value
	for(i = k-1; i > 0; i--){
		for(j = 0; j < TIMER_SIZE; j++){
			splitValues[i][j] = splitValues[i-1][j];
		}
	}

	// insert most recent split values at the front of splitValue array
	for(j = 0; j < TIMER_SIZE; j++){
		splitValues[0][j] = timeValues[j];
	}

	// redraw splits to the LCD in correct positions
	for(i = 0; i < k; i++){
		LCD_draw_split(splitValues[i], 80, 60+25*(i), 2, *splitNum - i);
	}

	// clear edge capture flags
	*key_ptr = 0xF;
}


// Function to pause the stopwatch on button press
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


// Function to toggle hour mode
void mode_toggle(bool* mode)
{
	// toggle mode
	*mode = !(*mode);
	// clear edge capture flags
	*key_ptr = 0xF;
}


// Increment hundredths timer value, display values on LCD
void hundredths(unsigned int* timeValue)
{
	*timeValue = (*timeValue +1)% 100;

	// update LCD display with current time value
	LCD_draw_unit(*timeValue, 200, 25, 2);
}


// Increment seconds timer value, display values on LCD
void seconds(unsigned int* timeValue)
{
	*timeValue = (*timeValue +1)% 60;

	// update LCD display with current time value
	LCD_draw_unit(*timeValue, 140, 20, 3);
}


// Increment minutes timer value, display values on LCD
void minutes(unsigned int* timeValue)
{
	*timeValue = (*timeValue +1)% 60;

	// update LCD display with current time value
	LCD_draw_unit(*timeValue, 80, 20, 3);
}


// Increment hours timer value, display values on LCD
void hours(unsigned int* timeValue)
{
	*timeValue = (*timeValue +1)% 24;

	// update LCD display with current time value
	LCD_draw_unit(*timeValue, 20, 20, 3);
}


// Function to introduce the stopwatch on the Seven Segment displays and LCD
void introduction()
{
	unsigned int lastIncrTime [3] = {0};
	unsigned int introPeriods [3] = {PERIOD/5, PERIOD/20, PERIOD/50};

	int n=1; int i=0; int colour=0; int y=0; int x=6;

	// clear the LCD
	LCD_reset();

	// remain in display loop until any key pressed
	while (!(*key_ptr)) {

		// LED and seven seg update
		if ((lastIncrTime[0] - Timer_readValue()) >= introPeriods[0]) {
			n = (n*2)%511;
			i++;

			*LED_ptr = (n | 512/n);
			SevenSeg_display_msg(i);

			lastIncrTime[0] -= introPeriods[0];
		}

		// LCD position update
		if ((lastIncrTime[1] - Timer_readValue()) >= introPeriods[1]){
			y++;
			lastIncrTime[1] -= introPeriods[1];
		}

		// LCD colour update and print
		if ((lastIncrTime[2] - Timer_readValue()) >= introPeriods[2]){
			colour++;

			LCD_draw_moving_msg(x, y, colour);

			lastIncrTime[2] -= introPeriods[2];
		}

		HPS_ResetWatchdog();
	};

	// clear LEDs
	*LED_ptr = 0;
	// clear edge capture flags
	*key_ptr = 0xF;
}


void stopwatch()
{
	unsigned int lastIncrTime[TIMER_SIZE] = {0};						// all timers start incrementing immediately
	unsigned int timeValues[TIMER_SIZE] = {0};						// all time values initialised to 0
	unsigned int splitValues[SPLIT_MAX][TIMER_SIZE];					// 2D array to store most recent splits
	const unsigned int incrPeriod[TIMER_SIZE] = {PERIOD/100,PERIOD,PERIOD*60,PERIOD*3600}; 	// set the increment period for all timer units
	TaskFunction taskFunctions[TIMER_SIZE] = {&hundredths,&seconds,&minutes,&hours};	// define task function struct to call increment functions when required

	bool mode = false; int splitNum = 0; int i;

	//	/* TESTING */
	//	double t_elapsed;
	//	clock_t test_timer;

	LCD_preset();

	SevenSeg_set(timeValues, mode);		// Initialise to '00 00 00'

	pause();				// wait for keypress to start

	Timer_setLoad(0xFFFFFFFF);		// reset timer before main loop

//	test_timer = clock();			// Begin test clock

	/* Main Stopwatch Loop */
	while(!(*key_ptr & 0x1)) {
		// exit on key 1 press (reset)

		// poll key 2
		if (*key_ptr & 0x2) { split(timeValues, splitValues, &splitNum); }

		// poll key 3
		if (*key_ptr & 0x4) { pause(); }

		// poll key 4
		if (*key_ptr & 0x8) { mode_toggle(&mode); }

		// if elapsed time is greater than any specified unit period,
		// increment the associated timeValue with task scheduler
		for (i = 0; i < TIMER_SIZE; i++) {
			if ((lastIncrTime[i] - Timer_readValue()) >= incrPeriod[i]) {
				taskFunctions[i](&timeValues[i]);
				lastIncrTime[i] -= incrPeriod[i];

//				/* TESTING */
//				if (i == 2) {
//					// displays the cumulative times at selected unit increment
//					t_elapsed = ((double)(clock() - test_timer)/CLOCKS_PER_SEC);
//					printf("%s: Private timer value = %d. Time.h value = %f. \n", "MINUTES",timeValues[i]*60,t_elapsed);
//				}
			}
		}
		// Update Seven segments with display mode
		SevenSeg_set(timeValues, mode || timeValues[3]); // force hour mode if more than an hour has passed

		// LEDs fill up every second then reset back to 0
		*LED_ptr =  ~((signed int) -1 << (timeValues[0]/10)+1);
		// check timer interrupt flag, reset if required
		Timer_clearInterrupt();
		// reset system watchdog
		HPS_ResetWatchdog();
	}
	// reset edge capture buttons
	*key_ptr = 0xF;
}
