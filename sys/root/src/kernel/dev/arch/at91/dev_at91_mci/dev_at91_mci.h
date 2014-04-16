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


/*============================================
| Compiler Directive
==============================================*/
#ifndef _DEV_AT91_MCI_H
#define _DEV_AT91_MCI_H
/*============================================
| Includes
==============================================*/
#include "kernel/core/core_rttimer.h"

/*============================================
| Declaration
==============================================*/
enum MCI_offset_register {
      MCI_CR      = 0x0000,
      MCI_MR      = MCI_CR + 0x0001, //0x0004
      MCI_DTOR    = MCI_CR + 0x0002, //0x0008
      MCI_SDCR    = MCI_CR + 0x0003, //0x000c
      MCI_ARGR    = MCI_CR + 0x0004, //0x0010
      MCI_CMDR    = MCI_CR + 0x0005, //0x0014,
      MCI_RSV1    = MCI_CR + 0x0006, //0x0018,
      MCI_RSV2    = MCI_CR + 0x0007, //0x001c,
      MCI_RSPR1   = MCI_CR + 0x0008, //0x0020,
      MCI_RSPR2   = MCI_CR + 0x0009, //0x0024,
      MCI_RSPR3   = MCI_CR + 0x000a, //0x0028,
      MCI_RSPR4   = MCI_CR + 0x000b, //0x002c,
      MCI_RDR     = MCI_CR + 0x000c, //0x0030,
      MCI_TDR     = MCI_CR + 0x000d, //0x0034,
      MCI_RSV3    = MCI_CR + 0x000e, //0x0038,
      MCI_RSV4    = MCI_CR + 0x000f, //0x003c,
      MCI_SR      = MCI_CR + 0x0010, //0x0040,
      MCI_IER     = MCI_CR + 0x0011, //0x0044,
      MCI_IDR     = MCI_CR + 0x0012, //0x0048,
      MCI_IMR     = MCI_CR + 0x0013, //0x004c,
};

enum MCI_PDC_offset_register {
      MCI_PDC_RPR       = 0x0040,
      MCI_PDC_RCR       = MCI_PDC_RPR + 0x0001, //0x0104
      MCI_PDC_TPR       = MCI_PDC_RPR + 0x0002, //0x0108
      MCI_PDC_TCR       = MCI_PDC_RPR + 0x0003, //0x010c
      MCI_PDC_RNPR      = MCI_PDC_RPR + 0x0004, //0x0110
      MCI_PDC_RNCR      = MCI_PDC_RPR + 0x0005, //0x0114
      MCI_PDC_TNPR      = MCI_PDC_RPR + 0x0006, //0x0118
      MCI_PDC_TNCR      = MCI_PDC_RPR + 0x0007, //0x011c
      MCI_PDC_PTCR      = MCI_PDC_RPR + 0x0008, //0x0120
      MCI_PDC_PTSR      = MCI_PDC_RPR + 0x0009, //0x0124
};

//
//merge it with USBDP_READ_* and USBDP_WRITE_*
//from install/cyg/hal/hal_io.h
#define MCI_READ_UINT8( __register__, __value__ ) \
      ((__value__) = *((volatile unsigned char *)(__register__)))

#define MCI_WRITE_UINT8( __register__, __value__ ) \
      (*((volatile unsigned char *)(__register__)) = (__value__))

#define MCI_READ_UINT32( __register__, __value__ ) \
      ((__value__) = *((volatile unsigned int *)(__register__)))

#define MCI_WRITE_UINT32( __register__, __value__ ) \
      (*((volatile unsigned int *)(__register__)) = (__value__))

//registers value
//CR
#define AT91_MCI_CR_MCIEN        ((unsigned int)0x1 <<  0) // (MCI) Multimedia Interface Enable
#define AT91_MCI_CR_MCIDIS       ((unsigned int)0x1 <<  1) // (MCI) Multimedia Interface Disable
#define AT91_MCI_CR_PWSEN        ((unsigned int)0x1 <<  2) // (MCI) Power Save Mode Enable
#define AT91_MCI_CR_PWSDIS       ((unsigned int)0x1 <<  3) // (MCI) Power Save Mode Disable
#define AT91_MCI_CR_SWRST        ((unsigned int)0x1 <<  7) // (MCI) MCI Software reset

