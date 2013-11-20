#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_dac.h>
#include <stm32f10x_dma.h>
#include <stm32f10x_tim.h>
#include <misc.h>
#include <string.h>
#include "spidma.h"
#include "mmcbb.h"
#include "ff.h"
#include "parse.h"
#include "player.h"

#define WAVE_FORMAT_PCM 0x01
#define Mono 0x01

FRESULT rc;
DIR dir;
FILINFO fno;
UINT bw, br;
FATFS Fatfs;     /* File system object */
FIL Fil;         /* File object */

void die( FRESULT rc );

int readckhd( FIL *fid, struct ckhd *hd, uint32_t ckID)
{
  int br;
  int err;
  err = f_read( fid, hd, sizeof(struct ckhd), &br );
  if( br != sizeof(struct ckhd) )
    return 1;
  if( ckID && (ckID != hd->ckID) )
    return 2;

  return 0;
}

void init_sound( char *filename, FIL *fid )
{
  int i, readStatus;
  uint16_t *ptrAudiobuf = Audiobuf;

  uint32_t waveid;
  struct ckhd hd;
  struct fmtck fck;

  /**** Parse the audio file here ****/
  // Mount and open the file on SD card
  rc = f_mount( 0, &Fatfs );/* Register volume work area */
  rc = f_open( &Fil, filename, FA_READ );
  
  /* Read the structures in the header chunk*/
  readStatus = readckhd( &Fil, &hd, 'FFIR');  // MASTER CHUNK
  rc = f_read( &Fil, &waveid, sizeof(waveid), &br );
  if( rc || !br ) return;

  /* Read the structures in the format chunk */
  readStatus = readckhd( &Fil, &hd, ' tmf'); // FORMAT CHUNK
  rc = f_read( &Fil, &fck, sizeof(struct fmtck), &br );
  if( rc || !br ) return;

  // HOUSEKEEPING!
  if( fck.wFormatTag != WAVE_FORMAT_PCM || fck.nChannels != Mono ) return;

  // Skip NON-DATA chunk
  if( hd.cksize != 16) {
    f_lseek( &Fil, f_tell(&Fil) + hd.cksize - 16 );
  }

  // Skip to DATA
  while(1) { 
    readStatus = readckhd( &Fil, &hd, 0);
    if (hd.ckID == 'atad')
      break;
    f_lseek( &Fil, f_tell(&Fil) + hd.cksize );
  }

  //Get this value from the nSamplesPerSec field of the FORMAT chunk
  audioplayerInit( fck.nSamplesPerSec );

  // 1st: Get handle of WAV data and FILL the entire buffer
  rc = f_read( &Fil, Audiobuf, AUDIOBUFSIZE, &br );

  // 2nd: Enable all the clocks
  audioplayerStart();

  // 3rd: Loop
  uint32_t temp = hd.cksize;
  while(temp > 0) {
    if( audioplayerHalf ) {
      ptrAudiobuf = Audiobuf;
      if (temp < AUDIOBUFSIZE/2)
	bzero(ptrAudiobuf, AUDIOBUFSIZE/2);
      rc = f_read(&Fil, ptrAudiobuf, AUDIOBUFSIZE/2, &br );
      temp -= br;       // hd.cksize -= br;
      audioplayerHalf = 0;                                                                         
    }
    else if (audioplayerWhole) {
      ptrAudiobuf = &Audiobuf[AUDIOBUFSIZE/2];
      if (temp < AUDIOBUFSIZE/2)
	bzero(ptrAudiobuf, AUDIOBUFSIZE/2);
      rc = f_read(&Fil, ptrAudiobuf, AUDIOBUFSIZE/2, &br );
      temp -= br; // hd.cksize -= br;
      audioplayerWhole = 0;
    }
  }

  // 4th: Disable the clocks
  audioplayerStop();

}

/****  DMAHandlerstart  ****/
int completed; // count cycles

void DMA1_Channel3_IRQHandler(void)
{
  int i;
  if (DMA_GetITStatus(DMA1_IT_TC3)){      // Transfer complete
    audioplayerWhole++;
    DMA_ClearITPendingBit(DMA1_IT_TC3);
  }
  else if (DMA_GetITStatus(DMA1_IT_HT3)){ // Half Transfer complete
    audioplayerHalf++; 
    DMA_ClearITPendingBit(DMA1_IT_HT3);
  }
}
/****  DMAHandlerstop  ****/


void die (FRESULT rc) {
  // xprintf("Failed with rc=%u.\n", rc);
  for (;;) ;
}

/*---------------------------------------------------------*/
/* User Provided Timer Function for FatFs module           */
/*---------------------------------------------------------*/

DWORD get_fattime (void) {
  return    ((DWORD)(2012 - 1980) << 25)  /* Year = 2012 */
    | ((DWORD)1 << 21)                            /* Month = 1 */
    | ((DWORD)1 << 16)                            /* Day_m = 1*/
    | ((DWORD)0 << 11)                            /* Hour = 0 */
    | ((DWORD)0 << 5)                             /* Min = 0 */
    | ((DWORD)0 >> 1);                            /* Sec = 0 */
}


// Timer code
static __IO uint32_t TimingDelay;
extern void Delay( uint32_t nTime ); 
extern void SysTick_Handler (void); 
extern void assert_failed (uint8_t * file, uint32_t line);
