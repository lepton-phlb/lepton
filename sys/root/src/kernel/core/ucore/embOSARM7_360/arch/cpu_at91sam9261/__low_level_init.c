/*********************************************************************
*               SEGGER MICROCONTROLLER SYSTEME GmbH                  *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (C) 2006   SEGGER Microcontroller Systeme GmbH               *
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
*       embOS version: 3.32e                                         *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : __low_level_init.c     (for ATMEL AT91SAM9261-EK Eval board)
Purpose : Initializes pll, and SDRAM controller
          Feel free to modify this file acc. to your
          target system.
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.H"
#include "OS_Config.h"           /* contains default clock settings */
#include "kernel/dev/arch/arm9/at91sam9261/dev_at91sam9261_cpu/dev_at91sam9261_cpu.h"
#include "kernel/core/kal.h"
//#include <AT91SAM9261.h>
/*********************************************************************
*
*       Configuration
*
*********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
*
*       Clock frequency settings
*/
#ifndef OS_FSYS
  #error "OS_FSYS has to be defined in OS_Config.h"
#endif



/****** Check PLL configuration settings ****************************/
#if (OS_FSYS != 200908800uL)
  #error "Please modify configuration of PLL and main clock oscillator"
#endif

/********************************************************************/

#define _DELAY_PLL       (1000)
#define _DELAY_MAIN_FREQ (1000)

/*********************************************************************
*
*       Local defines (sfrs used in __low_level_init.c)
*
**********************************************************************
*/

/*      Reset controller */
#define _RSTC_BASE    (0xFFFFFD00)
#define _RSTC_CR      (*(volatile OS_U32*) (_RSTC_BASE + 0x00))
#define _RSTC_SR      (*(volatile OS_U32*) (_RSTC_BASE + 0x04))
#define _RSTC_MR      (*(volatile OS_U32*) (_RSTC_BASE + 0x08))
#define _RSTC_BODIEN  (1 << 16)  /* Brownout interrupt enable   */
#define _RSTC_URSTIEN (1 <<  4)  /* User reset interrupt enable */
#define _RSTC_BODSTS  (1 <<  1)  /* Brownout status             */
#define _RSTC_URSTS   (1 <<  0)  /* User reset status           */
#define _RSTC_URSTEN  (1 <<  0)  /* User reset enable           */

/*      Watchdog */
#define _WDT_BASE     (0xFFFFFD40)
#define _WDT_CR       (*(volatile OS_U32*) (_WDT_BASE + 0x00))
#define _WDT_MR       (*(volatile OS_U32*) (_WDT_BASE + 0x04))
#define _WDT_SR       (*(volatile OS_U32*) (_WDT_BASE + 0x08))
#define _WDT_WDFIEN   (1 << 12)   /* Watchdog interrupt enable flag in mode register */
#define _WDT_WDERR    (1 <<  1)   /* Watchdog error status flag                      */
#define _WDT_WDUNF    (1 <<  0)   /* Watchdog underflow status flag                  */

/*      Power management controller */
#define _PMC_BASE   (0xFFFFFC00)


#define _PMC_CKGR_PLLAR (*(volatile OS_U32*) (_PMC_BASE + 0x28))  /* PLLA register */
#define _PMC_CKGR_PLLBR (*(volatile OS_U32*) (_PMC_BASE + 0x2c))  /* PLLB register */

#define _PMC_PCER     (*(volatile OS_U32*) (_PMC_BASE + 0x10))  /* Peripheral clock enable register */
#define _PMC_MOR      (*(volatile OS_U32*) (_PMC_BASE + 0x20))  /* main oscillator register */
#define _PMC_PLLR     (*(volatile OS_U32*) (_PMC_BASE + 0x2c))  /* PLL register */
#define _PMC_MCKR     (*(volatile OS_U32*) (_PMC_BASE + 0x30))  /* Master clock register */
#define _PMC_SR       (*(volatile OS_U32*) (_PMC_BASE + 0x68))  /* status register */
#define _PMC_IMR      (*(volatile OS_U32*) (_PMC_BASE + 0x6C))  /* interrupt mask register */

