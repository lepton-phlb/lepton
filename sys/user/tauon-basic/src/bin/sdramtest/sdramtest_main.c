

//#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "lib/libc/stdio/stdio.h"


//*****************************************************************************
//
// The starting and ending address for the 8MB SDRAM chip (4Meg x 16bits) on
// the SDRAM daughter board.
//
//*****************************************************************************
#define SDRAM_START_ADDRESS 0x000000
#define SDRAM_END_ADDRESS 0x3FFFFF

#ifndef EXT_RAM_REGION
#define EXT_RAM_REGION      _Pragma("location = \"EXT_RAM\"")
#endif

//*****************************************************************************
//
// A pointer to the EPI memory aperture.  Note that g_pusEPISdram is declared
// as volatile so the compiler should not optimize reads out of the image.
//
//*****************************************************************************
static volatile unsigned short *g_pusEPISdram;
//EXT_RAM_REGION unsigned char g_EPISdram_buff[SDRAM_END_ADDRESS+1];

int sdramtest_main(int argc,char* argv[])
{
    //
    // Set the EPI memory pointer to the base of EPI memory space.  Note that
    // g_pusEPISdram is declared as volatile so the compiler should not
    // optimize reads out of the memory.  With this pointer, the memory space
    // is accessed like a simple array.
    //
    g_pusEPISdram = (unsigned short *)0x60000000;
//    g_pusEPISdram = (unsigned short*)g_EPISdram_buff;

    //
    // Read the initial data in SDRAM, and display it on the console.
    //
    printf("  SDRAM Initial Data:\n");
    printf("     Mem[0x6000.0000] = 0x%4x\n",
               g_pusEPISdram[SDRAM_START_ADDRESS]);
    printf("     Mem[0x6000.0001] = 0x%4x\n",
               g_pusEPISdram[SDRAM_START_ADDRESS + 1]);
    printf("     Mem[0x603F.FFFE] = 0x%4x\n",
               g_pusEPISdram[SDRAM_END_ADDRESS - 1]);
    printf("     Mem[0x603F.FFFF] = 0x%4x\n\n",
               g_pusEPISdram[SDRAM_END_ADDRESS]);

    //
    // Display what writes we are doing on the console.
    //
    printf("  SDRAM Write:\n");
    printf("     Mem[0x6000.0000] <- 0xabcd\n");
    printf("     Mem[0x6000.0001] <- 0x1234\n");
    printf("     Mem[0x603F.FFFE] <- 0xdcba\n");
    printf("     Mem[0x603F.FFFF] <- 0x4321\n\n");

    //
    // Write to the first 2 and last 2 address of the SDRAM card.  Since the
    // SDRAM card is word addressable, we will write words.
    //
    g_pusEPISdram[SDRAM_START_ADDRESS] = 0xabcd;
    g_pusEPISdram[SDRAM_START_ADDRESS + 1] = 0x1234;
    g_pusEPISdram[SDRAM_END_ADDRESS - 1] = 0xdcba;
    g_pusEPISdram[SDRAM_END_ADDRESS] = 0x4321;

    //
    // Read back the data you wrote, and display it on the console.
    //
    printf("  SDRAM Read:\n");
    printf("     Mem[0x6000.0000] = 0x%4x\n",
               g_pusEPISdram[SDRAM_START_ADDRESS]);
    printf("     Mem[0x6000.0001] = 0x%4x\n",
               g_pusEPISdram[SDRAM_START_ADDRESS + 1]);
    printf("     Mem[0x603F.FFFE] = 0x%4x\n",
               g_pusEPISdram[SDRAM_END_ADDRESS - 1]);
    printf("     Mem[0x603F.FFFF] = 0x%4x\n\n",
               g_pusEPISdram[SDRAM_END_ADDRESS]);

    //
    // Check the validity of the data.
    //
    if((g_pusEPISdram[SDRAM_START_ADDRESS] == 0xabcd) &&
       (g_pusEPISdram[SDRAM_START_ADDRESS + 1] == 0x1234) &&
       (g_pusEPISdram[SDRAM_END_ADDRESS - 1] == 0xdcba) &&
       (g_pusEPISdram[SDRAM_END_ADDRESS] == 0x4321))
    {
        //
        // Read and write operations were successful.  Return with no errors.
        //
        printf("Read and write to external SDRAM was successful!\n");
        return(0);
    }

    //
    // Display on the console that there was an error.
    //
    printf("Read and/or write failure!\n");
    printf(" Check if your SDRAM card is plugged in.\n");

//    //
//    // Read and/or write operations were unsuccessful.  Wait in while(1) loop
//    // for debugging.
//    //
//    while(1)
//    {
//    }
    return 0;
}

