# ELEC5620M Written Assignment Repository - Stopwatch 


This repository contains source and header C files to that define the function of a digital timer a variety of user interfaces. The code was designed for and implemented on the DE1-SoC board. This code requires a number of drivers for each of the utilised hardware components, including: the A9 private timer, the 6 available Seven Segment displays and the LT24 LCD. This drivers can be observed in my personal [ELEC5620 driver repository](https://github.com/leeds-embedded-systems/ELEC5620M-Student-jamesheavey). 

A video demonstration of the implemented code can be seen using the link provided: [VIDEO DEMO](https://github.com/leeds-embedded-systems/ELEC5620M-Assignment2-jamesheavey/blob/6b7727a44f356afdfd38136ff60aa7a4e5cc5345/Stop_Watch_Demo.mp4)

## Stopwatch Functionality
The digital stopwatch designed during this project exhibits the all the required functionalities specified in the brief, including additional functionality and user interfacing for ease of use. Upon starting the stopwatch code, an introduction screen appears, remaining in this state until a button is pressed to start the timer. Once started the elapsed time is displayed on both the LCD and the seven segment displays in real time. Four buttons are available to the user.

* **BUTTON 1:** Resets the timer, returning to the, returning to the introduction screen
* **BUTTON 2:** Pause the timer
* **BUTTON 3:** Split (displayed on the LCD)
* **BUTTON 4:** Toggle the seven segment display mode from MM:SS:FF to HH:MM:SS. 

This encompasses all the user requirements for a suitable digital stopwatch.

## Function List
Each of the functions designed for this project are listed allow, along with their inputs/outouts and purpose within the system.

| FUNCTION NAME | INPUTS | PURPOSE |
| ---  | --- | --- |
| `init_timer` | void | Initialise the A9 Private Timer. |
| `init_LCD` | void | Initialise the LT24 LCD. |
| `reset_LCD` | void | Clear the screen and write the preset timer structure '00:00:00.00'. |
| `draw_split` | timeValues, x, y, scale, splitNum | Draws the current timeValues on the LCD at the specified (x,y) coordinates. |
| `intro` | void | Intro screen loop, displays a message on the LCD and the Seven Segments. Calles `stopwatch()` on button 1 press. |
| `pause` | void | Disables the timer, waits until button 2 is pressed again, re-enables timer.  Function called when _**button 3**_ is pressed in the main timer loop |
| `split` | timeValues, &splitNum | Increment the splitNum, call `draw_split`, wait until split button is released. Function called when _**button 2**_ is pressed in the main timer loop |
| `mode_toggle` | &mode | Toggles mode variable transitioning between hour:min:sec and min:sec:hundredth modes. Function called when _**button 4**_ is pressed in the main timer loop. |
| `hundredths` | &timeValue | Increments the 'hundredths' array variable value using a pointer to its address. This function is called by a task scheduler when its associated time-period condition is fulfilled. |
| `seconds` | &timeValue | Increments the 'seconds' array variable value using a pointer to its address. This function is called by a task scheduler when its associated time-period condition is fulfilled. |
| `minutes` | &timeValue | Increments the 'minutes' array variable value using a pointer to its address. This function is called by a task scheduler when its associated time-period condition is fulfilled. |
| `hours` | &timeValue | Increments the 'hours' array variable value using a pointer to its address. This function is called by a task scheduler when its associated time-period condition is fulfilled. |
| `stopwatch` | void | The main stopwatch function. Contains while loop for both the core clock task scheduler and button press polling. Calls `intro()` on reset. |

---

#### By James Heavey

#### SID: 201198933

#### University of Leeds, Department of Electrical Engineering