#define _PMC_MOSCS    (1 <<  0)
#define _CKGR_MAINRDY (1 << 16)
#define _PMC_LOCKA    (1 <<  1)
#define _PMC_LOCKB    (1 <<  2)
#define _PMC_MCKRDY   (1 <<  3)

#define _PMC_CSS_MAIN_CLK_SELECT (0x1)

/*      MATRIX + EBI interface */
#define _MATRIX_BASE_ADDR   (0xFFFFEE00)                                // MATRIX Base Address

#define _MATRIX_MCFG   (*(volatile OS_U32*) (_MATRIX_BASE_ADDR + 0x00)) // MATRIX Master configuration register
#define _MATRIX_EBICSA (*(volatile OS_U32*) (_MATRIX_BASE_ADDR + 0x30)) // MATRIX EBI Chip Select Assignment register

/*      PIOC, used as data BUS */
#define _PIOC_BASE_ADDR     (0xFFFFF800)
#define _PIOC_PDR      (*(volatile OS_U32*) (_PIOC_BASE_ADDR + 0x04))   // PIOC disable register
#define _PIOC_ASR      (*(volatile OS_U32*) (_PIOC_BASE_ADDR + 0x70))   // PIOC peripheral A select register

/*      SDRAM controller */
#define _SDRAMC_BASE_ADDR  (0xFFFFEA00)   // SDRAMC Base Address
#define _SDRAMC_MR     (*(volatile OS_U32*) (_SDRAMC_BASE_ADDR + 0x00)) // (SDRAMC) SDRAM Controller Mode Register
#define _SDRAMC_TR     (*(volatile OS_U32*) (_SDRAMC_BASE_ADDR + 0x04)) // (SDRAMC) SDRAM Controller Refresh timer Register
#define _SDRAMC_CR     (*(volatile OS_U32*) (_SDRAMC_BASE_ADDR + 0x08)) // (SDRAMC) SDRAM Controller Configuration Register

#define _SDRAMC_MODE_NORMAL_CMD   (0x0) // (SDRAMC) Normal Mode
#define _SDRAMC_MODE_LMR_CMD      (0x3) // (SDRAMC) Issue a Load Mode Register at every access
#define _SDRAMC_MODE_RFSH_CMD     (0x4) // (SDRAMC) Issue a Refresh
#define _SDRAMC_MODE_PRCGALL_CMD  (0x2) // (SDRAMC) Issue a All Banks Precharge Command at every access

#define SDRAM_BASE_ADDR   (0x20000000)

  
/*********************************************************************
*
*       MMU and cache configuration
*/
#pragma data_alignment=16384
__no_init static unsigned int _TranslationTable [0x1000];

/*********************************************************************
*
*       local functions
*
**********************************************************************
*/

/*********************************************************************
*
*       _InitFlash
*
* Function description
*   Initialize wait states and BUS interface for external Flash access
*   Needs to be called before main clock is switched to high speed
*   Depends on specific hardware and has to be implemented by user.
*/
#pragma location="ICODE"
static void _InitFlash(void) {
    int busWidth= 16;
 /* 
    // Configure SMC
    AT91C_BASE_SMC->SMC_SETUP0 = 0x00000002;
    AT91C_BASE_SMC->SMC_PULSE0 = 0x0A0A0A06;
    AT91C_BASE_SMC->SMC_CYCLE0 = 0x000A000A;
    AT91C_BASE_SMC->SMC_CTRL0  = (AT91C_SMC_READMODE
                                  | AT91C_SMC_WRITEMODE
                                  | AT91C_SMC_NWAITM_NWAIT_DISABLE
                                  | ((0x1 << 16) & AT91C_SMC_TDF));
    
    if (busWidth == 8) {
        AT91C_BASE_SMC->SMC_CTRL0 |= AT91C_SMC_DBW_WIDTH_EIGTH_BITS;
    }
    else if (busWidth == 16) {
        AT91C_BASE_SMC->SMC_CTRL0 |= AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS;
    }
    else if (busWidth == 32) {
        AT91C_BASE_SMC->SMC_CTRL0 |= AT91C_SMC_DBW_WIDTH_THIRTY_TWO_BITS;
    }
    */
}

