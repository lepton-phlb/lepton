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
File    : RTOSInit_AT91SAM7X256.c
          for ATMEL AT91SAM7X CPUs

Purpose : Initializes and handles the hardware for the OS as far
          as required by the OS.
          Feel free to modify this file acc. to your
          target system.

--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.H"
#include "JLINKDCC.h"
#include "OS_Config.h"           /* contains default clock settings */


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

//#define OS_FSYS (47923200L)

#ifndef   OS_TICK_FREQ
  #define OS_TICK_FREQ (1000)
#endif

#ifndef   OS_PCLK_TIMER
  #define OS_PCLK_TIMER  (OS_FSYS)
#endif

#define OS_TIMER_PRESCALE (16) /* prescaler for system timer fixed to 16 */

/*********************************************************************
*
*       UART settings for OSView
*       If you do not want (or can not due to hardware limitations)
*       to dedicate a UART to OSView, please define it to be -1
*       Currently the standard code enables UART 0 per default
*/
#ifndef   OS_UART
  #define OS_UART (0)
#endif

#ifndef   OS_BAUDRATE
  #define OS_BAUDRATE (38400)
#endif

#ifndef   OS_PCLK_UART
  #define OS_PCLK_UART  (OS_FSYS)
#endif

/****** End of configuration settings *******************************/

#define OS_UART_USED ((OS_UART == 0) || (OS_UART == 1))

/*********************************************************************
*
*       Local defines (sfrs used in RTOSInit.c)
*
**********************************************************************
*/

/*      USART, used for OSView communication */
#define _USART0_BASE_ADDR  (0xFFFC0000)
#define _USART1_BASE_ADDR  (0xFFFC4000)

/*      Debug unit */
#define _DBGU_BASE_ADDR    (0xFFFFF200)
#define _DBGU_IMR     (*(volatile OS_U32*) (_DBGU_BASE_ADDR + 0x10)) /* Interrupt Mask Register */
#define _DBGU_SR      (*(volatile OS_U32*) (_DBGU_BASE_ADDR + 0x14)) /* Channel Status Register */
#define DBGU_COMMRX   ((OS_U32)1 << 31)
#define DBGU_COMMTX   (1 << 30)
#define DBGU_RXBUFF   (1 << 12)
#define DBGU_TXBUFE   (1 << 11)
#define DBGU_TXEMPTY  (1 <<  9)
#define DBGU_PARE     (1 <<  7)
#define DBGU_FRAME    (1 <<  6)
#define DBGU_OVRE     (1 <<  5)
#define DBGU_ENDTX    (1 <<  4)
#define DBGU_ENDRX    (1 <<  3)
#define DBGU_TXRDY    (1 <<  1)
#define DBGU_RXRDY    (1 <<  0)
#define DBGU_MASK_ALL (DBGU_COMMRX | DBGU_COMMTX  | DBGU_RXBUFF |  \
                       DBGU_TXBUFE | DBGU_TXEMPTY | DBGU_PARE   |  \
                       DBGU_FRAME  | DBGU_OVRE    | DBGU_ENDTX  |  \
                       DBGU_ENDRX  | DBGU_TXRDY   | DBGU_RXRDY)

/*      Reset controller */
#define _RSTC_BASE_ADDR    (0xFFFFFD00)
#define _RSTC_CR      (*(volatile OS_U32*) (_RSTC_BASE_ADDR + 0x00))
#define _RSTC_SR      (*(volatile OS_U32*) (_RSTC_BASE_ADDR + 0x04))
#define _RSTC_MR      (*(volatile OS_U32*) (_RSTC_BASE_ADDR + 0x08))
#define RSTC_BODIEN   (1 << 16)  /* Brownout interrupt enable   */
#define RSTC_URSTIEN  (1 <<  4)  /* User reset interrupt enable */
#define RSTC_BODSTS   (1 <<  1)  /* Brownout status             */
#define RSTC_URSTS    (1 <<  0)  /* User reset status           */

