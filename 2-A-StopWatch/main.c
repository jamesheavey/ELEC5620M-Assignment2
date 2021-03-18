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

volatile unsigned int *key_ptr = (unsigned int *)0xFF200050;  // key buttons base address
volatile unsigned int *LED_ptr = (unsigned int *) 0xFF200000;

const unsigned int scaler = 200 - 1;
const unsigned int period = 225000000/(scaler+1); 			  // 225MHz

const unsigned int TIMER_SIZE = 4;
const unsigned int LED_MAX = 1024;                            // 2^10 as there are 10 LEDs

void init_timer() {
	Timer_initialise(0xFFFEC600);
	Timer_setLoad(0xFFFFFFFF);
	Timer_setControl(scaler, 0, 1, 1);
}

void init_lcd() {
	LT24_initialise(0xFF200060,0xFF200080);
	HPS_ResetWatchdog();
	LT24_clearDisplay(LT24_BLACK);
}

void intro() {
	const int num_chars = 27;
	unsigned int intro_message[num_chars] = {0xF,0xF,0x9,0xA,0xF,0xB,0xC,0xD,0xE,0x6,0x8,0xF,0xF,0xF,0x0,0x1,0x2,0xF,0x3,0x4,0xF,0x5,0x6,0x6,0x7,0x8,0xF};
	unsigned int lastIncrTime = 0;
	int n = 1;
	int i = 0;

	while (!(*key_ptr & 0x1)) {

		if ((lastIncrTime - Timer_readValue()) >= period/5) {
			lastIncrTime = lastIncrTime - period/5;
			*LED_ptr = (n | 512/n);
			n = (n*2)%511;
			i++;
		}

		DE1SoC_SevenSeg_SetSingleLetter(5,intro_message[i%num_chars]);
		DE1SoC_SevenSeg_SetSingleLetter(4,intro_message[(i+1)%num_chars]);
		DE1SoC_SevenSeg_SetSingleLetter(3,intro_message[(i+2)%num_chars]);
		DE1SoC_SevenSeg_SetSingleLetter(2,intro_message[(i+3)%num_chars]);
		DE1SoC_SevenSeg_SetSingleLetter(1,intro_message[(i+4)%num_chars]);
		DE1SoC_SevenSeg_SetSingleLetter(0,intro_message[(i+5)%num_chars]);

		HPS_ResetWatchdog();
	};

	*LED_ptr = 0;

	while (*key_ptr & 0x1) {HPS_ResetWatchdog();};

	Timer_setLoad(0xFFFFFFFF);
}

void update_lcd(unsigned int time_values[]){
	// split each into 2 parts
	int scale = 4;
	int width = 5;
	int height = 8;

	int x = 20;
	int y = 20;
	int i, j;

	LT24_clearDisplay(LT24_BLACK);

	for (i = 0, j = 0; i < TIMER_SIZE; i++, j= j + width*scale + 5){
		LT24_drawChar(BF_fontMap[16 + time_values[i]%10],  LT24_WHITE, x + j, y, width, height, scale);
	}

}

unsigned int timer_to_LEDs(unsigned int time[]){

	if(time[1] < LED_MAX){
		return time[1];
	}else if(time[2] < LED_MAX){
		return time[2];
	}else{
		return time[3];
	}
}

void pause(unsigned int timer_val){
	while (*key_ptr & 0x4) {HPS_ResetWatchdog();};
	while (!(*key_ptr & 0x4)){HPS_ResetWatchdog();};
	while (*key_ptr & 0x4) {HPS_ResetWatchdog();};

	Timer_setLoad(timer_val);
}

void split(unsigned int time[]){
	*LED_ptr = timer_to_LEDs(time);
	while (*key_ptr & 0x2) {HPS_ResetWatchdog();};
}

void mode_toggle(bool* mode){
	while (*key_ptr & 0x8) {HPS_ResetWatchdog();};
	*mode = !(*mode);
}

void timer() {
	unsigned int lastIncrTime[TIMER_SIZE] = {0};
	unsigned int timeValues[TIMER_SIZE] = {0};
	const unsigned int incrPeriod[TIMER_SIZE] = {period/100,period,period*60,period*3600};
	bool mode = false;

	*LED_ptr = 0;

	init_timer();

	intro();

	/* Main Run Loop */
	while(1) {
		int i;

		if (*key_ptr & 0x1) { timer(); }

		if (*key_ptr & 0x2) { split(timeValues); }

		if (*key_ptr & 0x4) { pause(Timer_readValue()); }

		if (*key_ptr & 0x8) { mode_toggle(&mode); }

		for (i = 0; i < TIMER_SIZE; i++) {
			if ((lastIncrTime[i] - Timer_readValue()) >= incrPeriod[i]) {
				timeValues[i] =  timeValues[i] + 1;
				lastIncrTime[i] = lastIncrTime[i] - incrPeriod[i];
			}
		}

		if ((timeValues[3] >= 1)) { mode = true; }

		if (mode) {
			DE1SoC_SevenSeg_SetDoubleDec(0,timeValues[1]%60);
			DE1SoC_SevenSeg_SetDoubleDec(2,timeValues[2]%60);
			DE1SoC_SevenSeg_SetDoubleDec(4,timeValues[3]%24);
		} else {
			DE1SoC_SevenSeg_SetDoubleDec(0,timeValues[0]%100);
			DE1SoC_SevenSeg_SetDoubleDec(2,timeValues[1]%60);
			DE1SoC_SevenSeg_SetDoubleDec(4,timeValues[2]%60);
		}

		//update_lcd(timeValues);

		Timer_clearInterrupt();
		HPS_ResetWatchdog();
	}
}

//Main Function
int main(void) {
	//init_lcd();
	timer();
}
