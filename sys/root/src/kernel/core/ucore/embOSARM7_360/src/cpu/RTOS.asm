/*********************************************************************
*                  SEGGER MICROCONTROLLER SYSTEME GmbH               *
*          Solutions for real time microcontroller applications      *
**********************************************************************
*                                                                  *
*      (C) 1996-2007   SEGGER Microcontroller Systeme GmbH         *
*                                                                  *
*      www.segger.com  Support: support@segger.com                 *
*                                                                  *
********************************************************************

--------------------------------------------------------------------
File        : RTOS_ARM.ASM
Purpose     : Assembler kernel for the OS and ARM core


Modes:
Tasks:     SYS mode
Scheduler: SVC mode
IRQs:      IRQ mode, then SVC


--------------- END-OF-HEADER ----------------------------------------
*/
; ********************************************************************
; *
; *     Const defines
; *
; ********************************************************************

;  Displacements in C structure OS_TASK declared in RTOS.h

#define DISP_TASK_pStack    4

;   ARM CPU mode bits

IRQ_MODE        DEFINE  0x12            ; Interrupt Request mode
SVC_MODE        DEFINE  0x13            ; Supervisor mode
SYS_MODE        DEFINE  0x1F            ; System mode
FLAG_I          DEFINE  0x80            ; IRQ disable flag

; ********************************************************************
; *
; *     Add postfix for public
; *
; ********************************************************************

; Not currently required, this section is for compatibilty only

; ********************************************************************
; *
; *     Forward declarations of segments used
; *
; ********************************************************************

        MODULE  OS_Switch
        RSEG    IRQ_STACK:DATA(2)
        RSEG    CSTACK:DATA(2)
        RSEG    CODE:CODE:NOROOT(2)

; ********************************************************************
; *
; *     Publics
; *
; ********************************************************************

        RSEG    CODE:CODE:REORDER:NOROOT(2)

        PUBLIC  OS_InitMode
        PUBLIC  OS_Start
        PUBLIC  OS_StartTask
        PUBLIC  OS_Switch
        PUBLIC  OS_SwitchFromInt
        PUBLIC  IRQ_Handler

; ********************************************************************
; *
; *     Externals, code
; *
; ********************************************************************

        EXTERN  OS_ChangeTask
        EXTERN  OS_DecRegionCnt
        EXTERN  OS_irq_handler        ; RTOSINIT.C

; ********************************************************************
; *
; *     Externals, data
; *
; ********************************************************************

        EXTERN OS_Counters
        EXTERN OS_pCurrentTask

; ********************************************************************
; *
; *     MACROS to simplify assembler code
; *
; ********************************************************************

MODE_SYS_EI MACRO
        msr     CPSR_c, #SYS_MODE
        ENDM

MODE_SYS_DI MACRO
        msr     CPSR_c, #SYS_MODE | FLAG_I
        ENDM

MODE_SVC_DI MACRO
        msr     CPSR_c, #SVC_MODE | FLAG_I
        ENDM

MODE_SVC_EI MACRO
        msr     CPSR_c, #SVC_MODE
        ENDM

MODE_IRQ_DI MACRO
        msr     CPSR_c, #IRQ_MODE | FLAG_I
        ENDM

CALL_C MACRO Para
#if 1                          // Faster if C is compiled in Thumb
        ldr     R0,=Para
        mov     LR,PC
        bx      R0


#else
        bl      Para           // Faster if C is compiled in ARM Mode (Requires V4.41a or newer since linker needs to put in a veneer)
#endif

        ENDM


; ********************************************************************
; *
; *     CODE segment
; *
; ********************************************************************

        CODE32

/**********************************************************************
*
*      OS_StartTask
*
*  Function description
*    This code is the first code executed by a newly created task.
*    It makes sure that interrupts are enabled, loads the pContext parameter
*    and continues execution in the actual user task.
*
*  Port specific notes:
*    (1)  On ARM CPUs, the mode needs to be SYS (System)
*/
OS_StartTask:
        MODE_SYS_EI
        mov     R0, R4                  ; void * pContext
#if (__CORE__ == __ARM5__) | (__CORE__ == __ARM5E__) | (__CORE__ == __ARM6__)
        ldmia   SP!, {PC}               ; pop
#else
        ldmia   SP!, {LR}               ; pop
        bx      LR
#endif

/*********************************************************************
*
*     OS_Switch: Scheduler entrance point, cooperative
*
* Context: Called from a task
*          RegionCnt > 0
*          Interrupts enabled
*/
OS_Switch:
        MODE_SYS_EI                          // We are and stay in SYS-mode. Purpose: Enable interrupts!
