/*
 * main.c
 *
 *  Created on: Mar 17, 2021
 *      Author: James
 */

#include "HPS_Watchdog/HPS_Watchdog.h"
#include "HPS_PrivateTimer/HPS_PrivateTimer.h"
#include "DE1SoC_SevenSeg/DE1SoC_SevenSeg.h"

volatile unsigned int *key_ptr = (unsigned int *)0xFF200050;  // key buttons base address
volatile unsigned int *LED_ptr = (unsigned int *) 0xFF200000;

const unsigned int scaler = 200 - 1;
const unsigned int period = 225000000/(scaler+1); 			  // 225MHz

const unsigned int TIMER_SIZE = 4;
const unsigned int RESET[TIMER_SIZE] = {0};
const unsigned int COEFFICIENTS[TIMER_SIZE] = {1/100, 1, 60, 3600};


unsigned int timer_to_seconds(unsigned int time[]){

	unsigned int total_time = 0;

	int i;
	for (i = 1; i < TIMER_SIZE; ++i) {
		total_time += time[i] * COEFFICIENTS[i];
	}

	return total_time;
}

//Main Function
int main(void) {
    /* Local Variables */
	unsigned int lastIncrementTimerValue[TIMER_SIZE] = {0};
	unsigned int time_vals[TIMER_SIZE] = {0};
	const unsigned int incrementPeriod[TIMER_SIZE] = {period/100,period,period*60,period*3600};

	int hour_trigger = 0;
	bool first = true;

	Timer_initialise(0xFFFEC600);
	Timer_setLoad(0xFFFFFFFF);
	Timer_setControl(scaler, 0, 1, 1);

	/* Main Run Loop */
	while(1) {
		int i;

		if (*key_ptr & 0x1) { memcpy(time_vals, RESET, sizeof(RESET)); }


		if (*key_ptr & 0x2) { *LED_ptr = timer_to_seconds(time_vals); }

		for (i = 0; i < TIMER_SIZE; i++) {
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
