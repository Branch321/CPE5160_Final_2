/* Names: Tyler Andrews, Brennan Campbell, Tyler Tetens
 * Project: Experiment 5
 * Class: Cpe5160
 * Professor: Mr. Younger
*/

#ifndef _SEOS_H
#define _SEOS_H

#include "main.h"

/* Desc: Initializes the Simple Embedded Operating System
 * Variables: interval - sets timer interval (in ms)
*/
void sEOS_init(uint8_t interval);

/* Desc: Sets the microcontroller to an idle state
*/
void go_to_sleep(void);

#endif