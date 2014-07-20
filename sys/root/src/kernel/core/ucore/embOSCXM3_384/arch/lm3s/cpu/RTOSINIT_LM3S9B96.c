/*********************************************************************
*               SEGGER MICROCONTROLLER GmbH & Co KG                  *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (c) 1995 - 2011  SEGGER Microcontroller GmbH & Co KG         *
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
*       OS version: 3.84                                             *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : RTOSInit_LM3S9B96.c
Purpose : Initializes and handles the hardware for the OS as far
          as required by the OS.
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.H"
#include "exceptions.h"           /* CMSIS compatible irq interface */

/*********************************************************************
*
*       Configuration
*
*********************************************************************/

/*********************************************************************
*
*       Clock frequency settings
*/
#ifndef   OS_FSYS                   /* CPU main clock frequency      */
  #define OS_FSYS 80000000uL
#endif

#ifndef   OS_PCLK_TIMER             /* Peripheral clock for timer   */
  #define OS_PCLK_TIMER OS_FSYS     /* May vary from CPU clock      */
#endif                              /* depending on CPU             */

#ifndef   OS_PCLK_UART              /* Peripheral clock for UART    */
  #define OS_PCLK_UART OS_FSYS      /* May vary from CPU clock      */
#endif                              /* depending on CPU             */

#ifndef   OS_TICK_FREQ
  #define OS_TICK_FREQ (1000)
#endif

#ifndef   OS_USE_VARINTTABLE        /* The interrupt vector table   */
  #define OS_USE_VARINTTABLE (0)    /* may be located in RAM        */
#endif

#define OS_Systick SysTick_Handler  /* Map OS systick handler to CMSIS compatible handler name */

/*********************************************************************
*
*       Configuration of communication to OSView
*/
#ifndef   OS_VIEW_ENABLE            // Global enable of communication
  #define OS_VIEW_ENABLE (1)        // Default: on
#endif

/****** End of configuration settings *******************************/

#define OS_UART_USED (0)  // OS_UART not supported for this device

#define OS_USE_JLINKMEM OS_VIEW_ENABLE

#if OS_USE_JLINKMEM
  #include "JLINKMEM.h"
#endif

/*********************************************************************
*
*       Local defines (sfrs used in RTOSInit.c)
*
**********************************************************************
*/

#define SYS_TICK_BASE_ADDR       (0xE000E010)
#define SYS_TICK_CONTROL         (*(volatile OS_U32*)(SYS_TICK_BASE_ADDR + 0x00))
#define SYS_TICK_RELOAD          (*(volatile OS_U32*)(SYS_TICK_BASE_ADDR + 0x04))
#define SYS_TICK_VALUE           (*(volatile OS_U32*)(SYS_TICK_BASE_ADDR + 0x08))
#define SYS_TICK_CALIBRATION     (*(volatile OS_U32*)(SYS_TICK_BASE_ADDR + 0x0C))

#define SYS_TICK_ENABLE_BIT      (0)
#define SYS_TICK_INT_ENABLE_BIT  (1)
#define SYS_TICK_CLK_SOURCE_BIT  (2)

#define SYS_HANDLER_STATE        (*(volatile OS_U32*)(0xE000ED24))
#define SYS_TICK_ACT_BIT         (11)

#define SYSPRI1_ADDR             (0xE000ED18)
#define SYSHND_CTRL_ADDR         (0xE000ED24)    // System Handler Control and State
#define SYSHND_CTRL              (*(volatile OS_U32*) (SYSHND_CTRL_ADDR))

#define NVIC_SYS_HND_CTRL_MEM    (0x00010000uL)  // Mem manage fault enable
#define NVIC_SYS_HND_CTRL_BUS    (0x00020000uL)  // Bus fault enable
#define NVIC_SYS_HND_CTRL_USAGE  (0x00040000uL)  // Usage fault enable

#define NVIC_BASE_ADDR           (0xE000E000)

#define NVIC_PRIOBASE_ADDR       (0xE000E400)
#define NVIC_ENABLE_ADDR         (0xE000E100)
#define NVIC_DISABLE_ADDR        (0xE000E180)
#define NVIC_VTOREG_ADDR         (0xE000ED08)
#define NVIC_VTOR                (*(volatile unsigned long *) (0xE000ED08))

#define NUM_INTERRUPTS           (16+55)


