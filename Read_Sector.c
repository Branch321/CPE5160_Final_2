/* Names: Tyler Andrews, Brennan Campbell, and Tyler Tetens
 * Title: Experiment #4 - Fat File System
 */
#include "Read_Sector.h"
#include "SPI.h"
#include "SDCard.h"
#include <stdio.h>

// Same error values as Directory_Functions_struct.h
#define No_Disk_Error 0x00
#define Disk_Error 0xF0

uint8_t Read_Sector(uint32_t sector_number,uint16_t sector_size, uint8_t * array_for_data)
{
	uint8_t SDtype;
	uint8_t error_flag = No_Disk_Error;

	// Grab SD card type, high capacity or standard capacity
	SDtype=Return_SD_Card_Type();
	// Start communication with SD card by clearing channel
	SPI_Select_Clear(SD_Card_Select);
	// Send command 17 for read block, sector_number adjusted for HC or SC
	error_flag=SEND_COMMAND(17,(sector_number<<SDtype));
	// Error check and read the block
	if(error_flag==no_errors) 
	{
		error_flag=read_block(sector_size, array_for_data);
	}
	// Set SPI select back high to end transmission
	SPI_Select_Set(SD_Card_Select);
	// Final error check
	if(error_flag!=no_errors)
	{
		error_flag=Disk_Error;
	}
	return error_flag;
}

