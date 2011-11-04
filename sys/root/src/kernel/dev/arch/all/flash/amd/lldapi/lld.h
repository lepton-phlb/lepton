
/* 
modification history
--------------------
01,20sep02,jgp written.
02,09sep03,jch Added delays for Clarifications C2 and C3.  These delays are   
   enabled with WAIT_4us_FOR_DATA_POLLING_BITS_TO_BECOME_ACTIVE and         
   PAUSE_BETWEEN_ERASE_SUSPENDS.                                            
03,02oct03,jch Improve lld_StatusGet's interleave handling, renamed functions
   to the new standard, implemented E2 and E3.
*/


#ifndef __INC_lldh
#define __INC_lldh

/* NOTICE
MirrorBit flash devices requires 4us from the time    
a programming command is issued before the data polling 
bits can be read.  Without the delay, it is likely
that you will read invalid status from the flash.
The invalid status may lead the software to believe
that programming finished early without problems or 
that programming failed.  If your system has more 
than 4us of delay inherently, you don't need any 
additional delay.  Otherwise, change the #undef to 
a #define
WAIT_4us_FOR_DATA_POLLING_BITS_TO_BECOME_ACTIVE
in lld.h.  Make sure your optimization does not          
remove the delay loop.  You must replace DELAY_4us    
with a value which makes sense for your system.       

It is possible to suspend the erase operation 
with such frequency that it is unable to complete 
the erase operation and eventually times-out.                                            
Change the #undef to #define PAUSE_BETWEEN_ERASE_SUSPENDS 
in lld.h if you are using erase suspend and the 
following is true.
Time between suspends is less that 10 milliseconds    
AND total number of suspends per erase can exceed 5000.         
Make sure that your optimization does not remove the  
delay loop.  You must replace DELAY_10ms with a value 
which make sense in your system.

For more information, visit our web site at www.spansion.com,
email us at software@spansion.com or go to www.ask.amd.com.
*/

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* LLD System Specific Typedefs */
typedef unsigned char  BYTE;   /* 8 bits wide */
typedef unsigned short WORD;   /* 16 bits wide */
typedef unsigned long  DWORD;  /* 32 bits wide */
typedef DWORD ADDRESS;            /* Used for system level addressing */
typedef unsigned long WORDCOUNT;  /* used for multi-byte operations */

/* boolean macros */
#ifndef TRUE
 #define TRUE  (1)
#endif
#ifndef FALSE
 #define FALSE (0)
#endif

/* Data Bus Configurations */
#define A_MINUS_1        (0x0100)
#define MULTI_DEVICE_CFG (0x0200)

#define DEVICE_TYPE_MASK (0x000F)
#define X8_DEVICE        (0x0001)
#define X16_DEVICE       (0x0002)
#define X8X16_DEVICE     (0x0004)
#define X16X32_DEVICE    (0x0008)

#define DATA_WIDTH_MASK  (0x00F0)
#define X8_DATA          (0x0010)
#define X16_DATA         (0x0020)
#define X32_DATA         (0x0040)
#define X64_DATA         (0x0080) /* RFU */

/* device/bus width configurations */
#define X8_AS_X8      (X8_DEVICE     | X8_DATA)
#define X8X16_AS_X8   (X8X16_DEVICE  | X8_DATA |    A_MINUS_1)

#define X8_AS_X16     (X8_DEVICE     | X16_DATA | MULTI_DEVICE_CFG)
#define X16_AS_X16    (X16_DEVICE    | X16_DATA)
#define X8X16_AS_X16  (X8X16_DEVICE  | X16_DATA)
#define X16X32_AS_X16 (X16X32_DEVICE | X16_DATA | A_MINUS_1)

#define X8_AS_X32     (X8_DEVICE     | X32_DATA | MULTI_DEVICE_CFG)
#define X16_AS_X32    (X16_DEVICE    | X32_DATA | MULTI_DEVICE_CFG)
#define X8X16_AS_X32  (X8X16_DEVICE  | X32_DATA | MULTI_DEVICE_CFG)
#define X16X32_AS_X32 (X16X32_DEVICE | X32_DATA) 

/* polling routine options */
typedef enum
{
LLD_P_POLL_PGM = 1,
LLD_P_POLL_WRT_BUF_PGM,
LLD_P_POLL_SEC_ERS,
LLD_P_POLL_CHIP_ERS,
LLD_P_POLL_RESUME
}POLLING_TYPE;

