/*
 * main.h
 *
 *  Created on: Mar 19, 2021
 *      Author:		James
 *      SID:		201198933
 *      Affiliation:	University of Leeds
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
volatile unsigned int *key_ptr = (unsigned int *) 0xFF20005C;	// key buttons edge capture base address
volatile unsigned int *LED_ptr = (unsigned int *) 0xFF200000;	// LEDs base address

// CONSTANT DEFINITIONS
const unsigned int SCALER = 200 - 1;
const unsigned int PERIOD = 225000000/(SCALER+1);		// A9 Private timer freq. = 225MHz

const unsigned int TIMER_SIZE = 4;				// hundredths, seconds, minutes, hours

// TYPE DEFINITIONS
typedef void (*TaskFunction)(unsigned int*, bool);


/*
 *	INIT LCD & TIMER FUNCTIONS
 */

// Initialises A9 Private Timer
// Loads in max timer value, sets control bits
void init_timer( void );

// Sends command to initialise LCD
void init_lcd( void );


/*
 *	LCD FUNCTIONS
 */

// Clears the display, writes starting timer structure
// i.e. '00:00:00.00'
void reset_lcd( void );

// Used for one time write of instantaneous timeValues when split button pressed.
// Split display location determined my splitNum value
void draw_split(unsigned int timeValues[], int x, int y, int scale, int splitNum);

  
/*
 *	SEVEN-SEGEMNT FUNCTIONS
 */

// used to set all the seven segment displays on reset or mode toggle
void set_7seg(unsigned int timeValues[], bool mode);


/*
 *	BUTTON FUNCTIONS
 */  

// Function to pause the timer on button press
void pause( void );

// Function to print split value to LCD on button press
void split(unsigned int timeValues[], int *splitNum);

// Function to toggle hour mode
void mode_toggle(bool* mode);


/*
 *	TIME UNIT INCREMENT FUNCTIONS
 */

// Increment hundredths timer value, display values
void hundredths(unsigned int* timeValue, bool mode);

// Increment seconds timer value, display values
void seconds(unsigned int* timeValue, bool mode);

// Increment minutes timer value, display values
void minutes(unsigned int* timeValue, bool mode);

// Increment hours timer value, display values
void hours(unsigned int* timeValue, bool mode);


/*
 *	INTRO FUNCTION
 */

// Function to introduce the timer on the Seven Segment displays
void intro( void );


/*
 *	MAIN STOPWATCH FUNCTION
 */

// main stopwatch function while loop
void stopwatch( void );

#endif /* MAIN_H_ */
