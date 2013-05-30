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
File    : OS_ARM_MMU.c
Purpose : Basic MMU setup and initialization for MMU and Cache
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
static __interwork __arm unsigned int _ReadCacheType(void) {
  return __MRC(15, 0, 0, 0, 1);
}

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
*       _InvalidateICache()
*/
static __interwork __arm void _InvalidateICache(void) {
  register unsigned int sbz = 0;
  __MCR (15, 0, sbz, 7, 5, 0);
}

/*********************************************************************
*
*       _CleanDCacheIDX()
*/
static __interwork __arm void _CleanDCacheIDX(unsigned int index) {
  __MCR (15, 0, index, 7, 10, 2);
}

/*********************************************************************
*
*       _CleanDCacheLine()
*/
static __interwork __arm void _CleanDCacheLine(unsigned int Addr) {
  register unsigned int mva;

  mva = Addr;
  __MCR (15, 0, mva, 7, 10, 1);
}

/*********************************************************************
*
*       _InvalidateDCache()
*/
static __interwork __arm void _InvalidateDCache(void) {
  register unsigned int sbz = 0;
  __MCR (15, 0, sbz, 7, 6, 0);
}

/*********************************************************************
*
*       _InvalidateDCacheLine()
*/
static __interwork __arm void _InvalidateDCacheLine(unsigned int Addr) {
  register unsigned int mva;

  mva = Addr;
  __MCR (15, 0, mva, 7, 6, 1);
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
*       _InvalidateITLB()
*/
static __interwork __arm void _InvalidateITLB(void) {
  register unsigned int sbz = 0;
  __MCR (15, 0, sbz, 8, 5, 0);
}

/*********************************************************************
*
*       _InvalidateDTLB()
*/
static __interwork __arm void _InvalidateDTLB(void) {
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
*       OS_ARM_CACHE_Sync()
*
*  Function description
*    Cleans the data cache and invalidates the instruction cache
*    to ensure cache coherency.
*  
*    The cache configuration is read from cache ID register.
*    The parameter for cache clean by index is calculated accorrding the
*    ARM Architecture reference manual, ARM DDI 0100E
*    page B2-10, B2-11
*    Cache size field DCache (ID register b23..12):
*    b1..0: len
*    b2:    M
*    b5..3: asoc
*    b8..6: size
*
*    calculation:
*    LINELEN = 1 << (len+3) // In bytes, Example: LINELEN = 1 << (2 + 3) = 32
*    MULTIPLIER = 2 + M     // Example: Multiplier = 2 + 0 = 2
*    ASSOCIATIVITY = MULTIPLIER << (assoc-1)  // Example: ASSOCIATIVITY = MULTIPLIER << (assoc-1) = 2 << (2-1) = 4 
*    NSETS = 1 << (size + 6 - assoc - len) // Example: NSETS = 1 << (4 + 6 - 2 - 2) = (1 << 6) = 64
*    CacheSize = LINELEN * ASSOCIATIVITY * NSETS
*
*    ARM DDI 0100E, page B5-16
*    The set/index parameter for Clean data cache line command:
*    index = index per set => max index = (total size / num sets / linelen) - 1
*    set   = 0..(max number of sets - 1)
*    Set/Index Parameter is assembled as follows:
*    (index << 32-A) | (set << L)
*    with A = log2(ASSOCIATIVITY)
*    L = log2(LINELEN)
*/
void OS_ARM_CACHE_Sync(void) {
  unsigned int CacheType;
  unsigned int Index;
  unsigned int SIndex;
  unsigned int NumIndex;
  unsigned int NumSet;
  unsigned int SPos;
  unsigned int IPos;

  CacheType = _ReadCacheType();
  //
  // Check whether CPU has separate I- and d-cache (b24) and if cache is write through (Ctypye > 1)
  //
  if ((CacheType & (1 << 24)) && ((CacheType & (0xF << 25)) > (1 << 25))) {
    //
    // CPU has separate stacks, clean operation fpr DCache required
    //
    CacheType >>= 12;        // Extract D-Cache size
    CacheType &= (0xFFF);
    //
    // Calculate number of sets, which is cache size / Line length / associativity
    // eg 1 << (size + 6 - assoc - len)
    //
    NumSet    = (1 << ((CacheType >> 6) + 6 - ((CacheType >> 3) & 0x7) - (CacheType & 0x3)));
    //
    // Calculate numner of ways which is cache size / Line length / number of sets
    // eg MULTIPLIER << (assoc-1)
    //
    NumIndex  = (2 + ((CacheType >> 2) & 0x01)) << (((CacheType >> 3) & 0x7) - 1);
    //
    // Calculate position of set parameter
    //
    Index     = 1 << ((CacheType & 0x03) + 3);   // LineLength
    Index   >>= 1;
    SPos      = 0;
    do {
      SPos++;
      Index >>= 1;
    } while (Index);
    //
    // Calculate position of index parameter
    //
    Index     = NumIndex;
    Index   >>= 1;
    IPos      = 32;
    do {
      IPos--;
      Index >>= 1;
    } while (Index);
    //
    // For all Sets, clean all cache lines
    //
    for (Index = 0; Index < NumIndex; Index++) {
      for (SIndex = 0; SIndex < NumSet; SIndex++) {
        _CleanDCacheIDX((Index << IPos) | (SIndex << SPos));
      }
    }
    //
    // Finally drain the write buffer and invalidate iCache
    //
    _DrainWriteBuffer();
    _InvalidateICache();
  }
}

/*********************************************************************
*
*       OS_ARM_DCACHE_CleanRange()
*/
void OS_ARM_DCACHE_CleanRange(void* p, unsigned int NumBytes) {
  unsigned int Addr;
  unsigned int AddrEnd;

  Addr = (unsigned) p;
  AddrEnd = Addr + NumBytes - 1;
  Addr &= ~31;     // Align to cache line (32-byte boundary)
  do {
    _CleanDCacheLine(Addr);
    Addr += 32;
  } while (Addr <= AddrEnd);
  //
  // Finally drain the write buffer
  //
  _DrainWriteBuffer();
}

/*********************************************************************
*
*       OS_ARM_DCACHE_Enable()
*/
void OS_ARM_DCACHE_Enable(void) {
  unsigned int ctl;

  ctl = _ReadControl();
  ctl |= (1 << 2);
  _WriteControl(ctl);
}

/*********************************************************************
*
*       OS_ARM_DCACHE_InvalidateRange()
*/
void OS_ARM_DCACHE_InvalidateRange(void* p, unsigned int NumBytes) {
  unsigned Addr;
  unsigned NumLines;

  Addr = (unsigned) p;
  NumLines = NumBytes >> 5;       // One cache line is 32 bytes
  //
  // StartAddress and NumBytes have to be cache line aligned,
  //  
#if OS_DEBUG
  if (Addr & 31) {
    OS_Error(OS_ERR_NON_ALIGNED_INVALIDATE);
  }
  if (NumBytes & 31) {
    OS_Error(OS_ERR_NON_ALIGNED_INVALIDATE);
  }
#endif
  do {
    _InvalidateDCacheLine(Addr);
    Addr += 32;
  } while (--NumLines);
}

/*********************************************************************
*
*       OS_ARM_ICACHE_Enable()
*/
void OS_ARM_ICACHE_Enable(void) {
  unsigned int ctl;

  ctl = _ReadControl();
  ctl |= (1 << 12);
  _WriteControl(ctl);
}

/*********************************************************************
*
*       OS_ARM_MMU_AddTTEntries()
*
*  Function description
*    Assigns a number of translation table entries.
*    All assigned entries are 1MB each. (Coarse)
*    This code does not affect the hardware (MMU or caches)
*
*  Parameters
*    pTranslationTable  Points to a translation table with 4096 entries of 4 bytes each. Must be in RAM.
*    CacheMode          Defines the cache mode. Either MODE_NC_NB, MODE_C_NB, MODE_NC_B, MODE_C_B
*    VIndex             Virtual Index. Defines the virtual addr.:   VAddr = VIndex * 0x100000
*    PIndex             Physical Index. Defines the physical addr.: PAddr = PIndex * 0x100000
*    NumEntries         Number of 1MB entries to set.
*/
void OS_ARM_MMU_AddTTEntries(unsigned int* pTranslationTable, unsigned int CacheMode, unsigned int VIndex, unsigned int PIndex, unsigned int NumEntries) {
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

/*********************************************************************
*
*       OS_ARM_MMU_Enable()
*/
void OS_ARM_MMU_Enable(unsigned int* pTranslationTable) {
  unsigned int ctl;

  _InvalidateDTLB();
  _InvalidateDCache();
  _InvalidateITLB();
  _InvalidateICache();
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
*       OS_ARM_MMU_InitTT()
*
*  Function description
*    Sets up the MMU translation table with default values.
*    It does not affect the hardware (MMU or caches)
*    Typically, the sequence of events is the following:
*
*   OS_ARM_MMU_InitTT()
*   OS_ARM_MMU_AddTTEntries()
*   OS_ARM_MMU_AddTTEntries()
*   OS_ARM_MMU_AddTTEntries()
*   OS_ARM_MMU_AddTTEntries()
*   OS_ARM_MMU_Enable()
*   OS_ARM_MMU_EnableICache()
*   OS_ARM_MMU_EnableDCache()
*/
void OS_ARM_MMU_InitTT(unsigned int* pTranslationTable) {
  int i;
  //
  // Clear Table
  //
  for (i = 0; i < 4096; ++i) {
    pTranslationTable[i] = 0;
  }
}

/****** EOF *********************************************************/

