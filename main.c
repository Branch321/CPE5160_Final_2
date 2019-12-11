/* Names: Tyler Andrews, Brennan Campbell, and Tyler Tetens
 * Title: Experiment #4 - Fat File System
 */

#include "AT89C51RC2.h"
#include "stdio.h"
#include "main.h"
#include "PORT.H"
#include "UART.h"
#include "print_bytes.h"
#include "SPI.h"
#include "SDcard.h"
#include "Long_Serial_In.h"
#include "LCD_routines.h"
#include "Timer0_hardware_delay_1ms.h"
#include "Outputs.h"
#include "LED_Control.h"
#include "Directory_Functions_struct.h"
#include "Read_Sector.h"
#include <stdio.h>
#include "sEOS.h"
#include "STA013.h"
#include "MP3.h"

uint8_t code SD_start[]="SD Card Init...";
uint8_t code SDSC[]="Std. Capacity";
uint8_t code SDHC[]="High Capacity";


xdata uint8_t buffer1[512];
xdata uint8_t buffer2[512];

main()
{
   uint32_t idata block_num;
   uint16_t idata i;
   uint8_t idata error_flag;
   uint8_t idata SD_stat;
   uint32_t idata cluster_num;
   uint32_t idata current_directory_sector;
   uint32_t idata num_entries;
   FS_values_t * Mounted_Drive_values = Export_Drive_values();
   AUXR=0x0c;   // make all of XRAM available, ALE always on
   if(OSC_PER_INST==6)  // sets the x2 bit according to the OSC_PER_INST value
   {
       CKCON0=0x01;  // set X2 clock mode
   }
   else if(OSC_PER_INST==12)
   {
       CKCON0=0x00;  // set standard clock mode
   }
   
   P2_set_bit(0x0F);  // sets lower four bits of port 2 for switch inputs.
   LEDS_OFF(Amber_LED|Yellow_LED|Green_LED);
   LEDS_ON(Red_LED);
   i=0;
   while(i<=60000) i++;
   LEDS_OFF(Red_LED);
   uart_init(9600);
   printf("Done initalizing uart!\r\n");
   LCD_Init();
   printf("Done initalizing LCD!\r\n");
   LCD_Print(line1,0,SD_start);   
   error_flag=SPI_Master_Init(400000UL);
   printf("Starting SPI\r\n");
   STA013_init();
   printf("Done initalizing STA013!\r\n");
   if(error_flag!=no_errors)
   {
      LEDS_ON(Red_LED);  // An error causes the program to stop
	  UART_Transmit('S');
      while(1);
   }
   LEDS_ON(Amber_LED);
   // SD Card Initialization
   error_flag=SD_card_init();
   if(error_flag!=no_errors)
   {
      LEDS_ON(Red_LED);  // An error causes the program to stop
	  UART_Transmit('C');
      while(1);
   }
   LEDS_OFF(Amber_LED);
   error_flag=SPI_Master_Init(25000000UL);
   if(error_flag!=no_errors)
   {
      LEDS_ON(Red_LED);  // An error causes the program to stop
	  UART_Transmit('P');
      while(1);
   }
   for(i=0;i<512;i++)
   {
      buffer1[i]=0xff;  // erase valout for debug
//      buf2[i]=0xff;
   }
   SD_stat=Return_SD_Card_Type();
   if(SD_stat==Standard_Capacity)
   {
      LCD_Print(line2,0,SDSC);
   }
   else if(SD_stat==High_Capacity)
   {
      LCD_Print(line2,0,SDHC);
   }
// Main Loop
   error_flag = Mount_Drive(buffer1);
   current_directory_sector = Mounted_Drive_values->FirstRootDirSec;
   while(1)
   {
      num_entries = Print_Directory(current_directory_sector, buffer1);
      printf("Input Entry #: ");
      block_num=long_serial_input(); //block_num is entry number for Read_Dir_Entry()
      while ((block_num <= 0) || (block_num > num_entries))
      {
         printf("Invalid selection. Please select a valid entry.");
         block_num = long_serial_input();
      }

	  LEDS_ON(Green_LED);
	  cluster_num = Read_Dir_Entry(current_directory_sector, block_num, buffer1);
     LCD_Print(line1,0,buffer1[block_num]);
	  if((cluster_num &directory_bit)!=0) // directory mask
	  {
	  	  printf("Entry is a directory...Opening now...\r\n");
	      cluster_num &= 0x0FFFFFFF;
          current_directory_sector = First_Sector(cluster_num);
	  }
	  else // if entry is a file
	  {
	  	   printf("Entry is a file...Opening now...\r\n");
	  	   cluster_num &= 0x0FFFFFFF;
	      //Open_File(cluster_num, buf2); //this needs to be gotten rid of
		  Play_MP3_file(cluster_num);
		  
	  }
   }
} 