/*********************************************************************
*
* The following can be used as as arguments for the PLL activation
* if required in __low_level_init()
*
**********************************************************************
*/
#define HWREG(x)                (*((volatile OS_U32*)(x)))

#define SYSCTL_RCC              0x400FE060uL  // Run-mode clock config register
#define SYSCTL_RCC2             0x400FE070uL  // Run-mode clock config register 2
#define SYSCTL_MISC             0x400FE058uL  // Interrupt status register
#define SYSCTL_RCC2_USEFRACT    0x40000000uL  // Use fractional divider
#define SYSCTL_RIS              0x400FE050uL  // Raw interrupt status register

#define SYSCTL_SYSDIV_2_5       0xC1000000uL  // Processor clock is pll / 2.5
#define SYSCTL_USE_PLL          0x00000000uL  // System clock is the PLL clock
#define SYSCTL_XTAL_16MHZ       0x00000540uL  // External crystal is 16 MHz
#define SYSCTL_OSC_MAIN         0x00000000uL  // Oscillator source is main osc
#define SYSCTL_RCC2_SYSDIV2_M   0x1F800000uL  // System clock divider
#define SYSCTL_RCC_BYPASS       0x00000800uL  // PLL bypass
#define SYSCTL_RCC_USESYSDIV    0x00400000uL  // Enable System Clock Divider
#define SYSCTL_RCC2_BYPASS2     0x00000800uL  // PLL bypass
#define SYSCTL_RCC_IOSCDIS      0x00000002uL  // Internal oscillator disable
#define SYSCTL_RCC_MOSCDIS      0x00000001uL  // Main oscillator disable
#define SYSCTL_RCC_XTAL_M       0x000007C0uL  // Crystal attached to main osc
#define SYSCTL_RCC_OSCSRC_M     0x00000030uL  // Oscillator input select
#define SYSCTL_RCC_PWRDN        0x00002000uL  // PLL power down
#define SYSCTL_RCC_OEN          0x00001000uL  // PLL Output Ena
#define SYSCTL_RCC2_USERCC2     0x80000000uL  // Use RCC2
#define SYSCTL_RCC2_OSCSRC2_M   0x00000070uL  // System Clock Source
#define SYSCTL_RCC2_PWRDN2      0x00002000uL  // PLL power down
#define SYSCTL_INT_PLL_LOCK     0x00000040uL  // PLL lock interrupt
#define SYSCTL_RCC_SYSDIV_M     0x07800000uL  // System Clock Divisor
#define SYSCTL_RCC2_FRACT       0x00400000uL  // Fractional divide

/*****  Interrupt ID numbers ****************************************/
#define ISR_ID_NMI              (2)           // NMI fault
#define ISR_ID_HARD             (3)           // Hard fault
#define ISR_ID_MPU              (4)           // MPU fault
#define ISR_ID_BUS              (5)           // Bus fault
#define ISR_ID_USAGE            (6)           // Usage fault
#define ISR_ID_SVCALL           (11)          // SVCall
#define ISR_ID_DEBUG            (12)          // Debug monitor
#define ISR_ID_PENDSV           (14)          // PendSV
#define ISR_ID_SYSTICK          (15)          // System Tick

#define OS_ISR_ID_TICK          ISR_ID_SYSTICK    // We use Sys-Timer

/****** MAP UART initialization function ****************************/

#if (OS_UART_USED || OS_USE_JLINKMEM)
  #define OS_COM_INIT() OS_COM_Init()
#else
  #define OS_COM_INIT()
#endif

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

#if OS_USE_JLINKMEM
  // Size of the communication buffer for JLINKMEM
const OS_U32 OS_JLINKMEM_BufferSize = 32;
#else
const OS_U32 OS_JLINKMEM_BufferSize = 0;     // Communication not used
#endif

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/


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
* Function description
*   Called from startup code.
*   Sets the clocking to run directly from the crystal, but
*   can be used to initialize PLL as early as possible.
*
*/
#ifdef __cplusplus
extern "C" {
#endif
OS_INTERWORK int __low_level_init(void);    // Avoid "no prototype" warning
#ifdef __cplusplus
  }
#endif

