/*
 * main.c
 *
 *  Created on: Mar 17, 2021
 *      Author: James
 */

//Include ResetWDT() macro:
#include "HPS_Watchdog/HPS_Watchdog.h"

//Main Function
int main(void) {
    /* Main Run Loop */
    while(1) {
        HPS_ResetWatchdog();
    }
}
