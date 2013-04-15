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
*       OS version: 3.82u                                            *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : RTOSInit_CMSIS.c
Purpose : RTOSInit for CMSIS. Initializes and
          handles the hardware for the OS as far as required by the OS.
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.h"
#include "kernel/core/kernelconf.h"

#if (__tauon_cpu_device__==__tauon_cpu_device_cortexM3_LM3S__)  //GD
    #include "Device/lm3s/lm3s_cmsis.h"
#elif (__tauon_cpu_device__ == __tauon_cpu_device_cortexM4_stm32f4__)
   #include "Device/st/stm32f2xx/stm32f2xx.h"
#else
#error "Device specific CMSIS include missing. Wrong CPU device chosen?"
#endif
/*********************************************************************
*
*       Configuration
*
*********************************************************************/

#define NVIC_VTOR           (*(volatile unsigned long *) (0xE000ED08))

/*********************************************************************
*
*       Clock frequency settings
*/
#ifndef   OS_FSYS                   /* CPU main clock frequency     */
  #define OS_FSYS SystemCoreClock
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

/*********************************************************************
*
*       Settings for embOSView
*/
#ifndef   OS_VIEW_ENABLE
  #define OS_VIEW_ENABLE   (1)
#endif

#ifndef   OS_VIEW_USE_UART          // If set, UART will be used for communication
  #define OS_VIEW_USE_UART (0)      // Default: 0 => memory access is used
#endif                              // if OS_VIEW_ENABLE is on

/****** End of configuration settings *******************************/

#ifndef   DEBUG
  #define DEBUG (0)
#endif

/*********************************************************************
*
*       JLINKMEM and UART settings for OSView
*
*       Automatically generated from configuration settings
*/
#define OS_USE_JLINKMEM   (OS_VIEW_ENABLE && (OS_VIEW_USE_UART == 0))

#define OS_UART_USED      (OS_VIEW_ENABLE && (OS_VIEW_USE_UART != 0) && (0))

#if OS_USE_JLINKMEM
  #include "JLINKMEM.h"
#endif

/*********************************************************************
*
*       Vector table
*/
#ifdef __ICCARM__
  #define __Vectors    __vector_table
#else
  extern unsigned char __Vectors;
#endif

/*********************************************************************
*
*       Local defines (sfrs used in RTOSInit.c)
*
**********************************************************************
*/

#define SYS_PENDSTSET_BIT             (26)

/****** Map systick exception to OS timer handler *******************/


/****** MAP UART initialization function ****************************/

#if  (OS_UART_USED || OS_USE_JLINKMEM)
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
*       SysTick_Handler
*
* Function description
*   This is the code that gets called when the processor receives a
*   _SysTick exception. SysTick is used as OS timer tick.
*/
void SysTick_Handler(void);      // Avoid warning, Systick_Handler is not prototyped in any CMSIS header
void SysTick_Handler(void) {
  OS_EnterNestableInterrupt();
  OS_HandleTick();
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
void OS_InitHW(void) {
  uint32_t  TickPrio;

  OS_IncDI();
  //
  // We assume, the PLL and core clock was already set by the SystemInit() function
  // Which was called from the startup code
  // Therefore, we don't have to initailize anything here,
  // we just set the periodic system timer tick for embOS.
  //
  SystemCoreClockUpdate();                             // Update the system clock variable (might not have been set before)
  if (SysTick_Config (OS_PCLK_TIMER / OS_TICK_FREQ)) { // Setup SysTick Timer for 1 msec interrupts
    while (1);                                         // Handle Error
  }
  //
  // Initialize NVIC vector base address. Might be necessary for RAM targets or application not running from 0
  //
  NVIC_VTOR = (OS_U32)&__Vectors;
  //
  // Set the interrupt priority for the system timer to 2nd lowest level to ensure the timer can preempt PendSV handler
  //
  NVIC_SetPriority(SysTick_IRQn, (uint32_t) -1);
  TickPrio  = NVIC_GetPriority(SysTick_IRQn);
  TickPrio -=1;
  NVIC_SetPriority(SysTick_IRQn, (uint32_t)TickPrio);

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
      #ifdef __ICCARM__  // IAR
        __asm(" wfi");
      #endif
      #ifdef __CC_ARM    // KEIL
        __wfi();
      #endif
    #endif
  }
}

