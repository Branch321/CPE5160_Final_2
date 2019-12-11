/* Names: Tyler Andrews, Brennan Campbell, Tyler Tetens
 * Project: Experiment 5
 * Class: Cpe5160
 * Professor: Mr. Younger
*/
#include "Buttons.h"
#include "AT89C51RC2.h"
#include "Blinking_lights.h"

// Buttons used throughout project
SW_values_t SW1_values_g,SW2_values_g,SW3_values_g,SW4_values_g;

SW_values_t * Return_SW1_address(){
	return &SW1_values_g;
}

SW_values_t * Return_SW2_address(){
	return &SW2_values_g;
}

SW_values_t * Return_SW3_address(){
	return &SW3_values_g;
}

SW_values_t * Return_SW4_address(){
	return &SW4_values_g;
}

void Switch_Init(void)
{
	SW1_values_g.SW_mask=1<<0;
	SW1_values_g.debounce_time=0;
	SW1_values_g.SW_state=not_pressed;

	SW2_values_g.SW_mask=1<<1;
	SW2_values_g.debounce_time=0;
	SW2_values_g.SW_state=not_pressed;

	SW3_values_g.SW_mask=1<<2;
	SW3_values_g.debounce_time=0;
	SW3_values_g.SW_state=not_pressed;

	SW4_values_g.SW_mask=1<<3;
	SW4_values_g.debounce_time=0;
	SW4_values_g.SW_state=not_pressed;
}

void Read_Switch(SW_values_t * SW_input_p)
{
	if(SW_input_p->SW_state==not_pressed)
	{
		if((P2&SW_input_p->SW_mask)==0)
		{
			SW_input_p->debounce_time=0;
			SW_input_p->SW_state=debounce_p;
		}
	}
	else if(SW_input_p->SW_state==debounce_p)
	{
		SW_input_p->debounce_time+=interval;
		if(SW_input_p->debounce_time>=debounce_timer)
		{
			if((P2&SW_input_p->SW_mask)==0)
			{
				SW_input_p->SW_state=pressed;
			}
			else
			{
				SW_input_p->SW_state=not_pressed;
			}
		 }
	}
	else if(SW_input_p->SW_state==held)
	{
		if((P2&SW_input_p->SW_mask)!=0)
		{
			SW_input_p->SW_state = debounce_r;
		}
	}
	else if(SW_input_p->SW_state==debounce_r)
	{
		SW_input_p->debounce_time+=interval;
		if(SW_input_p->debounce_time>=debounce_timer)
		{
			if((P2&SW_input_p->SW_mask)!=0)
			{
				SW_input_p->SW_state=not_pressed;
			}
			else
			{
				SW_input_p->SW_state=held;
			}
		}
	 }
}
