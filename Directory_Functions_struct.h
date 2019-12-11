/* Names: Tyler Andrews, Brennan Campbell, and Tyler Tetens
 * Title: Experiment #4 - Fat File System
 */
#ifndef _Directory_Func_H
#define _Directory_Func_H

#include "Main.H"

//------- Public Constants  -----------------------------------------
#define FAT32 (4)
#define FAT16 (2)
#define FAT32_shift (2)
#define FAT16_shift (1)

#define Disk_Error (0xF0)
#define FAT_Unsupported (0xF1)
#define No_Disk_Error (0)
#define more_entries (0x8000)   
#define no_entry_found (0x80000000)  // msb set to indicate error
#define directory_bit  (0x10000000)  // lsb of first nibble (bit28)

//----------------- Typedefs ----------------------------------------

/*
 * Desc: This struct holds all constants from BPB on SD card
 */
typedef struct
{
  uint8_t SecPerClus;
  uint8_t FATtype;
  uint8_t BytesPerSecShift; // Not used?
  uint8_t FATshift;
  uint16_t BytesPerSec;
  uint32_t FirstRootDirSec;
  uint32_t FirstDataSec;
  uint32_t StartofFAT;
  uint32_t RootDirSecs;
} FS_values_t;

// ------ Public function prototypes -------------------------------

// Docs in this function are in Directory_Functions_struct.c
uint16_t Print_Directory(uint32_t Sector_num, uint8_t xdata * array_in);

// Docs in this function are in Directory_Functions_struct.c
uint32_t Read_Dir_Entry(uint32_t Sector_num, uint16_t Entry, uint8_t xdata * array_in);

// Docs in this function are in Directory_Functions_struct.c
FS_values_t * Export_Drive_values(void);

// ------ Function prototypes needed (These can be defined in a seperate file) -------------

/*
 * Desc: Returns a byte read from array_name with some offset
 * Pre: data must already be read into xdata before passing
 */
uint8_t read8(uint16_t offset, uint8_t * array_name);

/*
 * Desc: Returns 2 bytes read from array_name with some offset
 * Note: corrects for little endian
 * Pre: data must already be read into xdata before passing
 */
uint16_t read16(uint16_t offset, uint8_t * array_name);

/*
 * Desc: Returns 4 bytes read from array_name with some offset
 * Note: corrects for little endian
 * Pre: data must already be read into xdata before passing
 */
uint32_t read32(uint16_t offset, uint8_t * array_name);

/*
 * Desc: Mounts the SD card
 * Pre: SPI mst be initializing before calling this function
 * Notes: array_name is used to read in MBR and BPB sectors
 * Warning: array_name will be modified
 */
uint8_t Mount_Drive(uint8_t xdata * array_name);

/*
 * Desc: Finds and returns the first sector of cluster passed to it
 */
uint32_t First_Sector (uint32_t Cluster_num);

/*
 * Desc: Finds the next cluster of data
 * Warning: array_name will be modified
 */
uint32_t Find_Next_Clus(uint32_t Cluster_num, uint8_t xdata * array_name);

/*
 * Desc: Reads the data of a file and outputs to screen
 * Warning: array_name will be modified
 */
uint8_t Open_File(uint32_t Cluster, uint8_t xdata * array_in);


#endif