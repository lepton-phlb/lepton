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
*       embOS version: 3.28q                                         *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : RTOSInit_AT91SAM7S.c              (for ATMEL AT91SAM7S CPUs)
Purpose : Initializes and handles the hardware for embOS as far
          as required by embOS.
          Feel free to modify this file acc. to your
          target system.
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.H"
#include "OS_Config.h"           /* contains default clock settings */

/*********************************************************************
*
*       Configuration
*
*********************************************************************/

/*********************************************************************
*
*       Clock frequency settings
*/
#ifndef OS_FSYS
  #error "OS_FSYS has to be defined in OS_Config.h"
#endif

#ifndef OS_TICK_FREQ
  #define OS_TICK_FREQ 1000
#endif

#define OS_TIMER_PRESCALE 16 /* prescaler for system timer fixed to 16 */

/*********************************************************************
*
*       UART settings for embOSView
*       If you do not want (or can not due to hardware limitations)
*       to dedicate a UART to embOSView, please define it to be -1
*       Currently the standard code enables UART 0 per default
*/

/*#ifndef OS_UART
  #define OS_UART 0
#endif

#ifndef OS_BAUDRATE
  #define OS_BAUDRATE 38400
#endif
*/

/****** End of configuration settings *******************************/

//#define OS_UART_USED (OS_UART == 0)

/*********************************************************************
*
*       Local defines (sfrs used in RTOSInit.c)
*
**********************************************************************
*/

/*      USART, used for embOSView communication */
#define __USART0_BASE 0xFFFC0000

/*      Debug unit */
#define __DBGU_BASE 0xFFFFF200
#define __DBGU_IMR  *(volatile OS_U32*) (__DBGU_BASE + 0x10) /* Interrupt Mask Register */
#define __DBGU_SR   *(volatile OS_U32*) (__DBGU_BASE + 0x14) /* Channel Status Register */
#define DBGU_COMMRX  (1 << 31)
#define DBGU_COMMTX  (1 << 30)
#define DBGU_RXBUFF  (1 << 12)
#define DBGU_TXBUFE  (1 << 11)
#define DBGU_TXEMPTY (1 <<  9)
#define DBGU_PARE    (1 <<  7)
#define DBGU_FRAME   (1 <<  6)
#define DBGU_OVRE    (1 <<  5)
#define DBGU_ENDTX   (1 <<  4)
#define DBGU_ENDRX   (1 <<  3)
#define DBGU_TXRDY   (1 <<  1)
#define DBGU_RXRDY   (1 <<  0)
#define DBGU_MASK_ALL (DBGU_COMMRX | DBGU_COMMTX  | DBGU_RXBUFF |  \
                       DBGU_TXBUFE | DBGU_TXEMPTY | DBGU_PARE   |  \
                       DBGU_FRAME  | DBGU_OVRE    | DBGU_ENDTX  |  \
                       DBGU_ENDRX  | DBGU_TXRDY   | DBGU_RXRDY)

/*      Reset controller */
#define __RSTC_BASE 0xFFFFFD00
#define __RSTC_CR   *(volatile OS_U32*) (__RSTC_BASE + 0x00)
#define __RSTC_SR   *(volatile OS_U32*) (__RSTC_BASE + 0x04)
#define __RSTC_MR   *(volatile OS_U32*) (__RSTC_BASE + 0x08)
#define RSTC_BODIEN  (1 << 16)  /* Brownout interrupt enable   */
#define RSTC_URSTIEN (1 <<  4)  /* User reset interrupt enable */
#define RSTC_BODSTS  (1 <<  1)  /* Brownout status             */
#define RSTC_URSTS   (1 <<  0)  /* User reset status           */

/*      Real time timer */
#define __RTT_BASE  0xFFFFFD20
#define __RTT_MR    *(volatile OS_U32*) (__RTT_BASE + 0x00)
#define __RTT_SR    *(volatile OS_U32*) (__RTT_BASE + 0x0C)
#define RTT_RTTINCIEN   (1 << 17)
#define RTT_ALMIEN      (1 << 16)
#define RTT_RTTINC      (1 << 1)
#define RTT_ALMS        (1 << 0)

/*      Periodic interval timer */
#define __PIT_BASE  0xFFFFFD30
#define __PIT_MR    *(volatile OS_U32*) (__PIT_BASE + 0x00)
#define __PIT_SR    *(volatile OS_U32*) (__PIT_BASE + 0x04)
#define __PIT_PIVR  *(volatile OS_U32*) (__PIT_BASE + 0x08)
#define __PIT_PIIR  *(volatile OS_U32*) (__PIT_BASE + 0x0C)

