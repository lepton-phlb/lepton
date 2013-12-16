/*********************************************************************
*               SEGGER MICROCONTROLLER GmbH & Co KG                  *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (c) 1995 - 2013  SEGGER Microcontroller GmbH & Co KG         *
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
*       OS version: 3.88a                                            *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : RTOSInit_AT91SAM9261.c
          for ATMEL AT91SAM9261 CPU

Purpose : Initializes and handles the hardware for the OS as far
          as required by the OS.
          Feel free to modify this file acc. to your
          target system.
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.h"

/*********************************************************************
*
*       Configuration
*
**********************************************************************
*/

#define ALLOW_NESTED_INTERRUPTS 0   // Caution: Nesting interrupts will cause higher stack load on system stack CSTACK

/*********************************************************************
*
*       Clock frequency settings
*/

#define OS_FSYS 200908800uL

#ifndef   OS_PCLK_TIMER
  #define OS_PCLK_TIMER (OS_FSYS / 2)
#endif

#ifndef   OS_TICK_FREQ
  #define OS_TICK_FREQ (1000)
#endif

#define OS_TIMER_PRESCALE (16) // prescaler for system timer is fixed to 16

#define MUL_PLLA     (109)      // Multiplier
#define OUT_PLLA     (0x02)     // High speed setting
#define COUNT_PLLA   (0x20)     // startup counter
#define DIV_PLLA     (10)       // Divider

#define PLLAR_VALUE  ((1uL << 29)                \
                     |((MUL_PLLA-1) << 16)       \
                     |(OUT_PLLA     << 14)       \
                     |(COUNT_PLLA   << 8)        \
                     |(DIV_PLLA     << 0))

#define MUL_PLLB     (26*2)     // Multiplier
#define OUT_PLLB     (0x00)     // High speed setting
#define COUNT_PLLB   (0x20)     // startup counter
#define DIV_PLLB     (10)       // Divider
#define USB_DIV      (01)       // USB-clock Divider

#define PLLBR_VALUE  ((USB_DIV      << 28)       \
                     |((MUL_PLLB-1) << 16)       \
                     |(OUT_PLLB     << 14)       \
                     |(COUNT_PLLB   << 8)        \
                     |(DIV_PLLB     << 0))

#define MCKR_MDIV    (0x01)     // Main clock is processor clock / 2
#define MCKR_PRES    (0)        // Processor clock is selected clock
#define MCKR_CSS     (0x02)     // PLLA is selected clock

#define MCKR_VALUE   ((MCKR_MDIV << 8)   \
                     |(MCKR_PRES << 2)   \
                     |(MCKR_CSS  << 0))

/*********************************************************************
*
*       Configuration of communication to OSView
*/
#ifndef   OS_VIEW_ENABLE            // Global enable of communication
  #define OS_VIEW_ENABLE    (1)     // Default: on
#endif

#ifndef   OS_VIEW_USE_UART          // If set, UART will be used
  #define OS_VIEW_USE_UART  (0)     // Default: 0 => DCC is used
#endif                              // if OS_VIEW_ENABLE is on

/*********************************************************************
*
*       UART settings for OSView
*       If you do not want (or can not due to hardware limitations)
*       to dedicate a UART to OSView, please define it to be -1
*       Currently UART0 and UART1 are supported and the standard
*       setup enables UART 0 per default
*       When using DCC for communiction, the UART is not used for embOSView,
*       regardless the OS_UART settings
*/
#ifndef   OS_UART
  #define OS_UART (0)
#endif

#ifndef   OS_PCLK_UART
  #define OS_PCLK_UART (OS_FSYS / 2)
#endif

#ifndef   OS_BAUDRATE
  #define OS_BAUDRATE (38400)
#endif

/********************************************************************/

#ifndef   SDRAM_TARGET
  #define SDRAM_TARGET 0
#endif

/****** End of configuration settings *******************************/


/*********************************************************************
*
*       DCC and UART settings for OSView
*
*       Automatically generated from configuration settings
*/
#define OS_USE_DCC     (OS_VIEW_ENABLE && (OS_VIEW_USE_UART == 0))

#define OS_UART_USED   (OS_VIEW_ENABLE && (OS_VIEW_USE_UART != 0)) && ((OS_UART == 0) || (OS_UART == 1) || (OS_UART == 2))

/********************************************************************/

#if (DEBUG || OS_USE_DCC)
  #include "JLINKDCC.h"
#endif

/*********************************************************************
*
*       Local defines (sfrs used in RTOSInit.c)
*
**********************************************************************
*/

/*      USART, used for OSView communication */
#define USART0_BASE_ADDR  (0xFFFB0000)
#define USART1_BASE_ADDR  (0xFFFB4000)
#define USART2_BASE_ADDR  (0xFFFB8000)

/*      Debug unit */
#define DBGU_BASE_ADDR    (0xFFFFF200)
#define DBGU_IMR      (*(volatile OS_U32*) (DBGU_BASE_ADDR + 0x10)) /* Interrupt Mask Register */
#define DBGU_SR       (*(volatile OS_U32*) (DBGU_BASE_ADDR + 0x14)) /* Channel Status Register */
#define DBGU_COMMRX   (1uL << 31)
#define DBGU_COMMTX   (1uL << 30)
#define DBGU_RXBUFF   (1uL << 12)
#define DBGU_TXBUFE   (1uL << 11)
#define DBGU_TXEMPTY  (1uL <<  9)
#define DBGU_PARE     (1uL <<  7)
#define DBGU_FRAME    (1uL <<  6)
#define DBGU_OVRE     (1uL <<  5)
#define DBGU_ENDTX    (1uL <<  4)
#define DBGU_ENDRX    (1uL <<  3)
#define DBGU_TXRDY    (1uL <<  1)
#define DBGU_RXRDY    (1uL <<  0)
#define DBGU_MASK_ALL (DBGU_COMMRX | DBGU_COMMTX  | DBGU_RXBUFF |  \
                       DBGU_TXBUFE | DBGU_TXEMPTY | DBGU_PARE   |  \
                       DBGU_FRAME  | DBGU_OVRE    | DBGU_ENDTX  |  \
                       DBGU_ENDRX  | DBGU_TXRDY   | DBGU_RXRDY)

/*      Reset controller */
#define RSTC_BASE_ADDR    (0xFFFFFD00)
#define RSTC_CR       (*(volatile OS_U32*) (RSTC_BASE_ADDR + 0x00))
#define RSTC_SR       (*(volatile OS_U32*) (RSTC_BASE_ADDR + 0x04))
#define RSTC_MR       (*(volatile OS_U32*) (RSTC_BASE_ADDR + 0x08))
#define RSTC_URSTEN   (1uL <<  0)  /* User reset enable           */
#define RSTC_BODIEN   (1uL << 16)  /* Brownout interrupt enable   */
#define RSTC_URSTIEN  (1uL <<  4)  /* User reset interrupt enable */
#define RSTC_BODSTS   (1uL <<  1)  /* Brownout status             */
#define RSTC_URSTS    (1uL <<  0)  /* User reset status           */

