/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Chauvin-Arnoux.
Portions created by Chauvin-Arnoux are Copyright (C) 2011. All Rights Reserved.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under
the MPL, indicate your decision by deleting  the provisions above and replace
them with the notice and other provisions required by the [eCos GPL] License.
If you do not delete the provisions above, a recipient may use your version of this file under
either the MPL or the [eCos GPL] License."
*/

/* Define how to access your hardware.                        */
/* Describe to the LLD the flash chip configuration */
/* un-comment the one that matches your system      */

/* no-interleaving, a single x8 device                  */
/* #define LLD_CONFIGURATION X8_AS_X8 */

/* no-interleaving, a single x8/x16 device in x8 mode   */
/* #define LLD_CONFIGURATION X8X16_AS_X8 */

/* two x8 devices interleaved to form x16               */
/* #define LLD_CONFIGURATION X8_AS_X16 */

/* no-interleaving, a single x16 device in x16 mode     */
#define LLD_CONFIGURATION X16_AS_X16

/* no-interleaving, a single x8/x16 device in x16 mode  */
/*#define LLD_CONFIGURATION X8X16_AS_X16*/

/* no-interleaving, a single x16/x32 devices in x16 mode*/
/* #define LLD_CONFIGURATION X16X32_AS_X16 */

/* four x8 devices interleaved to form x32              */
/* #define LLD_CONFIGURATION X8_AS_X32 */

/* two x16 devices interleaved to form x32              */
//#define LLD_CONFIGURATION X16_AS_X32

/* two xx/x16 devices interleaved to for x32            */
/* #define LLD_CONFIGURATION X8X16_AS_X32 */

/* no-interleaving, a single x16/x32 device in x32 mode */
/* #define LLD_CONFIGURATION X16X32_AS_X32 */


/* If your part has advanced sector protection, uncomment     */
/* the following macro.                                       */
//phlb modif
//#define INCLUDE_ADVANCED_SECTOR_PROTECTION
#define ASP "lld_wsxxx_asp.c"

/* #define TRACE                     */
/* determines whether or not your system supports timestamp in the trace */
/* can also be used to turn off printing of the timestamp in the trace   */
/* #define PRINT_TIMESTAMP	*/

/* enables code to execute commands from a file instead of directly from */
/* the command line. */
#define ENABLE_SCRIPTING_MACRO

/* Displays file name and line number information when an LLD_ASSERT call */
/* fails.  This information is displayed in LLDCmdlineAssert().           */
/* #define ASSERT_DIAGNOSTICS */

#define FLASH_OFFSET(b,o)       (*(( (volatile FLASHDATA*)(b) ) + (o)))

#ifdef TRACE
/* This is a good place to add instrumentation.               */
   #define FLASH_WR(b,o,d)         FlashWrite(b, o, d)
   #define FLASH_RD(b,o)           FlashRead(b, o)

#else

   #define FLASH_WR(b,o,d)         FLASH_OFFSET((b),(o)) = (d)
   #define FLASH_RD(b,o)           FLASH_OFFSET((b),(o))

#endif


/* If you are worried about code size, you can      */
/* remove an LLD function by un-commenting the      */
/* appropriate macro below.                         */
/*                                                  */
/*                                                  */
/*  #define REMOVE_CHECK_WRITE_BUFFER_STATUS        */
/*  #define REMOVE_CHECK_TIMEOUT_STATUS             */
/*  #define REMOVE_CHECK_TOGGLE_BIT_STATUS          */
/*  #define REMOVE_ACTIVE_MASK                      */
/*  #define REMOVE_LLD_SECTOR_ERASE_CMD             */
/*  #define REMOVE_LLD_CHIP_ERASE_CMD               */
/*  #define REMOVE_LLD_PROGRAM_CMD                  */
/*  #define REMOVE_LLD_UNLOCK_BYPASS_ENTRY_CMD      */
/*  #define REMOVE_LLD_UNLOCK_BYPASS_PROGRAM_CMD    */
/*  #define REMOVE_LLD_UNLOCK_BYPASS_RESET_CMD      */
/*  #define REMOVE_LLD_AUTOSELECT_ENTRY_CMD         */
/*  #define REMOVE_LLD_AUTOSELECT_EXIT_CMD          */
/*  #define REMOVE_LLD_SECSI_SECTOR_ENTRY_CMD       */
/*  #define REMOVE_LLD_SECTI_SECTOR_EXIT_CMD        */
/*  #define REMOVE_WRITE_BUFFER_PROGRAMMING         */
/*  #define REMOVE_LLD_PROGRAM_SUSPEND_CMD          */
/*  #define REMOVE_LLD_ERASE_SUSPEND_CMD            */
/*  #define REMOVE_LLD_ERASE_RESUME_CMD             */
/*  #define REMOVE_LLD_PROGRAM_RESUME_CMD           */
/*  #define REMOVE_LLD_CFI_ENTRY_CMD                */
/*  #define REMOVE_LLD_CFI_EXIT_CMD                 */
/*  #define REMOVE_LLD_POLL                         */
/*  #define REMOVE_LLD_STATUS_GET                   */
/*  #define REMOVE_LLD_PROGRAM_OP                   */
/*  #define REMOVE_LLD_SECTOR_ERASE_OP              */
/*  #define REMOVE_LLD_CHIP_ERASE_OP                */
/*  #define REMOVE_DELAY_MILLISECONDS               */
/*  #define REMOVE_DELAY_MICROSECONDS               */
/*  #define REMOVE_LLD_READ_OP                      */
/*  #define REMOVE_LLD_GET_DEVICE_ID                */
/*  #define REMOVE_LLD_READ_CFI_WORD                */

/* Some of the functions require minimal delays.        */
/* You need to provide info relative to your system     */
#ifndef REMOVE_DELAY_MILLISECONDS
   #define DELAY_MS(milliseconds) DelayMilliseconds(milliseconds)
#endif
#ifndef REMOVE_DELAY_MICROSECONDS
   #define DELAY_US(microseconds) DelayMicroseconds(microseconds)
#endif
/* Tell the LLD how many for loops of i=i it takes to burn    */
/* up 1 microsecond of time.                                  */
#define DELAY_1us 38 //150 38:ARM7 at 32 MHz

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

If you are using MirrorBit devices, change the macro
#undef MIRRORBIT_DEVICE to #define MIRRORBIT_DEVICE.

For more information, visit our web site at www.spansion.com,
email us at software@spansion.com or go to www.ask.amd.com.
*/
#define MIRRORBIT_DEVICE

#ifdef MIRRORBIT_DEVICE
   #define WAIT_4us_FOR_DATA_POLLING_BITS_TO_BECOME_ACTIVE
   #undef  PAUSE_BETWEEN_ERASE_SUSPENDS
#endif


