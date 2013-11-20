#include "WiiLCDTranslate.h"
#include <st7735_init.h>

void WiiJoystick_LCD ( uint8_t* data, int8_t* x_pos, int8_t* y_pos )
{
  int16_t JX = data[0] - ST7735_WIDTH; // Center data
  int16_t JY = data[1] - ST7735_WIDTH;

  *x_pos = (int8_t) (JX * 64 / ST7735_WIDTH) / 5; // Renormalize
  *y_pos = (int8_t) -(JY * 80 / ST7735_WIDTH) / 5;
}

void WiiButton_LCD ( uint8_t* data, uint8_t* button_press )
{
  button_press[0] = data[5] & 0x01;
  button_press[1] = (data[5] >> 1) & 0x01;
}

void WiiAccelerometer_LCD ( uint8_t* data, int16_t* x_pos, int16_t* y_pos )
{
  float pitch, roll;
  int16_t AX = data[2] << 2;
  int16_t AY = data[3] << 2;
  int16_t AZ = data[4] << 2;
  AZ += ( ( ( data[5] >> 6 ) & 0x03 ) - 512 );
  AY += ( ( ( data[5] >> 4 ) & 0x03 ) - 512 );
  AX += ( ( ( data[5] >> 2 ) & 0x03 ) - 512 );

  pitch = atan( AX / sqrt( AY*AY + AZ*AZ ) );
  roll = atan( AY / sqrt( AX*AX + AZ*AZ ) );

  *x_pos = (int16_t) (pitch * ST7735_HEIGHT / PI);
  *y_pos = (int16_t) (roll * ST7735_WIDTH / PI);
}


/*
  float pitch, roll;
  int16_t AX = data[2] << 2;
  int16_t AY = data[3] << 2;
  int16_t AZ = data[4] << 2;
  AZ += ( ( ( data[5] >> 6 ) & 0x03 ) - 512 );
  AY += ( ( ( data[5] >> 4 ) & 0x03 ) - 512 );
  AX += ( ( ( data[5] >> 2 ) & 0x03 ) - 512 );

  pitch = atan( AX / sqrt( AY*AY + AZ*AZ ) );
  roll = atan( AY / sqrt( AX*AX + AZ*AZ ) );

  *x_pos = (int16_t) (pitch * 128 / PI);
  *y_pos = (int16_t) (roll * 160 / PI);

 */
