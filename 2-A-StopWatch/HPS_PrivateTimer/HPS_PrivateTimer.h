#include <stdbool.h>

#define TIMER_SUCCESS       0
#define TIMER_ERRORNOINIT  -1

signed int Timer_initialise( unsigned int base_address );
bool Timer_isInitialised( void );
//signed int Timer_setPrescaler(unsigned int scaler);
signed int Timer_setLoad(unsigned int load_value);
signed int Timer_setControl(unsigned int prescaler, unsigned int I, unsigned int A, unsigned int E);
signed int Timer_readValue( void );
signed int Timer_clearInterrupt( void );