/*      Real time timer */
#define _RTT_BASE_ADDR     (0xFFFFFD20)
#define _RTT_MR       (*(volatile OS_U32*) (_RTT_BASE_ADDR + 0x00))
#define _RTT_SR       (*(volatile OS_U32*) (_RTT_BASE_ADDR + 0x0C))
#define RTT_RTTINCIEN (1 << 17)
#define RTT_ALMIEN    (1 << 16)
#define RTT_RTTINC    (1 << 1)
#define RTT_ALMS      (1 << 0)

/*      Periodic interval timer */
#define _PIT_BASE_ADDR     (0xFFFFFD30)
#define _PIT_MR       (*(volatile OS_U32*) (_PIT_BASE_ADDR + 0x00))
#define _PIT_SR       (*(volatile OS_U32*) (_PIT_BASE_ADDR + 0x04))
#define _PIT_PIVR     (*(volatile OS_U32*) (_PIT_BASE_ADDR + 0x08))
#define _PIT_PIIR     (*(volatile OS_U32*) (_PIT_BASE_ADDR + 0x0C))

/*      Watchdog */
#define _WDT_BASE_ADDR     (0xFFFFFD40)
#define _WDT_CR       (*(volatile OS_U32*) (_WDT_BASE_ADDR + 0x00))
#define _WDT_MR       (*(volatile OS_U32*) (_WDT_BASE_ADDR + 0x04))
#define _WDT_SR       (*(volatile OS_U32*) (_WDT_BASE_ADDR + 0x08))
#define WDT_WDFIEN    (1 << 12) /* Watchdog interrupt enable flag in mode register */
#define WDT_WDERR     (1 <<  1) /* Watchdog error status flag                      */
#define WDT_WDUNF     (1 <<  0) /* Watchdog underflow status flag                  */

/*      Memory controller */
#define _MC_BASE_ADDR      (0xFFFFFF00)
#define _MC_FMR       (*(volatile OS_U32*) (_MC_BASE_ADDR + 0x60))

#define _MC_FWS_0FWS  ((unsigned int) 0x0 <<  8) /* (MC) 1 cycle for Read,  2 for Write operations */
#define _MC_FWS_1FWS  ((unsigned int) 0x1 <<  8) /* (MC) 2 cycles for Read, 3 for Write operations */
#define _MC_FWS_2FWS  ((unsigned int) 0x2 <<  8) /* (MC) 3 cycles for Read, 4 for Write operations */
#define _MC_FWS_3FWS  ((unsigned int) 0x3 <<  8) /* (MC) 4 cycles for Read, 4 for Write operations */
#if   (OS_FSYS <= 30000000)
  #define _MC_FWS (_MC_FWS_0FWS)
#elif (OS_FSYS <= 50000000)
  #define _MC_FWS (_MC_FWS_1FWS)
#else
  #error "Please examine settings for memory controller, Flash wait states"
#endif

/*      PIO control register */
#define _PIOA_BASE_ADDR    (0xfffff400)
#define _PIOA_PDR     (*(volatile OS_U32*) (_PIOA_BASE_ADDR + 0x04)) /* PIOA disable register               */
#define _PIOA_ASR     (*(volatile OS_U32*) (_PIOA_BASE_ADDR + 0x70)) /* PIOA "A" peripheral select register */
#define _PIOA_BSR     (*(volatile OS_U32*) (_PIOA_BASE_ADDR + 0x74)) /* PIOA "B" peripheral select register */

