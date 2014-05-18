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
| Compiler Directive
==============================================*/
#ifndef _DEV_SD_H_
#define _DEV_SD_H_
/*============================================
| Includes
==============================================*/


/*============================================
| Declaration
==============================================*/
// Status register constants
#define SD_STATUS_READY_FOR_DATA    (1 << 8)
#define SD_STATUS_IDLE              (0 << 9)
#define SD_STATUS_READY             (1 << 9)
#define SD_STATUS_IDENT             (2 << 9)
#define SD_STATUS_STBY              (3 << 9)
#define SD_STATUS_TRAN              (4 << 9)
#define SD_STATUS_DATA              (5 << 9)
#define SD_STATUS_RCV               (6 << 9)
#define SD_STATUS_PRG               (7 << 9)
#define SD_STATUS_DIS               (8 << 9)
#define SD_STATUS_STATE             (0xF << 9)

#define SD_READ_MASK                (0xffffe00)
#define SD_WRITE_MASK               (0xfffff00)

// OCR registers
#define OCR_VDD_16_17          (1 << 4)
#define OCR_VDD_17_18          (1 << 5)
#define OCR_VDD_18_19          (1 << 6)
#define OCR_VDD_19_20          (1 << 7)
#define OCR_VDD_20_21          (1 << 8)
#define OCR_VDD_21_22          (1 << 9)
#define OCR_VDD_22_23          (1 << 10)
#define OCR_VDD_23_24          (1 << 11)
#define OCR_VDD_24_25          (1 << 12)
#define OCR_VDD_25_26          (1 << 13)
#define OCR_VDD_26_27          (1 << 14)
#define OCR_VDD_27_28          (1 << 15)
#define OCR_VDD_28_29          (1 << 16)
#define OCR_VDD_29_30          (1 << 17)
#define OCR_VDD_30_31          (1 << 18)
#define OCR_VDD_31_32          (1 << 19)
#define OCR_VDD_32_33          (1 << 20)
#define OCR_VDD_33_34          (1 << 21)
#define OCR_VDD_34_35          (1 << 22)
#define OCR_VDD_35_36          (1 << 23)
#define OCR_CARD_POWER_UP_BUSY (1 << 31)

#define OCR_MMC_HOST_VOLTAGE_RANGE     (OCR_VDD_27_28 + \
                                        OCR_VDD_28_29 + \
                                        OCR_VDD_29_30 + \
                                        OCR_VDD_30_31 + \
                                        OCR_VDD_31_32 + \
                                        OCR_VDD_32_33)
#define OCR_CCS    (1 << 30)

// CSD register access macros.
#define SD_CSD(pSd, bitfield, \
               bits)   ((((pSd)->csd)[3-(bitfield)/32] >> ((bitfield)%32)) & ((1 << (bits)) - 1))
#define SD_CSD_STRUCTURE(pSd)          SD_CSD(pSd, 126, 2) ///< CSD structure 00b  Version 1.0 01b version 2.0 High Cap
#define SD_CSD_TAAC(pSd)               SD_CSD(pSd, 112, 8) ///< Data read-access-time-1
#define SD_CSD_NSAC(pSd)               SD_CSD(pSd, 104, 8) ///< Data read access-time-2 in CLK cycles
#define SD_CSD_TRAN_SPEED(pSd)         SD_CSD(pSd, 96,  8) ///< Max. data transfer rate
#define SD_CSD_READ_BL_LEN(pSd)        SD_CSD(pSd, 80,  4) ///< Max. read data block length
#define SD_CSD_READ_BL_PARTIAL(pSd)    SD_CSD(pSd, 79,  1) ///< Bartial blocks for read allowed
#define SD_CSD_WRITE_BLK_MISALIGN(pSd) SD_CSD(pSd, 78,  1) ///< Write block misalignment
#define SD_CSD_READ_BLK_MISALIGN(pSd)  SD_CSD(pSd, 77,  1) ///< Read block misalignment
#define SD_CSD_DSR_IMP(pSd)            SD_CSD(pSd, 76,  1) ///< DSP implemented
#define SD_CSD_C_SIZE(pSd)             ((SD_CSD(pSd, 72,  2) << 10) + \
                                        (SD_CSD(pSd, 64,  8) << 2)  + \
                                        SD_CSD(pSd, 62,  2)) ///< Device size
