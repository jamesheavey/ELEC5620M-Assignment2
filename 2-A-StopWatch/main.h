/*
 * main.h
 *
 *  Created on: Mar 19, 2021
 *      Author: James
 */

#ifndef MAIN_H_
#define MAIN_H_

volatile unsigned int *key_ptr = (unsigned int *)0xFF200050;  // key buttons base address
volatile unsigned int *LED_ptr = (unsigned int *) 0xFF200000;

const unsigned int scaler = 200 - 1;
const unsigned int period = 225000000/(scaler+1); 			  // 225MHz

const unsigned int TIMER_SIZE = 4;
const unsigned int LED_MAX = 1024;                            // 2^10 as there are 10 LEDs

typedef void (*TaskFunction)(unsigned int*);


void init_timer()
{
	Timer_initialise(0xFFFEC600);
	Timer_setLoad(0xFFFFFFFF);
	Timer_setControl(scaler, 0, 1, 1);
}

void init_lcd()
{
	LT24_initialise(0xFF200060,0xFF200080);
	HPS_ResetWatchdog();
	LT24_clearDisplay(LT24_BLACK);

	LT24_drawCharDoubleDec(0, LT24_WHITE, 20 , 20, 5, 8, 3);
	LT24_drawCharDoubleDec(0, LT24_WHITE, 80 , 20, 5, 8, 3);
	LT24_drawCharDoubleDec(0, LT24_WHITE, 140 , 20, 5, 8, 3);
	LT24_drawCharDoubleDec(0, LT24_WHITE, 200 , 36, 5, 8, 1.5);

	LT24_drawChar(BF_fontMap[26], LT24_WHITE, 65, 20, 5, 8, 3);
	LT24_drawChar(BF_fontMap[26], LT24_WHITE, 125, 20, 5, 8, 3);
	LT24_drawChar(BF_fontMap[14], LT24_WHITE, 185, 36, 5, 8, 1.5);
}

void intro()
{
	const int num_chars = 27;
	unsigned int intro_message[num_chars] = {0xF,0xF,0x9,0xA,0xF,0xB,0xC,0xD,0xE,0x6,0x8,0xF,0xF,0xF,0x0,0x1,0x2,0xF,0x3,0x4,0xF,0x5,0x6,0x6,0x7,0x8,0xF};
	unsigned int lastIncrTime = 0;
	int n = 1;
	int i = 0;
	int j;

	while (*key_ptr & 0x1) {HPS_ResetWatchdog();};

	while (!(*key_ptr & 0x1)) {

		if ((lastIncrTime - Timer_readValue()) >= period/5) {
			lastIncrTime = lastIncrTime - period/5;
			*LED_ptr = (n | 512/n);
			n = (n*2)%511;
			i++;
		}

		for (j = 0; j<6; j++){
			DE1SoC_SevenSeg_SetSingleLetter(5-j,intro_message[(i+j)%num_chars]);
		}

		HPS_ResetWatchdog();
	};

	*LED_ptr = 0;

	while (*key_ptr & 0x1) {HPS_ResetWatchdog();};
}

unsigned int timer_to_LEDs(unsigned int time[])
{

	if(time[1] < LED_MAX){
		return time[1];
	}else if(time[2] < LED_MAX){
		return time[2];
	}else{
		return time[3];
	}
}

void pause()
{
	Timer_setControl(scaler, 0, 1, 0);

	while (*key_ptr & 0x4) {HPS_ResetWatchdog();};
	while (!(*key_ptr & 0x4)){HPS_ResetWatchdog();};
	while (*key_ptr & 0x4) {HPS_ResetWatchdog();};

	Timer_setControl(scaler, 0, 1, 1);
}

void split(unsigned int timeValues[], int *splitNum)
{
	*splitNum++;
	*LED_ptr = timer_to_LEDs(timeValues);

	while (*key_ptr & 0x2) {HPS_ResetWatchdog();};
}

void mode_toggle(bool* mode)
{
	while (*key_ptr & 0x8) {HPS_ResetWatchdog();};
	*mode = !(*mode);
}

void hundredths(unsigned int* timeValue)
{
	*timeValue = (*timeValue +1)% 100;

	LT24_drawDoubleChar(97, LT24_BLACK, 200 , 36, 5, 8, 1.5);
	LT24_drawCharDoubleDec(*timeValue, LT24_WHITE, 200 , 36, 5, 8, 1.5);
}

void seconds(unsigned int* timeValue)
{
	*timeValue = (*timeValue +1)% 60;

	LT24_drawDoubleChar(97, LT24_BLACK, 140 , 20, 5, 8, 3);
	LT24_drawCharDoubleDec(*timeValue, LT24_WHITE, 140 , 20, 5, 8, 3);
}

void minutes(unsigned int* timeValue)
{
	*timeValue = (*timeValue +1)% 60;

	LT24_drawDoubleChar(97, LT24_BLACK, 80 , 20, 5, 8, 3);
	LT24_drawCharDoubleDec(*timeValue, LT24_WHITE, 80 , 20, 5, 8, 3);
}

void hours(unsigned int* timeValue)
{
	*timeValue = (*timeValue +1)% 24;

	LT24_drawDoubleChar(97, LT24_BLACK, 20 , 20, 5, 8, 3);
	LT24_drawCharDoubleDec(*timeValue, LT24_WHITE, 20 , 20, 5, 8, 3);
}

void timer() {
	unsigned int lastIncrTime[TIMER_SIZE] = {0};
	unsigned int timeValues[TIMER_SIZE] = {0};
	const unsigned int incrPeriod[TIMER_SIZE] = {period/100,period,period*60,period*3600};
	TaskFunction taskFunctions[TIMER_SIZE] = {&hundredths,&seconds,&minutes,&hours};
	bool mode = false;
	int splitNum = 0;

	LT24_clearDisplay(LT24_BLACK);
	*LED_ptr = 0;

	init_timer();

	intro();

	init_lcd();

	Timer_setLoad(0xFFFFFFFF);  // reset timer

	/* Main Run Loop */
	while(1) {
		int i;

		if (*key_ptr & 0x1) { timer(); }

		if (*key_ptr & 0x2) { split(timeValues, &splitNum); }

		if (*key_ptr & 0x4) { pause(); }

		if ((timeValues[3] >= 1)) { mode = true; }
		else if (*key_ptr & 0x8) { mode_toggle(&mode); }

		for (i = 0; i < TIMER_SIZE; i++) {
			if ((lastIncrTime[i] - Timer_readValue()) >= incrPeriod[i]) {

				taskFunctions[i](&timeValues[i]);

				lastIncrTime[i] = lastIncrTime[i] - incrPeriod[i];

				DE1SoC_SevenSeg_SetDoubleDec(0,timeValues[0+mode]);
				DE1SoC_SevenSeg_SetDoubleDec(2,timeValues[1+mode]);
				DE1SoC_SevenSeg_SetDoubleDec(4,timeValues[2+mode]);
			}

			*LED_ptr =  ~((signed int) -1 << timeValues[0]/10);
		}

		Timer_clearInterrupt();
		HPS_ResetWatchdog();
	}
}

#endif /* MAIN_H_ */
