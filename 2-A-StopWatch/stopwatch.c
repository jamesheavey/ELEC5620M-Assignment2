/*
 * stopwatch.c
 *
 *  Created on: 25 Mar 2021
 *      Author: James
 */

#include "stopwatch.h"

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
	signed char clear [1] = {0x1};	 // used to clear LCD splits
	// if spits extend off screen, clear the splits and start again at the top
	if ((*splitNum % 10) == 0) { LCD_clear(60); }

	// draw the splits at the specified location
	LCD_draw_split(timeValues, 80, 60+25*(*splitNum % 10), 2, *splitNum + 1);

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
	SevenSeg_display_unit(0 - 2*mode, *timeValue);

	// update LCD display with current time value
	LCD_draw_unit(*timeValue, 200, 25, 2);
}


// Increment seconds timer value, display values
void seconds(unsigned int* timeValue, bool mode)
{
	*timeValue = (*timeValue +1)% 60;

	// update 7Segment display with current time values
	SevenSeg_display_unit(2 - 2*mode, *timeValue);

	// update LCD display with current time value
	LCD_draw_unit(*timeValue, 140, 20, 3);
}


// Increment minutes timer value, display values
void minutes(unsigned int* timeValue, bool mode)
{
	*timeValue = (*timeValue +1)% 60;

	// update 7Segment display with current time values
	SevenSeg_display_unit(4 - 2*mode, *timeValue);

	// update LCD display with current time value
	LCD_draw_unit(*timeValue, 80, 20, 3);
}


// Increment hours timer value, display values
void hours(unsigned int* timeValue, bool mode)
{
	*timeValue = (*timeValue +1)% 24;

	// update 7Segment display with current time values
	SevenSeg_display_unit(6 - 2*mode, *timeValue);

	// update LCD display with current time value
	LCD_draw_unit(*timeValue, 20, 20, 3);
}


// Function to introduce the timer on the Seven Segment displays
void intro()
{
	unsigned int lastIncrTime [3] = {0};
	unsigned int introPeriods [3] = {PERIOD/5, PERIOD/20, PERIOD/50};

	int n=1; int i=0; int j=0; int k=0; int x=6;

	// clear edge capture flags, on reset
	*key_ptr = 0xF;

	Timer_setLoad(0xFFFFFFFF);
	LCD_reset();

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
			j++;
			lastIncrTime[1] -= introPeriods[1];
		}

		// LCD colour update and print
		if ((lastIncrTime[2] - Timer_readValue()) >= introPeriods[2]){
			LCD_draw_moving_msg(x, j, k);
			k++;
			lastIncrTime[2] -= introPeriods[2];
		}

		SevenSeg_display_msg(i);

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
	const unsigned int incrPeriod[TIMER_SIZE] = {PERIOD/100,PERIOD,PERIOD*60,PERIOD*3600}; 	// set the increment period for all timer units
	TaskFunction taskFunctions[TIMER_SIZE] = {&hundredths,&seconds,&minutes,&hours};	// define task function struct to call increment functions when required

	bool mode = false; int splitNum = 0; int i;

	//	// TESTING
	//	double t_elapsed;
	//	clock_t test_timer;

	LCD_preset();

	SevenSeg_set(timeValues, mode);		// Initialise to '00 00 00'

	Timer_setLoad(0xFFFFFFFF);		// reset timer before main loop

//	test_timer = clock();			// Begin test clock

	/* Main Run Loop */
	while(1) {
		// poll key 1
		if (*key_ptr & 0x1) { break; }

		// poll key 2
		if (*key_ptr & 0x2) { split(timeValues, &splitNum); }

		// poll key 3
		if (*key_ptr & 0x4) { pause(); }

		// poll key 4
		if (timeValues[3] >= 1) { mode = true; SevenSeg_set(timeValues, mode); }		// if it has been more than 1 hour, force hour mode,
		else if (*key_ptr & 0x8) { mode_toggle(&mode); SevenSeg_set(timeValues, mode); }	// else toggle hour mode

		// if elapsed time is greater than any specified unit period,
		// increment the associated timeValue with task scheduler
		for (i = 0; i < TIMER_SIZE; i++) {
			if ((lastIncrTime[i] - Timer_readValue()) >= incrPeriod[i]) {
				taskFunctions[i](&timeValues[i], mode);
				lastIncrTime[i] -= incrPeriod[i];

//				// TESTING
//				if (i == 2) {
//					// displays the cumulative times at selected unit incrment
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
