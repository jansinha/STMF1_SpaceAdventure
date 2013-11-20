#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_dma.h>
#include "spidma.h"
#include "bmp.h"
#include "mmcbb.h"
#include "ff.h"
#include "st7735_init.h"
#include "xprintf.h"
#include "fataccess.h"

extern void Delay(uint32_t nTime);

FATFS Fatfs;     /* File system object */
FIL Fil;         /* File object */

struct bmpfile_magic magic;
struct bmpfile_header header;
BITMAPINFOHEADER info;
struct bmppixel iBuff[ST7735_HEIGHT];
uint16_t oBuff[ST7735_HEIGHT];


void loadBMP(char *filename)
{
  FRESULT rc;
  DIR dir;
  FILINFO fno;
  UINT bw, br;
  uint8_t i, j, select=0;
  uint8_t tmp1, tmp2, tmp3;

  // Configure SysTick Timer
  if ( SysTick_Config(SystemCoreClock / 1000) )
    while (1);

  rc = f_mount( 0, &Fatfs );/* Register volume work area */

  rc = f_open( &Fil, filename, FA_READ );
    
  if (!rc) {   
    /* Read the structures in the the header */
    rc = f_read ( &Fil, &magic, sizeof(struct bmpfile_magic), &br );
    if (rc || !br) return ;
    rc = f_read ( &Fil, &header, sizeof(struct bmpfile_header), &br );
    if (rc || !br) return ;
    rc = f_read ( &Fil, &info, sizeof(BITMAPINFOHEADER), &br );
    if (rc || !br) return ;
    if (info.height != ST7735_HEIGHT || info.width !=ST7735_WIDTH || info.bitspp != 24) return;

    // Set address window
    ST7735_setAddrWindow( 0, 0, ST7735_WIDTH-1, ST7735_HEIGHT-1, MADCTLBMP );
    for( i=0; i < info.height; i++ ) {
      rc = f_read( &Fil, &iBuff, sizeof(struct bmppixel)*ST7735_WIDTH, &br ); 
      if (rc || !br) return ;
      for ( j=0; j < info.width; j++ ) {
	oBuff[j]=((iBuff[j].r >> 3) << 11) + ((iBuff[j].g >> 2) << 5) + (iBuff[j].b >> 3);
      }
      ST7735_pushColor( oBuff, info.width );
    }
    if (rc) die(rc);

    rc = f_close(&Fil);
    if (rc) die(rc);
  }

}

static __IO uint32_t TimingDelay;
extern void Delay( uint32_t nTime );
extern void SysTick_Handler (void);
extern void assert_failed (uint8_t * file, uint32_t line);