#define SD_CSD_C_SIZE_HC(pSd)          ((SD_CSD(pSd, 64,  6) << 16) + \
                                        (SD_CSD(pSd, 56,  8) << 8)  + \
                                        SD_CSD(pSd, 48,  8)) ///< Device size v2.0 High Capacity
#define SD_CSD_VDD_R_CURR_MIN(pSd)     SD_CSD(pSd, 59,  3) ///< Max. read current @VDD min
#define SD_CSD_VDD_R_CURR_MAX(pSd)     SD_CSD(pSd, 56,  3) ///< Max. read current @VDD max
#define SD_CSD_VDD_W_CURR_MIN(pSd)     SD_CSD(pSd, 53,  3) ///< Max. write current @VDD min
#define SD_CSD_VDD_W_CURR_MAX(pSd)     SD_CSD(pSd, 50,  3) ///< Max. write current @VDD max
#define SD_CSD_C_SIZE_MULT(pSd)        SD_CSD(pSd, 47,  3) ///< Device size multiplier
#define SD_CSD_ERASE_BLK_EN(pSd)       SD_CSD(pSd, 46,  1) ///< Erase single block enable
#define SD_CSD_SECTOR_SIZE(pSd)        ((SD_CSD(pSd, 40,  6) << 1) + SD_CSD(pSd, 39,  1)) ///< Erase sector size
#define SD_CSD_WP_GRP_SIZE(pSd)        SD_CSD(pSd, 32,  7) ///< Write protect group size
#define SD_CSD_WP_GRP_ENABLE(pSd)      SD_CSD(pSd, 31,  1) ///< write protect group enable
#define SD_CSD_R2W_FACTOR(pSd)         SD_CSD(pSd, 26,  3) ///< Write speed factor
#define SD_CSD_WRITE_BL_LEN(pSd)       ((SD_CSD(pSd, 24,  2) << 2) + SD_CSD(pSd, 22,  2)) ///< Max write block length
#define SD_CSD_WRITE_BL_PARTIAL(pSd)   SD_CSD(pSd, 21,  1) ///< Partial blocks for write allowed
#define SD_CSD_FILE_FORMAT_GRP(pSd)    SD_CSD(pSd, 15,  1) ///< File format group
#define SD_CSD_COPY(pSd)               SD_CSD(pSd, 14,  1) ///< Copy flag (OTP)
#define SD_CSD_PERM_WRITE_PROTECT(pSd) SD_CSD(pSd, 13,  1) ///< Permanent write protect
#define SD_CSD_TMP_WRITE_PROTECT(pSd)  SD_CSD(pSd, 12,  1) ///< Temporary write protection
#define SD_CSD_FILE_FORMAT(pSd)        SD_CSD(pSd, 11,  2) ///< File format
#define SD_CSD_CRC(pSd)                SD_CSD(pSd,  1,  7) ///< CRC
#define SD_CSD_MULT(pSd)               (1 << (SD_CSD_C_SIZE_MULT(pSd) + 2))
#define SD_CSD_BLOCKNR(pSd)            ((SD_CSD_C_SIZE(pSd) + 1) * SD_CSD_MULT(pSd))
#define SD_CSD_BLOCKNR_HC(pSd)         ((SD_CSD_C_SIZE_HC(pSd) + 1) * 1024)
#define SD_CSD_BLOCK_LEN(pSd)          (1 << SD_CSD_READ_BL_LEN(pSd))
#define SD_CSD_TOTAL_SIZE(pSd)         (SD_CSD_BLOCKNR(pSd) * SD_CSD_BLOCK_LEN(pSd))
#define SD_CSD_TOTAL_SIZE_HC(pSd)      ((SD_CSD_C_SIZE_HC(pSd) + 1) * 512* 1024)
#define SD_TOTAL_SIZE(pSd)             ((pSd)->total_size)
#define SD_TOTAL_BLOCK(pSd)            ((pSd)->block_nb)

