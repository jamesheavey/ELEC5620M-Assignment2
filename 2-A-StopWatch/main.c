/*
 * main.c
 *
 *  Created on: Mar 17, 2021
 *      Author: James
 */

//Include ResetWDT() macro:
#include "HPS_Watchdog/HPS_Watchdog.h"
#include "HPS_PrivateTimer/HPS_PrivateTimer.h"
#include "DE1SoC_SevenSeg/DE1SoC_SevenSeg.h"

volatile unsigned int *key_ptr = (unsigned int *)0xFF200050;  // key buttons base address

//Main Function
int main(void) {
    /* Local Variables */
	unsigned int lastBlinkTimerValue[4] = {0};
	unsigned int time_vals[4] = {0};
	const unsigned int blinkPeriod[4] = {500000,50000000,3000000000,180000000000};

	Timer_initialise(0xFFFEC600);
	Timer_setLoad(0xFFFFFFFF);
	Timer_setPrescaler(0);
	Timer_setControl(0, 1, 1);

	/* Main Run Loop */
	while(1) {
		// Read the current time
		unsigned int currentTimerValue = Timer_readCurrentValue();
		int i;

		for (i = 0; i < 4; i++) {
			if ((lastBlinkTimerValue[i] - currentTimerValue) >= blinkPeriod[i]) {

				time_vals[i] =  time_vals[i] + 1;

				lastBlinkTimerValue[i] = lastBlinkTimerValue[i] - blinkPeriod[i];
			}
		}

		DE1SoC_SevenSeg_SetDoubleDec(0,time_vals[0]%99);
		DE1SoC_SevenSeg_SetDoubleDec(2,time_vals[1]%60);
		DE1SoC_SevenSeg_SetDoubleDec(4,time_vals[2]%60);

		Timer_clearInterruptFlag();
		// Finally, reset the watchdog timer.
		HPS_ResetWatchdog();
	}
}
