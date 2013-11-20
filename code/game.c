#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>
#include <stdio.h>
#include "I2C.h"
#include "spidma.h"
#include "xprintf.h"
#include "st7735_init.h"
#include "st7735_draw.h"
#include "uart.h"
#include "WiiLCDTranslate.h"
#include "state.h"
#include "landscape.h"

/* Main Game Entry point and processing

  Game flow
    0) Initialize player structure
    1) Initialize landscape structure
    2) Start scheduler, which handles different events at different intervals
    Some events
    3) Display values in status bar
    4) Read Nunchuk and move player
    5) Change fuel based on button presses
    6) Check if craft was landed
    7) Check if cradt collided
    8) Fuel cost for maintaining systems
    9) If Crash or No Fuel -> Restart level
   10) If crash, load last state, reset landscape

  Completed:
  1) Use Systick timer to control the fuel decrease rate.
  2) Other behavior, like small or large vertical rises, will reduce fuel.
  3) 'Gravity' is implemented as shift in the ship position downward by a fixed small amount every iteration

*/

void Delay(uint32_t nTime);
void myputchar ( unsigned char c);
unsigned char mygetchar ();

// Parameters & Constants 
uint8_t flag10 = 0;
uint8_t flag50 = 0;
uint8_t flag200 = 0;
uint8_t flag5000 = 0;
int status = 0;

void main( void ) {
  // Configure SysTick Timer
  if ( SysTick_Config(SystemCoreClock / 1000) )
    while (1);

  // Initialize uart SO WE CAN debug
  uart_open(USART1, 9600, 0);
  xfunc_in = mygetchar;
  xfunc_out = myputchar;
  
  //Initialize SPI
  spiInit( SPI2 );       // Initialize SPI protocol

  // Initialize LCD screen and set it to BLACK
  ST7735_init( );        // Initialize LCD display
  fillScreen(black);     // Set LCD background to black
  ST7735_backLight( 1 ); // Turn on LCD backlight

  // Create player and spaceship structures
  Player_State playerstate;           // Create Player_State structure
  Spaceship_State spaceship;          // Create Spaceship_State
  Player_State *ptrPlayer = &playerstate; // Create Player_State pointer
  Player_State LastPlayerState;       // Create a copy of Player_State
  Spaceship_State LastSpaceship;
  Player_State *ptrLastPlayerState = &LastPlayerState;

  // Initialize player state structures
  state_init( &spaceship, &playerstate );
  state_init( &LastSpaceship, &LastPlayerState );

  // Initialize landscapes
  Landing_Site Levels[10];
  Landing_Site *ptrLandscape = Levels;
  LevelInit( ptrLandscape );

  // Show start splash screen : load BMP from SD card
  loadBMP("TITLE150.BMP");
  play_sound("XFLYBY1.WAV");
  Delay(3000);
  fillScreen(black);     // Set LCD background to black

  // Initialize the lanscape structure
  landscape( ptrPlayer->level, ptrLandscape );

    // Start Game Loop
  while ( ptrPlayer->spaceship->lives ) {
    while ( !(ptrPlayer->land) && ptrPlayer->spaceship->lives ) {
      if ( !(ptrPlayer->crash) && ptrPlayer->fuel  ) {
        if (flag50 == 1) {
          display_state( ptrPlayer ); // Display values in status bar
          flag50 = 0;
        }
        if (flag200 == 1) {
          play_level( ptrPlayer ); // Read Nunchuk and move player
          update_fuel( ptrPlayer ); // Change fuel based on button presses
          landing_check( ptrPlayer ); // Check if craft was landed
          collision_check( ptrPlayer ); // Check if cradt collided
          if (ptrPlayer->land)
            ptrPlayer->crash = 0;
          flag200 = 0;
        }	    
        if (flag5000 == 1) {
          ptrPlayer->fuel -= 10; // fuel cost for maintaining systems
          flag5000 = 0;
        }
      }
  	  else {// If Crash or No Fuel -> Restart level
        last_state( ptrPlayer, ptrLastPlayerState );
        loadBMP("CRASH.BMP");
        Delay(2000);
        fillScreen(black); // Set LCD background to black
        landscape( ptrPlayer->level, ptrLandscape );
      }
    }
    if (ptrPlayer->land) {
      update_points( ptrPlayer );
      update_state( ptrPlayer );
    	update_state( ptrLastPlayerState );
    	ptrLastPlayerState->points = ptrPlayer->points;
    	loadBMP("NEWLEVEL.BMP");
    	Delay(2000);
    	fillScreen(black);
    	landscape( ptrPlayer->level, ptrLandscape );
    }
  }
  display_state( ptrPlayer ); // Display values in status bar

  // Show end splash screen : load BMP from SD card
  loadBMP("GAMEOVER.BMP");
}

void myputchar ( unsigned char c) {
  uart_putc( c, USART1 );
}

unsigned char mygetchar () {
  return uart_getc( USART1 );
}

// Timer code
static __IO uint32_t TimingDelay;

void Delay( uint32_t nTime ){
  TimingDelay = nTime;
  while ( TimingDelay != 0 );
}
// Enable scheduler to schedule events at different granularity
void SysTick_Handler (void){
  if ( TimingDelay != 0x00 )
    TimingDelay--;

  static uint16_t count = 0;
  count++;
  if( (count % 10) == 0)
    flag10 = 1;
  if( (count % 50) == 0)
    flag50 = 1;
  if( (count % 200) == 0)
    flag200 = 1;
  if( (count % 5000) == 0)
    flag5000 = 1;
}

#ifdef USE_FULL_ASSERT
void assert_failed (uint8_t * file, uint32_t line){
  /* Infinite loop */
  /* Use GDB to find out why we're here */
  while ( 1 );
}
#endif
