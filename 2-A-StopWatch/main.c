/*
 * main.c
 *
 *  Created on: Mar 17, 2021
 *      Author:		James Heavey
 *      SID:		201198933
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

// Initialises A9 Private Timer
// Loads in max timer value, sets control bits
void init_timer()
{
	Timer_initialise(0xFFFEC600);
	Timer_setLoad(0xFFFFFFFF);
	Timer_setControl(199, 0, 1, 1);
}

// Sends command to initialise LCD
void init_lcd()
{
	LT24_initialise(0xFF200060,0xFF200080);
	HPS_ResetWatchdog();
}


//Main Function
int main(void) {
	// initialise LCD
	init_lcd();

	// initialise Timer
	init_timer();

	// begin introduction loop
	while(1){
		intro();

		stopwatch();
	}
}
