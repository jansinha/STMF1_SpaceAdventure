#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include <stdint.h>

/*
Create a structure for levels with a single landing site
*/

typedef struct {
  uint8_t Xstart;
  uint8_t Ystart; // the coordinates for the top left of a landing site
  uint8_t Xend;
  uint8_t Yend; // the coordinates for the bottom right of a landing site
  uint16_t color;
}Landing_Site;


enum LEVELS {
  level1, level2, level3, level4, level5, level6, level7, level8, level9, level10
};

void LevelInit( Landing_Site *LS );
void landscape( int level, Landing_Site *LS );

#endif


