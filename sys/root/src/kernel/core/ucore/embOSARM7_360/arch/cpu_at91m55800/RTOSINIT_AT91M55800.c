/*********************************************************************
*               SEGGER MICROCONTROLLER SYSTEME GmbH                  *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (C) 2005   SEGGER Microcontroller Systeme GmbH               *
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
*       embOS version: 3.28n                                         *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : RTOSInit_AT91M55800.c     (for ATMEL AT91M55800 CPUs)
Purpose : Initializes and handles the hardware for embOS as far
          as required by embOS.
          Feel free to modify this file acc. to your
          target system.
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.H"

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
  #define OS_FSYS 32000000
#endif

#ifndef OS_TICK_FREQ
  #define OS_TICK_FREQ 1000
#endif

#define TIMER_PRESCALE  2             // prescaler for timer frequency  

/*********************************************************************
*
*       UART settings for embOSView
*       If you do not want (or can not due to hardware limitations)
*       to dedicate a UART to embOSView, please define it to be -1
*       Currently the standard code enables UART 0 per default
*       and supports UART0 and UART1
*/

#ifndef OS_UART
  #define OS_UART -1
#endif    

#ifndef OS_BAUDRATE
  #define OS_BAUDRATE 38400
#endif

/****** End of configuration settings *******************************/

#define OS_UART_USED ((OS_UART == 0) || (OS_UART == 1) || (OS_UART == 2))

/*********************************************************************
*
*       Local defines (sfrs used in RTOSInit.c)
*
**********************************************************************
*/

#define __USART0_BASE   0xfffc0000
#define __USART1_BASE   0xfffc4000
#define __USART2_BASE   0xfffc8000

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

/*      PIO control register */
#define __PIO_PDR   *(volatile OS_U32*)0xfffec004
 
/*      Timer 0  */
#define __TC_CCRC0  *(volatile OS_U32*)0xfffd0000
#define __TC_CMRC0  *(volatile OS_U32*)0xfffd0004
#define __TC_CVC0   *(volatile OS_U32*)0xfffd0010
#define __TC_RCC0   *(volatile OS_U32*)0xfffd001c
#define __TC_SRC0   *(volatile OS_U32*)0xfffd0020
#define __TC_IERC0  *(volatile OS_U32*)0xfffd0024
#define __TC_IDRC0  *(volatile OS_U32*)0xfffd0028
#define __TC_BMR    *(volatile OS_U32*)0xfffd00c4

/*      Peripheral clock enable register */
#define __PCER      *(volatile OS_U32*)0xffff4010

/*      Advanced interrupt controller (AIC) */
#define __AIC_SMR0  *(volatile OS_U32*)0xfffff000
#define __AIC_SMR1  *(volatile OS_U32*)0xfffff004
#define __AIC_SMR2  *(volatile OS_U32*)0xfffff008
#define __AIC_SMR3  *(volatile OS_U32*)0xfffff00c
#define __AIC_SMR4  *(volatile OS_U32*)0xfffff010
#define __AIC_SMR6  *(volatile OS_U32*)0xfffff018
#define __AIC_SVR0  *(volatile OS_U32*)0xfffff080
#define __AIC_SVR1  *(volatile OS_U32*)0xfffff084
#define __AIC_SVR2  *(volatile OS_U32*)0xfffff088
#define __AIC_SVR3  *(volatile OS_U32*)0xfffff08c
#define __AIC_SVR4  *(volatile OS_U32*)0xfffff090
#define __AIC_SVR6  *(volatile OS_U32*)0xfffff098
#define __AIC_IVR   *(volatile OS_U32*)0xfffff100
#define __AIC_IPR   *(volatile OS_U32*)0xfffff10c
#define __AIC_IDCR  *(volatile OS_U32*)0xfffff124
#define __AIC_ICCR  *(volatile OS_U32*)0xfffff128
#define __AIC_IECR  *(volatile OS_U32*)0xfffff120 
#define __AIC_EOICR *(volatile OS_U32*)0xfffff130 

/* PLL */
#define __APMC_CGMR  *(volatile unsigned int*)0xffff4020
#define __APMC_SR    *(volatile unsigned int*)0xffff4030

