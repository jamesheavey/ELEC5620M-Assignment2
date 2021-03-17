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
const unsigned int scaler = 200 - 1;
const unsigned int period = 225000000/(scaler+1); 			  // 225MHz

const unsigned int RESET[4] = {0};


//Main Function
int main(void) {
    /* Local Variables */
	unsigned int lastIncrementTimerValue[4] = {0};
	unsigned int time_vals[4] = {0};
	const unsigned int incrementPeriod[4] = {period/100,period,period*60,period*60*60};

	int hour_trigger = 0;
	bool first = true;

	Timer_initialise(0xFFFEC600);
	Timer_setLoad(0xFFFFFFFF);
	Timer_setControl(scaler, 0, 1, 1);

	/* Main Run Loop */
	while(1) {


		for (int i = 0; i < 4; i++) {
			if ((lastIncrementTimerValue[i] - Timer_readCurrentValue()) >= incrementPeriod[i]) {

				time_vals[i] =  time_vals[i] + 1;
				lastIncrementTimerValue[i] = lastIncrementTimerValue[i] - incrementPeriod[i];
			}
		}

		if (first && time_vals[2] >= 60) { hour_trigger++; first = false; }

		DE1SoC_SevenSeg_SetDoubleDec(0,time_vals[hour_trigger + 0]%100);
		DE1SoC_SevenSeg_SetDoubleDec(2,time_vals[hour_trigger + 1]%60);
		DE1SoC_SevenSeg_SetDoubleDec(4,time_vals[hour_trigger + 2]%60);

		Timer_clearInterruptFlag();
		HPS_ResetWatchdog();
	}
}

