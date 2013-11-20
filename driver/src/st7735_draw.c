#include "state.h"
#include "st7735_draw.h"
#include "glcdfont.h"
#include "sprites.h"


void newline(uint16_t x0, uint16_t y0, uint16_t xE, uint16_t yE, uint16_t bg)
{// Not fully implemented
  x0 = 0;
  y0 += LETTER_HEIGHT;
  xE = x0 + LETTER_WIDTH;
  yE = y0 + LETTER_HEIGHT;
  fillScreen(bg); // Clear screen by painting it all white
}

void newPage(uint16_t x0, uint16_t y0, uint16_t xE, uint16_t yE, uint16_t bg)
{ // Not fully implemented
  x0 = 0; // Start x-position
  y0 = 0; // Start y-position
  xE = x0 + LETTER_WIDTH;
  yE = y0 + LETTER_HEIGHT;
  fillScreen(bg); // Clear screen by painting it all white
}

void buildCharArray( uint8_t Px, uint16_t *colorArray , uint16_t bg , uint16_t fg)
{
  uint8_t idx1, idx2;
  uint16_t letter[5];

  for (idx1=0; idx1 < CHAR_WIDTH; idx1++)
    letter[idx1] = ASCII[ (Px * CHAR_WIDTH) + idx1 ];

  // Build color array for drawing window
  for (idx1 = 0; idx1 < CHAR_HEIGHT; idx1++) { // Iterate through rows
    for (idx2 = 0; idx2 < CHAR_WIDTH; idx2++) { // Iterate through columns
      if ( letter[ idx2 ] & 0x1 )
	colorArray[ (LETTER_WIDTH * idx1) + idx2 ] = bg;
      else
	colorArray[ (LETTER_WIDTH * idx1) + idx2 ] = fg;
      letter[ idx2 ] >>= 1; //Shift bits right by 1
    }
  }
}

void buildSprites( uint8_t Px, uint16_t *colorArray , uint16_t bg , uint16_t fg)
{
  uint8_t idx1, idx2;
  uint16_t sprite[8];

  for (idx1=0; idx1 < SPRITE_WIDTH; idx1++)
    sprite[idx1] = SPRITES[ (Px * SPRITE_WIDTH) + idx1 ];

  // Build color array for drawing window
  for (idx1 = 0; idx1 < SPRITE_HEIGHT; idx1++) { // Iterate through rows
    for (idx2 = 0; idx2 < SPRITE_WIDTH; idx2++) { // Iterate through columns
      if ( sprite[ idx2 ] & 0x1 )
        colorArray[ (SPRITE_WIDTH * idx1) + idx2 ] = bg;
      else
        colorArray[ (SPRITE_WIDTH * idx1) + idx2 ] = fg;
      sprite[ idx2 ] >>= 1; //Shift bits right by 1
    }
  }
}



