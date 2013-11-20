#include <stm32f10x_rcc.h>
#include <stm32f10x_spi.h>
#include <stm32f10x_i2c.h>
#include "spidma.h"
#include "I2C.h"
#include "st7735_init.h"
#include "st7735_draw.h"
#include "state.h"
#include "landscape.h"


void LevelInit( Landing_Site *LS )
{
  LS[0].Xstart = 80;
  LS[0].Ystart = 100;
  LS[0].Xend   = 74;
  LS[0].Yend   = 128;
  LS[0].color  = green;
  
  LS[1].Xstart = 100;
  LS[1].Ystart = 120;
  LS[1].Xend   = 64;
  LS[1].Yend   = 128;
  LS[1].color  = green;

  LS[2].Xstart = 40;
  LS[2].Ystart = 55;
  LS[2].Xend   = 50;
  LS[2].Yend   = 128;
  LS[2].color  = yellow;

  LS[3].Xstart = 120;
  LS[3].Ystart = 100;
  LS[3].Xend   = 135;
  LS[3].Yend   = 128;
  LS[3].color  = yellow;
}


void landscape( int level, Landing_Site *LS )
{
  // Parameters & Constants for the LCD
  int x, y;
  uint16_t Xstart, Xend, Ystart, Yend;
  uint16_t color = {green};
  uint16_t *ptrcolor = color;

  switch ( level ) {
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

  ST7735_setAddrWindow(Xstart, Ystart, Xend, Yend, MADCTLGRAPHICS);
  for (x=0; x < (Xend-Xstart); x++) {
    for (y=0; y < (Yend-Ystart); y++) {
      ST7735_pushColor(&color,1);
    }
  }

}