/*      Real time timer */
#define RTT_BASE_ADDR     (0xFFFFFD20)
#define RTT_MR        (*(volatile OS_U32*) (RTT_BASE_ADDR + 0x00))
#define RTT_SR        (*(volatile OS_U32*) (RTT_BASE_ADDR + 0x0C))
#define RTT_RTTINCIEN (1uL << 17)
#define RTT_ALMIEN    (1uL << 16)
#define RTT_RTTINC    (1uL << 1)
#define RTT_ALMS      (1uL << 0)

/*      Periodic interval timer */
#define PIT_BASE_ADDR     (0xFFFFFD30)
#define PIT_MR        (*(volatile OS_U32*) (PIT_BASE_ADDR + 0x00))
#define PIT_SR        (*(volatile OS_U32*) (PIT_BASE_ADDR + 0x04))
#define PIT_PIVR      (*(volatile OS_U32*) (PIT_BASE_ADDR + 0x08))
#define PIT_PIIR      (*(volatile OS_U32*) (PIT_BASE_ADDR + 0x0C))

/*      Watchdog */
#define WDT_BASE_ADDR     (0xFFFFFD40)
#define WDT_CR        (*(volatile OS_U32*) (WDT_BASE_ADDR + 0x00))
#define WDT_MR        (*(volatile OS_U32*) (WDT_BASE_ADDR + 0x04))
#define WDT_SR        (*(volatile OS_U32*) (WDT_BASE_ADDR + 0x08))
#define WDT_WDFIEN    (1uL << 12) /* Watchdog interrupt enable flag in mode register */
#define WDT_WDERR     (1uL <<  1) /* Watchdog error status flag                      */
#define WDT_WDUNF     (1uL <<  0) /* Watchdog underflow status flag                  */

/*      PIO control register */
#define PIOA_BASE_ADDR    (0xfffff400)
#define PIOB_BASE_ADDR    (0xfffff600)
#define PIOC_BASE_ADDR    (0xfffff800)


/*      Power management controller */
#define PMC_BASE_ADDR     (0xFFFFFC00)

#define PMC_CKGR_PLLAR (*(volatile OS_U32*) (PMC_BASE_ADDR + 0x28))  /* PLLA register */
#define PMC_CKGR_PLLBR (*(volatile OS_U32*) (PMC_BASE_ADDR + 0x2c))  /* PLLB register */

#define PMC_SCDR     (*(volatile OS_U32*) (PMC_BASE_ADDR + 0x04))  /* System Clock Disable Register */
#define PMC_PCER     (*(volatile OS_U32*) (PMC_BASE_ADDR + 0x10))  /* Peripheral clock enable register */
#define PMC_MOR      (*(volatile OS_U32*) (PMC_BASE_ADDR + 0x20))  /* main oscillator register */
#define PMC_PLLR     (*(volatile OS_U32*) (PMC_BASE_ADDR + 0x2c))  /* PLL register */
#define PMC_MCKR     (*(volatile OS_U32*) (PMC_BASE_ADDR + 0x30))  /* Master clock register */
#define PMC_SR       (*(volatile OS_U32*) (PMC_BASE_ADDR + 0x68))  /* status register */
#define PMC_IMR      (*(volatile OS_U32*) (PMC_BASE_ADDR + 0x6C))  /* interrupt mask register */

#define PMC_MOSCS    (1uL <<  0)
#define CKGR_MAINRDY (1uL << 16)
#define PMC_LOCKA    (1uL <<  1)
#define PMC_LOCKB    (1uL <<  2)
#define PMC_MCKRDY   (1uL <<  3)
#define PMC_PCKRDY2  (1uL << 10)
#define PMC_PCKRDY1  (1uL <<  9)
#define PMC_PCKRDY0  (1uL <<  8)
#define PMC_MASK_ALL (PMC_PCKRDY2 | PMC_PCKRDY1 | PMC_PCKRDY0 | \
                      PMC_MCKRDY  | PMC_LOCKB   | PMC_MOSCS)

/*      Advanced interrupt controller (AIC) */
#define AIC_BASE_ADDR      (0xfffff000)
#define AIC_SMR_BASE_ADDR  (AIC_BASE_ADDR + 0x00)
#define AIC_SVR_BASE_ADDR  (AIC_BASE_ADDR + 0x80)
#define AIC_SVR0      (*(volatile OS_U32*) (AIC_SVR_BASE_ADDR + 0x00))
#define AIC_SVR1      (*(volatile OS_U32*) (AIC_SVR_BASE_ADDR + 0x04))
#define AIC_IVR       (*(volatile OS_U32*) (AIC_BASE_ADDR + 0x100))
#define AIC_ISR       (*(volatile OS_U32*) (AIC_BASE_ADDR + 0x108))
#define AIC_IPR       (*(volatile OS_U32*) (AIC_BASE_ADDR + 0x10c))
#define AIC_IDCR      (*(volatile OS_U32*) (AIC_BASE_ADDR + 0x124))
#define AIC_ICCR      (*(volatile OS_U32*) (AIC_BASE_ADDR + 0x128))
#define AIC_IECR      (*(volatile OS_U32*) (AIC_BASE_ADDR + 0x120))
#define AIC_EOICR     (*(volatile OS_U32*) (AIC_BASE_ADDR + 0x130))
#define AIC_SPU       (*(volatile OS_U32*) (AIC_BASE_ADDR + 0x134))
#define AIC_DCR       (*(volatile OS_U32*) (AIC_BASE_ADDR + 0x138))
#define AIC_FFDR      (*(volatile OS_U32*) (AIC_BASE_ADDR + 0x144))

/*      AIC interrupt sources and peripheral IDs        */
#define SYSTEM_IRQ_ID  (1)   /* System IRQ ID             */
#define US0IRQ_ID      (6)   /* USART Channel 0 interrupt */
#define US1IRQ_ID      (7)   /* USART Channel 1 interrupt */
#define US2IRQ_ID      (8)   /* USART Channel 2 interrupt */

#ifndef   NUM_INT_SOURCES
  #define NUM_INT_SOURCES   (32)
#endif

#define INT_PRIORITY_MASK (0x07)
#define NUM_INT_PRIORITIES   (8)

/*      MATRIX + EBI interface */
#define MATRIX_BASE_ADDR   (0xFFFFEE00)                                // MATRIX Base Address