/*********************************************************************
*
*       _InitClocks
*
* Function description
*   Initialize main clock and PLLs
*
* NOTES: (1) Settings used here are for AT91SAM9261-EK eval board
*            and may have to be modified for other target board or frequency
*/
#pragma location="ICODE"
static void _InitClocks(OS_U32 PLLA_Val, OS_U32 PLLB_Val, OS_U32 MCKR_Val) {
  //
  // Enable main clock OSC and wait until clock is stable
  //
  _PMC_MOR = ((48/8) << 8) + 0x01; // Startup time = 48 slow clocks, Main oscillator enable
  while ((_PMC_SR & _PMC_MOSCS) == 0) {
  }
  //
  // Initialize PLL A for Main clock
  //
  _PMC_CKGR_PLLAR = PLLA_Val;              // Setup PLL A
  //
  // Wait until PLLA locks
  //
  while((_PMC_SR & _PMC_LOCKA) == 0) {
  }
  //
  // Wait until the master clock is stable
  //
  while((_PMC_SR & _PMC_MCKRDY) == 0) {
  }
  //
  // Initialize PLL B for USB clock
  //
  _PMC_CKGR_PLLBR = PLLB_Val;         // Setup PLL B
  //
  // Wait until PLLB locks
  //
  while((_PMC_SR & _PMC_LOCKB) == 0)  {
  }
  //
  // Wait until the master clock is stable
  //
  while ((_PMC_SR & _PMC_MCKRDY) == 0) {
  }
  //
  // Select PLL A clock as clock source
  //
  _PMC_MCKR = MCKR_Val;
  //
  // Wait until the master clock is stable */
  //
  while((_PMC_SR & _PMC_MCKRDY) == 0) {
  }
}

