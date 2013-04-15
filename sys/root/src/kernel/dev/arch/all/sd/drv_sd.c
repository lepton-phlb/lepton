/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Philippe Le Boulanger.
Portions created by Philippe Le Boulanger are Copyright (C) 2011 <lepton.phlb@gmail.com>.
All Rights Reserved.

Contributor(s): Jean-Jacques Pitrolle <lepton.jjp@gmail.com>.

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
| Includes
==============================================*/
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/ioctl_hd.h"

#include "kernel/fs/vfs/vfsdev.h"

#include "lib/libc/ctype/ctype.h"
#include "kernel/core/types.h"
#include "kernel/core/time.h"
#include "kernel/core/devio.h"
#include "kernel/core/errno.h"
#include "kernel/core/libstd.h"

#include "dev_sd.h"
/*============================================
| Global Declaration
==============================================*/
static int _sd_low_level_init(board_inf_sd_t * p_inf_sd, desc_t desc_next);
static int _sd_send_command(board_inf_sd_t * p_inf_sd, desc_t desc_next);
static int _sd_pon(board_inf_sd_t * p_inf_sd, desc_t desc_next);
static int _sd_cmd0(board_inf_sd_t * p_inf_sd, desc_t desc_next);
static int _sd_cmd1(board_inf_sd_t * p_inf_sd, desc_t desc_next);
static int _sd_cmd2(board_inf_sd_t * p_inf_sd, desc_t desc_next, unsigned int *pCid);
static int _sd_cmd3(board_inf_sd_t * p_inf_sd, desc_t desc_next);
static int _sd_cmd7(board_inf_sd_t * p_inf_sd, desc_t desc_next, unsigned int address);
static int _sd_cmd8(board_inf_sd_t * p_inf_sd, desc_t desc_next, unsigned char supplyVoltage);
static int _sd_cmd9(board_inf_sd_t * p_inf_sd, desc_t desc_next);
static int _sd_cmd12(board_inf_sd_t * p_inf_sd, desc_t desc_next);
static int _sd_cmd13(board_inf_sd_t * p_inf_sd, desc_t desc_next, unsigned int *pStatus);
static int _sd_cmd16(board_inf_sd_t * p_inf_sd, desc_t desc_next, unsigned short blockLength);
static int _sd_cmd17(board_inf_sd_t * p_inf_sd, desc_t desc_next, unsigned char *pData,
                     unsigned int address);
static int _sd_cmd18(board_inf_sd_t * p_inf_sd, desc_t desc_next, unsigned short nbBlock,
                     unsigned char *pData,
                     unsigned int address);
static int _sd_cmd24(board_inf_sd_t * p_inf_sd, desc_t desc_next, unsigned char *pData,
                     unsigned int address);
static int _sd_cmd25(board_inf_sd_t * p_inf_sd, desc_t desc_next, unsigned short nbBlock,
                     unsigned char *pData,
                     unsigned int address);
static int _sd_cmd55(board_inf_sd_t * p_inf_sd, desc_t desc_next);
static int _sd_acmd6(board_inf_sd_t * p_inf_sd, desc_t desc_next, unsigned char busWidth);
static int _sd_acmd41(board_inf_sd_t * p_inf_sd, desc_t desc_next, unsigned char hcs,
                      unsigned char *pCCS);

//
static unsigned char g_sd_buf_rcv[SD_BUFFER_SIZE_NO_CACHE_RCV+SD_BLOCK_SIZE]
#if defined(CPU_ARM9)
__attribute__ ((section (".no_cache")))
#else
__attribute((aligned(4)))
#endif
;

static unsigned char g_sd_buf_snd[SD_BUFFER_SIZE_NO_CACHE_SND+SD_BLOCK_SIZE]
#if defined(CPU_ARM9)
__attribute__ ((section (".no_cache")))
#else
__attribute((aligned(4)))
#endif
;

//
#define  SD_RETRIES_CMD  5 //3

//
#define SD_ERROR_TAB_SIZE  16
typedef struct sd_err_st {
   unsigned short retries_left;
   unsigned short cmd;
   unsigned int error;
   unsigned int status;
} sd_err_t;

static sd_err_t _sd_error_tab[SD_ERROR_TAB_SIZE];
static unsigned short _sd_error_cb;