OS_INTERWORK int __low_level_init(void) {
  volatile unsigned long ulDelay;
  unsigned long ulRCC, ulRCC2;
  unsigned long ulConfig = SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ;
  //
  // Get the current value of the RCC and RCC2 registers.  If using a
  // Sandstorm-class device, the RCC2 register will read back as zero and the
  // writes to it from within this function will be ignored.
  //
  ulRCC  = HWREG(SYSCTL_RCC);
  ulRCC2 = HWREG(SYSCTL_RCC2);

  //
  // Bypass the PLL and system clock dividers for now.
  //
  ulRCC |= SYSCTL_RCC_BYPASS;
  ulRCC &= ~(SYSCTL_RCC_USESYSDIV);
  ulRCC2 |= SYSCTL_RCC2_BYPASS2;

  //
  // Write the new RCC value.
  //
  HWREG(SYSCTL_RCC) = ulRCC;
  HWREG(SYSCTL_RCC2) = ulRCC2;

  //
  // Make sure that the required oscillators are enabled.  For now, the
  // previously enabled oscillators must be enabled along with the newly
  // requested oscillators.
  //
  ulRCC &= (~(SYSCTL_RCC_IOSCDIS | SYSCTL_RCC_MOSCDIS) |
           (ulConfig & (SYSCTL_RCC_IOSCDIS | SYSCTL_RCC_MOSCDIS)));

  //
  // Write the new RCC value.
  //
  HWREG(SYSCTL_RCC) = ulRCC;

  for (ulDelay = 0; ulDelay < 524288; ulDelay++);

  //
  // Set the new crystal value, oscillator source, and PLL configuration.
  // Since the OSCSRC2 field in RCC2 overlaps the XTAL field in RCC, the
  // OSCSRC field has a special encoding within ulConfig to avoid the
  // overlap.
  //
  ulRCC &= ~(SYSCTL_RCC_XTAL_M | SYSCTL_RCC_OSCSRC_M |
             SYSCTL_RCC_PWRDN | SYSCTL_RCC_OEN);
  ulRCC |= ulConfig & (SYSCTL_RCC_XTAL_M | SYSCTL_RCC_OSCSRC_M |
                       SYSCTL_RCC_PWRDN | SYSCTL_RCC_OEN);
  ulRCC2 &= ~(SYSCTL_RCC2_USERCC2 | SYSCTL_RCC2_OSCSRC2_M |
              SYSCTL_RCC2_PWRDN2);
  ulRCC2 |= ulConfig & (SYSCTL_RCC2_USERCC2 | SYSCTL_RCC_OSCSRC_M |
                        SYSCTL_RCC2_PWRDN2);
  ulRCC2 |= (ulConfig & 0x00000008) << 3;

  //
  // Clear the PLL lock interrupt.
  //
  HWREG(SYSCTL_MISC) = SYSCTL_INT_PLL_LOCK;

  //
  // Write the new RCC value.
  //
  HWREG(SYSCTL_RCC2) = ulRCC2;
  HWREG(SYSCTL_RCC)  = ulRCC;

  //
  // Wait for a bit so that new crystal value and oscillator source can take
  // effect.
  //
  for (ulDelay = 0; ulDelay < 16; ulDelay++);

  //
  // Set the requested system divider and disable the appropriate
  // oscillators.  This will not get written immediately.
  //
  ulRCC &= ~(SYSCTL_RCC_SYSDIV_M | SYSCTL_RCC_USESYSDIV |
             SYSCTL_RCC_IOSCDIS | SYSCTL_RCC_MOSCDIS);
  ulRCC |= ulConfig & (SYSCTL_RCC_SYSDIV_M | SYSCTL_RCC_USESYSDIV |
                       SYSCTL_RCC_IOSCDIS | SYSCTL_RCC_MOSCDIS);
  ulRCC2 &= ~(SYSCTL_RCC2_SYSDIV2_M);
  ulRCC2 |= ulConfig & SYSCTL_RCC2_SYSDIV2_M;

  ulRCC |= SYSCTL_RCC_USESYSDIV;
  ulRCC2 &= ~(SYSCTL_RCC_USESYSDIV);
  ulRCC2 |= ulConfig & (SYSCTL_RCC2_USEFRACT | SYSCTL_RCC2_FRACT);

  //
  // Wait until the PLL has locked.
  //
  for(ulDelay = 32768; ulDelay > 0; ulDelay--)
  {
    if(SYSCTL_RIS & SYSCTL_INT_PLL_LOCK) {
      break;
    }
  }

  //
  // Enable use of the PLL.
  //
  ulRCC &= ~(SYSCTL_RCC_BYPASS);
  ulRCC2 &= ~(SYSCTL_RCC2_BYPASS2);

  //
  // Write the final RCC value.
  //
  HWREG(SYSCTL_RCC)  = ulRCC;
  HWREG(SYSCTL_RCC2) = ulRCC2;

  //
  // Delay for a little bit so that the system divider takes effect.
  //
  for (ulDelay = 0; ulDelay < 16; ulDelay++);

  //
  // Initialize NVIC vector base address. Might be necessary for RAM targets or application not running from 0
  //
  #ifdef __ICCARM__
    #define __Vectors    __vector_table
  #else
    extern unsigned char __Vectors;
  #endif
  NVIC_VTOR = (OS_U32)&__Vectors;

  return 1;                       // Always initialize segments !
}