typedef enum {
 DEV_STATUS_UNKNOWN = 0,
 DEV_NOT_BUSY,
 DEV_BUSY,
 DEV_EXCEEDED_TIME_LIMITS,
 DEV_SUSPEND,
 DEV_WRITE_BUFFER_ABORT,
 DEV_STATUS_GET_PROBLEM,
 DEV_VERIFY_ERROR,
 DEV_BYTES_PER_OP_WRONG
} DEVSTATUS;

#include "lld_target_specific.h"

#if    LLD_CONFIGURATION == X8_AS_X8
#define LLD_DEV_MULTIPLIER 0x00000001
#define LLD_DB_READ_MASK   0x000000FF
#define LLD_DEV_READ_MASK  0x000000FF
#define LLD_UNLOCK_ADDR1   0x00000555
#define LLD_UNLOCK_ADDR2   0x000002AA
#define LLD_BYTES_PER_OP   0x00000001
#define LLD_BUFFER_SIZE    16
typedef BYTE FLASHDATA;

#elif  LLD_CONFIGURATION == X8X16_AS_X8
#define LLD_DEV_MULTIPLIER 0x00000001
#define LLD_DB_READ_MASK   0x000000FF
#define LLD_DEV_READ_MASK  0x000000FF
#define LLD_UNLOCK_ADDR1   0x00000AAA
#define LLD_UNLOCK_ADDR2   0x00000555
#define LLD_BYTES_PER_OP   0x00000001
#define LLD_BUFFER_SIZE    16
typedef BYTE FLASHDATA;

#elif  LLD_CONFIGURATION == X8_AS_X16
#define LLD_DEV_MULTIPLIER 0x00000101
#define LLD_DB_READ_MASK   0x0000FFFF
#define LLD_DEV_READ_MASK  0x000000FF
#define LLD_UNLOCK_ADDR1   0x00000555
#define LLD_UNLOCK_ADDR2   0x000002AA
#define LLD_BYTES_PER_OP   0x00000002
#define LLD_BUFFER_SIZE    16
typedef WORD FLASHDATA;

#elif  LLD_CONFIGURATION == X16_AS_X16
#define LLD_DEV_MULTIPLIER 0x00000001
#define LLD_DB_READ_MASK   0x0000FFFF
#define LLD_DEV_READ_MASK  0x0000FFFF
#define LLD_UNLOCK_ADDR1   0x00000555
#define LLD_UNLOCK_ADDR2   0x000002AA
#define LLD_BYTES_PER_OP   0x00000002
#define LLD_BUFFER_SIZE    16
typedef WORD FLASHDATA;

#elif  LLD_CONFIGURATION == X8X16_AS_X16
#define LLD_DEV_MULTIPLIER 0x00000001
#define LLD_DB_READ_MASK   0x0000FFFF
#define LLD_DEV_READ_MASK  0x0000FFFF
#define LLD_UNLOCK_ADDR1   0x00000AAA
#define LLD_UNLOCK_ADDR2   0x00000555
#define LLD_BYTES_PER_OP   0x00000002
#define LLD_BUFFER_SIZE    16
typedef WORD FLASHDATA;

#elif  LLD_CONFIGURATION == X16X32_AS_X16
#define LLD_DEV_MULTIPLIER 0x00000001
#define LLD_DB_READ_MASK   0x0000FFFF
#define LLD_DEV_READ_MASK  0x0000FFFF
#define LLD_UNLOCK_ADDR1   0x00000AAA
#define LLD_UNLOCK_ADDR2   0x00000555
#define LLD_BYTES_PER_OP   0x00000002
#define LLD_BUFFER_SIZE    16
typedef WORD FLASHDATA;

#elif  LLD_CONFIGURATION == X8_AS_X32
#define LLD_DEV_MULTIPLIER 0x01010101
#define LLD_DB_READ_MASK   0xFFFFFFFF
#define LLD_DEV_READ_MASK  0x000000FF
#define LLD_UNLOCK_ADDR1   0x00000555
#define LLD_UNLOCK_ADDR2   0x000002AA
#define LLD_BYTES_PER_OP   0x00000004
#define LLD_BUFFER_SIZE    16
typedef DWORD FLASHDATA;

#elif  LLD_CONFIGURATION == X16_AS_X32
#define LLD_DEV_MULTIPLIER 0x00010001
#define LLD_DB_READ_MASK   0xFFFFFFFF
#define LLD_DEV_READ_MASK  0x0000FFFF
#define LLD_UNLOCK_ADDR1   0x00000555
#define LLD_UNLOCK_ADDR2   0x000002AA
#define LLD_BYTES_PER_OP   0x00000004
#define LLD_BUFFER_SIZE    16
typedef DWORD FLASHDATA;

