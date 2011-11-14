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

#ifndef DRV_SDCARD_H_INCLUDED
#define DRV_SDCARD_H_INCLUDED

/*----------------------------------------------------------------------------*/
// Includes

// #include "const.h"
// #include "io_registers.h"
#include "drv_sdcard_arch.h"

/*----------------------------------------------------------------------------*/
// Definitions generales

#define IDLE_TIMEOUT					(FREQUENCY_SPI_SD * 1 / 2)/8 			// 500 ms

/*----------------------------------------------------------------------------*/
// Definitions des commandes (SPI mode)

#define CMD0_GO_IDLE_STATE              0x00
#define CMD1_SEND_OP_COND               0x01
#define CMD6_SWITCH_FUNC                0x06
#define CMD8_SEND_IF_COND               0x08
#define CMD9_SEND_CSD                   0x09
#define CMD10_SEND_CID                  0x0A
#define CMD12_STOP_TRANSMISSION         0x0C
#define CMD13_SEND_STATUS               0x0D
#define CMD16_SET_BLOCKLEN              0x10
#define CMD17_READ_SINGLE_BLOCK         0x11
#define CMD18_READ_MULTIPLE_BLOCK       0x12
#define CMD24_WRITE_BLOCK               0x18
#define CMD25_WRITE_MULTIPLE_BLOCK      0x19
#define CMD27_PROGRAM_CSD               0x1B
#define CMD28_SET_WRITE_PROT            0x1C
#define CMD29_CLR_WRITE_PROT            0x1D
#define CMD30_SEND_WRITE_PROT           0x1E
#define CMD32_ERASE_WR_BLK_START_ADDR   0x20
#define CMD33_ERASE_WR_BLK_END_ADDR     0x21
#define CMD38_ERASE                     0x26
#define CMD42_LOCK_UNLOCK               0x2A
#define CMD55_APP_CMD                   0x37
#define CMD56_GEN_CMD                   0x38
#define CMD58_READ_OCR                  0x3A
#define CMD59_CRC_ON_OFF                0x3B

/*----------------------------------------------------------------------------*/
// Definitions des commandes specifiques applicatives (SPI mode)
// N.B. Toutes ces commandes doivent etre precedees par la commande CMD55_APP_CMD

#define ACMD13_SD_STATUS                0x0D
#define ACMD22_SEND_NUM_WR_BLOCKS       0x16
#define ACMD23_SET_WR_BLK_ERASE_COUNT   0x17
#define ACMD41_SD_SEND_OP_COND          0x29
#define ACMD42_SET_CLR_CARD_DETECT      0x30
#define ACMD51_SEND_SCR                 0x33

/*----------------------------------------------------------------------------*/
// Definitions utiles pour la creation des commandes

#define CMD_SIZE			         6
#define CMD_START_TRAME				0x40
#define CMD_END_TRAME_CMD0_CS		0x95 //Checksum de la CMD0 precalcule
#define CMD_END_TRAME_NO_CS		0xFF //Pas de checksum en mode SPI
#define CMD_NO_ARG				   0x00000000 //Pas de checksum en mode SPI

/*----------------------------------------------------------------------------*/
// Definitions utiles pour la lecture/ecriture de block
#define BLOCK_SIZE				512
#define CHECKSUM_SIZE			2
#define READ_TIMEOUT	     		(FREQUENCY_SPI_SD * 0.10)/8 // 100 ms
#define WRITE_TIMEOUT 			(FREQUENCY_SPI_SD * 0.25)/8 // 250 ms
#define DATA_RESP_TIMEOUT	     		8
#define TOKEN_MASK_START_SRW_MR		0xFE
#define TOKEN_MASK_START_MW			0xFC
#define TOKEN_MASK_STOP_MW			   0xFD
#define TOKEN_MASK_BUSY				   0x00
#define TOKEN_SIZE				      1
#define WRITE_CS_BIDON				   0xFFFF
#define TOKEN_MASK_WRITE_OK			0x05
#define TOKEN_MASK_WRITE_CRC_ERROR	0x0B
#define TOKEN_MASK_WRITE_ERROR		0x0D

/*----------------------------------------------------------------------------*/
// Definitions utiles pour la reception des reponses 
#define RESPONSE_R1				 1
#define RESPONSE_R2			    2
#define RESPONSE_R1_SIZE		 1
#define RESPONSE_R2_SIZE		 2
#define OCR_SIZE			   	 5
#define REGISTER_SIZE			 16
#define RESPONSE_TIMEOUT		 8 // 8 octets = 64 clocks cycle
#define RESPONSE_R1_MASK		 0x80 // si le MSB vaut 1, reponse invalide
#define RESPONSE_R1_BUSY		 0x7F00

/*----------------------------------------------------------------------------*/
// Status de la COM et de la SDCARD 

#define STATUS_SD_SPI_OK			      0x00000000	
#define STATUS_SD_IDLE_STATE			   0x00000100 // Resultats R1
#define STATUS_SD_RESPONSE_TIMEOUT		0x00008000 // Reponse a la cmd non recue
#define STATUS_SD_READ_ERROR			   0x00010000 // Resultat de lecture
#define STATUS_SD_READ_CC_ERROR			0x00020000
#define STATUS_SD_READ_ECC_ERROR		   0x00030000
#define STATUS_SD_READ_OUT_OF_RANGE		0x00040000
#define STATUS_SD_TOKEN_TIMEOUT			0x00080000 // start ou stop token non recu
#define STATUS_SD_WRITE_CRC_ERROR		0x00100000 // Resultat d'ecriture
#define STATUS_SD_WRITE_ERROR			   0x00200000
#define STATUS_SD_DATA_RESP_TIMEOUT		0x00400000
#define STATUS_SD_BUSY_TIMEOUT			0x00800000
#define STATUS_SD_TIMEOUT_IDLE			0x04000000 // Idle timeout

/*----------------------------------------------------------------------------*/
// Macros

/*----------------------------------------------------------------------------*/
// Structures
typedef struct
{
	unsigned char		volid[4];
	unsigned long		size;
} t_media;

/*----------------------------------------------------------------------------*/
// Prototypes

int sdcard_init(desc_t desc,t_media *sdcard);
int sdcard_write(desc_t desc,unsigned char *BufferWrite, unsigned long Add, unsigned long Size);
int sdcard_read(desc_t desc,unsigned char *BufferRead, unsigned long Add, unsigned long Size);

/*----------------------------------------------------------------------------*/

#endif

/******************************************************************************/
/* $Log: not supported by cvs2svn $
/* Revision 1.1  2009/03/30 11:16:08  jjp
/* First import of tauon
/*
/* Revision 1.3  2008/07/09 07:30:32  phlb
/* some bug fix. dev_sdcard_seek(), mutex unlock.
/* add ioctl() capabilities for I_LINK request.
/*
/* Revision 1.2  2008/06/05 12:11:11  mollo
/* Adaptation for arm9
/*
/* Revision 1.1  2008/04/11 12:58:50  mollo
/* no message
/*
*/