// SCR register access macros.
#define SD_SCR_BUS_WIDTHS(pScr)        ((pScr[1] >> 16) & 0xF) ///< Describes all the DAT bus that are supported by this card
#define SD_SCR_BUS_WIDTH_4BITS         (1 << 1) ///< 4bit Bus Width is supported
#define SD_SCR_BUS_WIDTH_1BIT          (1 << 0) ///< 1bit Bus Width is supported

// SD card block size in bytes.
#define SD_BLOCK_SIZE           512
// SD card block size binary shift value
#define SD_BLOCK_SIZE_BIT     9


// SD card operation states
enum {
   SD_STATE_STBY,
   SD_STATE_DATA,
   SD_STATE_RCV
};

// Card type
enum {
   UNKNOWN_CARD,
   CARD_SD,
   CARD_SDHC,
   CARD_MMC
};

//
#define SD_ADDRESS(pSd, address) (((pSd)->card_type == CARD_SDHC) ? \
                                  (address) : ((address) << SD_BLOCK_SIZE_BIT))

// Error type
enum {
   SD_NO_ERROR_DRIVER,
   SD_ERROR_DRIVER,
   SD_ERROR_NORESPONSE,
   SD_ERROR_NOT_INITIALIZED
};

// Low level error
enum {
   SD_LOW_LEVEL_NO_ERROR,
   SD_LOW_LEVEL_NO_RESPONSE,
   SD_LOW_LEVEL_TIMEOUT_ERROR,
   SD_LOW_LEVEL_ERROR,
   SD_LOW_LEVEL_CRC_ERROR
};

