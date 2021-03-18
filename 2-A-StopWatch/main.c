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
const unsigned int COEFFICIENTS[TIMER_SIZE] = {1/100, 1, 60, 3600};

void init_timer() {
	Timer_initialise(0xFFFEC600);
	Timer_setLoad(0xFFFFFFFF);
	Timer_setControl(scaler, 0, 1, 1);
}

void intro() {
	const int num_chars = 27;
	unsigned int intro_message[num_chars] = {0xF,0xF,0x9,0xA,0xF,0xB,0xC,0xD,0xE,0x6,0x8,0xF,0xF,0xF,0x0,0x1,0x2,0xF,0x3,0x4,0xF,0x5,0x6,0x6,0x7,0x8,0xF};
	unsigned int lastIncrementTimerValue = 0;
	int i = 0;

	while (!(*key_ptr & 0x1)) {

		if ((lastIncrementTimerValue - Timer_readCurrentValue()) >= period/5) {
			i++;
			lastIncrementTimerValue = lastIncrementTimerValue - period/5;
		}

		DE1SoC_SevenSeg_SetSingleLetter(5,intro_message[i%num_chars]);
		DE1SoC_SevenSeg_SetSingleLetter(4,intro_message[(i+1)%num_chars]);
		DE1SoC_SevenSeg_SetSingleLetter(3,intro_message[(i+2)%num_chars]);
		DE1SoC_SevenSeg_SetSingleLetter(2,intro_message[(i+3)%num_chars]);
		DE1SoC_SevenSeg_SetSingleLetter(1,intro_message[(i+4)%num_chars]);
		DE1SoC_SevenSeg_SetSingleLetter(0,intro_message[(i+5)%num_chars]);

		HPS_ResetWatchdog();
	};

	while (*key_ptr & 0x1) {HPS_ResetWatchdog();};

	Timer_setLoad(0xFFFFFFFF);
}

unsigned int timer_to_seconds(unsigned int time[]){

	unsigned int total_time = 0;

	int i;
	for (i = 1; i < TIMER_SIZE; ++i) {
		total_time += time[i] * COEFFICIENTS[i];
	}

	// if the number of seconds is too big for num LEDs (> 17 mins) convert to minutes,
	// if still too big (> 17 hrs), convert to hours
	while(total_time > 1023) { total_time = total_time / 60; }

	return total_time;
}

void pause(unsigned int timer_val){
	while (*key_ptr & 0x4) {HPS_ResetWatchdog();};
	while (!(*key_ptr & 0x4)){HPS_ResetWatchdog();};
	while (*key_ptr & 0x4) {HPS_ResetWatchdog();};

	Timer_setLoad(timer_val);
}

void split(unsigned int time[]){
	*LED_ptr = timer_to_seconds(time);
	while (*key_ptr & 0x2) {HPS_ResetWatchdog();};
}

void timer() {
	unsigned int lastIncrementTimerValue[TIMER_SIZE] = {0};
	unsigned int time_vals[TIMER_SIZE] = {0};
	const unsigned int incrementPeriod[TIMER_SIZE] = {period/100,period,period*60,period*3600};
	int hour_trigger = 0;
	bool first = true;

	*LED_ptr = 0;

	init_timer();

	intro();

	/* Main Run Loop */
	while(1) {
		int i;

		if (*key_ptr & 0x1) { timer(); }

		if (*key_ptr & 0x2) { split(time_vals); }

		if (*key_ptr & 0x4) { pause(Timer_readCurrentValue()); }

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

//Main Function
int main(void) {
	timer();
}
