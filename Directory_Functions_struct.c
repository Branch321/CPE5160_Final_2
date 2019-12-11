/* Names: Tyler Andrews, Brennan Campbell, and Tyler Tetens
 * Title: Experiment #4 - Fat File System
 */
#include "AT89C51RC2.h"
#include <stdio.h>
#include "main.h"
#include "PORT.H"
#include "UART.h"
#include "SPI.h"
#include "SDCard.h"
#include "print_bytes.h"
#include "Directory_Functions_struct.h"
#include "Read_Sector.h"
#include <string.h>
#include "print_bytes.h"
#include "Long_Serial_In.h"

FS_values_t idata Drive_values;

/***********************************************************************
DESC: Returns a pointer to the global structure Drive_values to export to other files
INPUT: void
RETURNS: Pointer to the structure Drive_values
CAUTION: 
************************************************************************/

FS_values_t * Export_Drive_values(void)
{
   return &Drive_values;
}

uint8_t read8(uint16_t offset, uint8_t * array_name)
{
	uint8_t return_value;
	return_value = array_name[offset];
	return return_value;

}

uint16_t read16(uint16_t offset, uint8_t * array_name)
{
	uint32_t return_value;
	uint8_t temp, index;
	return_value=0;

	// little endian to big endian
	// loop for 2 bytes
	for(index=0;index<2;index++)
	{
		temp=*(array_name+offset+(1-index));
		return_value=return_value<<8;
		return_value|=temp;
	}
	return return_value;
}

uint32_t read32(uint16_t offset, uint8_t * array_name)
{
	uint32_t return_value;
	uint8_t temp, index;
	return_value=0;

	// little endian to big endian
	// loop for 4 bytes
	for(index=0;index<4;index++)
	{
		temp=*(array_name+offset+(3-index));
		return_value=return_value<<8;
		return_value|=temp;
	}
	return return_value;
}

/***********************************************************************
DESC: Prints all short file name entries for a given directory
INPUT: Starting Sector of the directory and the pointer to a
block of memory in xdata that can be used to read blocks from the SD card
RETURNS: uint16_t number of entries found in the directory
CAUTION: Supports FAT16, SD_shift must be set before using this function
************************************************************************/

uint16_t  Print_Directory(uint32_t Sector_num, uint8_t xdata * array_in)
{ 
   uint32_t Sector, max_sectors;
   uint16_t i, entries;
   uint8_t temp8, j, attr, out_val, error_flag;
   uint8_t * values;


   values=array_in;
   entries=0;
   i=0;
   if (Drive_values.FATtype==FAT16)  // included for FAT16 compatibility
   { 
      max_sectors=Drive_values.RootDirSecs;   // maximum sectors in a FAT16 root directory
   }
   else
   {
      max_sectors=Drive_values.SecPerClus;
   }
   Sector=Sector_num;
   error_flag=Read_Sector(Sector, Drive_values.BytesPerSec, values);
   if(error_flag==no_errors)
   {
     do
     {
        temp8=read8(0+i,values);  // read first byte to see if empty
        if((temp8!=0xE5)&&(temp8!=0x00))
	    {  
	       attr=read8(0x0b+i,values);
		   if((attr&0x0E)==0)   // if hidden, system or Vol_ID bit is set do not print
		   {
		      entries++;
			  printf("%5d. ",entries);  // print entry number with a fixed width specifier
		      for(j=0;j<8;j++)
			  {
			     out_val=read8(i+j,values);   // print the 8 byte name
			     putchar(out_val);
			  }
              if((attr&0x10)==0x10)  // indicates directory
			  {
			     for(j=8;j<11;j++)
			     {
			        out_val=read8(i+j,values);
			        putchar(out_val);
			     }
			     printf("[DIR]\n");
			  }
			  else       // print a period and the three byte extension for a file
			  {
			     putchar(0x2E);       
			     for(j=8;j<11;j++)
			     {
			        out_val=read8(i+j,values);
			        putchar(out_val);
			     }
			     putchar(0x0d);
                 putchar(0x0a);
			  }
		    }
		}
		    i=i+32;  // next entry
		    if(i>510)
		    {
			  Sector++;
              if((Sector-Sector_num)<max_sectors)
			  {
                 error_flag=Read_Sector(Sector,Drive_values.BytesPerSec,values);
			     if(error_flag!=no_errors)
			     {
			        entries=0;   // no entries found indicates disk read error
				    temp8=0;     // forces a function exit
			     }
                 i=0;
			  }
			  else
			  {
			     entries=entries|more_entries;  // set msb to indicate more entries in another cluster
			     temp8=0;                       // forces a function exit
			  }
		    }
         
	  }while(temp8!=0);
	}
	else
	{
		printf("Error has occured");
	   entries=0;    // no entries found indicates disk read error
	}
   return entries;
 }


