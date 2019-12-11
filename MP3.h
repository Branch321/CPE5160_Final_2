#ifndef _MP3_H
#define _MP3_H

#include "Main.h"

/* DESC: Function is used to play mp3's by initializing the sEOS 
 * Variables: begin_cluster - the first cluster of the mp3 file to be played
*/
void Play_MP3_file(uint32_t begin_cluster);

/* DESC: Timeout used for the state machine to play music
 * Warnings: Uses timer0
*/
void MP3_clock_reset(void);

#endif
