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

//Main Function
int main(void) {
	// initialise LCD
	init_lcd();

	// initialise Timer
	init_timer();

	// begin main timer loop
	timer();
}
