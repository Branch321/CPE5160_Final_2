/* Names: Tyler Andrews, Brennan Campbell, Tyler Tetens
 * Project: Experiment 5
 * Class: Cpe5160
 * Professor: Mr. Younger
*/
#ifndef _BUTTONS_H
#define _BUTTONS_H

#include "main.h"

#define interval 1 // number to increment by inside of debounce check
#define debounce_timer 50 // Amount of time in ms used for debouncing

// Types used for Button States
typedef enum{
	not_pressed,
	debounce_p,
	pressed,
	held,
	debounce_r
} switch_state_t;

// Types used for Buttons
typedef struct{
	uint8_t SW_mask;
	uint8_t debounce_time;
	switch_state_t SW_state;
} SW_values_t;

/* Desc: Initializes 4 buttons used in this project
 */
void Switch_Init(void);

/* Desc: Reads the state of a button press based on a state machine
 * Variables: A pointer to the button you are checking
 * Pre: The button you are checking must initialized beforehand
 */
void Read_Switch(SW_values_t * SW_input_p);

/* Desc: Functions that return the address to the button global variables
 */
SW_values_t * Return_SW1_address();
SW_values_t * Return_SW2_address();
SW_values_t * Return_SW3_address();
SW_values_t * Return_SW4_address();

#endif
