#ifndef STATE_H
#define STATE_H

#include <stdint.h>

/* This header creates two data structures.
   This design facilitates future extension to multiple players.
 - Spaceship_State:
 Parameters that define the state and parameters of a spaceship
 - Player_State:
 Parameters that define the players status in the game.
 Spaceship_State is a nested structure that has player's spaceship status.
*/

typedef struct
{
  uint16_t xS_accel_curr;
  uint16_t yS_accel_curr; // position of spaceship sprite
  uint16_t xE_accel_curr;
  uint16_t yE_accel_curr; // last position of spaceship sprite
  uint16_t xS_accel_last;
  uint16_t yS_accel_last;
  int16_t xPos_accel;
  int16_t yPos_accel;
  uint8_t press[2];
  uint8_t shipWidth;
  uint8_t shipHeight;
  uint8_t lives; // number of lives
}Spaceship_State;

typedef struct
{
  Spaceship_State *spaceship;
  uint8_t level;  // current level
  uint16_t points; // current score
  uint8_t fuel;   // current fuel supply
  uint8_t land;
  uint8_t crash;
}Player_State;

void state_init( Spaceship_State *ptrSS, Player_State *ptrPS );
void play_level( Player_State *ptrPS );
void display_state( Player_State *ptrPS );
void last_state( Player_State *PS, Player_State *LastPS );
void update_state( Player_State *ptrPS );
void update_fuel( Player_State *ptrPS );
void update_points( Player_State *ptrPS );
void collision_check( Player_State *ptrPS );
void collision_check( Player_State *ptrPS );
void landing_check( Player_State *ptrPS );

#endif
