#include "HPS_PrivateTimer.h"

//Driver Base Addresses
volatile unsigned int   *timer_base_ptr     = 0x0;

//Driver Initialised
bool timer_initialised = false;

//Register Offsets
#define TIMER_LOAD      (0x00/sizeof(unsigned int))
#define TIMER_VALUE     (0x04/sizeof(unsigned int))
#define TIMER_CONTROL   (0x08/sizeof(unsigned int))
#define TIMER_INTERRUPT (0x0C/sizeof(unsigned int))

//Function to initialise the Timer
signed int Timer_initialise( unsigned int base_address ){

	timer_base_ptr = (unsigned int *) base_address;
    //Ensure timer initialises to disabled
    timer_base_ptr[TIMER_CONTROL] = 0;
    //Timer now initialised
    timer_initialised = true;
    return TIMER_SUCCESS;
}

//Check if driver initialised
bool Timer_isInitialised() {
    return timer_initialised;
}

//signed int Timer_setPrescaler(unsigned int scaler){
//	if (!Timer_isInitialised()) return TIMER_ERRORNOINIT;
//
//	timer_base_ptr[TIMER_CONTROL] = scaler << 8;
//	return TIMER_SUCCESS;
//}

signed int Timer_setLoad(unsigned int load_value){
	if (!Timer_isInitialised()) return TIMER_ERRORNOINIT;

	timer_base_ptr[TIMER_LOAD] = load_value;
	return TIMER_SUCCESS;
}

signed int Timer_setControl(unsigned int prescaler, unsigned int I, unsigned int A, unsigned int E){
	if (!Timer_isInitialised()) return TIMER_ERRORNOINIT;

	timer_base_ptr[TIMER_CONTROL] = (prescaler << 8 | I << 2 | A << 1 | E << 0);
	return TIMER_SUCCESS;
}

signed int Timer_readCurrentValue(){
	if (!Timer_isInitialised()) return TIMER_ERRORNOINIT;

	return (timer_base_ptr[TIMER_VALUE]);
}

signed int Timer_clearInterruptFlag(){
	if (!Timer_isInitialised()) return TIMER_ERRORNOINIT;

	if (timer_base_ptr[TIMER_INTERRUPT] & 0x1) {
		// If the timer interrupt flag is set, clear the flag
		timer_base_ptr[TIMER_INTERRUPT] = 0x1;
	}
	return TIMER_SUCCESS;
}
