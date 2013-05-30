/*********************************************************************
*               SEGGER MICROCONTROLLER GmbH & Co KG                  *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (c) 1995 - 2008  SEGGER Microcontroller GmbH & Co KG         *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
*       embOS * Real time operating system for microcontrollers      *
*                                                                    *
*                                                                    *
*       Please note:                                                 *
*                                                                    *
*       Knowledge of this file may under no circumstances            *
*       be used to write a similar product or a real-time            *
*       operating system for in-house use.                           *
*                                                                    *
*       Thank you for your fairness !                                *
*                                                                    *
**********************************************************************
*                                                                    *
*       embOS version: 3.60d                                         *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : OS_ARM720_MMU.c
Purpose : Basic MMU setup and initialization for MMU of ARM72x CPUs
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.h"
#include <intrinsics.h>

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/

/*********************************************************************
*
*       _ReadControl()
*/
static __interwork __arm unsigned int _ReadControl(void) {
  return __MRC(15, 0, 1, 0, 0);
}

/*********************************************************************
*
*       _WriteControl()
*/
static __interwork __arm void _WriteControl(unsigned int ctl) {
  __MCR (15, 0, ctl, 1, 0, 0);
}

/*********************************************************************
*
*       _WriteTTB()
*/
static __interwork __arm void _WriteTTB(unsigned int ttb) {
  __MCR (15, 0, (ttb & 0xFFFFC000), 2, 0, 0);
}

/*********************************************************************
*
*       _WriteDomain()
*/
static __interwork __arm void _WriteDomain(unsigned int domain) {
  __MCR (15, 0, domain, 3, 0, 0);
}

/*********************************************************************
*
*       _InvalidateCache()
*/
static __interwork __arm void _InvalidateCache(void) {
  register unsigned int sbz = 0;
  __MCR (15, 0, sbz, 7, 7, 0);
}

/*********************************************************************
*
*       _CleanCacheLine()
*/
static __interwork __arm void _CleanCacheLine(unsigned int Addr) {
  register unsigned int mva;

  mva = Addr;
  __MCR (15, 0, mva, 7, 11, 1);
}

/*********************************************************************
*
*       _InvalidateCacheLine()
*/
static __interwork __arm void _InvalidateCacheLine(unsigned int Addr) {
  register unsigned int mva;

  mva = Addr;
  __MCR (15, 0, mva, 7, 7, 1);
}

/*********************************************************************
*
*       _DrainWriteBuffer()
*/
static __interwork __arm void _DrainWriteBuffer(void) {
  register unsigned int sbz = 0;
  __MCR (15, 0, sbz, 7, 10, 4);
}

/*********************************************************************
*
*       _InvalidateDTLB()
*/
static __interwork __arm void _InvalidateTLB(void) {
  register unsigned int sbz = 0;
  __MCR (15, 0, sbz, 8, 6, 0);
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*       OS_ARM720_MMU_Enable()
*/
void OS_ARM720_MMU_Enable(unsigned int* pTranslationTable) {
  unsigned int ctl;
  //
  // Clean and reset caches first
  //
  _InvalidateTLB();
  _InvalidateCache();
  //
  // Now disable the MMU
  //
  ctl = _ReadControl();
  ctl &= ~(1 << 0);
  _WriteControl(ctl);
  //
  // Program the TTB
  //
  _WriteTTB((unsigned int) pTranslationTable);
  //
  // Program the domain access register
  //
  _WriteDomain(0xC0000000); // domain 15: access is not checked
  //
  // Now enable the MMU
  //
  ctl = _ReadControl();
  ctl |= (1 << 0);
  _WriteControl(ctl);
}

/*********************************************************************
*
*       OS_ARM720_CACHE_Enable()
*/
void OS_ARM720_CACHE_Enable(void) {
  unsigned int ctl;

  ctl = _ReadControl();
  ctl |= (1 << 2);
  _WriteControl(ctl);
}

/*********************************************************************
*
*       OS_ARM_CACHE_CleanRange()
*/
void OS_ARM720_CACHE_CleanRange(void* p, unsigned int NumBytes) {
  int NumLines;
  unsigned int Addr;

  Addr = (unsigned int) p;
  NumLines = (NumBytes + 31) >> 5;
  Addr &= ~31;     // Align to cache line (32-byte boundary)
  while (NumLines--) {
    _CleanCacheLine(Addr);
    Addr += 32;
  }
  //
  // Finally drain the write buffer
  //
  _DrainWriteBuffer();
}

/*********************************************************************
*
*       OS_ARM720_CACHE_InvalidateRange()
*/
void OS_ARM720_CACHE_InvalidateRange(void* p, unsigned int NumBytes) {
  int NumLines;
  unsigned int Addr;

  Addr = (unsigned int) p;
  NumLines = (NumBytes + 31) >> 5;
  Addr &= ~31;     // Align to cache line (32-byte boundary)
  while (NumLines--) {
    _InvalidateCacheLine(Addr);
    Addr += 32;
  }
}

/*********************************************************************
*
*       OS_ARM720_MMU_InitTT()
*
*  Function description
*    Sets up the MMU translation table.
*    It does not affect the hardware (MMU or caches)
*    Typically, the sequence of events is the following:
*
*   MMU_InitTT()
*   MMU_AddTTEntries()
*   MMU_AddTTEntries()
*   MMU_AddTTEntries()
*   MMU_AddTTEntries()
*   MMU_Enable()
*   MMU_EnableICache()
*   MMU_EnableDCache()
*/
void OS_ARM720_MMU_InitTT(unsigned int* pTranslationTable) {
  int i;
  //
  // Clear Table
  //
  for (i = 0; i < 4096; ++i) {
    pTranslationTable[i] = 0;
  }
}

/*********************************************************************
*
*       OS_ARM720_MMU_AddTTEntries()
*
*  Function description
*    Assigns a number of translation table entries.
*    All assigned entries are 1MB each. (Coarse)
*    This code does not affect the hardware (MMU or caches)
*
*  Parameters
*    pTranslationTable  Points to a translation table with 4096 entries of 4 bytes each. Must be in RAM.
*    Mode               Defines the cache mode. Either MODE_NC_NB, MODE_C_NB, MODE_NC_B, MODE_C_B
*    VIndex             Virtual Index. Defines the virtual addr.:   VAddr = VIndex * 0x100000
*    PIndex             Physical Index. Defines the physical addr.: PAddr = PIndex * 0x100000
*    NumEntries         Number of 1MB entries to set.
*/
void OS_ARM720_MMU_AddTTEntries(unsigned int* pTranslationTable, unsigned int CacheMode, unsigned int VIndex, unsigned int PIndex, unsigned int NumEntries) {
  unsigned int TableValue;
  //
  // Initialize all defined areas
  //
  for (; NumEntries > 0; NumEntries--) {
    TableValue = (PIndex << 20)
               | (1 << 10)             // Access in supervisor mode
               | (15 << 5)             // Domain
               | (1 << 4)              // Always 1 !
               | CacheMode             // configured mode
               | 0x02                  // 1 MB segment
               ;
    *(pTranslationTable + VIndex) = TableValue;
    PIndex++;
    VIndex++;
  }
}


/****** EOF *********************************************************/

