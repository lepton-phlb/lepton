//==============================================================================
//					 at91sam9261_mmu.c
//						-----------------
//
//  ----------------------------------------------------------------------------
//          ATMEL Microcontroller Software Support  -  ROUSSET  -
//  ----------------------------------------------------------------------------
//  DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
//  DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
//  OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//  ----------------------------------------------------------------------------
//==============================================================================
#include <pkgconf/system.h>
#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_arch.h>           // Register state info
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_intr.h>           // necessary?
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_if.h>             // calling interface
#include <cyg/hal/hal_misc.h>           // helper functions

#include "at91sam9261_mmu.h"


//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadIDCode
//* \brief Read ID code register
//*----------------------------------------------------------------------------
__inline static unsigned int AT91F_ARM_ReadIDCode(void)
{
   register unsigned int id;
   __asm("MRC p15, 0, %0, c0, c0, 0\n" : "=r" (id));
   return id;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadCacheType
//* \brief Read cache type
//*----------------------------------------------------------------------------
__inline static unsigned int AT91F_ARM_ReadCacheType(void)
{
   register unsigned int type;
   __asm("MRC p15, 0, %0, c0, c0, 1\n" : "=r" (type));
   return type;
}

// ****************************************************************************
// CP15 Register 1
//      Read: Control
//	Write: Control

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadControl
//* \brief Read Control register
//*----------------------------------------------------------------------------
__inline static unsigned int AT91F_ARM_ReadControl(void)
{
   register unsigned int ctl;
   __asm("MRC p15, 0, %0, c1, c0, 0\n" : "=r" (ctl));
   return ctl;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WriteControl
//* \brief Write Control register
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_WriteControl(unsigned int ctl)
{
   __asm("MCR p15, 0, %0, c1, c0, 0\n" : : "r" (ctl));
}

// ****************************************************************************
// CP15 Register 2
//      Read: Translation table Base
//	Write: Translation table Base

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadTTB
//* \brief Read Translation table base register
//*----------------------------------------------------------------------------
__inline static unsigned int AT91F_ARM_ReadTTB(void)
{
   register unsigned int ttb;
   __asm("MRC p15, 0, %0, c2, c0, 0\n" : "=r" (ttb));
   return ttb;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WriteTTB
//* \brief Write Translation table base  register
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_WriteTTB(unsigned int ttb)
{
   __asm("MCR p15, 0, %0, c2, c0, 0\n" : : "r" (ttb&0xFFFFC000));
}

// ****************************************************************************
// CP15 Register 3
//      Read: Read domain access control
//	Write: Write domain access control

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadDomain
//* \brief Read domain access control
//*----------------------------------------------------------------------------
__inline static unsigned int AT91F_ARM_ReadDomain(void)
{
   register unsigned int domain;
   __asm("MRC p15, 0, %0, c3, c0, 0\n" : "=r" (domain));
   return domain;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WriteDomain
//* \brief Write domain access control
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_WriteDomain(unsigned int domain)
{
   __asm("MCR p15, 0, %0, c3, c0, 0\n" : : "r" (domain));
}

// ****************************************************************************
// CP15 Register 5
//      Read: Read Fault Status
//	Write: Write Fault Status

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadDataFSR
//* \brief Read data FSR value
//*----------------------------------------------------------------------------
__inline static unsigned int AT91F_ARM_ReadDataFSR(void)
{
   register unsigned int dataFSR;
   __asm("MRC p15, 0, %0, c5, c0, 0\n" : "=r" (dataFSR));
   return dataFSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WriteDataFSR
//* \brief Write data FSR value
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_WriteDataFSR(unsigned int dataFSR)
{
   __asm("MCR p15, 0, %0, c5, c0, 0\n" : : "r" (dataFSR));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadPrefetchFSR
//* \brief Read prefetch FSR value
//*----------------------------------------------------------------------------
__inline static unsigned int AT91F_ARM_ReadPrefetchFSR(void)
{
   register unsigned int prefetchFSR;
   __asm("MRC p15, 0, %0, c5, c0, 1\n" : "=r" (prefetchFSR));
   return prefetchFSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WritePrefetchFSR
//* \brief Write prefetch FSR value
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_WritePrefetchFSR(unsigned int prefetchFSR)
{
   __asm("MCR p15, 0, %0, c5, c0, 1\n" : : "r" (prefetchFSR));
}

// ****************************************************************************
// CP15 Register 6
//      Read: Read Fault Address
//	Write: Write Fault Address

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadFAR
//* \brief Read FAR data
//*----------------------------------------------------------------------------
__inline static unsigned int AT91F_ARM_ReadFAR(void)
{
   register unsigned int dataFAR;
   __asm("MRC p15, 0, %0, c6, c0, 0\n" : "=r" (dataFAR));
   return dataFAR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WriteFAR
//* \brief Write FAR data
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_WriteFAR(unsigned int dataFAR)
{
   __asm("MCR p15, 0, %0, c6, c0, 0\n" : : "r" (dataFAR));
}

// ****************************************************************************
// CP15 Register 7
//      Read: Unpredictable
//	Write: Cache operations

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_InvalidateIDCache
//* \brief Invalidate ICache and DCache
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_InvalidateIDCache(void)
{
   register unsigned int sbz = 0;
   __asm("MCR p15, 0, %0, c7, c7, 0\n" : : "r" (sbz));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_InvalidateICache
//* \brief Invalidate ICache
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_InvalidateICache(void)
{
   register unsigned int sbz = 0;
   __asm("MCR p15, 0, %0, c7, c5, 0\n" : : "r" (sbz));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_InvalidateICacheMVA
//* \brief Invalidate ICache single entry (using MVA)
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_InvalidateICacheMVA(unsigned int mva)
{
   __asm("MCR p15, 0, %0, c7, c5, 1\n" : : "r" (mva&0xFFFFFFE0));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_PrefetchICacheLine
//* \brief Prefetch ICache line (using MVA)
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_PrefetchICacheLine(unsigned int mva)
{
   __asm("MCR p15, 0, %0, c7, c13, 1\n" : : "r" (mva&0xFFFFFFE0));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_InvalidateDCache
//* \brief Invalidate DCache
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_InvalidateDCache(void)
{
   register unsigned int sbz = 0;
   __asm("MCR p15, 0, %0, c7, c6, 0\n" : : "r" (sbz));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_InvalidateDCacheMVA
//* \brief Invalidate DCache single entry (using MVA)
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_InvalidateDCacheMVA(unsigned int mva)
{
   __asm("MCR p15, 0, %0, c7, c6, 1\n" : : "r" (mva&0xFFFFFFE0));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_CleanDCacheMVA
//* \brief Clean DCache single entry (using MVA)
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_CleanDCacheMVA(unsigned int mva)
{
   __asm("MCR p15, 0, %0, c7, c10, 1\n" : : "r" (mva&0xFFFFFFE0));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_CleanInvalidateDCacheMVA
//* \brief Clean and Invalidate DCache single entry (using MVA)
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_CleanInvalidateDCacheMVA(unsigned int mva)
{
   __asm("MCR p15, 0, %0, c7, c14, 1\n" : : "r" (mva&0xFFFFFFE0));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_CleanDCacheIDX
//* \brief Clean DCache single entry (using index)
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_CleanDCacheIDX(unsigned int index)
{
   __asm("MCR p15, 0, %0, c7, c10, 2\n" : : "r" (index&0xFC0000E0));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_CleanInvalidateDCacheIDX
//* \brief Clean and Invalidate DCache single entry (using index)
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_CleanInvalidateDCacheIDX(unsigned int index)
{
   __asm("MCR p15, 0, %0, c7, c14, 2\n" : : "r" (index&0xFC0000E0));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_DrainWriteBuffer
//* \brief Drain Write Buffer
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_DrainWriteBuffer(void)
{
   register unsigned int sbz = 0;
   __asm("MCR p15, 0, %0, c7, c10, 4\n" : : "r" (sbz));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WaitForInterrupt
//* \brief Wait for interrupt
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_WaitForInterrupt(void)
{
   register unsigned int sbz = 0;
   __asm("MCR p15, 0, %0, c7, c0, 4\n" : : "r" (sbz));
}

// ****************************************************************************
// CP15 Register 8
//      Read: Unpredictable
//	Write: TLB operations

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_InvalidateIDTLB
//* \brief Invalidate TLB(s)
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_InvalidateIDTLB(void)
{
   register unsigned int sbz = 0;
   __asm("MCR p15, 0, %0, c8, c7, 0\n" : : "r" (sbz));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_InvalidateITLB
//* \brief Invalidate I TLB
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_InvalidateITLB(void)
{
   register unsigned int sbz = 0;
   __asm("MCR p15, 0, %0, c8, c5, 0\n" : : "r" (sbz));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_InvalidateITLBMVA
//* \brief Invalidate I TLB single entry (using MVA)
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_InvalidateITLBMVA(unsigned int mva)
{
   __asm("MCR p15, 0, %0, c8, c5, 1\n" : : "r" (mva&0xFFFFFE00));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_InvalidateDTLB
//* \brief Invalidate D TLB
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_InvalidateDTLB(void)
{
   register unsigned int sbz = 0;
   __asm("MCR p15, 0, %0, c8, c6, 0\n" : : "r" (sbz));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_InvalidateDTLBMVA
//* \brief Invalidate D TLB single entry (using MVA)
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_InvalidateDTLBMVA(unsigned int mva)
{
   __asm("MCR p15, 0, %0, c8, c6, 1\n" : : "r" (mva&0xFFFFFE00));
}


// ****************************************************************************
// CP15 Register 9
//      Read: Cache lockdown
//	Write: Cache lockdown

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadDCacheLockdown
//* \brief Read D Cache lockdown
//*----------------------------------------------------------------------------
__inline static unsigned int AT91F_ARM_ReadDCacheLockdown(void)
{
   register unsigned int index;
   __asm("MRC p15, 0, %0, c9, c0, 0\n" : "=r" (index));
   return index;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WriteDCacheLockdown
//* \brief Write D Cache lockdown
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_WriteDCacheLockdown(
   unsigned int index)
{
   __asm("MCR p15, 0, %0, c9, c0, 0\n" : : "r" (index));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadICacheLockdown
//* \brief Read I Cache lockdown
//*----------------------------------------------------------------------------
__inline static unsigned int AT91F_ARM_ReadICacheLockdown(void)
{
   register unsigned int index;
   __asm("MRC p15, 0, %0, c9, c0, 1\n" : : "r" (index));
   return index;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WriteICacheLockdown
//* \brief Write I Cache lockdown
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_WriteICacheLockdown(
   unsigned int index)
{
   __asm("MCR p15, 0, %0, c9, c0, 1\n" : : "r" (index));
}

// ****************************************************************************
// CP15 Register 10
//      Read: TLB lockdown
//	Write: TLB lockdown

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadDTLBLockdown
//* \brief Read D TLB lockdown
//*----------------------------------------------------------------------------
__inline static unsigned int AT91F_ARM_ReadDTLBLockdown(void)
{
   register unsigned int lockdown;
   __asm("MRC p15, 0, %0, c10, c0, 0\n" : "=r" (lockdown));
   return lockdown;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WriteDTLBLockdown
//* \brief Write D TLB lockdown
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_WriteDTLBLockdown(
   unsigned int lockdown)
{
   __asm("MCR p15, 0, %0, c10, c0, 0\n" : : "r" (lockdown));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadITLBLockdown
//* \brief Read I TLB lockdown
//*----------------------------------------------------------------------------
__inline static unsigned int AT91F_ARM_ReadTLBLockdown(void)
{
   register unsigned int lockdown;
   __asm("MRC p15, 0, %0, c10, c0, 0\n" : "=r" (lockdown));
   return lockdown;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WriteITLBLockdown
//* \brief Write I TLB lockdown
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_WriteTLBLockdown(
   unsigned int lockdown)
{
   __asm("MCR p15, 0, %0, c10, c0, 0\n" : : "r" (lockdown));
}

// ****************************************************************************
// CP15 Register 13
//      Read: Read FCSE PID
//	Write: Write FCSE PID

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadFCSEPID
//* \brief Read FCSE PID
//*----------------------------------------------------------------------------
__inline static unsigned int AT91F_ARM_ReadFCSEPID(void)
{
   register unsigned int pid;
   __asm("MRC p15, 0, %0, c13, c0, 0\n" : "=r" (pid));
   return pid;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WriteFCSEPID
//* \brief Write FCSE PID
//*----------------------------------------------------------------------------
__inline static void AT91F_ARM_WriteFCSEPID(unsigned int pid)
{
   __asm("MCR p15, 0, %0, c13, c0, 0\n" : : "r" (pid & 0xFF000000));
}



//*----------------------------------------------------------------------------
//* \fn    AT91F_CleanDCache
//* \brief Clean and invalidate D Cache
//*----------------------------------------------------------------------------
void AT91F_CleanDCache()
{
   register char seg, index;
   for (seg = 0; seg < 8; ++seg) {
      for (index = 0; index < 64; ++index) {
         AT91F_ARM_CleanDCacheIDX((index << 26) | (seg << 5));
      }
   }
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ResetICache
//* \brief Reset I Cache (Should be run from a non cachable area)
//*----------------------------------------------------------------------------
void AT91F_ResetICache()
{
   // Flush I TLB
   AT91F_ARM_InvalidateITLB();
   // Flush I cache
   AT91F_ARM_InvalidateICache();
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ResetDCache
//* \brief Reset D Cache (Should be run from a non cachable area)
//*----------------------------------------------------------------------------
void AT91F_ResetDCache()
{
   // Achieve pending write operations
   AT91F_CleanDCache();
   // Flush write buffers
   AT91F_ARM_DrainWriteBuffer();
   // Flush D TLB
   AT91F_ARM_InvalidateDTLB();
   // Flush D cache
   AT91F_ARM_InvalidateDCache();
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_EnableMMU
//* \brief Enable MMU
//*----------------------------------------------------------------------------
void AT91F_EnableMMU()
{
   unsigned int ctl;

   ctl = AT91F_ARM_ReadControl();
   ctl |= (1 << 0);
   AT91F_ARM_WriteControl(ctl);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_DisableMMU
//* \brief Disable MMU
//*----------------------------------------------------------------------------
void AT91F_DisableMMU()
{
   unsigned int ctl;

   ctl = AT91F_ARM_ReadControl();
   ctl &= ~(1 << 0);
   AT91F_ARM_WriteControl(ctl);
}


//*----------------------------------------------------------------------------
//* \fn    AT91F_EnableICache
//* \brief Enable I Cache
//*----------------------------------------------------------------------------
void AT91F_EnableICache()
{
   unsigned int ctl;

   ctl = AT91F_ARM_ReadControl();
   ctl |= (1 << 12);
   AT91F_ARM_WriteControl(ctl);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_DisableICache
//* \brief Disable I Cache
//*----------------------------------------------------------------------------
void AT91F_DisableICache()
{
   unsigned int ctl;

   ctl = AT91F_ARM_ReadControl();
   ctl &= ~(1 << 12);
   AT91F_ARM_WriteControl(ctl);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_EnableDCache
//* \brief Enable D Cache
//*----------------------------------------------------------------------------
void AT91F_EnableDCache()
{
   unsigned int ctl;

   ctl = AT91F_ARM_ReadControl();
   ctl |= (1 << 2);
   AT91F_ARM_WriteControl(ctl);
}


//*----------------------------------------------------------------------------
//* \fn    AT91F_EnableDCache
//* \brief Enable I + D Cache
//*----------------------------------------------------------------------------
void AT91F_EnableIDCache()
{
   unsigned int ctl;

   ctl = AT91F_ARM_ReadControl();
   ctl |= (1 << 2) + (1 << 12);
   AT91F_ARM_WriteControl(ctl);

}


//*----------------------------------------------------------------------------
//* \fn    AT91F_DisableDCache
//* \brief Disable D Cache
//*----------------------------------------------------------------------------
void AT91F_DisableDCache()
{
   unsigned int ctl;

   ctl = AT91F_ARM_ReadControl();
   ctl &= ~(1 << 2);
   AT91F_ARM_WriteControl(ctl);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_LockITLB
//* \brief Lock one I TLB entry after entries previously locked
//*----------------------------------------------------------------------------
void AT91F_LockTLB(unsigned int address)
{
   // Set the P bit
   AT91F_ARM_WriteTLBLockdown( 1);      // base 0 victim 0 P 1
   // Prefetch the instruction assuming an TLB miss occurs, the entry base is loaded
   AT91F_ARM_PrefetchICacheLine(address);
   // Update base and victim values
   AT91F_ARM_WriteTLBLockdown((1 << 26) | (1 << 20));      // base 1 victim 1 P 0
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WriteITLBLockdown
//* \brief Write I TLB lockdown  (Should be run from a non cachable area)
//*----------------------------------------------------------------------------
void AT91F_LockICache(unsigned int startAddress, unsigned int size)
{
   unsigned int victim = 0;

   AT91F_ARM_InvalidateICache();
   AT91F_ARM_WriteICacheLockdown((victim++) << 26);
   while (size) {
      // Prefetch ICache line
      AT91F_ARM_PrefetchICacheLine(startAddress);
      startAddress += 32;
      // Test for segment 0, and if so increment victim pointer
      if ( !(startAddress & 0xE0) )
         AT91F_ARM_WriteICacheLockdown((victim++) << 26);
      size -= 32;

   }
   // If segment != 0 the increment victim pointer
   if ( (startAddress & 0xE0) )
      AT91F_ARM_WriteICacheLockdown(victim << 26);

}


// Write dirty cache lines to memory and invalidate the cache entries
// for the given address range.
void arm926e_dcache_flush_range( unsigned int start, unsigned int len )
{
   start &= 0xFFFFFFE0;
   len += start;
   do
   {
      // clean+invalidate D entry
      //=========================
      __asm("MCR p15, 0, %0, c7, c14, 1\n" : : "r" (start));

      start += 32;
   }
   while( start < len );

   // Drain Write Buffer
   //===================
   register unsigned int sbz = 0;
   __asm("MCR p15, 0, %0, c7, c10, 4\n" : : "r" (sbz));
}


// ARM Translation Table Base Bit Masks
#define ARM_TRANSLATION_TABLE_MASK               0xFFFFC000

// ARM Domain Access Control Bit Masks
#define ARM_ACCESS_TYPE_NO_ACCESS(domain_num)    (0x0 << (domain_num)*2)
#define ARM_ACCESS_TYPE_CLIENT(domain_num)       (0x1 << (domain_num)*2)
#define ARM_ACCESS_TYPE_MANAGER(domain_num)      (0x3 << (domain_num)*2)

struct ARM_MMU_FIRST_LEVEL_FAULT
{
   int id : 2;
   int sbz : 30;
};
#define ARM_MMU_FIRST_LEVEL_FAULT_ID 0x0

struct ARM_MMU_FIRST_LEVEL_PAGE_TABLE
{
   int id : 2;
   int imp : 2;
   int domain : 4;
   int sbz : 1;
   int base_address : 23;
};
#define ARM_MMU_FIRST_LEVEL_PAGE_TABLE_ID 0x1

struct ARM_MMU_FIRST_LEVEL_SECTION
{
   int id : 2;
   int b : 1;
   int c : 1;
   int imp : 1;
   int domain : 4;
   int sbz0 : 1;
   int ap : 2;
   int sbz1 : 8;
   int base_address : 12;
};
#define ARM_MMU_FIRST_LEVEL_SECTION_ID 0x2

struct ARM_MMU_FIRST_LEVEL_RESERVED
{
   int id : 2;
   int sbz : 30;
};
#define ARM_MMU_FIRST_LEVEL_RESERVED_ID 0x3

#define ARM_MMU_FIRST_LEVEL_DESCRIPTOR_ADDRESS(ttb_base, table_index) \
   (unsigned long *)((unsigned long)(ttb_base) + ((table_index) << 2))

#define ARM_FIRST_LEVEL_PAGE_TABLE_SIZE 0x1000 //0x4000 // 16 ko

//modify domain
#define ARM_MMU_SECTION(ttb_base, actual_base, virtual_base,              \
                        cacheable, bufferable, perm)                      \
   CYG_MACRO_START                                                       \
   register union ARM_MMU_FIRST_LEVEL_DESCRIPTOR desc;               \
                                                                          \
   desc.word = 0;                                                    \
   desc.section.id = ARM_MMU_FIRST_LEVEL_SECTION_ID;                 \
   desc.section.imp = 1;                                             \
   desc.section.domain = 0xffff;                                          \
   desc.section.c = (cacheable);                                     \
   desc.section.b = (bufferable);                                    \
   desc.section.ap = (perm);                                         \
   desc.section.base_address = (actual_base);                        \
   *ARM_MMU_FIRST_LEVEL_DESCRIPTOR_ADDRESS(ttb_base, (virtual_base)) \
      = desc.word;                                  \
   CYG_MACRO_END

#define X_ARM_MMU_SECTION(abase,vbase,size,cache,buff,access)      \
   { int i; int j = abase; int k = vbase;                         \
     for (i = size; i > 0; i--,j++,k++)                          \
     {                                                            \
        ARM_MMU_SECTION(ttb_base, j, k, cache, buff, access);      \
     }                                                            \
   }

union ARM_MMU_FIRST_LEVEL_DESCRIPTOR
{
   unsigned long word;
   struct ARM_MMU_FIRST_LEVEL_FAULT fault;
   struct ARM_MMU_FIRST_LEVEL_PAGE_TABLE page_table;
   struct ARM_MMU_FIRST_LEVEL_SECTION section;
   struct ARM_MMU_FIRST_LEVEL_RESERVED reserved;
};

#define ARM_UNCACHEABLE                         0
#define ARM_CACHEABLE                           1
#define ARM_UNBUFFERABLE                        0
#define ARM_BUFFERABLE                          1

#define ARM_ACCESS_PERM_NONE_NONE               0
#define ARM_ACCESS_PERM_RO_NONE                 0
#define ARM_ACCESS_PERM_RO_RO                   0
#define ARM_ACCESS_PERM_RW_NONE                 1
#define ARM_ACCESS_PERM_RW_RO                   2
#define ARM_ACCESS_PERM_RW_RW                   3


#define FLASH_PHYS              (AT91SAM9261_FLASH_PHYS_BASE >> 20)
#define FLASH_VIRT              (AT91SAM9261_FLASH_VIRT_BASE >> 20)
#define FLASH_NCVR              (AT91SAM9261_FLASH_NOCACHE_VIRT_BASE >> 20)

#define SDRAM_PHYS              (AT91SAM9261_SDRAM_PHYS_BASE >> 20)
#define SDRAM_VIRT              (AT91SAM9261_SDRAM_VIRT_BASE >> 20)                             // virtual cached
#define SDRAM_NCVR              (AT91SAM9261_SDRAM_NOCACHE_VIRT_BASE >> 20)             // virtual non cached

//sram address (see ram2 label in ecos target.ld) used for no cached mmu on device driver internals buffers
#define  SDRAM2_PHYS    (AT91SAM9261_SDRAM2_PHYS_BASE >> 20)
#define  SDRAM2_VIRT    (AT91SAM9261_SDRAM2_VIRT_BASE >> 20)            // virtual cached


//lcd user interface address
#define LCD_PHYS                        (0x00600000 >> 20)
#define LCD_VIRT                        (0x00600000 >> 20)                              // virtual cached

//sram address
#define SRAM_PHYS               (0x00300000 >> 20)
#define SRAM_VIRT               (0x00300000 >> 20)

//internal periphericals address
#define PERPH_PHYS              (0xFFF00000 >> 20)
#define PERPH_VIRT              (0xFFF00000 >> 20)

//CS0 for NORflash
#define NOR_FLASH_PHYS          (AT91SAM9261_FLASH_PHYS_BASE >> 20)
#define NOR_FLASH_VIRT          (AT91SAM9261_FLASH_PHYS_BASE >> 20)                             // virtual cached
#define NOR_FLASH_NCVR          (AT91SAM9261_FLASH_NOCACHE_VIRT_BASE >> 20)             // virtual non cached

//CS2 for ethernet DM9000a
#define EBI_CS2_PHYS            (0x30000000 >>20)
#define EBI_CS2_VIRT            (0x30000000 >>20)

unsigned int ttb_tab[ARM_FIRST_LEVEL_PAGE_TABLE_SIZE] __attribute__ ((aligned (16384),
                                                                      section (".no_init")));
void hal_mmu_init(void) {
   unsigned int * ttb_base = (unsigned int *)&ttb_tab;

   //Disable MMU first
   AT91F_DisableICache();
   AT91F_CleanDCache();
   AT91F_DisableDCache();
   AT91F_DisableMMU();

#ifdef CYG_HAL_ARM_AT91SAM9261_MMU
   // Set the TTB register
   asm volatile ("mcr  p15,0,%0,c2,c0,0" : : "r" (ttb_base) );

   // Set the Domain Access Control Register
   asm volatile ("mcr  p15,0,%0,c3,c0,0" : : "r" (0xC0000000) ); // domain 15: access are not checked

   // First clear all TT entries - ie Set them to Faulting
   memset((void *)ttb_base, 0, ARM_FIRST_LEVEL_PAGE_TABLE_SIZE*sizeof(int));

   //               Actual              Virtual         Size   Attributes                                                    Function
   //		         Base                   Base            MB     cached?           buffered?        access permissions
   //             xxx00000              xxx00000

   X_ARM_MMU_SECTION(0,                         0,      1, ARM_UNCACHEABLE,   ARM_UNBUFFERABLE,
                     ARM_ACCESS_PERM_RW_RW);                                                                              //first entry (vector address)
   X_ARM_MMU_SECTION(SRAM_PHYS,         SRAM_VIRT,      1, ARM_CACHEABLE,   ARM_UNBUFFERABLE,
                     ARM_ACCESS_PERM_RW_RW);                                                                            // SRAM cached
   X_ARM_MMU_SECTION(LCD_PHYS,          LCD_PHYS,       1, ARM_CACHEABLE,   ARM_UNBUFFERABLE,
                     ARM_ACCESS_PERM_RW_RW);                                                                            // LCD cached
   X_ARM_MMU_SECTION(SDRAM_PHYS,        SDRAM_VIRT,     63, ARM_CACHEABLE,   ARM_UNBUFFERABLE,
                     ARM_ACCESS_PERM_RW_RW);                                                                             // SDRAM cached
   //leave the next 1 MEGA byte unchache and unbuffarable
   X_ARM_MMU_SECTION(SDRAM2_PHYS,     SDRAM2_VIRT,      1, ARM_UNCACHEABLE,   ARM_UNBUFFERABLE,
                     ARM_ACCESS_PERM_RW_RW);
   //
   X_ARM_MMU_SECTION(PERPH_PHYS,        PERPH_PHYS,                     1,  ARM_UNCACHEABLE,
                     ARM_UNBUFFERABLE,
                     ARM_ACCESS_PERM_RW_RW);                                                                                           // Internal Peripherals
   //
   X_ARM_MMU_SECTION(NOR_FLASH_PHYS,    NOR_FLASH_VIRT,                 32,  ARM_UNCACHEABLE,
                     ARM_UNBUFFERABLE,
                     ARM_ACCESS_PERM_RW_RW);                                                                                            // CS0 for norflash
   X_ARM_MMU_SECTION(EBI_CS2_PHYS,      EBI_CS2_VIRT,                   32,  ARM_UNCACHEABLE,
                     ARM_UNBUFFERABLE,
                     ARM_ACCESS_PERM_RW_RW);                                                                                            // CS2 for ethernet DM9000a
   // Enable the MMU
   //===============
   AT91F_EnableMMU();
   // Enable I + D cache
   //===================
   AT91F_EnableIDCache();
   AT91F_ResetICache();
   AT91F_ResetDCache();
#endif
}