/*********************************************************************
*
*       Get time [cycles]
*
*       This routine is required for high
*       resolution time measurement functions.
*       It returns the system time in timer clock cycles.
*/
OS_U32 OS_GetTime_Cycles(void) {
  unsigned int t_cnt;
  OS_U32 time;
  time  = OS_Time;

  t_cnt = (OS_PCLK_TIMER/1000) - SysTick->VAL;
  if (SysTick->CTRL & (1uL << SYS_PENDSTSET_BIT)) {  /* Missed a counter interrupt? */
    t_cnt = (OS_PCLK_TIMER/1000) - SysTick->VAL;     /* Adjust result               */
    time++;
  }
  return (OS_PCLK_TIMER/1000) * time + t_cnt;
}

/*********************************************************************
*
*       OS_ConvertCycles2us
*
*       Convert Cycles into micro seconds.
*
*       If your clock frequency is not a multiple of 1 MHz,
*       you may have to modify this routine in order to get proper
*       diagnostics.
*
*       This routine is required for profiling or high resolution time
*       measurement only. It does not affect operation of the OS.
*/
OS_U32 OS_ConvertCycles2us(OS_U32 Cycles) {
  return Cycles/(OS_PCLK_TIMER/1000000);
}


/*********************************************************************
*
*       Optional communication with embOSView
*
**********************************************************************
*/

#if OS_USE_JLINKMEM                    // Communication via JTAG / SWD

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

#elif OS_UART_USED   // Communication via UART, can not be implemented generic

  #error "OS_UART can not be handled with current generic CMSIS version."

  //
  // Device specific versions of the 3 communication functions
  // OS_COM_IsrHandler(void)
  // OS_COM_Send1(OS_U8 c)
  // OS_COM_Init(void)
  // have to be implemented when a UART shall be used for communication to embOSView.
  // Samples can be found in our device specific projects.

#else  /*  No communication or UART selected, using dummies */

void OS_COM_Send1(OS_U8 c) {
  OS_USEPARA(c);           /* Avoid compiler warning */
  OS_COM_ClearTxActive();  /* Let the OS know that Tx is not busy */
}

#endif /*  OS_UART_USED  */

/****** Final check of configuration ********************************/

#ifndef OS_UART_USED
  #error "OS_UART_USED has to be defined"
#endif

#if (OS_TICK_FREQ != 1000)
  #error "OS_TICK_FREQ != 1000, ensure OS_GetTime_Cycles() and OS_ConvertCycles2us() work."
#endif




/***** Core dump *****************************************************/ //GD
unsigned long g_stack_minidump[8];  
char g_task_minidump_name[32];
void save_fault_cpu_state(void/*unsigned long * stack_ptr*/){
    OS_TASK* pRunningTask;  
    unsigned long * stack_ptr;
    stack_ptr = (unsigned long *) __get_PSP(); //__ASM("mrs r0, psp");
    g_stack_minidump[0]= *stack_ptr++; /* R0 */
    g_stack_minidump[1]= *stack_ptr++; /* R1 */
    g_stack_minidump[2]= *stack_ptr++; /* R2 */
    g_stack_minidump[3]= *stack_ptr++; /* R3 */
    g_stack_minidump[4]= *stack_ptr++; /* R12 */
    g_stack_minidump[5]= *stack_ptr++; /* LR */  
    g_stack_minidump[6]= *stack_ptr++; /* PC */
    g_stack_minidump[7]= *stack_ptr;   /* PSR */
    // Display or printf 
    // Get the running task name
    pRunningTask = OS_GetpCurrentTask();/* get ptr */  
//    printf("%s",pRunningTask->Name);  
    //Store task name
    strncpy(g_task_minidump_name, pRunningTask->Name, sizeof(g_task_minidump_name) );
}
/*****  End of file  *************************************************/