/*********************************************************************
*
*       OS_Systick
*
* Function description
*   This is the code that gets called when the processor receives a
*   SysTick exception. SysTick is used as OS timer tick.
*
* NOTES:
*   (1) It has to be inserted in the interrupt vector table, if RAM
*       vectors are not used. Therefore is is declared public
*/
void OS_Systick(void) {
  OS_EnterNestableInterrupt();
  OS_TICK_Handle();
#if OS_USE_JLINKMEM
  JLINKMEM_Process();
#endif
  OS_LeaveNestableInterrupt();
}

/*********************************************************************
*
*       OS_InitHW()
*
*       Initialize the hardware (timer) required for the OS to run.
*       May be modified, if an other timer should be used
*/
#define TICK_RELOAD (OS_PCLK_TIMER / OS_TICK_FREQ - 1)
void OS_InitHW(void) {
  OS_U8 TickPrio;

  OS_IncDI();
  //
  // Initialize OS timer, clock soure = core clock
  //
  SYS_TICK_RELOAD  = TICK_RELOAD;
  SYS_TICK_CONTROL = (1 << SYS_TICK_ENABLE_BIT) | (1 << SYS_TICK_CLK_SOURCE_BIT);
  //
  // Install Systick Timer Handler and enable timer interrupt
  //
  OS_ARM_InstallISRHandler(OS_ISR_ID_TICK, (OS_ISR_HANDLER*)OS_Systick);
  //
  // Set the interrupt priority for the system timer to 2nd lowest level to ensure the timer can preempt PendSV handler
  //
  OS_ARM_ISRSetPrio(OS_ISR_ID_TICK, 0xFF);               // Set to lowest level, ALL BITS set
  TickPrio  = OS_ARM_ISRSetPrio(OS_ISR_ID_TICK, 0xFF);   // Read priority back to examine relevant preemption-level-bits
  TickPrio -= 1;                                          // Set to low preemption level, 1 level higher than lowest
  OS_ARM_ISRSetPrio(OS_ISR_ID_TICK, TickPrio);
  OS_ARM_EnableISR(OS_ISR_ID_TICK);
  OS_COM_INIT();
  OS_DecRI();
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
void OS_Idle(void) {     // Idle loop: No task is ready to execute
  while (1) {
    #if ((OS_USE_JLINKMEM == 0) && (DEBUG == 0))     // Enter CPU halt mode when not in DEBUG build and J-Link communication not used
      __asm(" wfi");
    #endif
  }
}

/*********************************************************************
*
*       Get time [cycles]
*
*       This routine is required for high
*       resolution time maesurement functions.
*       It returns the system time in timer clock cycles.
*/
OS_U32 OS_GetTime_Cycles(void) {
  unsigned int t_cnt;
  OS_U32 time;

  time  = OS_Time + 1;
  t_cnt = SYS_TICK_VALUE;
  if (SYS_HANDLER_STATE & (1 << SYS_TICK_ACT_BIT)) {   /* if timer interrupt pending */
    t_cnt = SYS_TICK_VALUE;                             /* Adjust result              */
    time++;
  }
  return (OS_PCLK_TIMER/1000)*time - t_cnt;
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
  return Cycles/(OS_PCLK_TIMER/1000000);
}

#if OS_USE_JLINKMEM

/*********************************************************************
*
*       _JLINKMEM_OnRx()
*/
static void _JLINKMEM_OnRx(OS_U8 Data) {
  OS_OnRx(Data);
}

/*********************************************************************
*
*       _JLINKMEM_OnTx()
*/
static void _JLINKMEM_OnTx(void) {
  OS_OnTx();
}

/*********************************************************************
*
*       _JLINKMEM_GetNextChar()
*/
static OS_INT _JLINKMEM_GetNextChar(void) {
  return OS_COM_GetNextChar();
}

/*********************************************************************
*
*       OS_COM_Init()
*       Initialize memory access for OSView
*/
void OS_COM_Init(void) {
  JLINKMEM_SetpfOnRx(_JLINKMEM_OnRx);
  JLINKMEM_SetpfOnTx(_JLINKMEM_OnTx);
  JLINKMEM_SetpfGetNextChar(_JLINKMEM_GetNextChar);
}

/*********************************************************************
*
*       OS_COM_Send1()
*       Send 1 character via memory
*/
void OS_COM_Send1(OS_U8 c) {
  JLINKMEM_SendChar(c);
}

#else

/*********************************************************************
*
*       Communication for embOSView not selected
*
**********************************************************************
*/
void OS_COM_Send1(OS_U8 c) {
  OS_USEPARA(c);
  OS_COM_ClearTxActive();  /* Free OS transmitter buffer */
}

/*********************************************************************
*
*       OS_COM_IsrHandler
*
* Function description
*   The communication interrupt handler for UART communication
*   to embOSView.
*
* NOTES:
*   (1) It has to be inserted in the interrupt vector table, if RAM
*       vectors are not used. Therefore is is declared public
*/
void OS_COM_IsrHandler(void) {
  while(1);
}

#endif /* #if OS_USE_JLINKMEM */

/*********************************************************************
*
*       OS interrupt handler and ISR specific functions
*
**********************************************************************
*/

#if OS_USE_VARINTTABLE
  //
  // The interrupt vector table may be located anywhere in RAM
  //
  #ifdef __ICCARM__  // IAR
    #pragma data_alignment=512
    __no_init void (*g_pfnRAMVectors[NUM_INTERRUPTS]) (void);
  #endif  // __ICCARM__

  #ifdef __CC_ARM    // KEIL
     __attribute__ (zero_init, aligned(512)) void (*g_pfnRAMVectors[NUM_INTERRUPTS])(void);
  #endif

  #ifdef __GNUC__    // GCC
    void (*g_pfnRAMVectors[NUM_INTERRUPTS]) (void) __attribute__ ((aligned (512)));
  #endif
#endif

/*********************************************************************
*
*       OS_ARM_InstallISRHandler
*/
OS_ISR_HANDLER* OS_ARM_InstallISRHandler (int ISRIndex, OS_ISR_HANDLER* pISRHandler) {
#if OS_USE_VARINTTABLE
  OS_ISR_HANDLER*  pOldHandler;
  OS_U32           ulIdx;
  OS_U32*          pVect;

  pOldHandler = NULL;
  //
  // Check whether the RAM vector table has been initialized.
  //
  if ((*(OS_U32*)_NVIC_VTOREG_ADDR) != (unsigned long)g_pfnRAMVectors) {
    //
    // Copy the vector table from the beginning of FLASH to the RAM vector table.
    //
    pVect = (OS_U32*)(*(OS_U32*)_NVIC_VTOREG_ADDR);
    for(ulIdx = 0; ulIdx < NUM_INTERRUPTS; ulIdx++) {
      g_pfnRAMVectors[ulIdx] = (void (*)(void))(pVect[ulIdx]);
    }
    //
    // Program NVIC to point at the RAM vector table.
    //
    *(OS_U32*)_NVIC_VTOREG_ADDR = (OS_U32)g_pfnRAMVectors;
  }
  //
  // Save the interrupt handler.
  //
  pOldHandler = g_pfnRAMVectors[ISRIndex];
  g_pfnRAMVectors[ISRIndex] = pISRHandler;
  return (pOldHandler);
#else
  //
  // The function does nothing if vector table is constant
  //
  OS_USEPARA(ISRIndex);
  OS_USEPARA(pISRHandler);
  return (NULL);
#endif
}

/*********************************************************************
*
*       OS_ARM_EnableISR
*/
void OS_ARM_EnableISR(int ISRIndex) {
  OS_DI();
  if (ISRIndex < NUM_INTERRUPTS) {
    if (ISRIndex >= 16) {
      //
      // Enable standard "external" interrupts, starting at index 16
      //
      ISRIndex -= 16;
      *(((OS_U32*) NVIC_ENABLE_ADDR) + (ISRIndex >> 5)) = (1 << (ISRIndex & 0x1F));
    } else if (ISRIndex == ISR_ID_MPU) {
      //
      // Enable the MemManage interrupt.
      //
     SYSHND_CTRL |= NVIC_SYS_HND_CTRL_MEM;
    } else if (ISRIndex == ISR_ID_BUS) {
      //
      // Enable the bus fault interrupt.
      //
      SYSHND_CTRL |= NVIC_SYS_HND_CTRL_BUS;
    } else if (ISRIndex == ISR_ID_USAGE) {
      //
      // Enable the usage fault interrupt.
      //
      SYSHND_CTRL |= NVIC_SYS_HND_CTRL_USAGE;
    } else if (ISRIndex == ISR_ID_SYSTICK) {
      //
      // Enable the System Tick interrupt.
      //
      SYS_TICK_CONTROL |= (1 << SYS_TICK_INT_ENABLE_BIT);
    }
  }
  OS_RestoreI();
}

/*********************************************************************
*
*       OS_ARM_DisableISR
*/
void OS_ARM_DisableISR(int ISRIndex) {
  OS_DI();
  if (ISRIndex < NUM_INTERRUPTS) {
    if (ISRIndex >= 16) {
      //
      // Disable standard "external" interrupts
      //
      ISRIndex -= 16;
      *(((OS_U32*) NVIC_DISABLE_ADDR) + (ISRIndex >> 5)) = (1 << (ISRIndex & 0x1F));
    } else if (ISRIndex == ISR_ID_MPU) {
      //
      // Disable the MemManage interrupt.
      //
      SYSHND_CTRL &= ~NVIC_SYS_HND_CTRL_MEM;
    } else if (ISRIndex == ISR_ID_BUS) {
      //
      // Disable the bus fault interrupt.
      //
      SYSHND_CTRL &= ~NVIC_SYS_HND_CTRL_BUS;
    } else if (ISRIndex == ISR_ID_USAGE) {
      //
      // Disable the usage fault interrupt.
      //
      SYSHND_CTRL &= ~NVIC_SYS_HND_CTRL_USAGE;
    } else if (ISRIndex == ISR_ID_SYSTICK) {
      //
      // Enable the System Tick interrupt.
      //
      SYS_TICK_CONTROL &= ~(1uL << SYS_TICK_INT_ENABLE_BIT);
    }
  }
  OS_RestoreI();
}

/*********************************************************************
*
*       OS_ARM_ISRSetPrio
*
*   Notes:
*     (1) Some priorities of system handler are reserved
*         0..3 : Priority can not be set
*         7..10: Reserved
*         13   : Reserved
*     (2) System handler use different control register. This affects
*         ISRIndex 0..15
*/
int OS_ARM_ISRSetPrio(int ISRIndex, int Prio) {
  OS_U8* pPrio;
  int    OldPrio;

  OldPrio = 0;
  if (ISRIndex < NUM_INTERRUPTS) {
    if (ISRIndex >= 16) {
      //
      // Handle standard "external" interrupts
      //
      ISRIndex -= 16;                   // Adjust index
      OS_DI();
      pPrio = (OS_U8*)(NVIC_PRIOBASE_ADDR + ISRIndex);
      OldPrio = *pPrio;
      *pPrio = Prio;
      OS_RestoreI();
    } else {
      //
      // Handle System Interrupt controller
      //
      if ((ISRIndex < 4) | ((ISRIndex >= 7) && (ISRIndex <= 10)) | (ISRIndex == 13)) {
        //
        // Reserved ISR channel, do nothing
        //
      } else {
        //
        // Set priority in system interrupt priority control register
        //
        OS_DI();
        pPrio = (OS_U8*)(SYSPRI1_ADDR);
        ISRIndex -= 4;                  // Adjust Index
        OldPrio = pPrio[ISRIndex];
        pPrio[ISRIndex] = Prio;
        OS_RestoreI();
      }
    }
  }
  return OldPrio;
}

/*****  EOF  ********************************************************/
