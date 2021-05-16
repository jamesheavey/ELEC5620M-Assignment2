/*
 * stopwatch.h
 *
 *  Created on: 25 Mar 2021
 *      Author: James
 */

#ifndef STOPWATCH_H_
#define STOPWATCH_H_

#include "HPS_Watchdog/HPS_Watchdog.h"
#include "HPS_PrivateTimer/HPS_PrivateTimer.h"

// inclusion causes 'variable multiply defined" errors
//#include "display.h"

/* TESTING */
#include "time.h"
#include "stdio.h"

// ADDRESS DEFINITIONS
volatile unsigned int *key_ptr = (unsigned int *) 0xFF20005C;	// key buttons edge capture base address
volatile unsigned int *LED_ptr = (unsigned int *) 0xFF200000;	// LEDs base address

// CONSTANT DEFINITIONS
const unsigned int SCALER = 200 - 1;
const unsigned int PERIOD = 225000000/(SCALER+1);		// A9 Private timer freq. = 225MHz

const unsigned int TIMER_SIZE = 4;				// hundredths, seconds, minutes, hours
const unsigned int SPLIT_MAX = 10;				// Max number of splits displayed on screen simultaneously

// TYPE DEFINITIONS
typedef void (*TaskFunction)(unsigned int*);


/*
 *	TIMER INITIALISATION
 */

// Initialise A9 Private timer with preset parameters
void Timer_init( void );

/*
 *	BUTTON FUNCTIONS
 */

// Function to pause the timer on button press
void pause( void );

// Function to print split value to LCD on button press. Pointer parameters to update in main loop.
void split(unsigned int timeValues[], unsigned int splitValues[][TIMER_SIZE], int *splitNum);

// Function to toggle hour mode
void mode_toggle(bool* mode);


/*
 *	TIME UNIT INCREMENT FUNCTIONS
 */

// Increment hundredths timer value, display value on LCD
void hundredths(unsigned int* timeValue);

// Increment seconds timer value, display value on LCD
void seconds(unsigned int* timeValue);

// Increment minutes timer value, display value LCD
void minutes(unsigned int* timeValue);

// Increment hours timer value, display value LCD
void hours(unsigned int* timeValue);


/*
 *	INTRODUCTION FUNCTION
 */

// Function to introduce the timer on the Seven Segment displays and LCD
void introduction( void );


/*
 *	MAIN STOPWATCH FUNCTION
 */

// Function containing main stopwatch function while loop.
void stopwatch( void );


#endif /* STOPWATCH_H_ */
