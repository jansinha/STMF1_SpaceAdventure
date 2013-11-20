#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>
#include "spi_drive.h"
#include "st7735_init.h"
#include "st7735_draw.h"
#include "state.h"
#include <stdio.h>
#include <stdint.h>

/* Display the status of the player's spaceship.
   Sets pixels at the top of the screen that display from left to right:
   1) Lives (4 colored boxes),
   2) Level (1 digit),
   3) Points (3 digits), and
   4) Fuel level (9 colored boxes. L-R: 3 green boxes, 3 yellow boxes, 3 red boxes).
   
   Input:
   Player_State structure

   To Do:
   Add commands to pushColor to different parts of status bar.
   The values and colors are determined by values in Player_State.
*/

void display_state( Player_State *ptrPS )
{
  uint8_t idx1, idx2, i;
  uint8_t blockSize = LETTER_HEIGHT * LETTER_WIDTH;
  uint16_t x0 = 0; // Start x-position
  uint16_t y0 = 2; // Start y-position
  uint16_t xE = x0 + LETTER_WIDTH; // End x-position
  uint16_t yE = y0 + LETTER_HEIGHT; // End y-position

  uint8_t Px;
  char *msg = "L ";
  char *space = " ";
  uint16_t colorArray[] = {[0 ... 59] = black}; // Using a c99 complier enhancement
  uint16_t spriteArray[] = {[0 ... 63] = black}; // Using a c99 complier enhancement  

  // Display the number of LIVES
  int nlives = ptrPS->spaceship->lives;
  for( i=0; i<8; i++ )
    {
      if( ((i+1) % 2)==1 )
	Px = msg[1];
      else if( nlives ) {
	Px = msg[0];
	nlives = nlives - 1;
      }
      else
	Px = msg[1];
      
      buildCharArray( Px, colorArray, yellow, black );
      // Output pixels, Px, for the current character
      ST7735_setAddrWindow(x0, y0, xE-1, yE-1, MADCTLGRAPHICS);
      ST7735_pushColor( colorArray, blockSize );
      
      // Set the drawing window for the next character
      x0 += LETTER_WIDTH; // Shift 6 pixels for next character
      xE = x0 + LETTER_WIDTH;
  }

  // Insert a space
  Px = space[0];
  buildCharArray( Px, colorArray, yellow, black );
  // Output pixels, Px, for the current character
  ST7735_setAddrWindow(x0, y0, xE-1, yE-1, MADCTLGRAPHICS);
  ST7735_pushColor( colorArray, blockSize );
  // Set the drawing window for the next character
  x0 += LETTER_WIDTH; // Shift 6 pixels for next character
  xE = x0 + LETTER_WIDTH;


  // Display LEVEL number. 48 is the offset into the glcdfont file to get numbers.
  buildCharArray( ptrPS->level + 48, colorArray, yellow, black );
  // Output pixels, Px, for the current character
  ST7735_setAddrWindow(x0, y0, xE-1, yE-1, MADCTLGRAPHICS);
  ST7735_pushColor( colorArray, blockSize );
  // Set the drawing window for the next character
  x0 += LETTER_WIDTH; // Shift 6 pixels for next character
  xE = x0 + LETTER_WIDTH;


  // Insert space
  Px = space[0];
  buildCharArray( Px, colorArray, yellow, black );
  // Output pixels, Px, for the current character
  ST7735_setAddrWindow(x0, y0, xE-1, yE-1, MADCTLGRAPHICS);
  ST7735_pushColor( colorArray, blockSize );
  // Set the drawing window for the next character
  x0 += LETTER_WIDTH; // Shift 6 pixels for next character
  xE = x0 + LETTER_WIDTH;


  // Display the POINTS scored
  uint16_t digit[4], counter=4;
  uint16_t pts = ptrPS->points;
  for (i=0; i<4; i++) // reverse order of numbers
    {
      digit[i] = (pts % 10);
      pts = pts / 10;
    }
  for (i=0; i<4; i++)
    {
      if ( digit[counter-i-1] )
	Px = digit[counter-i-1] + 48;
      else
	Px = '0';

      buildCharArray( Px, colorArray, yellow, black );
      // Output pixels, Px, cfor the current character
      ST7735_setAddrWindow(x0, y0, xE-1, yE-1, MADCTLGRAPHICS);
      ST7735_pushColor( colorArray, blockSize );

      // Set the drawing window for the next character
      x0 += LETTER_WIDTH; // Shift 6 pixels for next character
      xE = x0 + LETTER_WIDTH;
  }


  // Insert space
  Px = space[0];
  buildCharArray( Px, colorArray, yellow, black );
  // Output pixels, Px, for the current character
  ST7735_setAddrWindow(x0, y0, xE-1, yE-1, MADCTLGRAPHICS);
  ST7735_pushColor( colorArray, blockSize );
  // Set the drawing window for the next character
  x0 += LETTER_WIDTH; // Shift 6 pixels for next character
  xE = x0 + LETTER_WIDTH;


  // Display FUEL level
  float fuel = ptrPS->fuel;
  for( i=0; i<9; i++ )
    {
      // Get character in string. If == 0, end of string
      Px = 222;
      if (fuel > 0) {
	fuel-=10;
	if (i<3)
	  buildCharArray( Px, colorArray, red, black );
	else if (i<6)
	  buildCharArray( Px, colorArray, yellow, black );
	else
	  buildCharArray( Px, colorArray, green, black );
      }
      else // Print a blank to erase any previous fuel block 
	buildCharArray( Px, colorArray, black, black );
    
      // Output pixels, Px, for the current character
      ST7735_setAddrWindow(x0, y0, xE-1, yE-1, MADCTLGRAPHICS);
      ST7735_pushColor( colorArray, blockSize );
      
      // Set the drawing window for the next character
      x0 += LETTER_WIDTH; // Shift 6 pixels for next character
      xE = x0 + LETTER_WIDTH;
    }
}