#elif  LLD_CONFIGURATION == X8X16_AS_X32
#define LLD_DEV_MULTIPLIER 0x00010001
#define LLD_DB_READ_MASK   0xFFFFFFFF
#define LLD_DEV_READ_MASK  0x0000FFFF
#define LLD_UNLOCK_ADDR1   0x00000AAA
#define LLD_UNLOCK_ADDR2   0x00000555
#define LLD_BYTES_PER_OP   0x00000004
#define LLD_BUFFER_SIZE    16
typedef DWORD FLASHDATA;

#elif  LLD_CONFIGURATION == X16X32_AS_X32
#define LLD_DEV_MULTIPLIER 0x00000001
#define LLD_DB_READ_MASK   0xFFFFFFFF
#define LLD_DEV_READ_MASK  0xFFFFFFFF
#define LLD_UNLOCK_ADDR1   0x00000AAA
#define LLD_UNLOCK_ADDR2   0x00000555
#define LLD_BYTES_PER_OP   0x00000004
#define LLD_BUFFER_SIZE    16
typedef DWORD FLASHDATA;

#endif                   
/* public function prototypes */

/* Operation Functions */
#ifndef REMOVE_LLD_READ_OP
extern FLASHDATA lld_ReadOp
(
FLASHDATA * base_addr,   /* device base address is system */
ADDRESS offset       /* address offset from base address */
);
#endif

#ifndef REMOVE_WRITE_BUFFER_PROGRAMMING
extern DEVSTATUS lld_WriteBufferProgramOp
(
FLASHDATA * base_addr,   /* device base address is system */
ADDRESS offset,      /* address offset from base address */
WORDCOUNT word_cnt,  /* number of words to program */
FLASHDATA *data_buf  /* buffer containing data to program */
);
#endif

#ifndef REMOVE_LLD_PROGRAM_OP
extern DEVSTATUS lld_ProgramOp
(
FLASHDATA * base_addr,   /* device base address is system */
ADDRESS offset,      /* address offset from base address */
FLASHDATA write_data /* variable containing data to program */
);
#endif

#ifndef REMOVE_LLD_SECTOR_ERASE_OP
extern DEVSTATUS lld_SectorEraseOp
(
FLASHDATA * base_addr,   /* device base address is system */
ADDRESS offset       /* address offset from base address */
);
#endif

#ifndef REMOVE_LLD_CHIP_ERASE_OP
extern DEVSTATUS lld_ChipEraseOp
(
FLASHDATA * base_addr    /* device base address is system */
);
#endif


/* Command Functions */
extern void lld_ResetCmd
(
FLASHDATA * base_addr   /* device base address in system */
);

#ifndef REMOVE_LLD_SECTOR_ERASE_CMD
extern void lld_SectorEraseCmd
(
FLASHDATA * base_addr,   /* device base address in system */
ADDRESS offset       /* address offset from base address */
);
#endif

#ifndef REMOVE_LLD_CHIP_ERASE_CMD
extern void lld_ChipEraseCmd
(
FLASHDATA * base_addr    /* device base address in system */
);
#endif

#ifndef REMOVE_LLD_PROGRAM_CMD
extern void lld_ProgramCmd
(
FLASHDATA * base_addr,               /* device base address in system */
ADDRESS offset,                  /* address offset from base address */
FLASHDATA *pgm_data_ptr          /* variable containing data to program */
);
#endif

#ifndef REMOVE_LLD_UNLOCK_BYPASS_ENTRY_CMD
extern void lld_UnlockBypassEntryCmd
(
FLASHDATA * base_addr    /* device base address in system */
);
#endif

#ifndef REMOVE_LLD_UNLOCK_BYPASS_PROGRAM_CMD
extern void lld_UnlockBypassProgramCmd
(
FLASHDATA * base_addr,               /* device base address in system */
ADDRESS offset,                  /* address offset from base address */
FLASHDATA *pgm_data_ptr          /* variable containing data to program */
);
#endif

#ifndef REMOVE_LLD_UNLOCK_BYPASS_RESET_CMD
extern void lld_UnlockBypassResetCmd
(
FLASHDATA * base_addr   /* device base address in system */
);
#endif

#ifndef REMOVE_LLD_AUTOSELECT_ENTRY_CMD
extern void lld_AutoselectEntryCmd
(
FLASHDATA * base_addr   /* device base address in system */
);
#endif

#ifndef REMOVE_LLD_AUTOSELECT_EXIT_CMD
extern void lld_AutoselectExitCmd
(
FLASHDATA * base_addr   /* device base address in system */
);
#endif

#ifndef REMOVE_LLD_SECSI_SECTOR_ENTRY_CMD
extern void lld_SecSiSectorEntryCmd
(
FLASHDATA * base_addr    /* device base address in system */
);
#endif