//MR
#define AT91_MCI_MR_CLKDIV_VAL(__value__)    (((unsigned int) (__value__)) <<  0) // (MCI) Clock Divider
#define AT91_MCI_MR_PWSDIV_VAL(__value__)    (((unsigned int) (__value__)) <<  8) // (MCI) Power Saving Divider
#define AT91_MCI_MR_PDCPADV                  ((unsigned int)0x1 << 14) // (MCI) PDC Padding Value
#define AT91_MCI_MR_PDCMODE                  ((unsigned int)0x1 << 15) // (MCI) PDC Oriented Mode
#define AT91_MCI_MR_BLKLEN_VAL(__value__)    (((unsigned int) (__value__)) << 18) // (MCI) Data Block Length

//DTOR
#define AT91_MCI_DTOR_DTOMUL_1         ((unsigned int)0x0 <<  4) // (MCI) DTOCYC x 1
#define AT91_MCI_DTOR_DTOMUL_16        ((unsigned int)0x1 <<  4) // (MCI) DTOCYC x 16
#define AT91_MCI_DTOR_DTOMUL_128       ((unsigned int)0x2 <<  4) // (MCI) DTOCYC x 128
#define AT91_MCI_DTOR_DTOMUL_256       ((unsigned int)0x3 <<  4) // (MCI) DTOCYC x 256
#define AT91_MCI_DTOR_DTOMUL_1024      ((unsigned int)0x4 <<  4) // (MCI) DTOCYC x 1024
#define AT91_MCI_DTOR_DTOMUL_4096      ((unsigned int)0x5 <<  4) // (MCI) DTOCYC x 4096
#define AT91_MCI_DTOR_DTOMUL_65536     ((unsigned int)0x6 <<  4) // (MCI) DTOCYC x 65536
#define AT91_MCI_DTOR_DTOMUL_1048576   ((unsigned int)0x7 <<  4) // (MCI) DTOCYC x 1048576

#define AT91_MCI_DTOR_DTOCYC_VAL(__value__)    ((unsigned int)(__value__)) // (MCI) Data Timeout Cycle Number

//SDCR
#define AT91_MCI_SDCR_1_BIT_WIDTH      (0) // (MCI) 1-bit data bus
#define AT91_MCI_SDCR_4_BITS_WIDTH     ((unsigned int)0x1 <<  7) // (MCI) 4-bits data bus

//CMDR
#define AT91_MCI_CMDR_CMDNB_VAL(__value__)      (__value__)
#define AT91_MCI_CMDR_RSPTYP_VAL(__value__)     ((unsigned int)(__value__) <<  6)
#define AT91_MCI_CMDR_SPCMD_VAL(__value__)      ((unsigned int)(__value__) <<  8)
#define AT91_MCI_CMDR_OPDCMD                    ((unsigned int)0x1 <<  11)
#define AT91_MCI_CMDR_MAXLAT                    ((unsigned int)0x1 <<  12)
#define AT91_MCI_CMDR_TRCMD_VAL(__value__)      ((unsigned int)(__value__) <<  16)
#define AT91_MCI_CMDR_TRDIR                     ((unsigned int)0x1 <<  18)
#define AT91_MCI_CMDR_TRTYP_VAL(__value__)      ((unsigned int)(__value__) <<  19)

//SR
#define AT91_MCI_SR_CMDRDY             ((unsigned int)0x1 <<  0)
#define AT91_MCI_SR_RXRDY              ((unsigned int)0x1 <<  1)
#define AT91_MCI_SR_TXRDY              ((unsigned int)0x1 <<  2)
#define AT91_MCI_SR_BLKE               ((unsigned int)0x1 <<  3)
#define AT91_MCI_SR_DTIP               ((unsigned int)0x1 <<  4)
#define AT91_MCI_SR_NOTBUSY            ((unsigned int)0x1 <<  5)
#define AT91_MCI_SR_ENDRX              ((unsigned int)0x1 <<  6)
#define AT91_MCI_SR_ENDTX              ((unsigned int)0x1 <<  7)
#define AT91_MCI_SR_RXBUFF             ((unsigned int)0x1 <<  14)
#define AT91_MCI_SR_TXBUFE             ((unsigned int)0x1 <<  15)
#define AT91_MCI_SR_RINDE              ((unsigned int)0x1 <<  16)
#define AT91_MCI_SR_RDIRE              ((unsigned int)0x1 <<  17)
#define AT91_MCI_SR_RCRCE              ((unsigned int)0x1 <<  18)
#define AT91_MCI_SR_RENDE              ((unsigned int)0x1 <<  19)
#define AT91_MCI_SR_RTOE               ((unsigned int)0x1 <<  20)
#define AT91_MCI_SR_DCRCE              ((unsigned int)0x1 <<  21)
#define AT91_MCI_SR_DTOE               ((unsigned int)0x1 <<  22)
#define AT91_MCI_SR_OVRE               ((unsigned int)0x1 <<  30)
#define AT91_MCI_SR_UNRE               ((unsigned int)0x1 <<  31)

