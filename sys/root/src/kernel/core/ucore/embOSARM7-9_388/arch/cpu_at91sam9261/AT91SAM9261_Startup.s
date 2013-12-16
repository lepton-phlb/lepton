;*********************************************************************
;*                SEGGER MICROCONTROLLER SYSTEME GmbH                *
;*        Solutions for real time microcontroller applications       *
;*********************************************************************
;*                                                                   *
;*      (C) 2005- 2007 SEGGER Microcontroller Systeme GmbH           *
;*                  www.segger.com                                   *
;*                                                                   *
;*********************************************************************
;
;---------------------------------------------------------------------
;File   : AT91SAM9261_Startup.s
;Purpose: Startup file for embOS and IAR embedded workbench V5.x
;-------- END-OF-HEADER ----------------------------------------------
;

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION RO_DATA:CODE
        SECTION IRQ_STACK:DATA:NOROOT(3)
        SECTION FIQ_STACK:DATA:NOROOT(3)
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        PUBLIC  __vector
        PUBLIC  __vector_0x14
        PUBLIC  __iar_program_start
        EXTERN  Undefined_Handler
        EXTERN  SWI_Handler
        EXTERN  Prefetch_Handler
        EXTERN  Abort_Handler
        EXTERN  IRQ_Handler
        EXTERN  FIQ_Handler

        ARM

__vector:
        ; All default exception handlers (except reset) are
        ; defined as weak symbol definitions.
        ; If a handler is defined by the application it will take precedence.

        ldr     PC,Reset_Addr           ; To allow remap, a relative branch is required !

        ldr     PC,Undefined_Addr       ; Undefined instructions
        ldr     PC,SWI_Addr             ; Software interrupt (SWI/SVC)
        ldr     PC,Prefetch_Addr        ; Prefetch abort
        ldr     PC,Abort_Addr           ; Data abort
__vector_0x14:
        DCD     SIZEOF(RO_DATA)         ; Size of all const and text sections.

        ldr     PC,IRQ_Addr             ; IRQ
        ldr     PC,FIQ_Addr             ; FIQ

        DATA

Reset_Addr:     DCD   __iar_program_start
Undefined_Addr: DCD   Undefined_Handler
SWI_Addr:       DCD   SWI_Handler
Prefetch_Addr:  DCD   Prefetch_Handler
Abort_Addr:     DCD   Abort_Handler
IRQ_Addr:       DCD   IRQ_Handler
FIQ_Addr:       DCD   FIQ_Handler

__vector_end:

; --------------------------------------------------
; ?cstartup -- low-level system initialization code.
;
; After a reser execution starts here, the mode is ARM, supervisor
; with interrupts disabled.
;


        SECTION .text:CODE:NOROOT(2)

        EXTERN  ?main
        REQUIRE __vector

        ARM

__iar_program_start:

/**********************************************************************
* ?CSTARTUP
*
* Execution starts here.
* After a reset, the mode is ARM, Supervisor, interrupts disabled.
*/
?cstartup:
        mrc     p15, 0, r0, c1, c0,0                ; read cp15 control register (cp15 r1) r0
        ldr     r3, =0xC0001085                     ; Disable Cache, Disable MMU
        ldr     r4, =0x40004000                     ; Round Robin replacement
        bic     r0, r0, r3
        orr     r0, r0, r4
        mcr     p15, 0, r0, c1, c0,0                ; write cp15 control register (cp15 r1)

; Initialize the stack pointers.
        ;; Set up the interrupt stack pointer.

        msr     cpsr_c,#0xd2            ; Change mode to IRQ mode, FIQ and IRQ disabled
        ldr     sp, =SFE(IRQ_STACK)     ; End of IRQ_STACK
        bic     sp,sp,#0x7              ; Make sure SP is aligned on 8 byte boundary
	
        ;; Set up the fast interrupt stack pointer.

        msr     cpsr_c,#0xd1            ; Change mode to FIQ mode, FIQ and IRQ disabled
        ldr     sp, =SFE(FIQ_STACK)     ; End of FIQ_STACK
        bic     sp,sp,#0x7              ; Make sure SP is aligned on 8 byte boundary

        ;; Set up the normal stack pointer.

        msr     cpsr_c,#0xdf            ; Change mode to SYS mode, FIQ and IRQ disabled
        ldr     sp, =SFE(CSTACK)        ; End of CSTACK
        bic     sp,sp,#0x7              ; Make sure SP is aligned on 8 byte boundary

; Jump to startup code in library
        ldr     r0,=?main
        bx      r0

        LTORG
        END

; ***** EOF **********************************************************
