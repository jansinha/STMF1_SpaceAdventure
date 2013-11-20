#ifndef WIILCDTRANSLATE_H
#define WIILCDTRANSLATE_H

#include <math.h>
#include <stm32f10x.h>

#define PI 3.14159265359
#define NUNCHUK_ADDRESS 0xA4

void WiiJoystick_LCD( uint8_t* data, int8_t *x_pos, int8_t *y_pos );
void WiiButton_LCD( uint8_t* data, uint8_t* button_press );
void WiiAccelerometer_LCD( uint8_t* data, int16_t *x_pos, int16_t *y_pos );


#endif
