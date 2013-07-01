/**
 * @file DhrystoneTest/main.c
 *
 * @brief DhrystoneTest main program performs Dhrystone benchmark.\n
 * 
 * @author Accent S.p.A. 
 *
 * @note
 *
 * @see 
 */

/** 
  @page Design_Documents

  @link TFS_APP_RtcTest TFS Application DhrystoneTest @endlink

  @page TFS_APP_DhrystoneTest TFS Application Dhrystone Test

  @todo 

*/

/*
** ===========================================================================
**
** 1. INCLUDE FILES
**
** ===========================================================================
*/

//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/libc/stdio/stdio.h"
//#include "intrinsics.h"

          
/*
** ===========================================================================
**
** 2. DEFINITIONS
**
** ===========================================================================
*/

/*
** ===========================================================================
**
** 3. TYPES
**
** ===========================================================================
*/

/*
** ===========================================================================
**
** 4. FUNCTION PROTOTYPES
**
** ===========================================================================
*/
__weak int sdramtest_main(int i,char* c)
{
    (int)i;
    (char*)c;
    printf("Unsupported device\n");
    return 0;
}

/** 
 * @fn      int main(void)
 *
 * @brief   main program: initialize the driver library and implements the example feature.
 *
 * @return  nothing.
 *
 * @see
 */
extern int sdramtest_main(int,char*);
int dhrystone_main(void)
{
  volatile int delay=0;
  char c;

  while (1) // (WDT_InitStr.appOverflowCounter <1)
  {
    fflush(stdin); //Clear any pending caracters
    printf("\nMenu\n");
//    printf("c: get rtc counter\n");
//    printf("f. flash setup\n");
//    printf("s. sys clock setup\n");
    printf("r: Run Dhrystone Test\n");
    printf("s: Run SDRAM Test\n");
    printf("q: quit\n");
    printf("choice >");
    
//    c = getc(stdin);//getchar();
//    scanf ("%s", &c);
    c = fgetc(stdin);
    printf("%c\n", c);
    
    switch (c)
    {
    case 'r':
      runDhrystone();
      break;
//    case 'f':
//      FlashCmdMenu();
//      break;
//    case 'c':
//      printf("RTC counter: ");
//      RTC_GetValue(RTC, &vr48);
//      printf("hex %08x %08x, ", vr48.h, vr48.l);
//      printf("dec (%u %u)\n", vr48.h, vr48.l);
//      break;
    case 's':
        if(sdramtest_main)
            sdramtest_main(0,0);
        break;
    case 'q':
        return 0;
    }
  }

}


/* END OF FILE */