/*      Watchdog */
#define __WDT_BASE  0xFFFFFD40
#define __WDT_CR    *(volatile OS_U32*) (__WDT_BASE + 0x00)
#define __WDT_MR    *(volatile OS_U32*) (__WDT_BASE + 0x04)
#define __WDT_SR    *(volatile OS_U32*) (__WDT_BASE + 0x08)
#define WDT_WDFIEN  (1 << 12) /* Watchdog interrupt enable flag in mode register */
#define WDT_WDERR   (1 <<  1) /* Watchdog error status flag                      */
#define WDT_WDUNF   (1 <<  0) /* Watchdog underflow status flag                  */

/*      Memory controller */
#define __MC_BASE   0xFFFFFF00
#define __MC_FMR    *(volatile OS_U32*) (__MC_BASE + 0x60)

#define __MC_FWS_0FWS  ((unsigned int) 0x0 <<  8) /* (MC) 1 cycle for Read,  2 for Write operations */
#define __MC_FWS_1FWS  ((unsigned int) 0x1 <<  8) /* (MC) 2 cycles for Read, 3 for Write operations */
#define __MC_FWS_2FWS  ((unsigned int) 0x2 <<  8) /* (MC) 3 cycles for Read, 4 for Write operations */
#define __MC_FWS_3FWS  ((unsigned int) 0x3 <<  8) /* (MC) 4 cycles for Read, 4 for Write operations */
#if   (OS_FSYS <= 30000000)
  #define __MC_FWS __MC_FWS_0FWS
#elif (OS_FSYS <= 50000000)
  #define __MC_FWS __MC_FWS_1FWS
#else
  #error "Please examine settings for memory controller, Flash wait states"
#endif

/*      PIO control register */
#define __PIOA_BASE 0xfffff400
#define __PIOA_PDR  *(volatile OS_U32*) (__PIOA_BASE + 0x04) /* PIOA disable register               */
#define __PIOA_ASR  *(volatile OS_U32*) (__PIOA_BASE + 0x70) /* PIOA "A" peripheral select register */
#define __PIOA_BSR  *(volatile OS_U32*) (__PIOA_BASE + 0x74) /* PIOA "B" peripheral select register */

/*      Power management controller */
#define __PMC_BASE  0xFFFFFC00
#define __PMC_PCER  *(volatile OS_U32*) (__PMC_BASE + 0x10)  /* Peripheral clock enable register */
#define __PMC_MOR   *(volatile OS_U32*) (__PMC_BASE + 0x20)  /* main oscillator register */
#define __PMC_PLLR  *(volatile OS_U32*) (__PMC_BASE + 0x2c)  /* PLL register */
#define __PMC_MCKR  *(volatile OS_U32*) (__PMC_BASE + 0x30)  /* Master clock register */
#define __PMC_SR    *(volatile OS_U32*) (__PMC_BASE + 0x68)  /* status register */
#define __PMC_IMR   *(volatile OS_U32*) (__PMC_BASE + 0x6C)  /* interrupt mask register */
#define PMC_PCKRDY2 (1 << 10)
#define PMC_PCKRDY1 (1 <<  9)
#define PMC_PCKRDY0 (1 <<  8)
#define PMC_MCKRDY  (1 <<  3)
#define PMC_LOCK    (1 <<  2)
#define PMC_MOSCS   (1 <<  0)
#define PMC_MASK_ALL (PMC_PCKRDY2 | PMC_PCKRDY1 | PMC_PCKRDY0 | \
                      PMC_MCKRDY  | PMC_LOCK    | PMC_MOSCS)

/*      Advanced interrupt controller (AIC) */
#define __AIC_BASE      0xfffff000
#define __AIC_SMR_BASE  (__AIC_BASE + 0x00)
#define __AIC_SVR_BASE  (__AIC_BASE + 0x80)
#define __AIC_SVR0  *(volatile OS_U32*) (__AIC_SVR_BASE + 0x00)
#define __AIC_SVR1  *(volatile OS_U32*) (__AIC_SVR_BASE + 0x04)
#define __AIC_IVR   *(volatile OS_U32*) (__AIC_BASE + 0x100)
#define __AIC_ISR   *(volatile OS_U32*) (__AIC_BASE + 0x108)
#define __AIC_IPR   *(volatile OS_U32*) (__AIC_BASE + 0x10c)
#define __AIC_IDCR  *(volatile OS_U32*) (__AIC_BASE + 0x124)
#define __AIC_ICCR  *(volatile OS_U32*) (__AIC_BASE + 0x128)
#define __AIC_IECR  *(volatile OS_U32*) (__AIC_BASE + 0x120)
#define __AIC_EOICR *(volatile OS_U32*) (__AIC_BASE + 0x130)
#define __AIC_SPU   *(volatile OS_U32*) (__AIC_BASE + 0x134)

