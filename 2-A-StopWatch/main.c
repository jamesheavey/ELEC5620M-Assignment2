/*
 * main.c
 *
 *  Created on: Mar 17, 2021
 *      Author: James Heavey
 *      Affiliation: University of Leeds
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