#ifndef REMOVE_LLD_SECTI_SECTOR_EXIT_CMD
extern void lld_SecSiSectorExitCmd
(
FLASHDATA * base_addr   /* device base address in system */
);
#endif

#ifndef REMOVE_WRITE_BUFFER_PROGRAMMING
extern void lld_WriteToBufferCmd
(
FLASHDATA * base_addr,   /* device base address in system */
ADDRESS offset       /* address offset from base address */
);
#endif

#ifndef REMOVE_WRITE_BUFFER_PROGRAMMING
extern void lld_ProgramBufferToFlashCmd
(
FLASHDATA * base_addr,   /* device base address in system */
ADDRESS offset       /* address offset from base address */
);
#endif

#ifndef REMOVE_WRITE_BUFFER_PROGRAMMING
extern void lld_WriteBufferAbortResetCmd
(
FLASHDATA * base_addr    /* device base address in system */
);
#endif

#ifndef REMOVE_LLD_PROGRAM_SUSPEND_CMD
extern void lld_ProgramSuspendCmd
(
FLASHDATA * base_addr,   /* device base address in system */
ADDRESS offset       /* address offset from base address */
);
#endif

#ifndef REMOVE_LLD_ERASE_SUSPEND_CMD
extern void lld_EraseSuspendCmd
(
FLASHDATA * base_addr,   /* device base address in system */
ADDRESS offset       /* address offset from base address */
);
#endif

#ifndef REMOVE_LLD_ERASE_RESUME_CMD
extern DEVSTATUS lld_EraseResumeCmd
(
FLASHDATA * base_addr,   /* device base address in system */
ADDRESS offset       /* address offset from base address */
);
#endif

#ifndef REMOVE_LLD_PROGRAM_RESUME_CMD
extern void lld_ProgramResumeCmd
(
FLASHDATA * base_addr,   /* device base address in system */
ADDRESS offset       /* address offset from base address */
);
#endif

#ifndef REMOVE_LLD_CFI_ENTRY_CMD
extern void lld_CfiEntryCmd
(
FLASHDATA * base_addr    /* device base address in system */
);
#endif

#ifndef REMOVE_LLD_CFI_EXIT_CMD
extern void lld_CfiExitCmd
(
FLASHDATA * base_addr    /* device base address in system */
);
#endif

/* Utility Functions */
#ifndef REMOVE_LLD_STATUS_GET
extern DEVSTATUS lld_StatusGet
(
FLASHDATA * base_addr,      /* device base address in system */
ADDRESS     offset          /* address offset from base address */
);
#endif

#ifndef REMOVE_LLD_POLL
extern DEVSTATUS lld_Poll
(
FLASHDATA * base_addr,          /* device base address in system */
ADDRESS offset,             /* address offset from base address */
FLASHDATA *exp_data_ptr,    /* expect data */
FLASHDATA *act_data_ptr,    /* actual data */
POLLING_TYPE polling_type   /* type of polling to perform */
);
#endif

#ifndef REMOVE_LLD_GET_DEVICE_ID
extern unsigned int lld_GetDeviceId
(
FLASHDATA * base_addr    /* device base address is system */
);
#endif

#ifndef REMOVE_WRITE_BUFFER_PROGRAMMING
DEVSTATUS lld_memcpy
(
FLASHDATA * base_addr,   /* device base address is system */
ADDRESS offset,      /* address offset from base address */
WORDCOUNT words_cnt, /* number of words to program */
FLASHDATA *data_buf  /* buffer containing data to program */
);
#endif

#ifndef REMOVE_LLD_READ_CFI_WORD
FLASHDATA lld_ReadCfiWord
(
FLASHDATA * base_addr,   /* device base address is system */
ADDRESS offset       /* address offset from base address */
);
#endif

/* WARNING - Make sure the macro DELAY_1Us (lld_target_specific.h) */
/* is defined appropriately for your system !!                     */
/* If you decide to use your own delay functions, change the       */
/* macros DELAY_MS and DELAY_US in lld_target_specific.h.          */
#ifndef REMOVE_DELAY_MILLISECONDS
extern void DelayMilliseconds
 (
 int milliseconds
 );
#endif

#ifndef REMOVE_DELAY_MICROSECONDS
extern void DelayMicroseconds
 (
 int microseconds
 );
#endif

/*****************************************************************************/
#ifdef TRACE
extern void FlashWrite(FLASHDATA * addr, ADDRESS offset, FLASHDATA data);
extern FLASHDATA FlashRead(FLASHDATA * addr, ADDRESS offset);
#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __INC_lldh */