//IER
#define AT91_MCI_IER_CMDRDY            AT91_MCI_SR_CMDRDY
#define AT91_MCI_IER_RXRDY             AT91_MCI_SR_RXRDY
#define AT91_MCI_IER_TXRDY             AT91_MCI_SR_TXRDY
#define AT91_MCI_IER_BLKE              AT91_MCI_SR_BLKE
#define AT91_MCI_IER_DTIP              AT91_MCI_SR_DTIP
#define AT91_MCI_IER_NOTBUSY           AT91_MCI_SR_NOTBUSY
#define AT91_MCI_IER_ENDRX             AT91_MCI_SR_ENDRX
#define AT91_MCI_IER_ENDTX             AT91_MCI_SR_ENDTX
#define AT91_MCI_IER_RXBUFF            AT91_MCI_SR_RXBUFF
#define AT91_MCI_IER_TXBUFE            AT91_MCI_SR_TXBUFE
#define AT91_MCI_IER_RINDE             AT91_MCI_SR_RINDE
#define AT91_MCI_IER_RDIRE             AT91_MCI_SR_RDIRE
#define AT91_MCI_IER_RCRCE             AT91_MCI_SR_RCRCE
#define AT91_MCI_IER_RENDE             AT91_MCI_SR_RENDE
#define AT91_MCI_IER_RTOE              AT91_MCI_SR_RTOE
#define AT91_MCI_IER_DCRCE             AT91_MCI_SR_DCRCE
#define AT91_MCI_IER_DTOE              AT91_MCI_SR_DTOE
#define AT91_MCI_IER_OVRE              AT91_MCI_SR_OVRE
#define AT91_MCI_IER_UNRE              AT91_MCI_SR_UNRE

//IDR
#define AT91_MCI_IDR_CMDRDY            AT91_MCI_SR_CMDRDY
#define AT91_MCI_IDR_RXRDY             AT91_MCI_SR_RXRDY
#define AT91_MCI_IDR_TXRDY             AT91_MCI_SR_TXRDY
#define AT91_MCI_IDR_BLKE              AT91_MCI_SR_BLKE
#define AT91_MCI_IDR_DTIP              AT91_MCI_SR_DTIP
#define AT91_MCI_IDR_NOTBUSY           AT91_MCI_SR_NOTBUSY
#define AT91_MCI_IDR_ENDRX             AT91_MCI_SR_ENDRX
#define AT91_MCI_IDR_ENDTX             AT91_MCI_SR_ENDTX
#define AT91_MCI_IDR_RXBUFF            AT91_MCI_SR_RXBUFF
#define AT91_MCI_IDR_TXBUFE            AT91_MCI_SR_TXBUFE
#define AT91_MCI_IDR_RINDE             AT91_MCI_SR_RINDE
#define AT91_MCI_IDR_RDIRE             AT91_MCI_SR_RDIRE
#define AT91_MCI_IDR_RCRCE             AT91_MCI_SR_RCRCE
#define AT91_MCI_IDR_RENDE             AT91_MCI_SR_RENDE
#define AT91_MCI_IDR_RTOE              AT91_MCI_SR_RTOE
#define AT91_MCI_IDR_DCRCE             AT91_MCI_SR_DCRCE
#define AT91_MCI_IDR_DTOE              AT91_MCI_SR_DTOE
#define AT91_MCI_IDR_OVRE              AT91_MCI_SR_OVRE
#define AT91_MCI_IDR_UNRE              AT91_MCI_SR_UNRE