#define __sd_rec_err(__cmd_no__, __retries_left__, __error__, __status__) { \
      _sd_error_tab[_sd_error_cb].retries_left = __retries_left__; \
      _sd_error_tab[_sd_error_cb].cmd = __cmd_no__; \
      _sd_error_tab[_sd_error_cb].error = __error__; \
      _sd_error_tab[_sd_error_cb].status = __status__; \
      _sd_error_cb = (_sd_error_cb + 1) & (SD_ERROR_TAB_SIZE -1); \
}

typedef struct csd_info_st {
   unsigned int taac;
   unsigned int nsac;
   unsigned int read_bl_len;
   unsigned int trans_speed;
   unsigned int write_bl_len;
} csd_info_t;

static csd_info_t g_csd_info;

/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name:_sd_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static unsigned int _sd_status_cmd13_wr;
int _sd_write(int desc, const char* buf, int cb) {
   board_inf_sd_t * p_inf_sd = (board_inf_sd_t *)ofile_lst[desc].p;
   unsigned int status;
   unsigned char error;

   //calcul plus propre
   unsigned int nb_blocks_to_write = (cb/SD_BLOCK_SIZE);
   unsigned int nb_blocks_to_read = 0;
   unsigned int block_addr = ofile_lst[desc].offset/SD_BLOCK_SIZE;
   unsigned int block_offset = ofile_lst[desc].offset%SD_BLOCK_SIZE;
   unsigned char * ptr_data = NULL;
   short retries=SD_RETRIES_CMD;

   //
   if(!p_inf_sd)
      return -1;

   //
   if(!nb_blocks_to_write) {
      nb_blocks_to_write = 1;
   }

   nb_blocks_to_read = nb_blocks_to_write;

   if(block_offset) {
      if((block_offset + cb) > SD_BLOCK_SIZE) {
         nb_blocks_to_write++;
      }
   }

   //if we write entire blocks, reading is useless
   if((cb%SD_BLOCK_SIZE)) {
      if(cb > SD_BLOCK_SIZE) {
         nb_blocks_to_write++;
      }

      //modify data
      if(_sd_read(desc, g_sd_buf_snd, nb_blocks_to_read*SD_BLOCK_SIZE, 1) < 0) {
         return -1;
      }
   }
   else {
      if(block_offset) {
         //modify data
         if(_sd_read(desc, g_sd_buf_snd, nb_blocks_to_read*SD_BLOCK_SIZE, 1) < 0) {
            return -1;
         }
      }
   }

   memcpy((void *)(g_sd_buf_snd+block_offset), buf, cb);

   while(retries--) {
      do {
         error = _sd_cmd13(p_inf_sd, ofile_lst[desc].desc_nxt[0], &_sd_status_cmd13_wr);
         if (error) {
            return -1;
         }
         status = _sd_status_cmd13_wr;
      } while((status & SD_WRITE_MASK) != (SD_STATUS_READY_FOR_DATA|SD_STATUS_TRAN));

      if(nb_blocks_to_write == 1) {
         //write single block
         error = _sd_cmd24(p_inf_sd, ofile_lst[desc].desc_nxt[0], (unsigned char *)g_sd_buf_snd,
                           SD_ADDRESS(p_inf_sd,block_addr));
         if(error) {
            __sd_rec_err(24, retries, error, 0);
            continue; //return -1;
         }
         else {
            break;
         }
      }
      else {
         //write multiple block
         error =
            _sd_cmd25(p_inf_sd, ofile_lst[desc].desc_nxt[0], nb_blocks_to_write,
                      (unsigned char *)g_sd_buf_snd,
                      SD_ADDRESS(p_inf_sd,
                                 block_addr));
         if(error) {
            __sd_rec_err(25, retries, error, 0);
            continue; //return -1;
         }
         else {
            break;
         }
      }
   }

   return error; //0;
}