/*      Power management controller */
#define _PMC_BASE_ADDR     (0xFFFFFC00)
#define _PMC_SCDR     (*(volatile OS_U32*) (_PMC_BASE_ADDR + 0x04))  /* System Clock Disable Register */
#define _PMC_PCER     (*(volatile OS_U32*) (_PMC_BASE_ADDR + 0x10))  /* Peripheral clock enable register */
#define _PMC_MOR      (*(volatile OS_U32*) (_PMC_BASE_ADDR + 0x20))  /* main oscillator register */
#define _PMC_PLLR     (*(volatile OS_U32*) (_PMC_BASE_ADDR + 0x2c))  /* PLL register */
#define _PMC_MCKR     (*(volatile OS_U32*) (_PMC_BASE_ADDR + 0x30))  /* Master clock register */
#define _PMC_SR       (*(volatile OS_U32*) (_PMC_BASE_ADDR + 0x68))  /* status register */
#define _PMC_IMR      (*(volatile OS_U32*) (_PMC_BASE_ADDR + 0x6C))  /* interrupt mask register */
#define PMC_PCKRDY2   (1 << 10)
#define PMC_PCKRDY1   (1 <<  9)
#define PMC_PCKRDY0   (1 <<  8)
#define PMC_MCKRDY    (1 <<  3)
#define PMC_LOCK      (1 <<  2)
#define PMC_MOSCS     (1 <<  0)
#define PMC_MASK_ALL  (PMC_PCKRDY2 | PMC_PCKRDY1 | PMC_PCKRDY0 | \
                       PMC_MCKRDY  | PMC_LOCK    | PMC_MOSCS)

/*      Advanced interrupt controller (AIC) */
#define _AIC_BASE_ADDR      (0xfffff000)
#define _AIC_SMR_BASE_ADDR  (_AIC_BASE_ADDR + 0x00)
#define _AIC_SVR_BASE_ADDR  (_AIC_BASE_ADDR + 0x80)
#define _AIC_SVR0      (*(volatile OS_U32*) (_AIC_SVR_BASE_ADDR + 0x00))
#define _AIC_SVR1      (*(volatile OS_U32*) (_AIC_SVR_BASE_ADDR + 0x04))
#define _AIC_IVR       (*(volatile OS_U32*) (_AIC_BASE_ADDR + 0x100))
#define _AIC_ISR       (*(volatile OS_U32*) (_AIC_BASE_ADDR + 0x108))
#define _AIC_IPR       (*(volatile OS_U32*) (_AIC_BASE_ADDR + 0x10c))
#define _AIC_IDCR      (*(volatile OS_U32*) (_AIC_BASE_ADDR + 0x124))
#define _AIC_ICCR      (*(volatile OS_U32*) (_AIC_BASE_ADDR + 0x128))
#define _AIC_IECR      (*(volatile OS_U32*) (_AIC_BASE_ADDR + 0x120))
#define _AIC_EOICR     (*(volatile OS_U32*) (_AIC_BASE_ADDR + 0x130))
#define _AIC_SPU       (*(volatile OS_U32*) (_AIC_BASE_ADDR + 0x134))

/*      AIC interrupt sources and peripheral IDs        */
#define _SYSTEM_IRQ_ID  (1)   /* System IRQ ID             */
#define _US0IRQ_ID      (6)   /* USART Channel 0 interrupt */
#define _US1IRQ_ID      (7)   /* USART Channel 1 interrupt */
#define _US2IRQ_ID      (8)   /* USART Channel 2 interrupt */
#define _TC0IRQ_ID     (12)   /* Timer Channel 0 interrupt */

#ifndef   _NUM_INT_SOURCES
  #define _NUM_INT_SOURCES   (32)
#endif

#define _INT_PRIORITY_MASK (0x07)
#define _NUM_INT_PRIORITIES   (8)

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

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
  OS_U32 IrqSource;
  IrqSource  = _PMC_IMR;
  IrqSource &= (_PMC_SR & PMC_MASK_ALL);
  if (IrqSource) {  /* PMC interrupt pending? */
    while(1);       /* Not implemented        */
  }
}

/*********************************************************************
*
*       _HandleRttIrq(), Real time timer interrupt handler
*       If not used for application, this handler may be removed
*/
static void _HandleRttIrq(void) {
  OS_U32 IrqStatus;
  OS_U32 IrqEnabled;

  IrqEnabled = _RTT_MR & (RTT_RTTINCIEN  | RTT_ALMIEN);
  IrqStatus  = _RTT_SR & (RTT_RTTINC | RTT_ALMS);
  if ((IrqStatus & RTT_RTTINC) && (IrqEnabled & RTT_RTTINCIEN )) { /* RTT inc. interrupt pending ? */
    while(1);                /* Not implemented */
  }
  if ((IrqStatus & RTT_ALMS) && (IrqEnabled & RTT_ALMIEN )) {      /* Alarm interrupt pending ? */
    while(1);                /* Not implemented */
  }
}

