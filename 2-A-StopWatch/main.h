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

/*
 *	INIT LCD & TIMER FUNCTIONS
 */

// Initialises A9 Private Timer
// Loads in max timer value, sets control bits
void init_timer( void );

// Sends command to initialise LCD
void init_lcd( void );


#endif /* MAIN_H_ */