/*-------------------------------------------
| Name:_sd_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static unsigned int _sd_status_cmd13_rd;
int _sd_read(int desc, char* buf, int cb, unsigned char no_off) {
   board_inf_sd_t * p_inf_sd = (board_inf_sd_t *)ofile_lst[desc].p;
   unsigned int status = 0;
   unsigned char error;

   //
   unsigned int nb_blocks = (cb/SD_BLOCK_SIZE);
   unsigned int block_addr = ofile_lst[desc].offset/SD_BLOCK_SIZE;
   unsigned int block_offset = ofile_lst[desc].offset%SD_BLOCK_SIZE;
   unsigned char * ptr_data = NULL;
   short retries=SD_RETRIES_CMD;

   if(!p_inf_sd)
      return -1;

   if(!nb_blocks) {
      nb_blocks = 1;
   }

   if(block_offset) {
      if((block_offset + cb) > SD_BLOCK_SIZE) {
         nb_blocks++;
      }
   }

   if((cb%SD_BLOCK_SIZE)) {
      if(cb > SD_BLOCK_SIZE) {
         nb_blocks++;
      }
   }

   //
   if(no_off) {
      block_offset = 0;
   }

   while(retries--) {
      //
      do {
         error = _sd_cmd13(p_inf_sd, ofile_lst[desc].desc_nxt[0], &_sd_status_cmd13_rd /*&status*/);
         if (error) {
            return -1;
         }
         //
         status = _sd_status_cmd13_rd;
         //
      } while ((status & SD_READ_MASK) != SD_STATUS_TRAN);

      if(nb_blocks == 1) {
         // read single block
         error = _sd_cmd17(p_inf_sd, ofile_lst[desc].desc_nxt[0], (unsigned char *)g_sd_buf_rcv,
                           SD_ADDRESS(p_inf_sd,block_addr));
         if(error) {
            __sd_rec_err(17, retries, error, 0);
            continue; //return -1;
         }
         else {
            break;
         }
      }
      else {
         // read multiple block
         error =
            _sd_cmd18(p_inf_sd, ofile_lst[desc].desc_nxt[0], nb_blocks,
                      (unsigned char *)g_sd_buf_rcv,
                      SD_ADDRESS(p_inf_sd,
                                 block_addr));
         if (error) {
            __sd_rec_err(18, retries, error, 0);
            continue; //return -1;
         }
         else {
            break;
         }
      }
   }

   //copy buffer in a non cache zone
   memcpy((void *)buf, g_sd_buf_rcv+block_offset, cb);

   return error; //0;
}

