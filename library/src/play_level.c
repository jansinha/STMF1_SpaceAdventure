#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_spi.h>
#include <stm32f10x_i2c.h>
#include <stm32f10x_usart.h>
#include "xprintf.h"
#include "spidma.h"
#include "I2C.h"
#include "st7735_init.h"
#include "st7735_draw.h"
#include "WiiLCDTranslate.h"
#include "uart.h"
#include "state.h"

extern void Delay(uint32_t nTime);
extern void myputchar ( unsigned char c);
extern unsigned char mygetchar ();

void play_level( Player_State *ptrPS )
{
  // Parameters & Constants for the Wii
  const uint8_t buf_init [] = {0xf0 , 0x55 };
  const uint8_t buf2_init [] = {0xfb , 0x00 };
  const uint8_t buf_read [] = {0};
  uint8_t data[6];

  // Initialize the Wii Nunchuk
  int ClockSpeed = 10000;
  I2C_LowLevel_Init( I2C1, ClockSpeed, NUNCHUK_ADDRESS );
  I2C_Write( I2C1, buf_init, 2, NUNCHUK_ADDRESS );
  I2C_Write( I2C1, buf2_init, 2, NUNCHUK_ADDRESS );

  // Parameters & Constants for the LCD
  int temp = 0;
  char msg_Z = 'A';
  char msg_blank = " ";
  uint8_t Px_Z = msg_Z, Px_Blank = (uint8_t) msg_blank;
  uint16_t colorArray[] = {[0 ... 59] = black}; // Using a c99 complier enhancement

  // Read data from Wii Nunchuk
  I2C_Write( I2C1, buf_read, 1, NUNCHUK_ADDRESS );
  I2C_Read( I2C1, data, 6, NUNCHUK_ADDRESS );

  // Custom function to find Nunchuk position information
  WiiAccelerometer_LCD( data, &(ptrPS->spaceship->xPos_accel),
			&(ptrPS->spaceship->yPos_accel) );
  WiiButton_LCD( data, ptrPS->spaceship->press );

  // DEBUG - WRITE DATA TO THE COMPUTER SCREEN USING USART
  xprintf("JStick Values: %d %d\n", data[2], data[3]);
  xprintf("JStick Offset: %d %d\n", &(ptrPS->spaceship->xPos_accel),
	  &(ptrPS->spaceship->yPos_accel));
  xprintf("C Button: %d, Z Button: %d\n\n",ptrPS->spaceship->press[0],
	  ptrPS->spaceship->press[1]);

  // Determine x-coordinates for accelerometers
  // Save current x_position to last x_position
  ptrPS->spaceship->xS_accel_last = ptrPS->spaceship->xS_accel_curr;
  temp = ptrPS->spaceship->xS_accel_curr;
  temp += ptrPS->spaceship->xPos_accel; // Create new current x_position
  if (temp < 0 + LETTER_WIDTH)
    ptrPS->spaceship->xS_accel_curr = 0;
  else if (temp > (ST7735_HEIGHT - LETTER_WIDTH) )
    ptrPS->spaceship->xS_accel_curr = ST7735_HEIGHT - LETTER_WIDTH;
  else
    ptrPS->spaceship->xS_accel_curr = (uint8_t) temp;

  // Determine y-coordinates accelerometers
  // Save current_ positions to last_positions
  ptrPS->spaceship->yS_accel_last = ptrPS->spaceship->yS_accel_curr ; 
  temp = ptrPS->spaceship->yS_accel_curr ; 
  temp += ptrPS->spaceship->yPos_accel; // Create new current_positions
  if (temp < LETTER_HEIGHT + 4)
    (ptrPS->spaceship->yS_accel_curr) = LETTER_HEIGHT + 4;
  else if (temp > (ST7735_WIDTH - LETTER_HEIGHT ) )
    (ptrPS->spaceship->yS_accel_curr) = ST7735_WIDTH - LETTER_HEIGHT/2;
  else
    (ptrPS->spaceship->yS_accel_curr) = (uint8_t) temp;
  
  //Handle "gravity"
  if ( !(ptrPS->spaceship->press[0]) ) {
    (ptrPS->spaceship->yS_accel_curr) -= 4; // Move toward top of screen by 1 pixels
    if ( ptrPS->spaceship->yS_accel_curr < LETTER_HEIGHT + 4)
      (ptrPS->spaceship->yS_accel_curr) = LETTER_HEIGHT + 4;
  }
  if ( !(ptrPS->spaceship->press[1]) ) {
    (ptrPS->spaceship->yS_accel_curr) -= 8; // Move toward top of screen by 2 pixels
    if ( ptrPS->spaceship->yS_accel_curr < LETTER_HEIGHT + 4)
      (ptrPS->spaceship->yS_accel_curr) = LETTER_HEIGHT + 4;
  }
  (ptrPS->spaceship->yS_accel_curr) += 4; // Steady descent due to "gravity"

  // ACCELEROMETERS
  // Blank out sprite in previous location
  buildCharArray(Px_Blank, colorArray, black, black);
  ST7735_setAddrWindow(ptrPS->spaceship->xS_accel_last,
		       ptrPS->spaceship->yS_accel_last,
		       ptrPS->spaceship->xS_accel_last+LETTER_WIDTH-1,
		       ptrPS->spaceship->yS_accel_last+LETTER_HEIGHT-1,
		       MADCTLGRAPHICS);
  ST7735_pushColor(colorArray, BLOCK_SIZE);      

  // Paint sprite at new location
  ptrPS->spaceship->xE_accel_curr = ptrPS->spaceship->xS_accel_curr+LETTER_WIDTH-1;
  ptrPS->spaceship->yE_accel_curr = ptrPS->spaceship->yS_accel_curr+LETTER_HEIGHT-1,
  buildCharArray(Px_Z, colorArray, green, black);
  ST7735_setAddrWindow(ptrPS->spaceship->xS_accel_curr,
		       ptrPS->spaceship->yS_accel_curr,
		       ptrPS->spaceship->xE_accel_curr,
		       ptrPS->spaceship->yE_accel_curr,
		       MADCTLGRAPHICS);
  ST7735_pushColor(colorArray, BLOCK_SIZE);

}


// Timer code
static __IO uint32_t TimingDelay;
extern void Delay( uint32_t nTime );
extern void SysTick_Handler (void);
extern void assert_failed (uint8_t * file, uint32_t line);