/***********************************************************************
DESC: Uses the same method as Print_Directory to locate short file names,
      but locates a specified entry and returns and cluster  
INPUT: Starting Sector of the directory, an entry number and a pointer to a 
block of memory in xdata that can be used to read blocks from the SD card
RETURNS: uint32_t with cluster in lower 28 bits.  Bit 28 set if this is 
         a directory entry, clear for a file.  Bit 31 set for error.
CAUTION: 
************************************************************************/

uint32_t Read_Dir_Entry(uint32_t Sector_num, uint16_t Entry, uint8_t xdata * array_in)
{ 
   uint32_t Sector, max_sectors, return_clus;
   uint16_t i, entries;
   uint8_t temp8, attr, error_flag;
   uint8_t * values;

   values=array_in;
   entries=0;
   i=0;
   return_clus=0;
   if (Drive_values.FATtype==FAT16)  // included for FAT16 compatibility
   { 
      max_sectors=Drive_values.RootDirSecs;   // maximum sectors in a FAT16 root directory
   }
   else
   {
      max_sectors=Drive_values.SecPerClus;
   }
   Sector=Sector_num;
   error_flag=Read_Sector(Sector,Drive_values.BytesPerSec,values);
   if(error_flag==no_errors)
   {
     do
     {
        temp8=read8(0+i,values);  // read first byte to see if empty
        if((temp8!=0xE5)&&(temp8!=0x00))
	    {  
	       attr=read8(0x0b+i,values);
		   if((attr&0x0E)==0)    // if hidden do not print
		   {
		      entries++;
              if(entries==Entry)
              {
			    if(Drive_values.FATtype==FAT32)
                {
                   return_clus=read8(21+i,values);
				   return_clus=return_clus<<8;
                   return_clus|=read8(20+i,values);
                   return_clus=return_clus<<8;
                }
                return_clus|=read8(27+i,values);
			    return_clus=return_clus<<8;
                return_clus|=read8(26+i,values);
			    attr=read8(0x0b+i,values);
			    if(attr&0x10) return_clus|=directory_bit;
                temp8=0;    // forces a function exit
              }
              
		    }
		}
		    i=i+32;  // next entry
		    if(i>510)
		    {
			  Sector++;
			  if((Sector-Sector_num)<max_sectors)
			  {
                 error_flag=Read_Sector(Sector,Drive_values.BytesPerSec,values);
			     if(error_flag!=no_errors)
			     {
			         return_clus=no_entry_found;
                     temp8=0; 
			     }
			     i=0;
			  }
			  else
			  {
			     temp8=0;                       // forces a function exit
			  }
		    }
         
	  }while(temp8!=0);
	}
	else
	{
	   return_clus=no_entry_found;
	}
	if(return_clus==0) return_clus=no_entry_found;
   return return_clus;
 }