/*********************************************************************
*
*       _InitSDRAM
*
* Function description
*   Initialize SDRAM controller
*
* NOTE: Settings used here are for AT91SAM9261-EK eval board
*       and may have to be modified for other target board
*/
#pragma location="ICODE"
static void _InitSDRAM(void) {
  volatile int i;
  volatile int Dummy;
  //
  // Setup bus interface for SDRAM
  //
  _MATRIX_EBICSA |= (1 << 1);    // Activate CS1 for SDRAM
  _PIOC_ASR      = 0xFFFF0000;   // Upper 16 lines are data lines for SDRAM
  _PIOC_PDR      = 0xFFFF0000;   // Disable PIO for upper 16 bits
  //
  // Initialize SDRAM controller
  //
  _SDRAMC_CR     = 0x01          // 9 Column bits
                 |(0x02 << 2)    // 13 Rows
                 |(0x02 << 5)    // 2 CAS cycles
                 |(0x01 << 4)    // 4 Banks
                 |(0x00 << 7)    // 32 bit Data BUS
                 |(0x02 << 8)    // 2 write recovery cycles
                 |(0x07 << 12)   // 7 RAS cycles
                 |(0x02 << 16)   // 2 RAS precharge time cycles
                 |(0x02 << 20)   // 2 cycles RAS to CAS delay
                 |(0x05 << 24)   // 5 cycles RAS active time
                 |(0x08 << 28)   // 8 cycles command recovery time
                 ;

  for (i = 0; i < 10000; i++) {  // Delay
  }
  _SDRAMC_MR     = _SDRAMC_MODE_PRCGALL_CMD;
  Dummy = *(volatile int*) SDRAM_BASE_ADDR;    // Dummy read

  for (i = 0; i < 10000; i++) {  // Delay
  }

  for (i = 0; i < 8; i++) {      // Auto-Refresh
    _SDRAMC_MR = _SDRAMC_MODE_RFSH_CMD;
    Dummy = *(volatile int*) SDRAM_BASE_ADDR;  // Dummy read
  }

  _SDRAMC_MR = _SDRAMC_MODE_LMR_CMD;
  *(volatile int*) (SDRAM_BASE_ADDR+20) = 0xcafedede;

  _SDRAMC_TR = (OS_FSYS * 2) / 2 / 1000000;
  _SDRAMC_MR = _SDRAMC_MODE_NORMAL_CMD;        // Switch to normal operation
  Dummy      = SDRAM_BASE_ADDR;                // Finally perform a dummy read!
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*       __low_level_init()
*
*       Initialize memory controller, clock generation and pll
*
*       Has to be modified, if another CPU clock frequency should be
*       used. This function is called during startup and
*       has to return 1 to perform segment initialization
*
* NOTE:
*  ICODE is the same segment as cstartup. By placing __low_level_init
*  in the same segment, we make sure it can be reached with BL.
*/
OS_INTERWORK int __low_level_init(void);  // Avoid "no ptototype" warning
#pragma location="ICODE"
OS_INTERWORK int __low_level_init(void) {
  //(be aware: this code not used in bootstrap because it's locked firmware access to watch dog) see at91_cstartup.s79
  //_WDT_MR = (1 << 15);                    // Initially disable watchdog
  
  //
  //  Setup waitstates for external BUS interface before switching to high speed
  //
  _InitFlash();
  //
  //  Initialize main clock oscillator and PLLs
  //
  _InitClocks(_PLLAR_VALUE, _PLLBR_VALUE, _MCKR_VALUE);
  //
  //  Initialize SDRAM
  //
  //_InitSDRAM();
  //
  //  Perform other initialization here, if required
  //
   //
  // Init MMU and caches
  //

  OS_ARM_MMU_InitTT      (&_TranslationTable[0]);

#if SDRAM_TARGET
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_C_B,     0x000, 0x200, 0x001);    // SDRAM, the first MB remapped to 0 to map vectors to correct address, cacheable, bufferable
#else
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_C_B,     0x000, 0x000, 0x001);    // Internal SRAM, cacheable, bufferable
#endif
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_NC_NB,   0x003, 0x003, 0x001);    // Internal SRAM, original address, NON cachable, NON bufferable
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_NC_NB,   0x006, 0x006, 0x001);    // LCD controller, NON cachable, NON bufferable
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_C_B,     0x200, 0x200, 0x040);    // SDRAM, original address, 64MB, cacheable, bufferable
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_C_NB,    0x240, 0x200, 0x040);    // SDRAM, mapped to 0x24000000, cacheable, NON bufferable
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_NC_NB,   0x280, 0x200, 0x040);    // SDRAM, mapped to 0x28000000, NON cacheable, NON bufferable
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_NC_NB,   0x300, 0x300, 0x001);    // External SRAM area CS2, connected to Ethernet controller
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_NC_NB,   0x400, 0x400, 0x020);    // NAND controller, NON cacheable, NON bufferable
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_NC_NB,   0xfff, 0xfff, 0x001);    // SFR area NON cacheable, NON bufferable
  OS_ARM_MMU_Enable      (&_TranslationTable[0]);
 
  OS_ARM_ICACHE_Enable();
  OS_ARM_DCACHE_Enable();

  //
  _RSTC_MR = ((0xA5 << 24) | _RSTC_URSTEN);  // write KEY and URSTEN to allow USER RESET
  return 1;
}

/********************************************************************/

#ifdef __cplusplus
}
#endif

/*****  EOF  ********************************************************/