#define MATRIX_MCFG   (*(volatile OS_U32*) (MATRIX_BASE_ADDR + 0x00)) // MATRIX Master configuration register
#define MATRIX_EBICSA (*(volatile OS_U32*) (MATRIX_BASE_ADDR + 0x30)) // MATRIX EBI Chip Select Assignment register

/*      PIOC, used as data BUS */
#define PIOC_PDR       (*(volatile OS_U32*) (PIOC_BASE_ADDR + 0x04))   // PIOC disable register
#define PIOC_MDDR      (*(volatile OS_U32*) (PIOC_BASE_ADDR + 0x54))    // PIOC multi driver disable register
#define PIOC_ASR       (*(volatile OS_U32*) (PIOC_BASE_ADDR + 0x70))   // PIOC peripheral A select register

/*      SDRAM controller */
#define SDRAMC_BASE_ADDR  (0xFFFFEA00)   // SDRAMC Base Address
#define SDRAMC_MR     (*(volatile OS_U32*) (SDRAMC_BASE_ADDR + 0x00)) // (SDRAMC) SDRAM Controller Mode Register
#define SDRAMC_TR     (*(volatile OS_U32*) (SDRAMC_BASE_ADDR + 0x04)) // (SDRAMC) SDRAM Controller Refresh timer Register
#define SDRAMC_CR     (*(volatile OS_U32*) (SDRAMC_BASE_ADDR + 0x08)) // (SDRAMC) SDRAM Controller Configuration Register
#define SDRAMC_LPR    (*(volatile OS_U32*) (SDRAMC_BASE_ADDR + 0x10)) // (SDRAMC) SDRAM Controller Low Power Register
#define SDRAMC_MDR    (*(volatile OS_U32*) (SDRAMC_BASE_ADDR + 0x24)) // (SDRAMC) SDRAM Controller Memory Device Register

#define SDRAMC_MODE_NORMAL_CMD   (0x0) // (SDRAMC) Normal Mode
#define SDRAMC_MODE_NOP_CMD      (0x1) // (SDRAMC) Issue a All Banks Precharge Command at every access
#define SDRAMC_MODE_PRCGALL_CMD  (0x2) // (SDRAMC) Issue a All Banks Precharge Command at every access
#define SDRAMC_MODE_LMR_CMD      (0x3) // (SDRAMC) Issue a Load Mode Register at every access
#define SDRAMC_MODE_RFSH_CMD     (0x4) // (SDRAMC) Issue a Refresh

#define SDRAM_BASE_ADDR   (0x20000000)

/*      SMC, static memory controller */

#define SMC_BASE_ADDR    (0xFFFFEC00)                  // SMC  Base Address
#define SMC2_BASE_ADDR   (SMC_BASE_ADDR + (0x10 * 2))  // SMC2 Base Address
#define SMC_SETUP2      (*(volatile OS_U32*) (SMC2_BASE_ADDR + 0x00))
#define SMC_PULSE2      (*(volatile OS_U32*) (SMC2_BASE_ADDR + 0x04))
#define SMC_CYCLE2      (*(volatile OS_U32*) (SMC2_BASE_ADDR + 0x08))
#define SMC_MODE2       (*(volatile OS_U32*) (SMC2_BASE_ADDR + 0x0C))

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

/*********************************************************************
*
*       MMU and cache configuration
*/
#ifdef __ICCARM__
  #pragma data_alignment=16384
  static __no_init unsigned int _TranslationTable [0x1000];
#endif
#ifdef __CC_ARM
  static unsigned int _TranslationTable [0x1000] __attribute__ ((aligned (16384), section (".no_init")));
#endif

#if DEBUG
static OS_U32 _SpuriousIrqCnt;
#endif

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/

/*********************************************************************
*
*       _HandlePmcIrq(), Power management controller interrupt
*       If not used for application, this handler may be removed
*/
static void _HandlePmcIrq(void) {
#if DEBUG
  OS_U32 IrqSource;
  IrqSource  = PMC_IMR;
  IrqSource &= (PMC_SR & PMC_MASK_ALL);
  if (IrqSource) {  /* PMC interrupt pending? */
    while(1);       /* Not implemented        */
  }
#endif
}

/*********************************************************************
*
*       _HandleRttIrq(), Real time timer interrupt handler
*       If not used for application, this handler may be removed
*/
static void _HandleRttIrq(void) {
#if DEBUG
  OS_U32 IrqStatus;
  OS_U32 IrqEnabled;

  IrqEnabled = RTT_MR & (RTT_RTTINCIEN  | RTT_ALMIEN);
  IrqStatus  = RTT_SR & (RTT_RTTINC | RTT_ALMS);
  if ((IrqStatus & RTT_RTTINC) && (IrqEnabled & RTT_RTTINCIEN )) { /* RTT inc. interrupt pending ? */
    while(1);                /* Not implemented */
  }
  if ((IrqStatus & RTT_ALMS) && (IrqEnabled & RTT_ALMIEN )) {      /* Alarm interrupt pending ? */
    while(1);                /* Not implemented */
  }
#endif
}

/*********************************************************************
*
*       _HandleDbguIrq(), Debug unit interrupt handler
*       If not used for application, this handler may be removed
*/
extern void (*g_p_fct_dbg_interrupt)(void);
static void _HandleDbguIrq(void) {
//#if DEBUG
  OS_U32 IrqSource;

  IrqSource  = DBGU_IMR;
  IrqSource &= (DBGU_SR & DBGU_MASK_ALL);
  if (IrqSource) { /* Any interrupt pending ? */
    //while(1);      /* Not implemented         */
    if(g_p_fct_dbg_interrupt)
      g_p_fct_dbg_interrupt();
  }
//#endif
}

/*********************************************************************
*
*       _HandleRstcIrq(), Reset controller interrupt handler
*       If not used for application, this handler may be removed
*/
static void _HandleRstcIrq(void) {
#if DEBUG
  OS_U32 IrqStatus;
  OS_U32 IrqEnabled;

  IrqEnabled = RSTC_MR & (RSTC_BODIEN | RSTC_URSTIEN);
  IrqStatus  = RSTC_SR & (RSTC_BODSTS | RSTC_URSTS);
  if ((IrqStatus & RSTC_BODSTS) && (IrqEnabled & RSTC_BODIEN )) {  /* Brownout interrupt pending ?   */
    while(1);                /* Not implemented */
  }
  if ((IrqStatus & RSTC_URSTS) && (IrqEnabled & RSTC_URSTIEN )) {  /* User reset interrupt pending ? */
    while(1);                /* Not implemented */
  }
#endif
}

