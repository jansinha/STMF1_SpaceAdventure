#include "mmcbb.h"
#include "ff.h"

void die (FRESULT rc) {
  xprintf("Failed with rc=%u.\n", rc);                                                         
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
