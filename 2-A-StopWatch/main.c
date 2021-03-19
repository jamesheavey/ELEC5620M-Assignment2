/*
 * main.c
 *
 *  Created on: Mar 17, 2021
 *      Author: James Heavey
 *      Affiliation: University of Leeds
 *
 */

#include "HPS_Watchdog/HPS_Watchdog.h"
#include "HPS_PrivateTimer/HPS_PrivateTimer.h"
#include "DE1SoC_SevenSeg/DE1SoC_SevenSeg.h"
#include "DE1SoC_LT24/DE1SoC_LT24.h"
#include "BasicFont/BasicFont.h"
#include "main.h"

//Main Function
int main(void) {
	init_lcd();
	timer();
}