/*      AIC interrupt sources and peripheral IDs        */
#define _SYSTEM_IRQ_ID 1   /* System IRQ ID             */
#define _US0IRQ_ID     6   /* USART Channel 0 interrupt */
#define _US1IRQ_ID     7   /* USART Channel 1 interrupt */
#define _TC0IRQ_ID    12   /* Timer Channel 0 interrupt */

#ifndef _NUM_INT_SOURCES
  #define _NUM_INT_SOURCES  32
#endif

#define _INT_PRIORITY_MASK 0x07
#define _NUM_INT_PRIORITIES   8

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static volatile OS_U32 _Dummy;

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
  IrqSource  = __PMC_IMR;
  IrqSource &= (__PMC_SR & PMC_MASK_ALL);
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

  IrqEnabled = __RTT_MR & (RTT_RTTINCIEN  | RTT_ALMIEN);
  IrqStatus  = __RTT_SR & (RTT_RTTINC | RTT_ALMS);
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

  IrqSource  = __DBGU_IMR;
  IrqSource &= (__DBGU_SR & DBGU_MASK_ALL);
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

  IrqEnabled = __RSTC_MR & (RSTC_BODIEN | RSTC_URSTIEN);
  IrqStatus  = __RSTC_SR & (RSTC_BODSTS | RSTC_URSTS);
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

  IrqStatus = __WDT_SR & (WDT_WDERR | WDT_WDUNF);
  if (IrqStatus && (__WDT_MR & WDT_WDFIEN)) { /* Watchdog error interrupt pending ? */
    while(1);                                 /* Not implemented */
  }
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
  IrqSource = __AIC_ISR;  /* detect source of uninstalled interrupt */
  while(IrqSource == __AIC_ISR);
}

/*********************************************************************
*
*       _SpuriousIrqHandler(), a dummy spurious IRQ handler
*/
static OS_U32 _SpuriousIrqCnt;
static void _SpuriousIrqHandler(void) {
  _SpuriousIrqCnt++;
}

