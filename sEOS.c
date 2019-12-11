/* Names: Tyler Andrews, Brennan Campbell, Tyler Tetens
 * Project: Experiment 5
 * Class: Cpe5160
 * Professor: Mr. Younger
*/

#include "sEOS.h"
#include "PORT.h"
#include "Main.h"

void sEOS_init(uint8_t interval) {
    uint16_t reload_16;
    uint8_t reload_8H;
	uint8_t reload_8L;

	T2CON=0;
    
    reload_16 = (uint16_t)(65536UL - ((OSC_FREQ*interval)/(OSC_PER_INST*1000UL))); // 16 bit reload value
    
    // 8 bit reload values
    reload_8H = (uint8_t)(reload_16 / 256);
    reload_8L = (uint8_t)(reload_16 % 256);
    
    // load timer 2 high byte
    TH2 = reload_8H;
    RCAP2H = reload_8H;
    
    // load timer 2 low byte
    TL2 = reload_8L;
    RCAP2L = reload_8L;
    
	TF2 = 0;
    ET2 = 1; // enable timer 2 interrupt
	EA = 1; // enable interrupts globally
	TR2 = 1; // start timer 2 interrupt   
}

void go_to_sleep(void) {
    PCON |= 0x01; // enter idle mode
}