//IMR
#define AT91_MCI_IMR_CMDRDY            AT91_MCI_SR_CMDRDY
#define AT91_MCI_IMR_RXRDY             AT91_MCI_SR_RXRDY
#define AT91_MCI_IMR_TXRDY             AT91_MCI_SR_TXRDY
#define AT91_MCI_IMR_BLKE              AT91_MCI_SR_BLKE
#define AT91_MCI_IMR_DTIP              AT91_MCI_SR_DTIP
#define AT91_MCI_IMR_NOTBUSY           AT91_MCI_SR_NOTBUSY
#define AT91_MCI_IMR_ENDRX             AT91_MCI_SR_ENDRX
#define AT91_MCI_IMR_ENDTX             AT91_MCI_SR_ENDTX
#define AT91_MCI_IMR_RXBUFF            AT91_MCI_SR_RXBUFF
#define AT91_MCI_IMR_TXBUFE            AT91_MCI_SR_TXBUFE
#define AT91_MCI_IMR_RINDE             AT91_MCI_SR_RINDE
#define AT91_MCI_IMR_RDIRE             AT91_MCI_SR_RDIRE
#define AT91_MCI_IMR_RCRCE             AT91_MCI_SR_RCRCE
#define AT91_MCI_IMR_RENDE             AT91_MCI_SR_RENDE
#define AT91_MCI_IMR_RTOE              AT91_MCI_SR_RTOE
#define AT91_MCI_IMR_DCRCE             AT91_MCI_SR_DCRCE
#define AT91_MCI_IMR_DTOE              AT91_MCI_SR_DTOE
#define AT91_MCI_IMR_OVRE              AT91_MCI_SR_OVRE
#define AT91_MCI_IMR_UNRE              AT91_MCI_SR_UNRE

#define AT91_MCI_ALL_ERRORS            (AT91_MCI_IMR_UNRE | AT91_MCI_IMR_OVRE \
                                       | AT91_MCI_IMR_DTOE | AT91_MCI_IMR_DCRCE | AT91_MCI_IMR_RTOE \
                                       | AT91_MCI_IMR_RENDE | AT91_MCI_IMR_RCRCE | AT91_MCI_IMR_RDIRE \
                                       | AT91_MCI_IMR_RINDE)

//PDC PTCR
#define AT91_MCI_PDC_PTCR_RXTEN        ((unsigned int) 0x1 <<  0) // (PDC) Receiver Transfer Enable
#define AT91_MCI_PDC_PTCR_RXTDIS       ((unsigned int) 0x1 <<  1) // (PDC) Receiver Transfer Disable
#define AT91_MCI_PDC_PTCR_TXTEN        ((unsigned int) 0x1 <<  8) // (PDC) Transmitter Transfer Enable
#define AT91_MCI_PDC_PTCR_TXTDIS       ((unsigned int) 0x1 <<  9) // (PDC) Transmitter Transfer Disable

//
#define UNSUPPORTED_COMMAND            -1
//
/// Transfer is pending.
#define MCI_STATUS_PENDING       1
/// Transfer has been aborted because an error occured.
#define MCI_STATUS_ERROR         2
/// Card did not answer command.
#define MCI_STATUS_NORESPONSE    3

/// MCI driver is currently in use.
#define MCI_ERROR_LOCK           1

/// Start new data transfer
#define MCI_NEW_TRANSFER         0
/// Continue data transfer
#define MCI_CONTINUE_TRANSFER    1

/// MCI SD Bus Width 1-bit
#define MCI_SDCBUS_1BIT          (0 << 7)
/// MCI SD Bus Width 4-bit
#define MCI_SDCBUS_4BIT          (1 << 7)

//------------------------------------------------------------------------------
/// MCI Transfer Request prepared by the application upper layer. This structure
/// is sent to the MCI_SendCommand function to start the transfer. At the end of
/// the transfer, the callback is invoked by the interrupt handler.
//------------------------------------------------------------------------------
typedef struct mci_cmd_st {
   /// Command status.
   volatile char status;
   /// Command code.
   unsigned int cmd;
   /// Command argument.
   unsigned int arg;
   /// Data buffer.
   unsigned char *p_data;
   /// Size of data buffer in bytes.
   unsigned short block_size;
   /// Number of blocks to be transfered
   unsigned short block_count;
   /// Indicates if the command is a read operation.
   unsigned char is_read;
   /// Response buffer.
   unsigned int  *p_resp;
   /// SD card response type.
   unsigned char  resp_type;
} mci_cmd_t;


typedef struct board_inf_mci_st {
   volatile unsigned int * base_addr;

   int irq_no;
   int irq_prio;

   /// Pointer to currently executing command.
   mci_cmd_t * command;

   //
   int desc_wr;
   int desc_rd;

   //
   unsigned char post_event;

   //
   tmr_t timer;
   rttmr_attr_t timer_attr;

   unsigned char prev_command;
} board_inf_mci_t;

#endif //DEV_AT91_MCI_H