/*********************************************************************
*
*       _OS_SystemIrqhandler()
*       the embOS system interrupt, handles embOS timer
*/
static void _OS_SystemIrqhandler(void) {
  if (__PIT_SR & (1 << 0)) {  /* Timer interupt pending?            */
    _Dummy = __PIT_PIVR;      /* Reset interrupt pending condition  */
    OS_HandleTick();          /* Call embOS tick handler            */
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

  __AIC_IDCR = 0xFFFFFFFF;                     /* Disable all interrupts     */
  __AIC_ICCR = 0xFFFFFFFF;                     /* Clear all interrupts       */
  __AIC_SVR0 = (int) _DefaultFiqHandler;       /* dummy FIQ handler          */
  __AIC_SPU  = (int) _SpuriousIrqHandler ;     /* dummy spurious handler     */
  papISR = (OS_ISR_HANDLER**) __AIC_SVR_BASE;
  for (i = 1; i < _NUM_INT_SOURCES; i++)  {    /* initially set all sources  */
    *(papISR + i) = &_DefaultIrqHandler;       /* to dummy irq handler       */
  }
  for (i = 0; i < _NUM_INT_PRIORITIES; i++) {
    __AIC_EOICR = 0;                           /* Reset interrupt controller */
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
*       Initialize the hardware (timer) required for embOS to run.
*       May be modified, if an other timer should be used
*/

#define OS_TIMER_RELOAD ((OS_FSYS/OS_TIMER_PRESCALE/OS_TICK_FREQ) - 1)
#if (OS_TIMER_RELOAD >= 0x00100000)
  #error "PIT timer can not be used, please check configuration"
#endif

void OS_InitHW(void) {
  OS_DI();
  _InitVIC();                   /* Initialize interrupt controller */
  /* Initialize PIT as embOS timer, enable timer + timer interrupt */
  __PIT_MR = ((OS_TIMER_RELOAD & 0x000FFFFF) | (1 << 25) | (1 << 24));
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
void OS_Idle(void) {         /* Idle loop: No task is ready to exec */
  while (1) {
  }
}

/*********************************************************************
*
*       Get time [cycles]
*
*       This routine is required for task-info via embOSView or high
*       resolution time maesurement functions.
*       It returns the system time in timer clock cycles.
*/
OS_U32 OS_GetTime_Cycles(void) {
  unsigned int t_cnt;
  OS_U32 time ;

  t_cnt = __PIT_PIIR;          /* Read current timer value   */
  time  = OS_GetTime32();      /* Read current embOS time    */
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
  Cycles *= 16;   /* eliminate effect of prescaler which is fixed to 16 */
  return Cycles/(OS_FSYS/1000000);
}

/*********************************************************************
*
*       Communication for embOSView via UART (optional)
*
**********************************************************************
*/

#if OS_UART_USED

  #if (OS_UART == 0)
    #define OS_UART_BASE    __USART0_BASE
    #define OS_UART_ID      _US0IRQ_ID
    #define OS_UART_RX_PIN  5
    #define OS_UART_TX_PIN  6
    #define PIO_PDR_USART ((1 << OS_UART_RX_PIN) | (1 << OS_UART_TX_PIN))
  #endif

  #define __USART_CR_OFFS     0x00
  #define __USART_MR_OFFS     0x04
  #define __USART_IER_OFFS    0x08
  #define __USART_IDR_OFFS    0x0C
  #define __USART_IMR_OFFS    0x10
  #define __USART_CSR_OFFS    0x14
  #define __USART_RHR_OFFS    0x18
  #define __USART_THR_OFFS    0x1C
  #define __USART_BRGR_OFFS   0x20
  #define __USART_RTOR_OFFS   0x24
  #define __USART_TIGR_OFFS   0x28

  #define __USART_TPR_OFFS    0x108
  #define __USART_TCR_OFFS    0x10c
  #define __USART_PTCR_OFFS   0x120

  #define US_CR   *(volatile OS_U32*)(OS_UART_BASE + __USART_CR_OFFS)
  #define US_MR   *(volatile OS_U32*)(OS_UART_BASE + __USART_MR_OFFS)
  #define US_IER  *(volatile OS_U32*)(OS_UART_BASE + __USART_IER_OFFS)
  #define US_IDR  *(volatile OS_U32*)(OS_UART_BASE + __USART_IDR_OFFS)
  #define US_IMR  *(volatile OS_U32*)(OS_UART_BASE + __USART_IMR_OFFS)
  #define US_CSR  *(volatile OS_U32*)(OS_UART_BASE + __USART_CSR_OFFS)
  #define US_RHR  *(volatile OS_U32*)(OS_UART_BASE + __USART_RHR_OFFS)
  #define US_THR  *(volatile OS_U32*)(OS_UART_BASE + __USART_THR_OFFS)
  #define US_BRGR *(volatile OS_U32*)(OS_UART_BASE + __USART_BRGR_OFFS)
  #define US_RTOR *(volatile OS_U32*)(OS_UART_BASE + __USART_RTOR_OFFS)
  #define US_TTGR *(volatile OS_U32*)(OS_UART_BASE + __USART_TIGR_OFFS)

  #define US_TPR  *(volatile OS_U32*)(OS_UART_BASE + __USART_TPR_OFFS)
  #define US_TCR  *(volatile OS_U32*)(OS_UART_BASE + __USART_TCR_OFFS)
  #define US_PTCR *(volatile OS_U32*)(OS_UART_BASE + __USART_PTCR_OFFS)

  #define US_RXRDY   (1 << 0)         /* Rx status flag                  */
  #define US_TXRDY   (1 << 1)         /* Tx RDY Status flag              */
  #define US_TXEMPTY (1 << 9)         /* Tx EMPTY Status flag            */
  #define US_ENDTX   (1 << 4)         /* Tx end flag                     */
  #define USART_RX_ERROR_FLAGS 0xE0   /* Parity, framing, overrun error  */

  #define OS_UART_PRIO    0x00        /* Lowest priority for UART interrupts */
  #define US_TX_INT_FLAG  US_ENDTX

/*********************************************************************
*
*       OS_COM_ISR_Usart() embOS USART interrupt handler
*       handles both, Rx and Tx interrupt
*/
static void OS_COM_ISR_Usart(void) {
  int UsartStatus;
  UsartStatus = US_CSR;                    /* Examine status register */
  do {
    if (UsartStatus & US_RXRDY) {          /* Data received?          */
      if (UsartStatus & USART_RX_ERROR_FLAGS) {  /* Any error ?       */
        _Dummy = US_RHR;                   /* => Discard data         */
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
*       Initialize UART for embOSView
*/
void OS_COM_Init(void) {            /* Initialize UART, enable UART interrupts */
  __PMC_PCER  = (1 << OS_UART_ID);  /* Enable peripheral clock for selected USART */
  __PIOA_PDR  = PIO_PDR_USART;      /* Enable peripheral output signals (disable PIO Port A) */
  __PIOA_ASR  = PIO_PDR_USART;      /* Select "A" peripherals on PIO A (USART Rx, TX) */
  __PIOA_BSR  = 0;                  /* Deselct "B" peripherals on PIO A */
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
  US_BRGR = (OS_FSYS / (OS_BAUDRATE * 16));
  US_IDR  = 0xFFFFFFFF;             /* Disable all interrupts     */
  US_IER  = (1 << 0) |              /* Enable Rx Interrupt        */
            (0 << 1);               /* Do not Enable Tx Interrupt */
  /* Setup interrupt controller for embOS USART */
  OS_ARM_InstallISRHandler(OS_UART_ID, &OS_COM_ISR_Usart);  /* embOS UART interrupt handler vector */
  OS_ARM_ISRSetPrio(OS_UART_ID, OS_UART_PRIO);              /* Level sensitive, selected priority. */
  __AIC_ICCR = (1 << OS_UART_ID);                           /* Clear embOS usart interrupt.        */
  OS_ARM_EnableISR(OS_UART_ID);                             /* Enable embOS usart interrupts       */
}

#else  /* selected UART not supported, using dummies */

void OS_COM_Init(void) {}
void OS_COM_Send1(OS_U8 c) {
  OS_USEPARA(c);           /* avoid compiler warning */
  OS_COM_ClearTxActive();  /* let embOS know that Tx is not busy */
}

#endif /*  OS_UART_USED  */

/****** Final check of configuration ********************************/
/*
#ifndef OS_UART_USED
  #error "OS_UART_USED has to be defined"
#endif
*/
/*********************************************************************
*
*       embOS interrupt handler and ISR specific functions
*
**********************************************************************
*/

/*********************************************************************
*
*       OS_irq_handler
*
*       Detect reason for IRQ and call correspondig service routine.
*       OS_irq_handler is called from embOS OS_IRQ_SERVICE function
*       found in RTOSVect.asm
*/
void OS_irq_handler(void) {
  OS_ISR_HANDLER* pISR;
  pISR = (OS_ISR_HANDLER*) __AIC_IVR;  /* Read interrupt vector to release NIRQ to CPU core          */
  OS_EnterNestableInterrupt();         /* Now interrupts may be reenabled. If nesting should be      */
                                       /* avoided generally, use OS_EnterInterrupt() here            */
  pISR();                              /* Call interrupt service routine                             */
  OS_DI();                             /* Disable interrupts and unlock                              */
  __AIC_EOICR = 0;                     /* interrupt controller =>  Restore previous priority         */
  OS_LeaveNestableInterrupt();         /* Replace by OS_LeaveInterrupt(), when nesting was disabled  */
}                                      /* by usage of OS_EnterInterrupt();                           */

/*********************************************************************
*
*       OS_ARM_InstallISRHandler
*
*/
OS_ISR_HANDLER* OS_ARM_InstallISRHandler (int ISRIndex, OS_ISR_HANDLER* pISRHandler) {
  OS_ISR_HANDLER*  pOldHandler;
  OS_ISR_HANDLER** papISR;

  pOldHandler = NULL;
  OS_DI();
  papISR = (OS_ISR_HANDLER**)__AIC_SVR_BASE;
  if (ISRIndex < _NUM_INT_SOURCES) {
    pOldHandler          = *(papISR + ISRIndex);
    *(papISR + ISRIndex) = pISRHandler;
  }
  OS_RestoreI();
  return pOldHandler;
}

/*********************************************************************
*
*       OS_ARM_EnableISR
*
*/
void OS_ARM_EnableISR(int ISRIndex) {
  OS_DI();
  if (ISRIndex < _NUM_INT_SOURCES) {
    __AIC_IECR = (1 << ISRIndex);
  }
  OS_RestoreI();
}

/*********************************************************************
*
*       OS_ARM_DisableISR
*
*/
void OS_ARM_DisableISR(int ISRIndex) {
  OS_DI();
  if (ISRIndex < _NUM_INT_SOURCES) {
    __AIC_IECR &= ~(1 << ISRIndex);
  }
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

  OS_DI();
  pPrio = (OS_U32*)__AIC_SMR_BASE;
  OldPrio = pPrio[ISRIndex];
  pPrio[ISRIndex] = (OldPrio & ~_INT_PRIORITY_MASK) | (Prio & _INT_PRIORITY_MASK);
  OS_RestoreI();
  return OldPrio & _INT_PRIORITY_MASK;
}

/*****  EOF  ********************************************************/