/*********************************************************************
*
*       _HandleWdtIrq(), watchdog timer interrupt handler
*       If not used for application, this handler may be removed
*/
static void _HandleWdtIrq(void) {
#if DEBUG
  OS_U32 IrqStatus;

  IrqStatus = WDT_SR & (WDT_WDERR | WDT_WDUNF);
  if (IrqStatus && (WDT_MR & WDT_WDFIEN)) {  /* Watchdog error interrupt pending ? */
    while(1);                                /* Not implemented */
  }
#endif
}

/*********************************************************************
*
*       _DefaultFiqHandler(), a dummy FIQ handler
*/
static void _DefaultFiqHandler(void) {
  while(1);
}

/*********************************************************************
*
*       _DefaultIrqHandler, a dummy IRQ handler
*
*       This handler is initially written into all AIC interrupt vectors
*       It is called, if no interrupt vector was installed for
*       specific interrupt source.
*       May be used during debugging to detect uninstalled interrupts
*/
static void _DefaultIrqHandler(void) {
  OS_U32 IrqSource;
  IrqSource = AIC_ISR;  /* detect source of uninstalled interrupt */
  while(IrqSource == AIC_ISR);
}

/*********************************************************************
*
*       _SpuriousIrqHandler(), a dummy spurious IRQ handler
*/
static void _SpuriousIrqHandler(void) {
#if DEBUG
  _SpuriousIrqCnt++;
#endif
}

/*********************************************************************
*
*       _OS_SystemIrqhandler()
*       the OS system interrupt, handles OS timer
*/
static void _OS_SystemIrqhandler(void) {
  volatile int Dummy;

  if (PIT_SR & (1uL << 0)) {  /* Timer interupt pending?            */
    Dummy = PIT_PIVR;         /* Reset interrupt pending condition  */
    OS_TICK_Handle();         /* Call OS tick handler               */
#if (DEBUG || OS_USE_DCC)
    DCC_Process();
#endif
  }
  /* Call to following handlers may be removed if not used by application */
  _HandlePmcIrq();
  _HandleRttIrq();
  _HandleDbguIrq();
  _HandleRstcIrq();
  _HandleWdtIrq();
}

/*********************************************************************
*
*       _InitAIC()
*
* Function description
*   Initialize interupt controller by setting default vectors
*   and clearing all interrupts
*
* NOTES:
*   This function may be called from __low_level_init() and therefore
*   must not use or call any function which relies on any variable,
*   because variables are not initialized before __low_level_init()
*   is called !
*/
static void _InitAIC(void) {
  int  i;
  OS_ISR_HANDLER** papISR;

  AIC_IDCR = 0xFFFFFFFF;                      /* Disable all interrupts     */
  AIC_ICCR = 0xFFFFFFFF;                      /* Clear all interrupts       */
  AIC_FFDR = 0xFFFFFFFF;                      /* Reset fast forcings        */
  AIC_SVR0 = (int) _DefaultFiqHandler;        /* dummy FIQ handler          */
  AIC_SPU  = (int) _SpuriousIrqHandler;       /* dummy spurious handler     */
  papISR = (OS_ISR_HANDLER**) AIC_SVR_BASE_ADDR;
  for (i = 1; i < NUM_INT_SOURCES; i++)  {    /* initially set all sources  */
    *(papISR + i) = &_DefaultIrqHandler;      /* to dummy irq handler       */
  }
  for (i = 0; i < NUM_INT_PRIORITIES; i++) {
    AIC_EOICR = 0;                            /* Reset interrupt controller */
  }
#if DEBUG            // For debugging activate AIC protected mode
  AIC_DCR |= 0x01;   // Enable AIC protected mode
#endif
}