uint8_t Mount_Drive(uint8_t xdata * array_name)
{
	//uint8_t i;
	uint8_t temp_8;
	uint8_t error_flag;
	// Below are constants from BPB used for calculations
	// for the values in struct FS_values_t
	uint16_t RsvdSectorCount;
	uint8_t NumFATS;
	uint16_t RootEntryCnt;
	uint16_t TotalSectors16;
	uint16_t FATsz16;
	uint32_t TotalSectors32;
	uint32_t FATsz32;
	uint32_t FATSz;
	//Ran out of space so now some vars are in xdata
	uint32_t xdata TotSec;
	uint32_t xdata DataSec;
	uint32_t RootCluster;
	uint32_t RelativeSectors;
	uint32_t CountofClus;
	
	// Read in BPB or MBR
	error_flag = Read_Sector(0, 512, array_name);
	// Check for BPB or MBR
	temp_8 = read8(0,array_name);
	printf("Debug:: Offset 0 of Sector 0 is %x\r\n",temp_8);
	if((temp_8!=0xEB)&&(temp_8!=0xE9))
	{
		printf("Found MBR...\r\n");
		RelativeSectors = read32(0x01C6,array_name);
		printf("Debug:: Offset 0 of Relative Sectors is %lx\r\n",RelativeSectors);
		error_flag = Read_Sector(RelativeSectors ,512,array_name);
		temp_8 = read8(0,array_name);
		printf("Debug:: temp8 is %x\r\n",temp_8);
		if((temp_8!=0xEB)&&(temp_8!=0xE9))
		{
			printf("Error BPB not Found!\r\n");
		}
		else
		{
			printf("BPB Found!\r\n");
		}
	}
	//print_memory(array_name, 512);
	Drive_values.BytesPerSec = read16(0x0B,array_name);
	printf("BytesPerSec:: %x\r\n",Drive_values.BytesPerSec);
	Drive_values.SecPerClus = read8(0x0D,array_name);
	//printf("SecPerClus:: %bx\r\n",Drive_values.SecPerClus);
	RsvdSectorCount = read16(0x0E,array_name);
	//printf("RsvdSectorCount:: %x\r\n",RsvdSectorCount);
	NumFATS = read8(0x10,array_name);
	//printf("NumFATS:: %bx\r\n",NumFATS);
	RootEntryCnt = read16(0x11,array_name);
	//printf("RootEntryCnt:: %x\r\n",RootEntryCnt);
	TotalSectors16 = read16(0x13,array_name);
	//printf("TotalSectors16:: %x\r\n",TotalSectors16);
	FATsz16 = read16(0x16,array_name);
	//printf("FATsz16:: %x\r\n",FATsz16);
	TotalSectors32 = read32(0x20,array_name);
	//printf("TotalSectors32:: %lx\r\n",TotalSectors32);
	FATsz32 = read32(0x24,array_name);
	//printf("FATsz32:: %lx\r\n",FATsz32);
	RootCluster = read32(0x2C, array_name);
	//printf("RootCluster:: %lx\r\n",RootCluster);
	Drive_values.StartofFAT = RsvdSectorCount + RelativeSectors;
	//printf("StartofFAT:: %lx\r\n",Drive_values.StartofFAT);
	Drive_values.RootDirSecs = ((RootEntryCnt*32) + (Drive_values.BytesPerSec-1))/Drive_values.BytesPerSec;
	//printf("RootDirSecs:: %lx\r\n",Drive_values.RootDirSecs);
	Drive_values.FirstDataSec = RsvdSectorCount + (NumFATS*FATsz32) + Drive_values.RootDirSecs + RelativeSectors;
	//printf("FirstDataSec:: %lx\r\n",Drive_values.FirstDataSec);
	Drive_values.FirstRootDirSec = ((RootCluster-2)*Drive_values.SecPerClus)+Drive_values.FirstDataSec;
	//printf("FirstRootDirSec:: %lx\r\n",Drive_values.FirstRootDirSec);
	Drive_values.FATshift = FAT32_shift;

	//Checks FAT Size to use
	if(FATsz16 != 0)
	{
		FATSz = FATsz16;
	}
	else
	{
		FATSz = FATsz32;
	}
	
	//Checks which TotalSectors to use
	if(TotalSectors16 != 0)
	{
		TotSec = TotalSectors16;
	}
	else
	{
		TotSec = TotalSectors32;
	}
	
	//Calculates Number of Data Sectors for CountofClus
	DataSec = (TotSec - (RsvdSectorCount + ((NumFATS * FATSz) + Drive_values.RootDirSecs)));
	
	//Calculates CountofClus to use for determining FATtype
	CountofClus = DataSec / Drive_values.SecPerClus;

	//Determines FAT type
	if(CountofClus < 65525)
	{
		//FAT16
		Drive_values.FATtype = FAT16;
		error_flag = FAT_Unsupported;
	}
	else
	{
		//FAT32
		Drive_values.FATtype = FAT32;
	}
	//printf("FATtype Detected: %x\r\n", Drive_values.FATtype);
	return error_flag;
}

uint32_t First_Sector (uint32_t Cluster_num)
{
    uint32_t FirstSecCluster;
    if(Cluster_num==0)
    {
        FirstSecCluster = Drive_values.FirstRootDirSec;
    }
    else
    {
        FirstSecCluster = ((Cluster_num-2)*Drive_values.SecPerClus)+Drive_values.FirstDataSec;
    }
    return FirstSecCluster;
}

uint32_t Find_Next_Clus(uint32_t Cluster_num, uint8_t xdata * array_name)
{
    uint32_t return_clus;
	uint16_t FAToffset;
	uint32_t sector = ((Cluster_num*4)/Drive_values.BytesPerSec)+Drive_values.StartofFAT;
	Read_Sector(sector,Drive_values.BytesPerSec,array_name);
	FAToffset = (uint16_t) ((4*Cluster_num)%Drive_values.BytesPerSec);
	return_clus = (read32(FAToffset,array_name)&0x0FFFFFFF);
	
    return return_clus;
}

uint8_t Open_File(uint32_t Cluster, uint8_t xdata * array_in)
{
	uint8_t error_flag = no_errors;
	uint32_t sector_num;
	uint32_t first_sec_num;

	do
	{
		first_sec_num = First_Sector(Cluster);
		sector_num = first_sec_num;
		while(sector_num!=Drive_values.SecPerClus+first_sec_num)
		{
			error_flag = Read_Sector(sector_num,Drive_values.BytesPerSec, array_in);
			sector_num++;
		}
		Cluster = Find_Next_Clus(Cluster,array_in);
	}while(Cluster!=0x0FFFFFFF);
	return error_flag;
}

