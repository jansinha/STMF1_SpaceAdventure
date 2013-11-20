#ifndef ST7735_DRAW_H
#define ST7735_DRAW_H

#include <stdint.h>
#include "st7735_init.h"

#define LETTER_WIDTH 6
#define LETTER_HEIGHT 10
#define CHAR_WIDTH 5
#define CHAR_HEIGHT 8
#define SPRITE_WIDTH  8
#define SPRITE_HEIGHT 8
#define BLOCK_SIZE 60 //= LETTER_HEIGHT * LETTER_WIDTH


#define black   0x0000
#define blue    0x001F
#define green   0x07E0
#define cyan    0x07FF
#define red     0xF800
#define magenta 0xF81F
#define yellow  0xFFE0
#define white   0xFFFF


void newline( uint16_t x0, uint16_t y0, uint16_t xE, uint16_t yE, uint16_t bg );
void newPage( uint16_t x0, uint16_t y0, uint16_t xE, uint16_t yE, uint16_t bg );
void buildCharArray( uint8_t Px, uint16_t *colorArray, uint16_t fg, uint16_t bg );
void buildSpriteArray( uint8_t Px, uint16_t *colorArray, uint16_t fg, uint16_t bg );


#endif