// SD operations
enum {
   GO_IDLE_STATE,
   SEND_OP_COND,
   ALL_SEND_CID,
   SEND_RELATIVE_ADDRESS,
   SET_DSR,
   IO_SEND_OP_COND,    //5
   SWITCH_FUNC,
   SELECT_DESELECT_CARD,
   SEND_IF_COND,
   SEND_CSD,
   SEND_CID,    //10
   NOT_SUPPORTED_11,
   STOP_TRANSMISSION,
   SEND_STATUS,
   NOT_SUPPORTED_14,
   GO_INACTIVE_STATE,   //15
   SET_BLOCKLEN,
   READ_SINGLE_BLOCK,
   READ_MULTIPLE_BLOCK,
   NOT_SUPPORTED_19,    //
   NOT_SUPPORTED_20,    //20
   NOT_SUPPORTED_21,    //
   NOT_SUPPORTED_22,    //
   NOT_SUPPORTED_23,    //
   WRITE_SINGLE_BLOCK,
   WRITE_MULTIPLE_BLOCK,    //25
   NOT_SUPPORTED_26,    //
   NOT_SUPPORTED_27,    //
   NOT_SUPPORTED_28,    //
   NOT_SUPPORTED_29,    //
   NOT_SUPPORTED_30,    //30
   NOT_SUPPORTED_31,    //
   NOT_SUPPORTED_32,    //
   NOT_SUPPORTED_33,    //
   NOT_SUPPORTED_34,    //
   NOT_SUPPORTED_35,    //35
   NOT_SUPPORTED_36,    //
   NOT_SUPPORTED_37,    //
   NOT_SUPPORTED_38,    //
   NOT_SUPPORTED_39,    //
   NOT_SUPPORTED_40,    //40
   NOT_SUPPORTED_41,    //
   NOT_SUPPORTED_42,    //
   NOT_SUPPORTED_43,    //
   NOT_SUPPORTED_44,    //
   NOT_SUPPORTED_45,    //45
   NOT_SUPPORTED_46,    //
   NOT_SUPPORTED_47,    //
   NOT_SUPPORTED_48,    //
   NOT_SUPPORTED_49,    //
   NOT_SUPPORTED_50,    //50
   NOT_SUPPORTED_51,    //
   NOT_SUPPORTED_52,    //
   NOT_SUPPORTED_53,    //
   NOT_SUPPORTED_54,    //
   APP_CMD,    //55
   NOT_SUPPORTED_56,    //
   NOT_SUPPORTED_57,    //
   NOT_SUPPORTED_58,
   NOT_SUPPORTED_59,
   NOT_SUPPORTED_60,    //60
   NOT_SUPPORTED_61,    //
   NOT_SUPPORTED_62,
   NOT_SUPPORTED_63,
   DUMMY_POWER_ON_INIT,
   //ACMD command begin
   NOT_SUPPORTED_65,    //65 (64+1)
   NOT_SUPPORTED_66,
   NOT_SUPPORTED_67,
   NOT_SUPPORTED_68,
   NOT_SUPPORTED_69,
   SET_BUS_WIDTH,    //70
   NOT_SUPPORTED_71,
   NOT_SUPPORTED_72,
   NOT_SUPPORTED_73,
   NOT_SUPPORTED_74,
   NOT_SUPPORTED_75,    //75
   NOT_SUPPORTED_76,
   NOT_SUPPORTED_77,
   NOT_SUPPORTED_78,
   NOT_SUPPORTED_79,
   NOT_SUPPORTED_80,    //80
   NOT_SUPPORTED_81,
   NOT_SUPPORTED_82,
   NOT_SUPPORTED_83,
   NOT_SUPPORTED_84,
   NOT_SUPPORTED_85,    //85
   NOT_SUPPORTED_86,
   NOT_SUPPORTED_87,
   NOT_SUPPORTED_88,
   NOT_SUPPORTED_89,
   NOT_SUPPORTED_90,    //90
   NOT_SUPPORTED_91,
   NOT_SUPPORTED_92,
   NOT_SUPPORTED_93,
   NOT_SUPPORTED_94,
   NOT_SUPPORTED_95,    //95
   NOT_SUPPORTED_96,
   NOT_SUPPORTED_97,
   NOT_SUPPORTED_98,
   NOT_SUPPORTED_99,
   NOT_SUPPORTED_100,    //100
   NOT_SUPPORTED_101,
   NOT_SUPPORTED_102,
   NOT_SUPPORTED_103,
   NOT_SUPPORTED_104,
   SD_APP_OP_COND,
   NOT_SUPPORTED_106,
   NOT_SUPPORTED_107,
   NOT_SUPPORTED_108,
   NOT_SUPPORTED_109,
   NOT_SUPPORTED_110,    //110
   NOT_SUPPORTED_111,
   NOT_SUPPORTED_112,
   NOT_SUPPORTED_113,
   NOT_SUPPORTED_114,
   SEND_SCR,
   NOT_SUPPORTED_116
};

//
#define  NO_RESPONSE    0
#define  R1             1
#define  R2             2
#define  R3             3
#define  R1b            4 //1
#define  R6             6 //1
#define  R7             7

//
typedef struct sd_cmd_st {
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
   unsigned char resp_type;
} sd_cmd_t;

//
typedef struct board_inf_sd_st {
   /// Current MCI command being processed.
   sd_cmd_t * command;
   /// SD card current address.
   unsigned short card_addr;
   /// Card-specific data.
   unsigned int csd[4];
   /// State after sd command complete
   unsigned char state;
   /// Card type
   unsigned char card_type;
   /// Card total size
   unsigned int total_size;
   /// Card block number
   unsigned int block_nb;
   /// Card access mode
   unsigned char mode;

} board_inf_sd_t;

//
#define  SD_BUFFER_SIZE_NO_CACHE_RCV    (SD_BLOCK_SIZE*4) //2ko
#define  SD_BUFFER_SIZE_NO_CACHE_SND    (SD_BLOCK_SIZE*4) //2ko

//
int _sd_init(board_inf_sd_t * p_inf_sd, desc_t desc_next);
int _sd_write(int desc, const char* buf, int cb);
int _sd_read(int desc, char* buf,int cb, unsigned char no_off);
#endif //_DEV_SD_H_