/*********************************************************************
*
*       _InitFlash
*
* Function description
*   Initialize wait states and BUS interface for external Flash access
*   Needs to be called before main clock is switched to high speed
*   Depends on specific hardware and has to be implemented by user.
*
* NOTES: (1) Settings used here are for AT91SAM9261-EK eval board
*            and may have to be modified for other target board or frequency
*
*        (2) This function may be called from __low_level_init() and therefore
*            must not use or call any function which relies on any variable,
*            because variables are not initialized before __low_level_init()
*            is called !
*/
static void _InitFlash(void) {
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
*
*        (2) This function may be called from __low_level_init() and therefore
*            must not use or call any function which relies on any variable,
*            because variables are not initialized before __low_level_init()
*            is called !
*/
static void _InitClocks(OS_U32 PLLA_Val, OS_U32 PLLB_Val, OS_U32 MCKR_Val) {
  //
  // Enable main clock OSC and wait until clock is stable
  //
  PMC_MOR = ((48/8) << 8) + 0x01; // Startup time = 48 slow clocks, Main oscillator enable
  while ((PMC_SR & PMC_MOSCS) == 0) {
  }
  //
  // Initialize PLL A for Main clock
  //
  PMC_CKGR_PLLAR = PLLA_Val;              // Setup PLL A
  //
  // Wait until PLLA locks
  //
  while((PMC_SR & PMC_LOCKA) == 0) {
  }
  //
  // Wait until the master clock is stable
  //
  while((PMC_SR & PMC_MCKRDY) == 0) {
  }
  //
  // Initialize PLL B for USB clock
  //
  PMC_CKGR_PLLBR = PLLB_Val;         // Setup PLL B
  //
  // Wait until PLLB locks
  //
  while((PMC_SR & PMC_LOCKB) == 0)  {
  }
  //
  // Wait until the master clock is stable
  //
  while ((PMC_SR & PMC_MCKRDY) == 0) {
  }
  //
  // Select PLL A clock as clock source
  //
  PMC_MCKR = MCKR_Val;
  //
  // Wait until the master clock is stable */
  //
  while((PMC_SR & PMC_MCKRDY) == 0) {
  }
}

/*********************************************************************
*
*       _InitRTT()
*
* Function description
*   Initialize Real time timer.
*
* NOTES: (1) Not used by embOS, we disable interrupts here to avoid unhandled interrupts
*            May be modified by user if RTT is required for application.
*/
static void _InitRTT(void) {
  //
  // Disable Real-Time Timer interrupts
  //
  RTT_MR &= ~ (RTT_RTTINCIEN  | RTT_ALMIEN);
}

/*********************************************************************
*
*       _InitSDRAM
*
* Function description
*   Initialize SDRAM controller
*
* NOTES: (1) Settings used here are for AT91SAM9261-EK eval board
*            and may have to be modified for other target board or frequency
*
*        (2) This function may be called from __low_level_init() and therefore
*            must not use or call any function which relies on any variable,
*            because variables are not initialized before __low_level_init()
*            is called !
*/
static void _InitSDRAM(void) {
  volatile int i;
  //
  // Setup bus interface for SDRAM if not already initialized
  //
  if ((MATRIX_EBICSA & (1uL << 1)) == 0) {
    //
    // Activate CS1 for SDRAM
    //
    MATRIX_EBICSA |=  (1uL << 1)    // Activate CS1 for SDRAM
                   |  (1uL << 16)   // Memory is powered from 3.3V
                   ;
    //
    // Setup PIO for SDRAM
    //
    PMC_PCER |= (1uL << 4);         // Enable peripheral clock for PIOC
    PIOC_MDDR = 0xFFFF0000;         // Disable Multi driver for data lines on PIOC
    PIOC_ASR  = 0xFFFF0000;         // Upper 16 lines are data lines for SDRAM
    PIOC_PDR  = 0xFFFF0000;         // Disable PIO for upper 16 bits
    //
    // Initialize SDRAM controller
    //
    SDRAMC_MR     = SDRAMC_MODE_NOP_CMD;  // Output one NOP via mode register
    //
    // Setup refresh timer. We assume one refresh cycle every 7.81 micro seconds.
    // If SDRAM runs with other refresh rates (for example 15.61 us), this may be changed
    //
    SDRAMC_TR     = (781 * (OS_FSYS / 2000)) / 100000;
    SDRAMC_TR     = (1562 * (OS_FSYS / 2000)) / 100000;

    SDRAMC_CR     = 0x01uL          // 9 Column bits
                  |(0x02uL << 2)    // 13 Rows
                  |(0x01uL << 4)    // 4 Banks
                  |(0x02uL << 5)    // 2 CAS cycles
                  |(0x00uL << 7)    // 32 bit Data BUS
                  |(0x02uL << 8)    // 2 write recovery cycles
                  |(0x07uL << 12)   // 7 RAS cycles
                  |(0x02uL << 16)   // 2 RAS precharge time cycles
                  |(0x02uL << 20)   // 2 cycles RAS to CAS delay
                  |(0x05uL << 24)   // 5 cycles RAS active time
                  |(0x08uL << 28)   // 8 cycles command recovery time
                  ;

    SDRAMC_LPR    = 0;            // Reset Low Power register
    SDRAMC_MDR    = 0;            // Set Memory device register to SDRAM device
    //
    // Delay at least 200 us
    //
    for (i = 0; i < 10000; i++) {  // Delay
    }
    *(volatile unsigned int*) (SDRAM_BASE_ADDR) = 0x0;       // Output a NOP to SDRAM
    for (i = 0; i < 10000; i++) {  // Delay
    }
    //
    // Perform a "precharge All" sequence
    //
    SDRAMC_MR     = SDRAMC_MODE_PRCGALL_CMD;
    *(volatile unsigned int*) (SDRAM_BASE_ADDR) = 0x1;       // Write to SDRAM
    //
    // Perform 8 auto-refresh (CBR) cycles.
    //
    SDRAMC_MR = SDRAMC_MODE_RFSH_CMD;        // Set mode
    for (i = 0; i < 8; i++) {                // Auto-Refresh, 8 times
      *(volatile unsigned int*) SDRAM_BASE_ADDR = i;  // Dummy write
    }
    //
    // Issue a mode register cycle
    //
    SDRAMC_MR     = SDRAMC_MODE_LMR_CMD;
    *(volatile unsigned int*) (SDRAM_BASE_ADDR) = 0x55555555uL;   // Write to SDRAM, the value does not care
    //
    // Finally set controller to normal mode and perform a dummy write to any location
    //
    SDRAMC_MR     = SDRAMC_MODE_NORMAL_CMD;
    *(volatile unsigned int*) (SDRAM_BASE_ADDR) = 0xababcafeuL;   // Write to SDRAM, the value does not care
  }
}

/*********************************************************************
*
*       _InitSMC
*
* Function description
*   Initialize static memory controller
*
* NOTES: (1) Settings used here are for AT91SAM9261-EK eval board
*            and may have to be modified for other target board or frequency
*
*        (2) This function may be called from __low_level_init() and therefore
*            must not use or call any function which relies on any variable,
*            because variables are not initialized before __low_level_init()
*            is called !
*/
static void _InitSMC(void) {
  #define   NRD_PULSE_2     (3)
  #define   NWR_PULSE_2     (3)
  #define   NRD_SETUP_2     (1)
  #define   NWR_SETUP_2     (1)
  #define   NCS_RD_SETUP_2  (0)
  #define   NCS_WR_SETUP_2  (0)
  #define   NCS_RD_PULSE_2  (NRD_SETUP_2 + NRD_PULSE_2 + 1 - NCS_RD_SETUP_2)
  #define   NCS_WR_PULSE_2  (NWR_SETUP_2 + NWR_PULSE_2 + 1 - NCS_WR_SETUP_2)
  #define   NRD_HOLD_2      (8)
  #define   NWR_HOLD_2      (9)
  SMC_SETUP2 = (NCS_RD_SETUP_2 << 24)
             | (NRD_SETUP_2 << 16)
             | (NCS_WR_SETUP_2 << 8)
             | (NWR_SETUP_2 << 0)
             ;

  SMC_PULSE2 = (NCS_RD_PULSE_2 << 24)
             | (NRD_PULSE_2 << 16)
             | (NCS_WR_PULSE_2 << 8)
             | (NWR_PULSE_2 << 0)
             ;

  SMC_CYCLE2 = ((NRD_SETUP_2 + NRD_PULSE_2 + NRD_HOLD_2) << 16)  // NRD_SETUP + NRD_PULSE + NRD_HOLD
             | ((NWR_SETUP_2 + NWR_PULSE_2 + NWR_HOLD_2) << 0)   // NWR_SETUP + WWR_PULSE + NWR_HOLD
             ;
  SMC_MODE2  = 0x0000
             | (1uL << 12)      // 16bit bus width
             | (1uL << 8)       // Enable BAT mode for write access
             | (0uL << 4)       // Disable NWAIT mode (not connected)
             | (1uL << 1)       // Set WRITE_MODE for write cycles (write controlled)
             | (1uL << 0)       // Set READ_MODE for read cycles (read controlled)
             ;
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*       OS_InitHW()
*
*       Initialize the hardware (timer) required for the OS to run.
*       May be modified, if an other timer should be used
*/
#define OS_TIMER_RELOAD ((OS_PCLK_TIMER/OS_TIMER_PRESCALE/OS_TICK_FREQ) - 1)
#if (OS_TIMER_RELOAD >= 0x00100000)
  #error "PIT timer can not be used, please check configuration"
#endif

void OS_InitHW(void) {
  OS_IncDI();
  OS_ARM_CACHE_Sync();          // Ensure, caches are synchronized
  /* Initialize PIT as OS timer, enable timer + timer interrupt */
  PIT_MR = ((OS_TIMER_RELOAD & 0x000FFFFF) | (1uL << 25) | (1uL << 24));
  OS_ARM_InstallISRHandler(SYSTEM_IRQ_ID, _OS_SystemIrqhandler);
  OS_ARM_EnableISR(SYSTEM_IRQ_ID);
  OS_COM_Init();
  OS_DecRI();
}

/*********************************************************************
*
*       OS_Idle()
*
*       Please note:
*       This is basically the "core" of the idle loop.
*       This core loop can be changed, but:
*       The idle loop does not have a stack of its own, therefore no
*       functionality should be implemented that relies on the stack
*       to be preserved. However, a simple program loop can be programmed
*       (like toggeling an output or incrementing a counter)
*/
void OS_Idle(void) {         /* Idle loop: No task is ready to exec */
  while (1) {
#if DEBUG == 0
    PMC_SCDR = 1;           // Switch off CPU clock to save power
#endif
  }
}

/*********************************************************************
*
*       OS_GetTime_Cycles()
*
*       This routine is required for task-info via OSView or high
*       resolution time maesurement functions.
*       It returns the system time in timer clock cycles.
*/
OS_U32 OS_GetTime_Cycles(void) {
  unsigned int t_cnt;
  OS_U32 time ;

  t_cnt = PIT_PIIR;            /* Read current timer value   */
  time  = OS_GetTime32();      /* Read current OS time       */
  if (t_cnt & 0xFFF00000) {    /* Timer Interrupt pending ?  */
    time  += (t_cnt >> 20);    /* Adjust result              */
    t_cnt &= 0x000FFFFF;
  }
  return (OS_TIMER_RELOAD * time) + t_cnt;
}

/*********************************************************************
*
*       OS_ConvertCycles2us()
*
*       Convert Cycles into micro seconds.
*
*       If your clock frequency is not a multiple of 1 MHz,
*       you may have to modify this routine in order to get proper
*       diagonstics.
*
*       This routine is required for profiling or high resolution time
*       measurement only. It does not affect operation of the OS.
*/
OS_U32 OS_ConvertCycles2us(OS_U32 Cycles) {
  return Cycles/(OS_PCLK_TIMER/OS_TIMER_PRESCALE/1000000);
}

/*********************************************************************
*
*       Communication for OSView via UART (optional)
*
**********************************************************************
*/
#if OS_UART_USED
  #if (OS_UART == 0)
    #define PIO_US_BASE_ADDR     (PIOC_BASE_ADDR)
    #define OS_UART_BASE_ADDR    (USART0_BASE_ADDR)
    #define OS_UART_ID           (US0IRQ_ID)
    #define OS_UART_RX_PIN       (9)
    #define OS_UART_TX_PIN       (8)
  #elif (OS_UART == 1)
    #define PIO_US_BASE_ADDR     (PIOC_BASE_ADDR)
    #define OS_UART_BASE_ADDR    (USART1_BASE_ADDR)
    #define OS_UART_ID           (US1IRQ_ID)
    #define OS_UART_RX_PIN       (13)
    #define OS_UART_TX_PIN       (12)
  #elif (OS_UART == 2)
    #define PIO_US_BASE_ADDR     (PIOC_BASE_ADDR)
    #define OS_UART_BASE_ADDR    (USART2_BASE_ADDR)
    #define OS_UART_ID           (US2IRQ_ID)
    #define OS_UART_RX_PIN       (15)
    #define OS_UART_TX_PIN       (14)
  #endif

  #define PIO_US_PDR  (*(volatile OS_U32*)(PIO_US_BASE_ADDR + 0x04)) // PIO disable register
  #define PIO_US_ASR  (*(volatile OS_U32*)(PIO_US_BASE_ADDR + 0x70)) // PIO peripheral "A" select register
  #define PIO_US_BSR  (*(volatile OS_U32*)(PIO_US_BASE_ADDR + 0x74)) // PIO peripheral "B" select register

  #define PIO_PDR_USART ((1uL << OS_UART_RX_PIN) | (1uL << OS_UART_TX_PIN))

  #define US_CR   (*(volatile OS_U32*)(OS_UART_BASE_ADDR + 0x00))
  #define US_MR   (*(volatile OS_U32*)(OS_UART_BASE_ADDR + 0x04))
  #define US_IER  (*(volatile OS_U32*)(OS_UART_BASE_ADDR + 0x08))
  #define US_IDR  (*(volatile OS_U32*)(OS_UART_BASE_ADDR + 0x0c))
  #define US_IMR  (*(volatile OS_U32*)(OS_UART_BASE_ADDR + 0x10))
  #define US_CSR  (*(volatile OS_U32*)(OS_UART_BASE_ADDR + 0x14))
  #define US_RHR  (*(volatile OS_U32*)(OS_UART_BASE_ADDR + 0x18))
  #define US_THR  (*(volatile OS_U32*)(OS_UART_BASE_ADDR + 0x1c))
  #define US_BRGR (*(volatile OS_U32*)(OS_UART_BASE_ADDR + 0x20))
  #define US_RTOR (*(volatile OS_U32*)(OS_UART_BASE_ADDR + 0x24))
  #define US_TTGR (*(volatile OS_U32*)(OS_UART_BASE_ADDR + 0x28))

  #define US_TPR  (*(volatile OS_U32*)(OS_UART_BASE_ADDR + 0x108))
  #define US_TCR  (*(volatile OS_U32*)(OS_UART_BASE_ADDR + 0x10c))
  #define US_PTCR (*(volatile OS_U32*)(OS_UART_BASE_ADDR + 0x120))

  #define US_RXRDY   (1uL << 0)         /* Rx status flag                  */
  #define US_TXRDY   (1uL << 1)         /* Tx RDY Status flag              */
  #define US_TXEMPTY (1uL << 9)         /* Tx EMPTY Status flag            */
  #define US_ENDTX   (1uL << 4)         /* Tx end flag                     */
  #define USART_RX_ERROR_FLAGS (0xE0)   /* Parity, framing, overrun error  */

  #define OS_UART_PRIO    (0x00)        /* Lowest priority for UART interrupts */
  #define US_TX_INT_FLAG  (US_TXRDY)

/*********************************************************************
*
*       OS_COM_ISR_Usart() OS USART interrupt handler
*       handles both, Rx and Tx interrupt
*/
static void OS_COM_ISR_Usart(void) {
  int UsartStatus;
  volatile int Dummy;

  UsartStatus = US_CSR;                   /* Examine status register */
  do {
    if (UsartStatus & US_RXRDY) {          /* Data received?         */
      if (UsartStatus & USART_RX_ERROR_FLAGS) {  /* Any error ?      */
        Dummy = US_RHR;                   /* => Discard data         */
        US_CR = (1uL << 8);               /* RSTSTA: Reset Status Bits PARE, FRAME, OVRE and RXBRK */
      } else {
        OS_OnRx(US_RHR);                  /* Process actual byte     */
      }
    }
    if (UsartStatus & US_IMR & US_TX_INT_FLAG) { /* Check Tx status => Send next character */
      if (OS_OnTx()) {                    /* No more characters to send ?  */
        US_IDR = US_TX_INT_FLAG;          /* Disable further tx interrupts */
      }
    }
    UsartStatus = US_CSR;                 /* Examine current status  */
  } while (UsartStatus & US_IMR & (US_TX_INT_FLAG | US_RXRDY));
}

/*********************************************************************
*
*       OS_COM_Send1()
*       Never call this function directly from your application
*/
void OS_COM_Send1(OS_U8 c) {
  while((US_CSR & US_TX_INT_FLAG) == 0) { // Wait until THR becomes available
  }
  US_THR = c;
  US_IER  = US_TX_INT_FLAG; /* enable Tx interrupt */
}

/*********************************************************************
*
*       OS_COM_Init()
*       Initialize UART for OSView
*/
void OS_COM_Init(void) {
  OS_IncDI();
  PMC_PCER  = (1uL << OS_UART_ID);   /* Enable peripheral clock for selected USART */
  PIO_US_PDR  = PIO_PDR_USART;       /* Enable peripheral output signals on PIO for USART (disable PIO Port) */
  PIO_US_ASR  = PIO_PDR_USART;       /* Select "A" peripherals on PIO for USART (USART Rx, TX) */
  PIO_US_BSR  = 0;                   /* Deselct "B" peripherals on PIO for USART */
  US_CR   = (1uL <<  2) |            /* RSTRX: Reset Receiver: 1 = The receiver logic is reset. */
            (1uL <<  3)              /* RSTTX: Reset Transmitter: 1 = The transmitter logic is reset. */
           ;
  US_CR   = (0uL <<  2) |            /* RSTRX: Reset Receiver: 1 = The receiver logic is reset. */
            (0uL <<  3) |            /* RSTTX: Reset Transmitter: 1 = The transmitter logic is reset. */
            (1uL <<  4) |            /* RXEN: Receiver Enable: 1 = The receiver is enabled if RXDIS is 0. */
            (0uL <<  5) |            /* RXDIS: Receiver Disable: 0 = Noeffect. */
            (1uL <<  6) |            /* TXEN: Transmitter Enable: 1 = The transmitter is enabled if TXDIS is 0. */
            (0uL <<  7) |            /* TXDIS: Transmitter Disable: 0 = Noeffect. */
            (1uL <<  8) |            /* RSTSTA: Reset Status Bits: 1 = Resets the status bits PARE, FRAME, OVRE and RXBRK in the US_CSR. */
            (0uL <<  9) |            /* STTBRK: Start Break: 0 = Noeffect. */
            (0uL << 10) |            /* STPBRK: Stop Break: 0 = Noeffect. */
            (0uL << 11) |            /* STTTO: Start Time-out: 0 = Noeffect. */
            (0uL << 12)              /* SENDA: Send Address: 0 = Noeffect. */
           ;
  US_MR   =  (0uL <<  4) |           /* USCLKS: Clock Selection: 0 = MCK */
             (3uL <<  6) |           /* CHRL: Character Length: 3 = Eight bits */
             (0uL <<  8) |           /* SYNC: Synchronous Mode Select: 0 = USART operates in Asynchronous Mode. */
           (0x4uL <<  9) |           /* PAR: Parity Type: 0x4 = No parity */
             (0uL << 12) |           /* NBSTOP: Number of Stop Bits: 0 = 1 stop bit */
             (0uL << 14) |           /* CHMODE: Channel Mode: 0 = Normal mode */
             (0uL << 17) |           /* MODE9: 9-bit Character Length: 0 = CHRL defines character length. */
             (0uL << 18)             /* CKLO: Clock Output Select: 0 = The USART does not drive the SCK pin. */
           ;
  US_BRGR = (OS_PCLK_UART / (OS_BAUDRATE * 16));
  US_IDR  = 0xFFFFFFFF;              /* Disable all interrupts     */
  US_IER  = (1uL << 0) |             /* Enable Rx Interrupt        */
             (0 << 1);               /* Do not Enable Tx Interrupt */
  /* Setup interrupt controller for OS USART */
  OS_ARM_InstallISRHandler(OS_UART_ID, &OS_COM_ISR_Usart);  /* OS UART interrupt handler vector */
  OS_ARM_ISRSetPrio(OS_UART_ID, OS_UART_PRIO);              /* Level sensitive, selected priority. */
  AIC_ICCR = (1uL << OS_UART_ID);                           /* Clear OS usart interrupt.        */
  OS_ARM_EnableISR(OS_UART_ID);                             /* Enable OS usart interrupts       */
  OS_DecRI();
}

#elif OS_USE_DCC

/*********************************************************************
*
*       _DCC_OnRx()
*/
static void _DCC_OnRx(unsigned Channel, OS_U8 Data) {
  (void) Channel;   // Not supported by this version, suppress warning
  OS_OnRx(Data);
}

/*********************************************************************
*
*       _DCC_OnTx()
*/
static void _DCC_OnTx(unsigned Channel) {
  (void) Channel;   // Not supported by this version, suppress warning
  OS_OnTx();
}

/*********************************************************************
*
*       OS_COM_Init()
*       Initialize DCC for OSView
*/
void OS_COM_Init(void) {
  JLINKDCC_SetpfOnRx(1, _DCC_OnRx);
  JLINKDCC_SetpfOnTx(1, _DCC_OnTx);
}

/*********************************************************************
*
*       OS_COM_Send1()
*       Send 1 character via DCC
*/
void OS_COM_Send1(OS_U8 c) {
  JLINKDCC_SendCharOnChannelNB(1, c);
}

#else  /* DCC not selected, selected UART not supported, using dummies */

void OS_COM_Init(void) {}
void OS_COM_Send1(OS_U8 c) {
  OS_USEPARA(c);           /* avoid compiler warning */
  OS_COM_ClearTxActive();  /* let the OS know that Tx is not busy */
}

#endif /*  OS_UART_USED  */

/****** Final check of configuration ********************************/

#ifndef OS_UART_USED
  #error "OS_UART_USED has to be defined"
#endif

/*********************************************************************
*
*       OS interrupt handler and ISR specific functions
*
**********************************************************************
*/

/*********************************************************************
*
*       OS_irq_handler
*
*       Detect reason for IRQ and call correspondig service routine.
*       OS_irq_handler is called from OS_IRQ_SERVICE function
*       found in RTOSVect.asm
*/
OS_INTERWORK void OS_irq_handler(void) {
  OS_ISR_HANDLER* pISR;
  pISR = (OS_ISR_HANDLER*) AIC_IVR;    // Read interrupt vector to release NIRQ to CPU core
#if DEBUG
  AIC_IVR = (OS_U32) pISR;             // Write back any value to IVR register to allow interrupt stacking in protected mode
#endif
#if ALLOW_NESTED_INTERRUPTS
  OS_EnterNestableInterrupt();         // Now interrupts may be reenabled. If nesting should be allowed
#else
  OS_EnterInterrupt();                 // Inform OS that interrupt handler is running
#endif
  pISR();                              // Call interrupt service routine
  OS_DI();                             // Disable interrupts and unlock
  AIC_EOICR = 0;                       // interrupt controller =>  Restore previous priority
#if ALLOW_NESTED_INTERRUPTS
  OS_LeaveNestableInterrupt();         // Leave nestable interrupt, perform task switch if required
#else
  OS_LeaveInterrupt();                 // Leave interrupt, perform task switch if required
#endif
}

/*********************************************************************
*
*       OS_ARM_InstallISRHandler
*/
OS_ISR_HANDLER* OS_ARM_InstallISRHandler (int ISRIndex, OS_ISR_HANDLER* pISRHandler) {
  OS_ISR_HANDLER*  pOldHandler;
  OS_ISR_HANDLER** papISR;

#if DEBUG
  if ((unsigned)ISRIndex >= NUM_INT_SOURCES) {
    OS_Error(OS_ERR_ISR_INDEX);
    return NULL;
  }
#endif
  OS_DI();
  papISR = (OS_ISR_HANDLER**)AIC_SVR_BASE_ADDR;
  pOldHandler          = *(papISR + ISRIndex);
  *(papISR + ISRIndex) = pISRHandler;
  OS_RestoreI();
  return pOldHandler;
}

/*********************************************************************
*
*       OS_ARM_EnableISR
*/
void OS_ARM_EnableISR(int ISRIndex) {
#if DEBUG
  if ((unsigned)ISRIndex >= NUM_INT_SOURCES) {
    OS_Error(OS_ERR_ISR_INDEX);
    return;
  }
#endif

  OS_DI();
  AIC_IECR = (1uL << ISRIndex);
  OS_RestoreI();
}

/*********************************************************************
*
*       OS_ARM_DisableISR
*/
void OS_ARM_DisableISR(int ISRIndex) {
#if DEBUG
  if ((unsigned)ISRIndex >= NUM_INT_SOURCES) {
    OS_Error(OS_ERR_ISR_INDEX);
    return;
  }
#endif

  OS_DI();
  AIC_IDCR = (1uL << ISRIndex);
  OS_RestoreI();
}

/*********************************************************************
*
*       OS_ARM_ISRSetPrio
*/
int OS_ARM_ISRSetPrio(int ISRIndex, int Prio) {
  OS_U32* pPrio;
  int     OldPrio;

#if DEBUG
  if ((unsigned)ISRIndex >= NUM_INT_SOURCES) {
    OS_Error(OS_ERR_ISR_INDEX);
    return 0;
  }
#endif
  OS_DI();
  pPrio = (OS_U32*)AIC_SMR_BASE_ADDR;
  OldPrio = pPrio[ISRIndex];
  pPrio[ISRIndex] = (OldPrio & ~INT_PRIORITY_MASK) | (Prio & INT_PRIORITY_MASK);
  OS_RestoreI();
  return OldPrio & INT_PRIORITY_MASK;
}

/*********************************************************************
*
*       __low_level_init()
*
*       Initialize memory controller, clock generation and pll
*
*       Has to be modified, if another CPU clock frequency should be
*       used. This function is called during startup and
*       has to return 1 to perform segment initialization
*/
#ifdef __cplusplus
extern "C" {
#endif
OS_INTERWORK int __low_level_init(void);  // Avoid "no ptototype" warning
#ifdef __cplusplus
  }
#endif
OS_INTERWORK int __low_level_init(void) {
  WDT_MR = (1uL << 15);                    // Initially disable watchdog
  //
  // Setup waitstates for external BUS interface before switching to high speed
  //
  _InitFlash();
  //
  // Initialize main clock oscillator and PLLs
  //
  _InitClocks(PLLAR_VALUE, PLLBR_VALUE, MCKR_VALUE);
  //
  // Initialize real time timer
  //
  _InitRTT();
  //
  // Initialize SDRAM
  //
  _InitSDRAM();
  //
  // Initialize static memory controller
  //
  _InitSMC();
  //
  // Init MMU and caches. This defines the virtual memory map, which is used during execution.
  // Memory mapping should be complete, meaning 4096 entries.
  // Code below fills in ascending VAddr order
  //
  //                                             Mode                      VAddr  PAddr  Size[MB]
#if SDRAM_TARGET
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_C_B,     0x000, 0x200, 0x001);    // SDRAM, the first MB remapped to 0 to map vectors to correct address
#else
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_C_B,     0x000, 0x000, 0x001);    // Internal SRAM
#endif
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_ILLEGAL, 0x001, 0x0,   0x002);
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_NC_B,    0x003, 0x003, 0x001);    // Internal SRAM, original address - used as LCD frame buffer. Should be bufferable, since it is not single-cycle thru AHB
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_ILLEGAL, 0x004, 0x0,   0x001);
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_NC_NB,   0x005, 0x005, 0x001);    // UHP: USB host controller
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_NC_NB,   0x006, 0x006, 0x001);    // LCD controller
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_ILLEGAL, 0x007, 0x0,   0x1F9);
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_C_B,     0x200, 0x200, 0x040);    // SDRAM, 64MB, (Program/data area)
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_ILLEGAL, 0x240, 0x0,   0x0c0);
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_NC_NB,   0x300, 0x300, 0x001);    // External SRAM area CS2, connected to Ethernet controller
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_ILLEGAL, 0x301, 0x0,   0x0FF);
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_NC_NB,   0x400, 0x400, 0x080);    // NAND controller
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_ILLEGAL, 0x480, 0x0,   0xB7F);
  OS_ARM_MMU_AddTTEntries(&_TranslationTable[0], OS_ARM_CACHEMODE_NC_NB,   0xFFF, 0xFFF, 0x001);    // SFR area
  OS_ARM_MMU_Enable      (&_TranslationTable[0]);
  OS_ARM_ICACHE_Enable();
  //OS_ARM_DCACHE_Enable();
  //
  // Initialize interrupt controller
  //
  _InitAIC();
  //
  // Perform other initialization here, if required
  //
  RSTC_MR = ((0xA5uL << 24) | RSTC_URSTEN);  // write KEY and URSTEN to allow USER RESET
  return 1;
}

/*****  End of file  ************************************************/