/*********************************************************************
*
*       _HandleDbguIrq(), Debug unit interrupt handler
*       If not used for application, this handler may be removed
*/
static void _HandleDbguIrq(void) {
  OS_U32 IrqSource;

  IrqSource  = _DBGU_IMR;
  IrqSource &= (_DBGU_SR & DBGU_MASK_ALL);
  if (IrqSource) { /* Any interrupt pending ? */
    while(1);      /* Not implemented         */
  }
}

/*********************************************************************
*
*       _HandleRstcIrq(), Reset controller interrupt handler
*       If not used for application, this handler may be removed
*/
static void _HandleRstcIrq(void) {
  OS_U32 IrqStatus;
  OS_U32 IrqEnabled;

  IrqEnabled = _RSTC_MR & (RSTC_BODIEN | RSTC_URSTIEN);
  IrqStatus  = _RSTC_SR & (RSTC_BODSTS | RSTC_URSTS);
  if ((IrqStatus & RSTC_BODSTS) && (IrqEnabled & RSTC_BODIEN )) {  /* Brownout interrupt pending ?   */
    while(1);                /* Not implemented */
  }
  if ((IrqStatus & RSTC_URSTS) && (IrqEnabled & RSTC_URSTIEN )) {  /* User reset interrupt pending ? */
    while(1);                /* Not implemented */
  }
}

/*********************************************************************
*
*       _HandleWdtIrq(), watchdog timer interrupt handler
*       If not used for application, this handler may be removed
*/
static void _HandleWdtIrq(void) {
  OS_U32 IrqStatus;

  IrqStatus = _WDT_SR & (WDT_WDERR | WDT_WDUNF);
  if (IrqStatus && (_WDT_MR & WDT_WDFIEN)) { /* Watchdog error interrupt pending ? */
    while(1);                                /* Not implemented */
  }
}

/*********************************************************************
*
*       _DefaultFiqHandler(), a dummy FIQ handler
*
*       This handler is initially written into the AIC FIQ vector.
*       It is called, if the FIQ vector is not initialized,
*       but any interrupting source was assigned to FIQ.
*       May be used during debugging to detect an uninstalled FIQ interrupt
*/
static void _DefaultFiqHandler(void) {
#if DEBUG
  while(1);
#endif
}

/*********************************************************************
*
*       _DefaultIrqHandler, a dummy IRQ handler
*
*       This handler is initially written into all AIC interrupt vectors.
*       It is called, if no interrupt vector was installed for
*       specific interrupt source.
*       May be used during debugging to detect uninstalled interrupts
*/
static void _DefaultIrqHandler(void) {
#if DEBUG
  volatile OS_U32 IrqSource;
  IrqSource = _AIC_ISR;        // Detect source of uninstalled interrupt
  OS_Error(OS_ERR_ISR_VECTOR);
#endif
}

/*********************************************************************
*
*       _SpuriousIrqHandler(), default spurious IRQ handler
*
*       A spurious interrupt is called under one of the following
*       circumstances:
*       1. An external interrupt source is programmed in level-sensitive mode
*          and an active level occurs for only a short time.
*       2. An internal interrupt source is programmed in level sensitive
*          and the output signal of the corresponding embedded peripheral
*          is activated for a short time. (As in the case for the Watchdog.)
*       3. An interrupt occurs just a few cycles before the software begins
*          to mask it, thus resulting in a pulse on the interrupt source.
*       The spurious handler is called to detect these errors.
*/
static void _SpuriousIrqHandler(void) {
#if DEBUG
  static OS_U32 SpuriousIRQCnt;
  SpuriousIRQCnt++;
#endif
}

