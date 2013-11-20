#ifndef PLAYER_H
#define PLAYER_H

#define AUDIOBUFSIZE 128

extern uint8_t Audiobuf[]; 
extern int audioplayerHalf;
extern int audioplayerWhole;

void audioplayerInit( unsigned int sampleRate );
void audioplayerStart( );
void audioplayerStop( );

#endif
