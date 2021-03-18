/*
 * DE1SoC_SevenSeg.c
 *
 *  Created on: 12 Feb 2021
 *      Author: Harry Clegg
 *      		You!
 */

#include "DE1SoC_SevenSeg.h"

// Base addresses of the seven segment display peripherals.
volatile unsigned char *sevenseg_base_lo_ptr = (unsigned char *)0xFF200020;
volatile unsigned char *sevenseg_base_hi_ptr = (unsigned char *)0xFF200030;

// There are four HEX displays attached to the low (first) address.
#define SEVENSEG_N_DISPLAYS_LO 4

// There are two HEX displays attached to the high (second) address.
#define SEVENSEG_N_DISPLAYS_HI 2

void DE1SoC_SevenSeg_Write(unsigned int display, unsigned char value) {
    // Select between the two addresses so that the higher level functions
    // have a seamless interface.
    if (display < SEVENSEG_N_DISPLAYS_LO) {
        // If we are targeting a low address, use byte addressing to access
        // directly.
        sevenseg_base_lo_ptr[display] = value;
    } else {
        // If we are targeting a high address, shift down so byte addressing
        // works.
        display = display - SEVENSEG_N_DISPLAYS_LO;
        sevenseg_base_hi_ptr[display] = value;
    }
}

void DE1SoC_SevenSeg_SetSingle(unsigned int display, unsigned int value) {
    // ToDo: Write the code for driving a single seven segment display here.
    // Your function should turn a real value 0-F into the correctly encoded
    // bits to enable the correct segments on the seven segment display to
    // illuminate. Use the DE1SoC_SevenSeg_Write function you created earlier
    // to set the bits of the display.

	// Initialise integer variable to store converted
	// 7seg display hex value
	int seven_seg_value;

	// Case statement that selects appropriate 7seg
	// display value based on the Hex input value
	switch(value){

		case 0x0: seven_seg_value = 0x3F; break;
		case 0x1: seven_seg_value = 0x06; break;
		case 0x2: seven_seg_value = 0x5B; break;
		case 0x3: seven_seg_value = 0x4F; break;
		case 0x4: seven_seg_value = 0x66; break;
		case 0x5: seven_seg_value = 0x6D; break;
		case 0x6: seven_seg_value = 0x7D; break;
		case 0x7: seven_seg_value = 0x07; break;
		case 0x8: seven_seg_value = 0x7F; break;
		case 0x9: seven_seg_value = 0x67; break;
		case 0xA: seven_seg_value = 0x77; break;
		case 0xB: seven_seg_value = 0x7C; break;
		case 0xC: seven_seg_value = 0x39; break;
		case 0xD: seven_seg_value = 0x5E; break;
		case 0xE: seven_seg_value = 0x79; break;
		case 0xF: seven_seg_value = 0x71; break;

		default: seven_seg_value = 0x40;

	}

	// Write the selected 7seg hex value to
	// the designated 7seg display
	DE1SoC_SevenSeg_Write(display, seven_seg_value);

}

void DE1SoC_SevenSeg_SetDoubleHex(unsigned int display, unsigned int value) {
    // ToDo: Write the code for setting a pair of seven segment displays
    // here. Good coding practice suggests your solution should call
    // DE1SoC_SevenSeg_SetSingle() twice.
    // This function should show the first digit of a HEXADECIMAL number on
    // the specified 'display', and the second digit on the display to
    // the left of the specified display.

    /** Some examples:
     *
     *    input | output | HEX(N+1) | HEX(N)
     *    ----- | ------ | -------- | ------
     *        5 |     05 |        0 |      5
     *       30 |     1E |        1 |      E
     *     0x60 |     60 |        6 |      0
     */

	// Select the LSB of the given Hex value and write it
	// to the given 7seg display
	DE1SoC_SevenSeg_SetSingle(display, value & 0xF);

	// Select the MSB and right shift it, then write it to the
	// 7seg display to the left of the LSB value
	DE1SoC_SevenSeg_SetSingle(display + 1, (value & 0xF0) >> 4);

}

void DE1SoC_SevenSeg_SetDoubleDec(unsigned int display, unsigned int value) {
    // ToDo: Write the code for setting a pair of seven segment displays
    // here. Good coding practice suggests your solution should call
    // DE1SoC_SevenSeg_SetSingle() twice.
    // This function should show the first digit of a DECIMAL number on
    // the specified 'display', and the second digit on the display to
    // the left of the specified display.

    /** Some examples:
     *
     *	  input | output | HEX(N+1) | HEX(N)
     *    ----- | ------ | -------- | ------
     *        5 |     05 |        0 |      5
     *       30 |     30 |        3 |      0
     *     0x90 |     96 |        9 |      6
     */

	// Calculate the remainder when base 16 to base 10 modulus
	// is conducted. This represents the LSB. Write this value
	// to the given display
	DE1SoC_SevenSeg_SetSingle(display, value % 10);

	// Calculate the 10s value by determining the number of 10 units
	// that fit into the given Hex value (rounding to the nearest whole).
	// This represents the MSB. Write this value to the 7seg display to the left
	// of the LSB.
	DE1SoC_SevenSeg_SetSingle(display + 1, value / 10);

}

void DE1SoC_SevenSeg_SetSingleLetter(unsigned int display, unsigned int value) {
    // ToDo: Write the code for driving a single seven segment display here.
    // Your function should turn a real value 0-F into the correctly encoded
    // bits to enable the correct segments on the seven segment display to
    // illuminate. Use the DE1SoC_SevenSeg_Write function you created earlier
    // to set the bits of the display.

	// Initialise integer variable to store converted
	// 7seg display hex value
	int seven_seg_value;

	// Case statement that selects appropriate 7seg
	// display value based on the Hex input value
	switch(value){

		case 0x0: seven_seg_value = 0x3E; break;  // U
		case 0x1: seven_seg_value = 0x54; break;  // n
		case 0x2: seven_seg_value = 0x10; break;  // i
		case 0x3: seven_seg_value = 0x3F; break;  // O
		case 0x4: seven_seg_value = 0x71; break;  // F
		case 0x5: seven_seg_value = 0x38; break;  // L
		case 0x6: seven_seg_value = 0x7B; break;  // e
		case 0x7: seven_seg_value = 0x5E; break;  // d
		case 0x8: seven_seg_value = 0x6D; break;  // S
		case 0x9: seven_seg_value = 0x7C; break;  // b
		case 0xA: seven_seg_value = 0x6E; break;  // Y
		case 0xB: seven_seg_value = 0x0F; break;  // J
		case 0xC: seven_seg_value = 0x77; break;  // a
		case 0xD: seven_seg_value = 0x33; break;  // left half m
		case 0xE: seven_seg_value = 0x27; break;  // right half m
		case 0xF: seven_seg_value = 0x00; break;  // off

		default: seven_seg_value = 0x40;

	}

	// Write the selected 7seg hex value to
	// the designated 7seg display
	DE1SoC_SevenSeg_Write(display, seven_seg_value);

}