/*      Advanced interrupt controller interrupt sources */
#define US0IRQ   2  /* USART Channel 0 interrupt */
#define US1IRQ   3  /* USART Channel 1 interrupt */
#define US2IRQ   4  /* USART Channel 2 interrupt */
#define TC0IRQ   6  /* Timer Channel 0 interrupt */

#ifndef NUM_INT_SOURCES
  #define NUM_INT_SOURCES  32
#endif

#define _INT_PRIORITY_MASK 0x07

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
*       OS_Tick interrupt Handler
*/

static void OS_ISR_Tick(void) {
  _Dummy = __TC_SRC0;   // Reset Timer Interrupt to reset/reenable further interrupts
  OS_HandleTick();      // Call embOS tick handler
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*       OS_InitPLL
*
* Function description
*   Initialize main clock and PLL
*   Called from startup code; needs to be done as early as possible
*   in order to keep boot time short
*/
void OS_InitPLL(void) {
  int i;
  __AIC_IDCR = 0xFFFFFFFF;      /* Disable all interrupts */
  __AIC_ICCR = 0xFFFFFFFF;      /* Clear all interrupts */
  /* setup PLL */
  #define APMC_MOSCS      1     /* Main oscillator status bit */
  #define APMC_PLL_LOCK   2     /* PLL lock status */
  /* Speed up the System Frequency. */
  __APMC_CGMR = 0x002F0002; // MOSCEN = 1, OSCOUNT = 47  (1.4ms/30us)
  /* Read the APMC Status register to detect when the oscillator is stabilized */
  while ((__APMC_SR & APMC_MOSCS) != APMC_MOSCS) ;
  /* Commuting from Slow Clock to Main Oscillator (16Mhz) */
  __APMC_CGMR = 0x002F4002; /* MOSCEN = 1, OSCOUNT = 47  (1.4ms/30us) */
  /* Setup the PLL to 32 MHz clock */
  __APMC_CGMR = 0x032F4102; /* MUL = 1, PLLCOUNT = 3, CSS = 1 */
  /* Read the APMC Status register to detect when the PLL is stabilized */
  /* Wait a little extra time to ensure stable PLL. */
  for (i = 0; i < 1000; i++) {
    while ((__APMC_SR & APMC_PLL_LOCK) != APMC_PLL_LOCK) ;
  }
  /* Commuting from 16Mhz to PLL @ 32MHz */
  __APMC_CGMR = 0x032F8102; /* CSS = 2, MUL = 1 */
  /* Now the Master clock is the output of PLL @ 32MHz */
}

/*********************************************************************
*
*       OS_InitHW()
*
*       Initialize the hardware (timer) required for embOS to run.
*       May be modified, if an other timer should be used
*/

#define OS_TIMER_RELOAD OS_FSYS/TIMER_PRESCALE/OS_TICK_FREQ
#define OS_TIMER_ID     TC0IRQ      // Assign to TC0IRQ channel number
#define OS_TIMER_PRIO   0x00        // lowest priority

void OS_InitHW(void) {
  OS_DI();
  // Initialize embOS timer (timer 0, capture mode channel C)
  __PCER     = (1 << OS_TIMER_ID);  // Enable peripheral clock for timer 0
  __TC_IDRC0 = 0xff;                // Disable all timer/counter 0 interrupts.
  __TC_BMR   = 0x02;                // Runs on internal clock
  __TC_CMRC0 = 0x00004000;          // Capture mode, CPCTRG=1, TCCLKS=0 (=> MCLK/2)
  __TC_RCC0  = OS_TIMER_RELOAD;     // Set compare register
  __TC_CCRC0 = 1;                   // Enable the clock.
  __TC_CCRC0 = 5;                   // Software trigger.
  _Dummy     = __TC_SRC0;           // Read timer/counter 0 status register to clear flags.
  __TC_IERC0 = (1<<4);              // Interrupt on RC compare.
  // Setup interrupt controller for embOS timer interrupt using timer 0
  __AIC_ICCR = 0x40;                // Clears timer/counter 0 interrupt.
  OS_ARM_InstallISRHandler(OS_TIMER_ID, &OS_ISR_Tick); // Timer/counter 0 interrupt vector.
  OS_ARM_ISRSetPrio(OS_TIMER_ID, 0x00);                // Timer/counter 0 interrupt level sensitive at lowest priority (0).
  __AIC_ICCR = (1 << OS_TIMER_ID);                     // Clears timer/counter 0 interrupt.
  OS_ARM_EnableISR(OS_TIMER_ID);                       // Enable timer/counter 0 interrupt.
  // optionally initialize UART for embOSView
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

void OS_Idle(void) {     // Idle loop: No task is ready to exec
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
  unsigned int t_cnt = __TC_CVC0;       // Read current timer value
  OS_U32 time = OS_GetTime32();         // Read current embOS time 
  if (__AIC_IPR & (1 << OS_TIMER_ID)) { // Timer Interrupt pending ?
    t_cnt = __TC_CVC0;                  // Adjust result     
    time++;
  }
  return OS_TIMER_RELOAD*time + t_cnt;
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
  Cycles *= TIMER_PRESCALE;           // eliminate effect of prescaler
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
    #define OS_UART_BASE __USART0_BASE
    #define AIC_SMR __AIC_SMR2
    #define AIC_SVR __AIC_SVR2
    #define OS_UART_ID US0IRQ   
    #define PIO_PDR_USART ((1<<15) | (1<<16))
  #elif (OS_UART == 1)
    #define OS_UART_BASE __USART1_BASE
    #define AIC_SMR __AIC_SMR3
    #define AIC_SVR __AIC_SVR3
    #define OS_UART_ID US1IRQ
    #define PIO_PDR_USART ((1<<18) | (1<<19))
  #elif (OS_UART == 2)
    #define OS_UART_BASE __USART2_BASE
    #define AIC_SMR __AIC_SMR4
    #define AIC_SVR __AIC_SVR4
    #define OS_UART_ID US2IRQ
    #define PIO_PDR_USART ((1<<21) | (1<<22))
  #endif
  
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

  #define US_RXRDY 0x01              // Rx status flag
  #define US_TXRDY 0x02              // Tx Status flag
  #define USART_RX_ERROR_FLAGS 0xE0  // Parity, framing, overrun error

  #define OS_UART_PRIO 0x00          // Lowest priority for UART interrupts
  
/*********************************************************************
*
*       OS_COM_ISR_Usart() embOS USART interrupt handler
*       handles both, Rx and Tx interrupt
*/
static void OS_COM_ISR_Usart(void) {
  int UsartStatus;
  do {
    UsartStatus = US_CSR;                  // Examine status register 
    if (UsartStatus & US_RXRDY) {          // Data received?
      if (UsartStatus & USART_RX_ERROR_FLAGS) {  // Any error ?
        _Dummy = US_RHR;                   // => Discard data
        US_CR = (1<<8);                    // RSTSTA: Reset Status Bits PARE, FRAME, OVRE and RXBRK
      } else {                              
        OS_OnRx(US_RHR);                   // Process actual byte
      }  
    }
    if (UsartStatus & US_IMR & US_TXRDY) { // Check Tx status => Send next character
      if (OS_OnTx()) {                     // No more characters to send ?
        US_IDR = US_TXRDY;                 // Disable further tx interrupts
      }
    }
  } while (UsartStatus & US_IMR & (US_TXRDY | US_RXRDY));    
}

/*********************************************************************
*
*       OS_COM_Send1()
*       Never call this function directly from your application
*/

void OS_COM_Send1(OS_U8 c) {
  US_THR = c;         // Send character
  US_IER = US_TXRDY;  // enable Tx Empty interrupt
}

/*********************************************************************
*
*       OS_COM_Init()
*       Initialize UART for embOSView
*/

void OS_COM_Init(void) {         // Initialize UART, enable UART interrupts
  __PCER  = (1 << OS_UART_ID);   // Enable peripheral clock for selected USART   
  __PIO_PDR   = PIO_PDR_USART;   // Enable peripheral output signals (disable PIO Port A)
  US_CR   = (1 <<  2) |          // RSTRX: Reset Receiver: 1 = The receiver logic is reset.
            (1 <<  3);           // RSTTX: Reset Transmitter: 1 = The transmitter logic is reset.
  US_CR   = (0 <<  2) |          // RSTRX: Reset Receiver: 1 = The receiver logic is reset.
            (0 <<  3) |          // RSTTX: Reset Transmitter: 1 = The transmitter logic is reset.
            (1 <<  4) |          // RXEN: Receiver Enable: 1 = The receiver is enabled if RXDIS is 0.
            (0 <<  5) |          // RXDIS: Receiver Disable: 0 = Noeffect.
            (1 <<  6) |          // TXEN: Transmitter Enable: 1 = The transmitter is enabled if TXDIS is 0.
            (0 <<  7) |          // TXDIS: Transmitter Disable: 0 = Noeffect.
            (1 <<  8) |          // RSTSTA: Reset Status Bits: 1 = Resets the status bits PARE, FRAME, OVRE and RXBRK in the US_CSR.
            (0 <<  9) |          // STTBRK: Start Break: 0 = Noeffect.
            (0 << 10) |          // STPBRK: Stop Break: 0 = Noeffect.
            (0 << 11) |          // STTTO: Start Time-out: 0 = Noeffect.
            (0 << 12);           // SENDA: Send Address: 0 = Noeffect.
  US_MR   = (0 <<  4) |          // USCLKS: Clock Selection: 0 = MCK
            (3 <<  6) |          // CHRL: Character Length: 3 = Eight bits
            (0 <<  8) |          // SYNC: Synchronous Mode Select: 0 = USART operates in Asynchronous Mode.
          (0x4 <<  9) |          // PAR: Parity Type: 0x4 = No parity
            (0 << 12) |          // NBSTOP: Number of Stop Bits: 0 = 1 stop bit
            (0 << 14) |          // CHMODE: Channel Mode: 0 = Normal mode
            (0 << 17) |          // MODE9: 9-bit Character Length: 0 = CHRL defines character length.
            (0 << 18);           // CKLO: Clock Output Select: 0 = The USART does not drive the SCK pin.
  US_BRGR = (OS_FSYS / (OS_BAUDRATE * 16));
  US_IER  = (1<<0) |             // Enable Rx Interrupt
            (1<<1);              // (Enable Tx Interrupt
  // Setup interrupt controller for embOS USART
  OS_ARM_InstallISRHandler(OS_UART_ID, &OS_COM_ISR_Usart);  // embOS UART interrupt handler vector
  OS_ARM_ISRSetPrio(OS_UART_ID, OS_UART_PRIO);              // Level sensitive, selected priority.
  __AIC_ICCR = (1 << OS_UART_ID);                           // Clear embOS usart interrupt.
  OS_ARM_EnableISR(OS_UART_ID);                             // Enable embOS usart interrupts
}

#else  /* selected UART not supported, using dummies */

void OS_COM_Init(void) {}
void OS_COM_Send1(OS_U8 c) {
  OS_USEPARA(c);           // avoid compiler warning
  OS_COM_ClearTxActive();  // let embOS know that Tx is not busy
}

#endif /*  OS_UART_USED  */

/****** Final check of configuration ********************************/

#ifndef OS_UART_USED
  #error "OS_UART_USED has to be defined"
#endif

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
  pISR = (OS_ISR_HANDLER*) __AIC_IVR;  // Read interrupt vector to release NIRQ to CPU core
  //OS_EnterNestableInterrupt();       // Now interrupts may be reenabled. If nesting should be
  OS_EnterInterrupt();                  // avoided generally, use OS_EnterInterrupt() here
  
  pISR();                              // Call interrupt service routine
  OS_DI();                             // Disable interrupts and unlock
  __AIC_EOICR = 0;                     // interrupt controller =>  Restore previous priority
  
  OS_LeaveInterrupt();
  //OS_LeaveNestableInterrupt();         // Replace by OS_LeaveInterrupt(), when nesting was disabled 
}                                      // by usage of OS_EnterInterrupt(); 

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
  papISR = (OS_ISR_HANDLER**)&__AIC_SVR0;
  if (ISRIndex < NUM_INT_SOURCES) {
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
  if (ISRIndex < NUM_INT_SOURCES) {
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
  if (ISRIndex < NUM_INT_SOURCES) {
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
  pPrio = (OS_U32*)&__AIC_SMR0;
  OldPrio = pPrio[ISRIndex];
  pPrio[ISRIndex] = (OldPrio & ~_INT_PRIORITY_MASK) | (Prio & _INT_PRIORITY_MASK);
  OS_RestoreI();
  return OldPrio & _INT_PRIORITY_MASK;
}

/*****  EOF  ********************************************************/