/*-------------------------------------------
| Name:_sd_init
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sd_init(board_inf_sd_t * p_inf_sd, desc_t desc_next) {
   unsigned char error;

   // Initialize SdCard structure
   p_inf_sd->card_addr = 0;
   p_inf_sd->state = SD_STATE_STBY;
   p_inf_sd->card_type = UNKNOWN_CARD;
   memset((void *)p_inf_sd->command, 0, sizeof(sd_cmd_t));

   // Initialization delay: The maximum of 1 msec, 74 clock cycles and supply ramp up time
   // Supply ramp up time provides the time that the power is built up to the operating level (the bus
   // master supply voltage) and the time to wait until the SD card can accept the first command

#if defined(CPU_ARM9)
   // Power On Init Special Command
   if((error = _sd_pon(p_inf_sd, desc_next))) {
      return error;
   }
#endif

   // After power-on or CMD0, all cards CMD lines are in input mode, waiting for start bit of the next command.
   // The cards are initialized with a default relative card address (RCA=0x0000) and with a default
   // driver stage register setting (lowest speed, highest driving current capability).

   if((error = _sd_low_level_init(p_inf_sd, desc_next))) {
      return -1; //error;
   }

   // In the case of a Standard Capacity SD Memory Card, this command sets the
   // block length (in bytes) for all following block commands (read, write, lock).
   // Default block length is fixed to 512 Bytes.
   // Set length is valid for memory access commands only if partial block read
   // operation are allowed in CSD.
   // In the case of a High Capacity SD Memory Card, block length set by CMD16
   // command does not affect the memory read and write commands. Always 512
   // Bytes fixed block length is used. This command is effective for LOCK_UNLOCK command.
   // In both cases, if block length is set larger than 512Bytes, the card sets the
   // BLOCK_LEN_ERROR bit.
   if (p_inf_sd->card_type == CARD_SD) {
      error = _sd_cmd16(p_inf_sd, desc_next, SD_BLOCK_SIZE);
      if (error) {
         return -1; //error;
      }
   }

   // If SD CSD v2.0
   if((p_inf_sd->card_type != CARD_MMC) && (SD_CSD_STRUCTURE(p_inf_sd) == 1)) {
      p_inf_sd->total_size = SD_CSD_TOTAL_SIZE_HC(p_inf_sd);
      p_inf_sd->block_nb = SD_CSD_BLOCKNR_HC(p_inf_sd);
   }
   else {
      p_inf_sd->total_size = SD_CSD_TOTAL_SIZE(p_inf_sd);
      p_inf_sd->block_nb = SD_CSD_BLOCKNR(p_inf_sd);
   }

   if (p_inf_sd->card_type == UNKNOWN_CARD) {
      return -1; //SD_ERROR_NOT_INITIALIZED;
   }

   //set new speed
   ofile_lst[desc_next].pfsop->fdev.fdev_ioctl(desc_next, HDSD_SETSPEED, p_inf_sd->command);

   //
   g_csd_info.nsac = SD_CSD_NSAC(p_inf_sd);
   g_csd_info.taac = SD_CSD_TAAC(p_inf_sd);
   g_csd_info.read_bl_len = SD_CSD_READ_BL_LEN(p_inf_sd);
   g_csd_info.trans_speed = SD_CSD_TRAN_SPEED(p_inf_sd);
   g_csd_info.write_bl_len = SD_CSD_WRITE_BL_LEN(p_inf_sd);
   return 0;
}

/*-------------------------------------------
| Name:_sd_init
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sd_low_level_init(board_inf_sd_t * p_inf_sd, desc_t desc_next) {
   unsigned int sdCid[4];
   unsigned char isCCSet;
   unsigned char error;
   unsigned int status;
   unsigned char cmd8Retries = 2;
   unsigned char cmd1Retries = 100;

   // The command GO_IDLE_STATE (CMD0) is the software reset command and sets card into Idle State
   // regardless of the current card state.
   error = _sd_cmd0(p_inf_sd, desc_next);
   if (error) {
      return error;
   }

   // CMD8 is newly added in the Physical Layer Specification Version 2.00 to support multiple voltage
   // ranges and used to check whether the card supports supplied voltage. The version 2.00 host shall
   // issue CMD8 and verify voltage before card initialization.
   // The host that does not support CMD8 shall supply high voltage range...
   do {
      error = _sd_cmd8(p_inf_sd, desc_next, 1);
   } while ((error == SD_ERROR_NORESPONSE) && (cmd8Retries-- > 0));

   if (error == SD_ERROR_NORESPONSE) {
      // No response : Ver2.00 or later SD Memory Card(voltage mismatch)
      // or Ver1.X SD Memory Card
      // or not SD Memory Card

      // ACMD41 is a synchronization command used to negotiate the operation voltage range and to poll the
      // cards until they are out of their power-up sequence.
      error = _sd_acmd41(p_inf_sd, desc_next, 0, &isCCSet);
      if (error) {
         // Acmd41 failed : MMC card or unknown card
         error = _sd_cmd0(p_inf_sd, desc_next);
         if (error) {
            return error;
         }

         do {
            error = _sd_cmd1(p_inf_sd, desc_next);
         } while ((error) && (cmd1Retries-- > 0));

         if (error) {
            return error;
         }
         else {
            p_inf_sd->card_type = CARD_MMC;
         }
      }
      else {
         if(isCCSet == 0) {
            p_inf_sd->card_type = CARD_SD;
         }
      }
   }
   else if (!error) {

      // Valid response : Ver2.00 or later SD Memory Card
      error = _sd_acmd41(p_inf_sd, desc_next, 1, &isCCSet);
      if (error) {
         return error;
      }
      if (isCCSet) {
         p_inf_sd->card_type = CARD_SDHC;
      }
      else {
         p_inf_sd->card_type = CARD_SD;
      }
   }
   else {
      return error;
   }

   // The host then issues the command ALL_SEND_CID (CMD2) to the card to get its unique card identification (CID) number.
   // Card that is unidentified (i.e. which is in Ready State) sends its CID number as the response (on the CMD line).
   error = _sd_cmd2(p_inf_sd, desc_next, sdCid);
   if (error) {
      return error;
   }

   // Thereafter, the host issues CMD3 (SEND_RELATIVE_ADDR) asks the
   // card to publish a new relative card address (RCA), which is shorter than CID and which is used to
   // address the card in the future data transfer mode. Once the RCA is received the card state changes to
   // the Stand-by State. At this point, if the host wants to assign another RCA number, it can ask the card to
   // publish a new number by sending another CMD3 command to the card. The last published RCA is the
   // actual RCA number of the card.
   error = _sd_cmd3(p_inf_sd, desc_next);
   if (error) {
      return error;
   }

   // The host issues SEND_CSD (CMD9) to obtain the Card Specific Data (CSD register),
   // e.g. block length, card storage capacity, etc...
   error = _sd_cmd9(p_inf_sd, desc_next);
   if (error) {
      return error;
   }

   // At this stage the Initialization and identification process is achieved
   // The SD card is supposed to be in Stand-by State
   do {
      error = _sd_cmd13(p_inf_sd, desc_next, &status);
      if (error) {
         return error;
      }
   } while ((status & SD_STATUS_READY_FOR_DATA) == 0);

   // If the 4 bit bus transfer is supported switch to this mode
   // Select the current SD, goto transfer state
   error = _sd_cmd7(p_inf_sd, desc_next, p_inf_sd->card_addr);
   if(error) {
      return error;
   }

   if (p_inf_sd->card_type != CARD_MMC) {
      // Switch to 4 bits bus width (All SD Card shall support 1-bit, 4 bitswidth
      error = _sd_acmd6(p_inf_sd, desc_next, 4);
      if (error) {
         return error;
      }
   }
   else {
      //MCI_SetBusWidth((Mci *)pSdDriver, MCI_SDCBUS_1BIT);
   }
   return 0;
}

/*-------------------------------------------
| Name:_sd_send_command
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sd_send_command(board_inf_sd_t * p_inf_sd, desc_t desc_next) {
   unsigned char error;
   unsigned int i;

   // Send command
   error = ofile_lst[desc_next].pfsop->fdev.fdev_ioctl(desc_next, HDSD_SENDCOMMAND,
                                                       p_inf_sd->command);
   if (error) {
      return SD_ERROR_DRIVER;
   }

   return p_inf_sd->command->status;
}

/*-------------------------------------------
| Name:_sd_cmd0
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sd_pon(board_inf_sd_t * p_inf_sd, desc_t desc_next) {
   unsigned int response;

   memset((void *)p_inf_sd->command, 0, sizeof(sd_cmd_t));

   //
   p_inf_sd->command->cmd = DUMMY_POWER_ON_INIT;
   p_inf_sd->command->p_resp = &response;

   // Set SD command state
   p_inf_sd->state = SD_STATE_STBY;

   //
   return _sd_send_command(p_inf_sd, desc_next);
}

/*-------------------------------------------
| Name:_sd_cmd0
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sd_cmd0(board_inf_sd_t * p_inf_sd, desc_t desc_next) {
   unsigned int response;

   memset((void *)p_inf_sd->command, 0, sizeof(sd_cmd_t));

   //
   p_inf_sd->command->cmd = GO_IDLE_STATE;
   p_inf_sd->command->p_resp = &response;

   // Set SD command state
   p_inf_sd->state = SD_STATE_STBY;

   //
   return _sd_send_command(p_inf_sd, desc_next);
}

/*-------------------------------------------
| Name:_sd_cmd8
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sd_cmd8(board_inf_sd_t * p_inf_sd, desc_t desc_next, unsigned char supplyVoltage) {
   unsigned int response[2];
   unsigned int error;

   memset((void *)p_inf_sd->command, 0, sizeof(sd_cmd_t));

   //
   p_inf_sd->command->cmd = SEND_IF_COND;
   p_inf_sd->command->arg = (supplyVoltage << 8) | (0xAA);
   p_inf_sd->command->resp_type = R7;
   p_inf_sd->command->p_resp = &response[0];

   // Set SD command state
   p_inf_sd->state = SD_STATE_STBY;

   // Send command
   error = _sd_send_command(p_inf_sd, desc_next);

   // Check result
   if (error == SD_LOW_LEVEL_NO_RESPONSE) {
      return SD_ERROR_NORESPONSE;
   }
   // SD_R7
   // Bit 0 - 7: check pattern
   // Bit 8 -11: voltage accepted
   else if (!error && ((response[0] & 0x00000FFF) == ((supplyVoltage << 8) | 0xAA))) {
      return 0;
   }
   else {
      return SD_ERROR_DRIVER;
   }
}

/*-------------------------------------------
| Name:_sd_cmd55
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sd_cmd55(board_inf_sd_t * p_inf_sd, desc_t desc_next) {
   unsigned int response;

   memset((void *)p_inf_sd->command, 0, sizeof(sd_cmd_t));

   //
   p_inf_sd->command->cmd = APP_CMD;
   p_inf_sd->command->arg = (p_inf_sd->card_addr << 16);
   p_inf_sd->command->resp_type = R1;
   p_inf_sd->command->p_resp = &response;

   // Set SD command state
   p_inf_sd->state = SD_STATE_STBY;

   //
   return _sd_send_command(p_inf_sd, desc_next);
}

/*-------------------------------------------
| Name:_sd_cmd8
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sd_acmd41(board_inf_sd_t * p_inf_sd, desc_t desc_next, unsigned char hcs,
               unsigned char *pCCS) {
   unsigned char error;
   unsigned int response;

   do {
      error = _sd_cmd55(p_inf_sd, desc_next);
      if (error) {
         return error;
      }

      memset((void *)p_inf_sd->command, 0, sizeof(sd_cmd_t));

      //
      p_inf_sd->command->cmd = SD_APP_OP_COND;
      p_inf_sd->command->arg = OCR_MMC_HOST_VOLTAGE_RANGE;
      if (hcs) {
         p_inf_sd->command->arg |= OCR_CCS;
      }

      p_inf_sd->command->resp_type = R3;
      p_inf_sd->command->p_resp = &response;

      // Set SD command state
      p_inf_sd->state = SD_STATE_STBY;

      //
      error = _sd_send_command(p_inf_sd, desc_next);
      if (error != SD_LOW_LEVEL_CRC_ERROR && error != 0) {
         return error;
      }
      *pCCS  = ((response & OCR_CCS) != 0);
   }
   while ((response & OCR_CARD_POWER_UP_BUSY) != OCR_CARD_POWER_UP_BUSY);

   return 0;
}

/*-------------------------------------------
| Name:_sd_cmd1
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sd_cmd1(board_inf_sd_t * p_inf_sd, desc_t desc_next) {
   unsigned char error;
   unsigned int response;

   memset((void *)p_inf_sd->command, 0, sizeof(sd_cmd_t));

   //
   p_inf_sd->command->cmd = SEND_OP_COND;
   p_inf_sd->command->arg = OCR_MMC_HOST_VOLTAGE_RANGE;
   p_inf_sd->command->resp_type = R3;
   p_inf_sd->command->p_resp = &response;

   // Set SD command state
   p_inf_sd->state = SD_STATE_STBY;

   //
   error = _sd_send_command(p_inf_sd, desc_next);
   if (error) {
      return error;
   }
   if ((response & OCR_CARD_POWER_UP_BUSY) == OCR_CARD_POWER_UP_BUSY) {
      return 0;
   }
   else {
      return SD_ERROR_DRIVER;
   }
}

/*-------------------------------------------
| Name:_sd_cmd2
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sd_cmd2(board_inf_sd_t * p_inf_sd, desc_t desc_next, unsigned int *pCid) {

   memset((void *)p_inf_sd->command, 0, sizeof(sd_cmd_t));

   //
   p_inf_sd->command->cmd = ALL_SEND_CID;
   p_inf_sd->command->resp_type = R2;
   p_inf_sd->command->p_resp = pCid;

   // Set SD command state
   p_inf_sd->state = SD_STATE_STBY;

   //
   return _sd_send_command(p_inf_sd, desc_next);
}

/*-------------------------------------------
| Name:_sd_cmd3
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sd_cmd3(board_inf_sd_t * p_inf_sd, desc_t desc_next) {
   unsigned int cardAddress;
   unsigned char error;

   memset((void *)p_inf_sd->command, 0, sizeof(sd_cmd_t));

   //
   p_inf_sd->command->cmd = SEND_RELATIVE_ADDRESS;

   // Assign relative address to MMC card
   if (p_inf_sd->card_type == CARD_MMC) {
      p_inf_sd->command->arg = (0x1 << 16);
   }
   p_inf_sd->command->resp_type = R6;
   p_inf_sd->command->p_resp = &cardAddress;

   // Set SD command state
   p_inf_sd->state = SD_STATE_STBY;

   //
   error = _sd_send_command(p_inf_sd, desc_next);
   if (error) {
      return error;
   }

   // Save card address in driver
   if (p_inf_sd->card_type != CARD_MMC) {
      p_inf_sd->card_addr = (cardAddress >> 16) & 0xFFFF;
   }
   else {
      // Default MMC RCA is 0x0001
      p_inf_sd->card_addr = 1;
   }

   return 0;
}

/*-------------------------------------------
| Name:_sd_cmd7
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sd_cmd7(board_inf_sd_t * p_inf_sd, desc_t desc_next, unsigned int address) {

   memset((void *)p_inf_sd->command, 0, sizeof(sd_cmd_t));

   //
   p_inf_sd->command->cmd = SELECT_DESELECT_CARD;
   p_inf_sd->command->arg = address << 16;
   p_inf_sd->command->resp_type = R1b;

   //
   p_inf_sd->state = SD_STATE_STBY;

   //
   return _sd_send_command(p_inf_sd, desc_next);
}

/*-------------------------------------------
| Name:_sd_cmd9
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sd_cmd9(board_inf_sd_t * p_inf_sd, desc_t desc_next) {

   memset((void *)p_inf_sd->command, 0, sizeof(sd_cmd_t));

   //
   p_inf_sd->command->cmd = SEND_CSD;
   p_inf_sd->command->arg = p_inf_sd->card_addr << 16;
   p_inf_sd->command->resp_type = R2;
   p_inf_sd->command->p_resp = p_inf_sd->csd;

   //
   p_inf_sd->state = SD_STATE_STBY;

   //
   return _sd_send_command(p_inf_sd, desc_next);
}

/*-------------------------------------------
| Name:_sd_cmd12
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sd_cmd12(board_inf_sd_t * p_inf_sd, desc_t desc_next) {
   unsigned int response;

   memset((void *)p_inf_sd->command, 0, sizeof(sd_cmd_t));

   //
   p_inf_sd->command->cmd = STOP_TRANSMISSION;
   p_inf_sd->command->resp_type = R1b;
   p_inf_sd->command->p_resp = &response;

   //
   p_inf_sd->state = SD_STATE_STBY;

   //
   return _sd_send_command(p_inf_sd, desc_next);
}
/*-------------------------------------------
| Name:_sd_cmd13
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sd_cmd13(board_inf_sd_t * p_inf_sd, desc_t desc_next, unsigned int *pStatus) {

   memset((void *)p_inf_sd->command, 0, sizeof(sd_cmd_t));

   //
   p_inf_sd->command->cmd = SEND_STATUS;
   p_inf_sd->command->arg = p_inf_sd->card_addr << 16;
   p_inf_sd->command->resp_type = R1;
   p_inf_sd->command->p_resp = pStatus;

   //
   p_inf_sd->state = SD_STATE_STBY;

   //
   return _sd_send_command(p_inf_sd, desc_next);
}

/*-------------------------------------------
| Name:_sd_cmd16
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sd_cmd16(board_inf_sd_t * p_inf_sd, desc_t desc_next, unsigned short blockLength) {
   unsigned char error;
   unsigned int response;

   memset((void *)p_inf_sd->command, 0, sizeof(sd_cmd_t));

   //
   p_inf_sd->command->cmd = SET_BLOCKLEN;
   p_inf_sd->command->arg = blockLength;
   p_inf_sd->command->resp_type = R1;
   p_inf_sd->command->p_resp = &response;

   //
   p_inf_sd->state = SD_STATE_STBY;

   //
   return _sd_send_command(p_inf_sd, desc_next);
}

/*-------------------------------------------
| Name:_sd_cmd17
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sd_cmd17(board_inf_sd_t * p_inf_sd, desc_t desc_next, unsigned char *pData,
              unsigned int address) {
   unsigned int response;

   memset((void *)p_inf_sd->command, 0, sizeof(sd_cmd_t));

   //
   p_inf_sd->command->cmd = READ_SINGLE_BLOCK;
   p_inf_sd->command->arg = address;
   p_inf_sd->command->block_size = SD_BLOCK_SIZE;
   p_inf_sd->command->block_count = 1;
   p_inf_sd->command->p_data = pData;
   p_inf_sd->command->is_read = 1;
   p_inf_sd->command->resp_type = R1;
   p_inf_sd->command->p_resp = &response;

   //
   p_inf_sd->state = SD_STATE_DATA;

   //
   return _sd_send_command(p_inf_sd, desc_next);
}

/*-------------------------------------------
| Name:_sd_cmd18
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sd_cmd18(board_inf_sd_t * p_inf_sd, desc_t desc_next, unsigned short nbBlock,
              unsigned char *pData,
              unsigned int address) {
   unsigned int response;

   memset((void *)p_inf_sd->command, 0, sizeof(sd_cmd_t));

   //
   p_inf_sd->command->cmd = READ_MULTIPLE_BLOCK;
   p_inf_sd->command->arg = address;
   p_inf_sd->command->block_size = SD_BLOCK_SIZE;
   p_inf_sd->command->block_count = nbBlock;
   p_inf_sd->command->p_data = pData;
   p_inf_sd->command->is_read = 1;
   p_inf_sd->command->resp_type = R1;
   p_inf_sd->command->p_resp = &response;

   //
   p_inf_sd->state = SD_STATE_DATA;

   //
   return _sd_send_command(p_inf_sd, desc_next);
}

/*-------------------------------------------
| Name:_sd_cmd24
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sd_cmd24(board_inf_sd_t * p_inf_sd, desc_t desc_next, unsigned char *pData,
              unsigned int address) {
   unsigned int response;

   memset((void *)p_inf_sd->command, 0, sizeof(sd_cmd_t));

   //
   p_inf_sd->command->cmd = WRITE_SINGLE_BLOCK;
   p_inf_sd->command->arg = address;
   p_inf_sd->command->block_size = SD_BLOCK_SIZE;
   p_inf_sd->command->block_count = 1;
   p_inf_sd->command->p_data = pData;
   p_inf_sd->command->is_read = 0;
   p_inf_sd->command->resp_type = R1;
   p_inf_sd->command->p_resp = &response;

   p_inf_sd->state = SD_STATE_DATA;
   //
   return _sd_send_command(p_inf_sd, desc_next);
}

/*-------------------------------------------
| Name:_sd_cmd25
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sd_cmd25(board_inf_sd_t * p_inf_sd, desc_t desc_next, unsigned short nbBlock,
              unsigned char *pData,
              unsigned int address) {
   unsigned int response;

   memset((void *)p_inf_sd->command, 0, sizeof(sd_cmd_t));

   //
   p_inf_sd->command->cmd = WRITE_MULTIPLE_BLOCK;
   p_inf_sd->command->arg = address;
   p_inf_sd->command->block_size = SD_BLOCK_SIZE;
   p_inf_sd->command->block_count = nbBlock;
   p_inf_sd->command->p_data = pData;
   p_inf_sd->command->is_read = 0;
   p_inf_sd->command->resp_type = R1;
   p_inf_sd->command->p_resp = &response;

   //
   p_inf_sd->state = SD_STATE_DATA;

   //
   return _sd_send_command(p_inf_sd, desc_next);
}

/*-------------------------------------------
| Name:_sd_acmd6
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sd_acmd6(board_inf_sd_t * p_inf_sd, desc_t desc_next, unsigned char busWidth) {
   unsigned char error;
   unsigned int response;

   // Delay
   error = _sd_cmd55(p_inf_sd, desc_next);
   if (error) {
      return error;
   }

   memset((void *)p_inf_sd->command, 0, sizeof(sd_cmd_t));

   //
   p_inf_sd->command->cmd = SET_BUS_WIDTH;
   if (busWidth == 4) {
      p_inf_sd->command->arg = SD_SCR_BUS_WIDTH_4BITS;
   }
   else {
      p_inf_sd->command->arg = SD_SCR_BUS_WIDTH_1BIT;
   }
   p_inf_sd->command->resp_type = R1;
   p_inf_sd->command->p_resp = &response;

   //
   p_inf_sd->state = SD_STATE_STBY;

   //
   return _sd_send_command(p_inf_sd, desc_next);
}

/*============================================
| End of Source  : drv_sd.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
