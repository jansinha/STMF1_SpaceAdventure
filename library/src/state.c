#include "state.h"
#include "st7735_draw.h"

void state_init( Spaceship_State *SS, Player_State *PS )
{
  PS->spaceship = SS; // Initialize memory for the spaceship structure

  // initial position of spaceship sprite
  PS->spaceship->xS_accel_curr = 10;
  PS->spaceship->yS_accel_curr = 30;
  // end position of spaceship sprite
  PS->spaceship->xE_accel_curr = PS->spaceship->xS_accel_curr + LETTER_WIDTH;
  PS->spaceship->yE_accel_curr = PS->spaceship->yS_accel_curr + LETTER_HEIGHT;
  // last position of spaceship sprite
  PS->spaceship->xS_accel_last = PS->spaceship->xS_accel_curr;
  PS->spaceship->yS_accel_last = PS->spaceship->yS_accel_curr;
  PS->spaceship->xPos_accel = 0;
  PS->spaceship->yPos_accel = 0;
  PS->spaceship->press[2];
  PS->spaceship->shipWidth = 8;
  PS->spaceship->shipHeight = 8;
  PS->spaceship->lives = 4;
  PS->level = 1;
  PS->points = 0;
  PS->fuel = 90;
  PS->land = 0;
  PS->crash = 0;
}

void last_state( Player_State *PS, Player_State *LastPS )
{
  PS->spaceship->xS_accel_curr = 30;
  PS->spaceship->yS_accel_curr = 30;
  PS->spaceship->xE_accel_curr = PS->spaceship->xS_accel_curr + LETTER_WIDTH;
  PS->spaceship->yE_accel_curr = PS->spaceship->yS_accel_curr + LETTER_HEIGHT;
  PS->spaceship->xS_accel_last = PS->spaceship->xS_accel_curr;
  PS->spaceship->yS_accel_last = PS->spaceship->yS_accel_curr;
  PS->spaceship->xPos_accel = 0;
  PS->spaceship->yPos_accel = 0;
  PS->spaceship->lives--;
  PS->points = LastPS->points;
  PS->fuel = 90;
  PS->land = 0;
  PS->crash = 0;
}


void update_state( Player_State *PS )
{
  // initial position of spaceship sprite
  PS->spaceship->xS_accel_curr = 30;
  PS->spaceship->yS_accel_curr = 30;
  // end position of spaceship sprite
  PS->spaceship->xE_accel_curr = PS->spaceship->xS_accel_curr + LETTER_WIDTH;
  PS->spaceship->yE_accel_curr = PS->spaceship->yS_accel_curr + LETTER_HEIGHT;
  // last position of spaceship sprite
  PS->spaceship->xS_accel_last = PS->spaceship->xS_accel_curr;
  PS->spaceship->yS_accel_last = PS->spaceship->yS_accel_curr;
  PS->spaceship->xPos_accel = 0;
  PS->spaceship->yPos_accel = 0;
  PS->spaceship->lives = 4;
  PS->level++;
  PS->fuel = 90;
  PS->land = 0;
  PS->crash = 0;
}


void update_fuel( Player_State *ptrPS )
{
  if (ptrPS->spaceship->press[0] == 0)
    {
      ptrPS->fuel -= 5;
      ptrPS->spaceship->press[0] = 1;
    }
  if (ptrPS->spaceship->press[1] == 0)
    {
      ptrPS->fuel -= 10;
      ptrPS->spaceship->press[1] = 1;
    }
}

void update_points( Player_State *ptrPS )
{
  ptrPS->points = ( (ptrPS->fuel)/5 + (ptrPS->spaceship->lives)*3 ) * ptrPS->level;
}

void collision_check( Player_State *ptrPS )
{
  uint16_t counter = 0;
  uint16_t Xstart, Xend, Ystart, Yend;
  switch ( ptrPS->level ) {
  case (1):
    Xstart = 80;
    Xend = 100;
    Ystart = 74;
    Yend = 128;

    break;
  case (2):
    Xstart = 100;
    Xend = 120;
    Ystart = 64;
    Yend = 128;

    break;
  case (3):
    Xstart = 40;
    Xend = 55;
    Ystart = 50;
    Yend = 128;

    break;
  case (4):
    Xstart = 120;
    Xend   = 135;
    Ystart = 100;
    Yend   = 128;

    break;
  default:
    break;
  }

  if ( ( ( Xstart - ptrPS->spaceship->xS_accel_curr ) > 0 ) &&	 
       ( ( Xstart - ptrPS->spaceship->xE_accel_curr ) < 0 ) &&	 
       ( ( Ystart - ptrPS->spaceship->yS_accel_curr ) < 0 ) &&
       ( ( Ystart - ptrPS->spaceship->yE_accel_curr ) < 0 ) )
    {
      counter = 1;
    }

  if ( ( ( Xend   - ptrPS->spaceship->xS_accel_curr ) > 0 ) &&
       ( ( Xend   - ptrPS->spaceship->xE_accel_curr ) < 0 ) &&	 
       ( ( Ystart - ptrPS->spaceship->yS_accel_curr ) < 0 ) &&
       ( ( Ystart - ptrPS->spaceship->yE_accel_curr ) < 0 ) )
    {
      counter = 1;
    }

  // NEED TO ADD COLLISON WITH BOTTOM OF SCREEN
  if ( ptrPS->spaceship->yE_accel_curr > ST7735_WIDTH )
      counter = 1;

  if (counter > 0) {
    ptrPS->crash = 1;
    counter = 0;
  }
}

void landing_check( Player_State *ptrPS )
{
  uint16_t Xstart, Xend, Ystart, Yend;
  switch ( ptrPS->level ) {
  case (1):
    Xstart = 80;
    Xend = 100;
    Ystart = 74;
    Yend = 128;
    
    break;
  case (2):
    Xstart = 100;
    Xend = 120;
    Ystart = 64;
    Yend = 128;
    
    break;
  case (3):
    Xstart = 40;
    Xend = 55;
    Ystart = 50;
    Yend = 128;
    
    break;
  case (4):
    Xstart = 120;
    Xend   = 135;
    Ystart = 100;
    Yend   = 128;
    
    break;
  default:
    break;
  }

  if ( ( ( Xstart - ptrPS->spaceship->xS_accel_curr ) < 0 ) &&
       ( ( Xstart - ptrPS->spaceship->xE_accel_curr ) < 0 ) &&	 
       ( ( Xend   - ptrPS->spaceship->xS_accel_curr ) > 0 ) &&
       ( ( Xend   - ptrPS->spaceship->xE_accel_curr ) > 0 ) &&
       ( ( Ystart - ptrPS->spaceship->yS_accel_curr ) > 0 ) &&
       ( ( Ystart - ptrPS->spaceship->yE_accel_curr ) <= 0 ) )
    {
      ptrPS->land = 1;
    }
}