/*********************************************************************
*
*       _OS_SystemIrqhandler()
*       the OS system interrupt, handles OS timer
*/
static void _OS_SystemIrqhandler(void) {
  volatile OS_U32 Dummy;     /* Must not be optimized away         */
  if (_PIT_SR & (1 << 0)) {  /* Timer interupt pending?            */
    Dummy = _PIT_PIVR;       /* Reset interrupt pending condition  */
    OS_HandleTick();         /* Call OS tick handler               */
#if DEBUG
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
*       _InitVIC()
*
*       Initialize interupt controller by setting default vectors
*       and clearing all interrupts
*/
static void _InitVIC(void) {
  int  i;
  OS_ISR_HANDLER** papISR;

  _AIC_IDCR = 0xFFFFFFFF;                     /* Disable all interrupts     */
  _AIC_ICCR = 0xFFFFFFFF;                     /* Clear all interrupts       */
  _AIC_SVR0 = (int) _DefaultFiqHandler;       /* dummy FIQ handler          */
  _AIC_SPU  = (int) _SpuriousIrqHandler ;     /* dummy spurious handler     */
  papISR = (OS_ISR_HANDLER**) _AIC_SVR_BASE_ADDR;
  for (i = 1; i < _NUM_INT_SOURCES; i++)  {   /* initially set all sources  */
    *(papISR + i) = &_DefaultIrqHandler;      /* to dummy irq handler       */
  }
  for (i = 0; i < _NUM_INT_PRIORITIES; i++) {
    _AIC_EOICR = 0;                           /* Reset interrupt controller */
  }
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
  OS_DI();
  /* Initialize PIT as OS timer, enable timer + timer interrupt */
  _PIT_MR = ((OS_TIMER_RELOAD & 0x000FFFFF) | (1 << 25) | (1 << 24));
  OS_ARM_InstallISRHandler(_SYSTEM_IRQ_ID, _OS_SystemIrqhandler);
  OS_ARM_EnableISR(_SYSTEM_IRQ_ID);
  OS_COM_Init();
  OS_RestoreI();
}

/*********************************************************************
*
*       Idle loop  (OS_Idle)
*
*       Please note:
*       This is basically the "core" of the idle loop.
*       This core loop can be changed, but:
*       The idle loop does not have a stack of its own, therefore no
*       functionality should be implemented that relies on the stack
*       to be preserved. However, a simple program loop can be programmed
*       (like toggeling an output or incrementing a counter)
*/
void OS_Idle(void) {  /* Idle loop: No task is ready to exec */
  while (1) {
#if DEBUG == 0
    _PMC_SCDR = 1;    // Switch off CPU clock to save power
#endif
  }
}

/*********************************************************************
*
*       Get time [cycles]
*
*       This routine is required for task-info via OSView or high
*       resolution time maesurement functions.
*       It returns the system time in timer clock cycles.
*/
OS_U32 OS_GetTime_Cycles(void) {
  unsigned int t_cnt;
  OS_U32 time ;

  t_cnt = _PIT_PIIR;           /* Read current timer value   */
  time  = OS_GetTime32();      /* Read current OS time    */
  if (t_cnt & 0xFFF00000) {    /* Timer Interrupt pending ?  */
    time  += (t_cnt >> 20);    /* Adjust result              */
    t_cnt &= 0x000FFFFF;
  }
  return (OS_TIMER_RELOAD * time) + t_cnt;
}

/*********************************************************************
*
*       OS_ConvertCycles2us
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
    #define OS_UART_BASE_ADDR  (_USART0_BASE_ADDR)
    #define OS_UART_ID         (_US0IRQ_ID)
    #define OS_UART_RX_PIN     (0)
    #define OS_UART_TX_PIN     (1)
  #elif (OS_UART == 1)
    #define OS_UART_BASE_ADDR  (_USART1_BASE_ADDR)
    #define OS_UART_ID         (_US1IRQ_ID)
    #define OS_UART_RX_PIN     (5)
    #define OS_UART_TX_PIN     (6)
  #endif

  #define PIO_PDR_USART ((1 << OS_UART_RX_PIN) | (1 << OS_UART_TX_PIN))

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

  #define US_RXRDY   (1 << 0)         /* Rx status flag                  */
  #define US_TXRDY   (1 << 1)         /* Tx RDY Status flag              */
  #define US_TXEMPTY (1 << 9)         /* Tx EMPTY Status flag            */
  #define US_ENDTX   (1 << 4)         /* Tx end flag                     */
  #define USART_RX_ERROR_FLAGS (0xE0) /* Parity, framing, overrun error  */

  #define OS_UART_PRIO    (0x00)      /* Lowest priority for UART interrupts */
  #define US_TX_INT_FLAG  (US_ENDTX)

/*********************************************************************
*
*       OS_COM_ISR_Usart() OS USART interrupt handler
*       handles both, Rx and Tx interrupt
*/
static void OS_COM_ISR_Usart(void) {
  volatile OS_U32 Dummy;                   /* Must not be optimized away */
  int UsartStatus;

  UsartStatus = US_CSR;                    /* Examine status register */
  do {
    if (UsartStatus & US_RXRDY) {          /* Data received?          */
      if (UsartStatus & USART_RX_ERROR_FLAGS) {  /* Any error ?       */
        Dummy = US_RHR;                    /* => Discard data         */
        US_CR = (1 << 8);                  /* RSTSTA: Reset Status Bits PARE, FRAME, OVRE and RXBRK */
      } else {
        OS_OnRx(US_RHR);                   /* Process actual byte     */
      }
    }
    if (UsartStatus & US_IMR & US_TX_INT_FLAG) { /* Check Tx status => Send next character */
      if (OS_OnTx()) {                     /* No more characters to send ?  */
        US_IDR = US_TX_INT_FLAG;           /* Disable further tx interrupts */
      }
    }
    UsartStatus = US_CSR;                  /* Examine current status  */
  } while (UsartStatus & US_IMR & (US_TX_INT_FLAG | US_RXRDY));
}

/*********************************************************************
*
*       OS_COM_Send1()
*       Never call this function directly from your application
*/
static OS_U8 buffer1;

void OS_COM_Send1(OS_U8 c) {
  buffer1 = c;
  US_TPR  = (OS_U32) &buffer1;
  US_TCR  = 1;
  US_PTCR = (1 << 8);      /* Enable transmission */
  US_IER = US_TX_INT_FLAG; /* enable Tx interrupt */
}

/*********************************************************************
*
*       OS_COM_Init()
*       Initialize UART for OSView
*/
void OS_COM_Init(void) {            /* Initialize UART, enable UART interrupts */
  _PMC_PCER  = (1 << OS_UART_ID);   /* Enable peripheral clock for selected USART */
  _PIOA_PDR  = PIO_PDR_USART;       /* Enable peripheral output signals (disable PIO Port A) */
  _PIOA_ASR  = PIO_PDR_USART;       /* Select "A" peripherals on PIO A (USART Rx, TX) */
  _PIOA_BSR  = 0;                   /* Deselct "B" peripherals on PIO A */
  US_CR   = (1 <<  2) |             /* RSTRX: Reset Receiver: 1 = The receiver logic is reset. */
            (1 <<  3);              /* RSTTX: Reset Transmitter: 1 = The transmitter logic is reset. */
  US_CR   = (0 <<  2) |             /* RSTRX: Reset Receiver: 1 = The receiver logic is reset. */
            (0 <<  3) |             /* RSTTX: Reset Transmitter: 1 = The transmitter logic is reset. */
            (1 <<  4) |             /* RXEN: Receiver Enable: 1 = The receiver is enabled if RXDIS is 0. */
            (0 <<  5) |             /* RXDIS: Receiver Disable: 0 = Noeffect. */
            (1 <<  6) |             /* TXEN: Transmitter Enable: 1 = The transmitter is enabled if TXDIS is 0. */
            (0 <<  7) |             /* TXDIS: Transmitter Disable: 0 = Noeffect. */
            (1 <<  8) |             /* RSTSTA: Reset Status Bits: 1 = Resets the status bits PARE, FRAME, OVRE and RXBRK in the US_CSR. */
            (0 <<  9) |             /* STTBRK: Start Break: 0 = Noeffect. */
            (0 << 10) |             /* STPBRK: Stop Break: 0 = Noeffect. */
            (0 << 11) |             /* STTTO: Start Time-out: 0 = Noeffect. */
            (0 << 12);              /* SENDA: Send Address: 0 = Noeffect. */
  US_MR   = (0 <<  4) |             /* USCLKS: Clock Selection: 0 = MCK */
            (3 <<  6) |             /* CHRL: Character Length: 3 = Eight bits */
            (0 <<  8) |             /* SYNC: Synchronous Mode Select: 0 = USART operates in Asynchronous Mode. */
          (0x4 <<  9) |             /* PAR: Parity Type: 0x4 = No parity */
            (0 << 12) |             /* NBSTOP: Number of Stop Bits: 0 = 1 stop bit */
            (0 << 14) |             /* CHMODE: Channel Mode: 0 = Normal mode */
            (0 << 17) |             /* MODE9: 9-bit Character Length: 0 = CHRL defines character length. */
            (0 << 18);              /* CKLO: Clock Output Select: 0 = The USART does not drive the SCK pin. */
  US_BRGR = (OS_PCLK_UART / (OS_BAUDRATE * 16));
  US_IDR  = 0xFFFFFFFF;             /* Disable all interrupts     */
  US_IER  = (1 << 0) |              /* Enable Rx Interrupt        */
            (0 << 1);               /* Do not Enable Tx Interrupt */
  /* Setup interrupt controller for OS USART */
  OS_ARM_InstallISRHandler(OS_UART_ID, &OS_COM_ISR_Usart);  /* OS UART interrupt handler vector */
  OS_ARM_ISRSetPrio(OS_UART_ID, OS_UART_PRIO);              /* Level sensitive, selected priority. */
  _AIC_ICCR = (1 << OS_UART_ID);                            /* Clear OS usart interrupt.        */
  OS_ARM_EnableISR(OS_UART_ID);                             /* Enable OS usart interrupts       */
}

#else  /* selected UART not supported, using dummies */

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
  pISR = (OS_ISR_HANDLER*) _AIC_IVR;   /* Read interrupt vector to release NIRQ to CPU core          */
#if ALLOW_NESTED_INTERRUPTS
  OS_EnterNestableInterrupt();         // Now interrupts may be reenabled. If nesting should be allowed
#else
  OS_EnterInterrupt();                 // Inform OS that interrupt handler is running
#endif
  pISR();                              /* Call interrupt service routine                             */
  OS_DI();                             /* Disable interrupts and unlock                              */
  _AIC_EOICR = 0;                      /* interrupt controller =>  Restore previous priority         */
#if ALLOW_NESTED_INTERRUPTS
  OS_LeaveNestableInterrupt();         // Leave nestable interrupt, perform task switch if required
#else
  OS_LeaveInterrupt();                 // Leave interrupt, perform task switch if required
#endif
}

/*********************************************************************
*
*       OS_ARM_InstallISRHandler
*
*/
OS_ISR_HANDLER* OS_ARM_InstallISRHandler (int ISRIndex, OS_ISR_HANDLER* pISRHandler) {
  OS_ISR_HANDLER*  pOldHandler;
  OS_ISR_HANDLER** papISR;

#if DEBUG
  if ((unsigned)ISRIndex >= _NUM_INT_SOURCES) {
    OS_Error(OS_ERR_ISR_INDEX);
    return NULL;
  }
#endif
  OS_DI();
  papISR = (OS_ISR_HANDLER**)_AIC_SVR_BASE_ADDR;
  pOldHandler          = *(papISR + ISRIndex);
  *(papISR + ISRIndex) = pISRHandler;
  OS_RestoreI();
  return pOldHandler;
}

/*********************************************************************
*
*       OS_ARM_EnableISR
*
*/
void OS_ARM_EnableISR(int ISRIndex) {
#if DEBUG
  if ((unsigned)ISRIndex >= _NUM_INT_SOURCES) {
    OS_Error(OS_ERR_ISR_INDEX);
    return;
  }
#endif
  OS_DI();
  _AIC_IECR = (1 << ISRIndex);
  OS_RestoreI();
}

/*********************************************************************
*
*       OS_ARM_DisableISR
*
*/
void OS_ARM_DisableISR(int ISRIndex) {
#if DEBUG
  if ((unsigned)ISRIndex >= _NUM_INT_SOURCES) {
    OS_Error(OS_ERR_ISR_INDEX);
    return;
  }
#endif
  OS_DI();
  _AIC_IECR &= ~(1 << ISRIndex);
  OS_RestoreI();
}

/*********************************************************************
*
*       OS_ARM_ISRSetPrio
*
*/
int OS_ARM_ISRSetPrio(int ISRIndex, int Prio) {
  OS_U32* pPrio;
  int     OldPrio;

#if DEBUG
  if ((unsigned)ISRIndex >= _NUM_INT_SOURCES) {
    OS_Error(OS_ERR_ISR_INDEX);
    return 0;
  }
#endif
  OS_DI();
  pPrio = (OS_U32*)_AIC_SMR_BASE_ADDR;
  OldPrio = pPrio[ISRIndex];
  pPrio[ISRIndex] = (OldPrio & ~_INT_PRIORITY_MASK) | (Prio & _INT_PRIORITY_MASK);
  OS_RestoreI();
  return OldPrio & _INT_PRIORITY_MASK;
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
OS_INTERWORK int __low_level_init(void);  /* Avoid "no ptototype" warning */
OS_INTERWORK int __low_level_init(void) {
  _WDT_MR = (1 << 15);                                           /* Initially disable watchdog */
  _MC_FMR = (0xFF << 16) & ((OS_FSYS/1000000) << 16) | _MC_FWS; /* Setup wait states          */
  /*
  * Init PLL to run at 96 MHz
  */
  _PMC_MOR  = (1 << 0)                   /* Enable main oscillator         */
             | (6 << 8);                  /* Set startup delay time         */
  while ((_PMC_SR & (1 << 0)) == 0);     /* Wait until end of startup time */
  _PMC_PLLR = 0x05                       /* Setup Pll multiplier           */
             | (28 << 8)
             | (25 << 16)
             | ( 1 << 28);                /* USBDIV is 2                    */
  while((_PMC_SR & (1 << 2)) == 0);      /* Wait until pll locked          */
  while((_PMC_SR & (1 << 3)) == 0);      /* Wait until main clock is ready */
  /* Selection of Master Clock and Processor Clock
  * select the PLL clock divided by 2
  * Switch main clock derived from pll output divided by 2
  */
  _PMC_MCKR = 3                          /* (PMC) Clock from PLL is selected  */
             | (1 <<  2);                 /* (PMC) Selected clock divided by 2 */
  while((_PMC_SR & (1 << 3)) == 0);      /* Wait until main clock is ready    */

  _RSTC_MR = (((OS_U32)0xA5 << 24) | (1 << 0));  /* write KEY and URSTEN to allow USER RESET  */
  _InitVIC();                   /* Initialize interrupt controller */
  return 1;
}

#if 1 // This is an optional speed optimization.

/*********************************************************************
*
*       OS_EnableInt()
*/
OS_INTERWORK __arm __ramfunc void OS_EnableInt(void) {
  __asm("mrs     r12,CPSR");
  __asm("bic     r12,r12,#128");
  __asm("msr     CPSR_c,r12");
}

/*********************************************************************
*
*       OS_DisableInt()
*/
OS_INTERWORK __arm __ramfunc void OS_DisableInt(void) {
  __asm("mrs     r12,CPSR");
  __asm("orr     r12,r12,#128");
  __asm("msr     CPSR_c,r12");
}

/*********************************************************************
*
*       OS_EnableIntSVC()
*/
OS_INTERWORK __arm __ramfunc void OS_EnableIntSVC(void) {
  __asm("msr     CPSR_c,#0x13");
}

/*********************************************************************
*
*       OS_DisableIntSVC()
*/
OS_INTERWORK __arm __ramfunc void OS_DisableIntSVC(void) {
  __asm("msr     CPSR_c,#0x93");
}
#endif

/*****  EOF  ********************************************************/

