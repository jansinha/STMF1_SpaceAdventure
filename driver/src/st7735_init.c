#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>
#include "st7735_init.h"
#include "spi_drive.h"

extern void Delay(uint32_t nTime);

static uint8_t madctlcurrent = MADVAL( MADCTLGRAPHICS );

void ST7735_init( void )
{
  const struct ST7735_cmdBuf *cmd;

  GPIO_InitTypeDef GPIO_InitStructure;
 
  // set up pins
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );

  // LCD - BACKLIGHT, LCD_CS, Rest, Data/Ctrl
  GPIO_StructInit( &GPIO_InitStructure ); // BackLight = BKL
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_BKL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( LCD_PORT_BKL, &GPIO_InitStructure );

  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SCE; // LCD_CS = LCD Select = SCE
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( LCD_PORT, &GPIO_InitStructure );

  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_RST; // LCD RESET
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( LCD_PORT, &GPIO_InitStructure );

  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_DC; // Data Control = DC = RS
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( LCD_PORT, &GPIO_InitStructure );

  GPIO_WriteBit( LCD_PORT, GPIO_PIN_SCE, HIGH );
  GPIO_WriteBit( LCD_PORT, GPIO_PIN_RST, HIGH );
  Delay(10);
  GPIO_WriteBit( LCD_PORT, GPIO_PIN_RST, LOW  );
  Delay(10);
  GPIO_WriteBit( LCD_PORT,GPIO_PIN_RST, HIGH );
  Delay(10);

  // Send initialization commands to ST7735
  for (cmd = initializers; cmd->command; cmd++)
  {
    LcdWrite(LCD_C, &(cmd->command), 1);
    if( cmd->len )
      LcdWrite( LCD_D, cmd->data, cmd->len );
    if( cmd->delay )
      Delay(cmd->delay);
  }
}

static void LcdWrite( char dc, const char *data, int nbytes )
{
  GPIO_WriteBit(LCD_PORT,GPIO_PIN_DC, dc); // dc: 1 = data, 0 = control
  GPIO_ResetBits(LCD_PORT,GPIO_PIN_SCE);
  spiReadWrite(SPILCD, 0, data, nbytes, LCDSPEED);
  GPIO_SetBits(LCD_PORT,GPIO_PIN_SCE);
}

static void LcdWrite16( char dc, const uint16_t *data, int cnt )
{
  GPIO_WriteBit(LCD_PORT,GPIO_PIN_DC, dc); // dc: 1 = data, 0 = control
  GPIO_ResetBits(LCD_PORT,GPIO_PIN_SCE);
  spiReadWrite16(SPILCD, 0, data, cnt, LCDSPEED);
  GPIO_SetBits(LCD_PORT,GPIO_PIN_SCE);
}

static void ST7735_writeCmd( uint8_t c ) {

  LcdWrite(LCD_C, &c, 1);
}

void ST7735_setAddrWindow( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
			  uint8_t madctl ) {

  madctl = MADVAL(madctl);
  if (madctl != madctlcurrent) {
    ST7735_writeCmd( ST7735_MADCTL );
    LcdWrite( LCD_D, &madctl, 1 );
    madctlcurrent = madctl;
  }
  ST7735_writeCmd( ST7735_CASET );
  LcdWrite16( LCD_D, &x0, 1 );
  LcdWrite16( LCD_D, &x1, 1 );

  ST7735_writeCmd( ST7735_RASET );
  LcdWrite16( LCD_D, &y0, 1 );
  LcdWrite16( LCD_D, &y1, 1 );

  ST7735_writeCmd(ST7735_RAMWR);
}

void ST7735_pushColor( uint16_t *color, int cnt ) {

  LcdWrite16( LCD_D, color, cnt );
}

void ST7735_backLight( uint8_t on ) {

  if (on)
    GPIO_WriteBit( LCD_PORT_BKL, GPIO_PIN_BKL, LOW );
  else
    GPIO_WriteBit( LCD_PORT_BKL, GPIO_PIN_BKL, HIGH );
}

void fillScreen( uint16_t color ) {

  uint8_t x,y;
  ST7735_setAddrWindow(0, 0, ST7735_HEIGHT-1, ST7735_WIDTH-1, MADCTLGRAPHICS);
  for (x=0; x < ST7735_WIDTH; x++) {
      for (y=0; y < ST7735_HEIGHT; y++) {
	  ST7735_pushColor(&color,1);
      }
  }
}