;
; push Permanent registers, OS_Counters and LR
;
        ldr     R0, =OS_Counters
        ldr     R3, [R0, #+0]
        stmdb   SP!,{R3-R11, LR}        ; push
;
; save SP in task control block
;
        ldr     R0, =OS_pCurrentTask
        ldr     R1, [R0, #+0]
        str     SP, [R1, #+DISP_TASK_pStack]
        MODE_SVC_EI
ChangeTask:
        CALL_C  OS_ChangeTask
;
; *** Switch back to system mode
;
        MODE_SYS_DI
;
; *** Restore task SP
;
        ldr     R0, =OS_pCurrentTask
        ldr     R1, [R0, #+0]
        ldr     SP, [R1, #+DISP_TASK_pStack]
;
; pop Permanent registers, OS_Counters and LR
;
        ldmia   SP!, {R3-R11, LR}       ; pop
        ldr     R0, =OS_Counters
        str     R3, [R0, #+0]
        bx      LR

/*********************************************************************
*
*     OS_Start
*
*  General Function description
*    Called from main().
*    This calls the scheduler for the first time, activating multitasking.
*    Never returns to caller.
*
*  Port specific information
*    CPU mode on call is irrelevant.
*/
OS_Start:
      // b     OS_SwitchAfterISR_Idle
/*********************************************************************
*
*     OS_SwitchAfterISR_Idle
*
*  General description
*    Executed after ISR initiating context switch interrupting the Idle-loop.
*    Since Idle is not a task, no context information needs to be saved.
*    The only operation is typically to load the stack pointer to the system stack and
*    activating the scheduler by calling OS_ChangeTask()
*
* Context: Activated after ISR
*          RegionCnt > 0
*          Interrupts enabled
*
*  Port specific information
*    CPU mode must be SVC !
*/
OS_SwitchAfterISR_Idle:
        ldr     SP,=(SFE(CSTACK) & 0xFFFFFFF8)
        b       ChangeTask


/*********************************************************************
*
*      OS_SwitchFromInt
*
*  General Function description
*    Initiate context switch after completion of ISR.
*    Called from ISR if a task switch needs to be perfromed and ONLY if RegionCnt == 0.
*    In order to avoid multiple calls to this routine before the actual task switch,
*    RegionCnt is incremented.
*
*  Context
*    Called from ISR.
*
*  Port specific information
*    Each call stores 8 bytes on the task stack.
*
*/
OS_SwitchFromInt:
;
; R0 = address in IRQ stack which contains saved LR_irq
;
        ldr     R0, =(SFE(IRQ_STACK) & 0xFFFFFFF8)-4
;
; *** Check if idle. If so, replace return addr with OS_SwitchAfterISR_Idle and return.
;
        ldr     R1, =OS_pCurrentTask
        ldr     R1, [R1, #+0]
        cmp     R1, #+0
        ldreq   R1,=OS_SwitchAfterISR_Idle+4
        ldreq   R2,=SVC_MODE                    // After Idle, we are o.k. to run in SVC mode
        beq     ReplaceReturnAddr
;
; *** Copy return adr. onto user stack. This is done only if a task has been interrupted, not if the Idle-loop has been interrupted.
;
        ldr     R1, [R0, #+0]
        ldr     R2, [R0,#-24]
        MODE_SYS_DI
        stmdb   SP!,{R1-R2}             ; push ret addr/PSW
        MODE_SVC_DI
;
; *** replace the topmost return adr on ISTACK
;
        ldr     R1,=OS_SwitchAfterISR+4
        mov     R2,#SYS_MODE                 // We need to continue to run in System mode in order to save all permanent registers as well
ReplaceReturnAddr:
        str     R1,[R0, #+0]
        str     R2,[R0, #-24]
;
; *** return
;
        bx      LR                      ; return


/*********************************************************************
*
*     OS_SwitchAfterISR
*
*  General Function description
*    This code is activated right after the ISR which has called OS_SwitchFromInt
*    Instead of returning to the task code, this code is executed.
*    The code in OS_SwitchFromInt() must have incremented RegionCnt.
*
*  On entry:
*    RegionCnt >0    // To avoid further task switched from ISR
*    DICnt     =0
*    EI
*
*  Port specific information
*    Runs in SYS mode. Original PC/CPSR is on SYS stack (task stack).
*
*/
OS_SwitchAfterISR:
        stmdb   SP!,{R0-R3,R12,LR}      ; save scratch registers & LR_sys
        bl      OS_Switch               ; Returns with interrupts disabled
        CALL_C  OS_DecRegionCnt
        add     R2,SP,#24               ; R2 = SP_sys + sizeof(scratch regs & LR_sys)
        MODE_IRQ_DI
        ldmia   R2!,{R0-R1}
        msr     SPSR_cxfs, R1           ; SPSR_irq = original SPSR_irq
        mov     LR,R0                   ; LR_irq = original return address
        MODE_SYS_DI
        ldmia   SP!,{R0-R3,R12,LR}      ; reload scratch registers & LR_sys
        add     SP,SP,#8                ; remove PC/PSW from SYS stack
        MODE_IRQ_DI
        subs    PC, LR,#4               ; RETI

/*********************************************************************
*
*     OS_InitMode
*
*  Function description
*    This is an ARM-specific function not present in most ports.
*    Called from  OS_InitKern();
*    It makes sure that the CPU is in SVC mode since the
*    kernel needs to run in SVC (Supervisor) mode.
*    Some start-ups however use a different mode, such as system mode.
*    We simply copy all relevant registers, incl. R13 (SP) from the current mode to SVC mode and
*    return in SVC mode.
*/
OS_InitMode:
        stmdb   SP!,{R4-R12,LR}         ; push all relevant registers
        mov     r0, r13
        MODE_SVC_DI
        mov     r13, r0
        ldmia   SP!,{R4-R12,LR}         ; pop all relevant registers
        bx      LR


/*********************************************************************
*
*      OS_IsInInt
*
*  Function description
*    Returns non 0 if an ISR is active (or in case of doubt or if function is not fully implemented)
*    Called in debug builds of embOS only.
*
*
*/
        PUBLIC  OS_IsInInt
OS_IsInInt:
;
;        ldr     R0, =SFE(IRQ_STACK)
;        mrs     R2, CPSR                        ; Load CPSR
;        msr     CPSR_c, #IRQ_MODE | FLAG_I      ; Switch to FIQ mode
;        mov     R1, R13
;        msr     CPSR_c, R2                      ; Switch mode back
;        sub r0, r0, r1

         mov     r0, #0
         bx      LR


; ********************************************************************
; *
; *     Interrupt service routine
; *
; ********************************************************************
;
; This routine should be activated by an IRQ. IRQ on the
; ARM 7/9 will jump to 0x18, where a jump to this routine
; should be located.
;
; Note: in the MSR instructions,
; c   means control field:        bits 0..7  (mode, T, F, I)
; x   means extension field       bits 8..15
; s   means status    field       bits 16..23
; f   means flag      field       bits 24..31

IRQ_Handler:
;
; Save temp. registers
;
        stmdb   SP!,{R0-R3,R12,LR}             ; push
;
; push SPSR (req. if we allow nested interrupts)
;
        mrs     R0, SPSR                       ; load SPSR
        stmdb   SP!,{R0}                       ; push SPSR_irq on IRQ stack
;
; Switch to supervisor mode (svc mode), keep interrupt disabled, don't modify FIQ bit
;
        mrs     R0, CPSR                       ;
        orr     R0, R0, #(SVC_MODE | FLAG_I)   ; We know that we are in IRQ mode (0x12), therfore we can use OR to switch to SVC mode (0x13)
        msr     CPSR_c, R0                     ; Write back modified mode
;
; Call "C" interrupt handler in RTOSINIT.c
;
        stmdb   SP!, {LR}                      ; push LR_svc on SVC stack
        CALL_C  OS_irq_handler
        ldmia   SP!, {LR}                      ; pop LR_svc from svc stack
;
; Switch back to IRQ mode, do not modify FIQ bit !
;
        mrs     R0, CPSR
        bic     R0, R0, #0x1F                  ; Reset current mode (Should be SVC mode, interrupts disabled)
        orr     R0, R0, #(IRQ_MODE| FLAG_I)    ; Set IRQ mode, interrupts disabled (Normally a BIC R0, 0x01 should do, because we should always arrive here with Ints disabled and supervisor mode)
        msr     CPSR_c, R0                     ; Write back modified mode
;
; pop SPSR
;
        ldmia   SP!, {R1}                      ; pop SPSR_irq from IRQ stack
        msr     SPSR_cxfs, R1
;
; Restore temp registers
;
        ldmia   SP!, {R0-R3,R12,LR}            ; pop
        subs    PC, LR, #4                     ; RETI

; ********************************************************************

        ENDMOD
; ****  End of file  *************************************************
        END

