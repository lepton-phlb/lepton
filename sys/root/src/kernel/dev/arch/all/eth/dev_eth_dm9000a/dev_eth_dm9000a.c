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
#include "kernel/core/kernel_pthread.h"
#define USE_IO_LINUX
#include "kernel/core/io.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/kal.h"
#include "kernel/core/interrupt.h"

#include "lib/libc/termios/termios.h"

#include "kernel/fs/vfs/vfsdev.h"
//for uip ethernet address
//#include "kernel\net\uip1.0\net\uipopt.h"

#include "kernel/core/ioctl_eth.h"

#if defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__)
   #include <atmel/ioat91sam9261.h>
#else
   #include "cyg/hal/at91sam9261.h"
   #include <string.h>
   #include <stdlib.h>
#endif

#include "kernel/dev/arch/all/eth/dev_eth_dm9000a/dev_eth_dm9000a.h"

/*============================================
| Global Declaration
==============================================*/

extern AT91PS_SYS AT91_SYS;

#define LWIP_ETHADDR0    0x00
#define LWIP_ETHADDR1    0xbd
#define LWIP_ETHADDR2    0x3b
#define LWIP_ETHADDR3    0x33
#define LWIP_ETHADDR4    0x05
#define LWIP_ETHADDR5    0x71

//
const char ADDMAC[6] = {   LWIP_ETHADDR0,
                           LWIP_ETHADDR1,
                           LWIP_ETHADDR2,
                           LWIP_ETHADDR3,
                           LWIP_ETHADDR4,
                           LWIP_ETHADDR5 };


//lwip
unsigned char lwip_ethaddr[6]={  LWIP_ETHADDR0,
                                 LWIP_ETHADDR1,
                                 LWIP_ETHADDR2,
                                 LWIP_ETHADDR3,
                                 LWIP_ETHADDR4,
                                 LWIP_ETHADDR5 };

//
/*
const char ADDMAC[6] = {   UIP_ETHADDR0,
                           UIP_ETHADDR1,
                           UIP_ETHADDR2,
                           UIP_ETHADDR3,
                           UIP_ETHADDR4,
                           UIP_ETHADDR5 };
*/

const char dev_eth_dm9000a_name[]="eth0\0";

//from linux driver
//Board/System/Debug information/definition

#define DM9KS_ID     0x90000A46
#define DM9010_ID    0x90100A46

enum dm9000_type_t {
   TYPE_DM9000E,   /* original DM9000 */
   TYPE_DM9000A,
   TYPE_DM9000B
};

#define DM9KS_NCR                  0x00 /* Network control Reg.*/
#define DM9KS_NSR                  0x01 /* Network Status Reg.*/
#define DM9KS_TCR                  0x02 /* TX control Reg.*/
#define DM9KS_RXCR              0x05    /* RX control Reg.*/
#define DM9KS_BPTR              0x08
#define DM9KS_EPCR              0x0b
#define DM9KS_EPAR              0x0c
#define DM9KS_EPDRL             0x0d
#define DM9KS_EPDRH             0x0e
#define DM9KS_GPCR      0x1e
#define DM9KS_GPR                  0x1f /* General purpose register */
#define DM9KS_TCR2              0x2d
#define DM9KS_SMCR              0x2f    /* Special Mode Control Reg.*/
#define DM9KS_ETXCSR            0x30    /* Early Transmit control/status Reg.*/
#define DM9KS_TCCR              0x31    /* Checksum cntrol Reg. */
#define DM9KS_RCSR              0x32    /* Receive Checksum status Reg.*/
#define DM9KS_MRCMDX            0xf0
#define DM9KS_MRCMD             0xf2
#define DM9KS_MDRAL             0xf4
#define DM9KS_MDRAH             0xf5
#define DM9KS_MWCMD             0xf8
#define DM9KS_TXPLL             0xfc
#define DM9KS_TXPLH             0xfd
#define DM9KS_ISR       0xfe
#define DM9KS_IMR                  0xff
#define DM9KS_REG05             0x38    /* SKIP_CRC/SKIP_LONG | PASS_ALL_MULTICAST */
#define DM9KS_REGFF             0xA3    /* IMR */
#define DM9KS_DISINTR   0x80

#define DM9KS_PHY                  0x40 /* PHY address 0x01 */
#define DM9KS_PKT_RDY   0x01    /* Packet ready to receive */

#define DM9KS_VID_L             0x28
#define DM9KS_VID_H             0x29
#define DM9KS_PID_L             0x2A
#define DM9KS_PID_H             0x2B
#define DM9KS_CHIPR     0x2C

#define CHIPR_DM9000A   0x19
#define CHIPR_DM9000B   0x1B

#define DM9KS_RX_INTR           0x01
#define DM9KS_TX_INTR           0x02
#define DM9KS_LINK_INTR         0x20

#define DM9KS_DWORD_MODE        1
#define DM9KS_BYTE_MODE         2
#define DM9KS_WORD_MODE         0

#define GPCR_GEP_CNTL       (1<<0)

#define TRUE                    1
#define FALSE                   0
// Number of continuous Rx packets
#define CONT_RX_PKT_CNT 10

#define DMFE_TIMER_WUT  jiffies+(HZ*5)  // timer wakeup time : 5 seconds

//
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

//
#if (__tauon_compiler__!=__compiler_gnuc__)
   #pragma pack(push, 1)
#endif

typedef struct __attribute__((aligned(1))) _RX_DESC
{
   u8 rxbyte;
   u8 status;
   u16 length;
} RX_DESC;

typedef union __attribute__((aligned(1))) {
   u8 buf[4];
   RX_DESC desc;
} rx_t;

#if (__tauon_compiler__!=__compiler_gnuc__)
   #pragma pack(pop)
#endif

//
enum DM9KS_PHY_mode {
   DM9KS_10MHD   = 0,
   DM9KS_100MHD  = 1,
   DM9KS_10MFD   = 4,
   DM9KS_100MFD  = 5,
   DM9KS_AUTO    = 8,
};

desc_t _eth_dm9000a_desc_rd=-1;
desc_t _eth_dm9000a_desc_wr=-1;

// Structure/enum
typedef struct board_info {

   enum dm9000_type_t type;

   u32 _input_r;
   u32 _input_w;
   u32 _input_tmp_w;

   u32 _output_r;
   u32 _output_w;

   //input buffer
   u8* _ptr_eth_dm9000a_input_buffer;
   //output buffer
   u8* _ptr_eth_dm9000a_output_buffer;
   //frame vector;
   u8* ptr_eth_frame_vector;

   //
   u32 reset_counter;           /* counter: RESET */
   u32 reset_tx_timeout;        /* RESET caused by TX Timeout */

   u32 io_addr;        /* Register I/O base address */
   u32 io_data;         /* Data I/O address */
   u32 irq_no;         /* irq number*/
   u32 irq_prio;  /* irq priority */

   int tx_pkt_cnt;

   u8 op_mode;         /* PHY operation mode */
   u8 io_mode;                  /* 0:word, 2:byte */
   u8 device_wait_reset;        /* device state */
   u8 Speed;     /* current speed */

   int cont_rx_pkt_cnt;      /* current number of continuos rx packets  */
   //struct timer_list timer;
   //struct net_device_stats stats;
   unsigned char srom[128];

   //spinlock_t lock;
   kernel_pthread_mutex_t mutex;
   //
   char mac_addr[6];

   //ethernet status
   eth_stat_t eth_stat;

} board_info_t;

#define ETH_DM9000_INPUT_BUFFER_SZ     (32*1024) /*8192*/ //(2048) //warning!!!! must be 2^N
#define __ETH_FRAME_VECTOR_SZ   (ETH_DM9000_INPUT_BUFFER_SZ/8+2)

#define __set_eth_frame_vector(db,pos){ \
      unsigned char byte_no = pos>>3; \
      unsigned char bit_no  = pos-(byte_no<<3); \
      unsigned char msk = 0x01<<bit_no; \
      db->ptr_eth_frame_vector[byte_no]|=msk; \
}

#define __unset_eth_frame_vector(db,pos){ \
      unsigned char byte_no = pos>>3; \
      unsigned char bit_no  = pos-(byte_no<<3); \
      unsigned char msk = 0x01<<bit_no; \
      db->ptr_eth_frame_vector[byte_no]&=~(msk); \
}

#define __flush_eth_frame_buffer(db){ \
      memset(db->ptr_eth_frame_vector,0,__ETH_FRAME_VECTOR_SZ); \
      db->_input_r=0; \
      db->_input_w=0; \
      db->_input_tmp_w=0; \
      db->_output_r=0; \
      db->_output_w=0; \
}

//static int dmfe_debug = 0;
// For module input parameter
static int mode       = DM9KS_AUTO;
static int media_mode = DM9KS_AUTO;

/*
static int mode       = DM9KS_10MHD;
static int media_mode = DM9KS_10MHD;
*/

#define DMFE_DBUG(dbug_now, msg, vaule)

static board_info_t g_board_info;
#if defined(__KERNEL_UCORE_EMBOS)
   #define mdelay(__mdelay__) OS_Delay(__mdelay__)
#elif defined(__KERNEL_UCORE_ECOS)
   #define mdelay(__mdelay__) cyg_thread_delay(__mdelay__)
#endif

//#define delay_1us 150//150:arm7@50MHz 38:ARM7@32MHz
#define delay_1us 10 //150:arm9 200MHz  // $BM 28.10.2008
#define udelay(__udelay__){ \
      int __i__,__j__; \
      for (__j__ = 0; __j__ < __udelay__; __j__++) \
         for(__i__ = 0; __i__ < delay_1us; __i__++) __i__ = __i__; \
}


#define __set_eth_status(__eth_status__,__value__){ \
      (__eth_status__)&=~(ETH_STAT_LINK_MASK); \
      (__eth_status__)|=((__value__)&ETH_STAT_LINK_MASK); \
}

#if defined(__KERNEL_UCORE_ECOS)

static int flag_i_int=0;
static int flag_o_int=0;

   #define __set_flag_fire_i_int() (flag_i_int=1)
   #define __set_flag_fire_o_int() (flag_o_int=1)

   #define __unset_flag_fire_i_int() (flag_i_int=0)
   #define __unset_flag_fire_o_int() (flag_o_int=0)

   #define __is_set_flag_fire_i_int() (flag_i_int)
   #define __is_set_flag_fire_o_int() (flag_o_int)

static cyg_handle_t _eth_dm9000_handle;
static cyg_interrupt _eth_dm9000_it;
#endif

/*===========================================
Implementation
=============================================*/

/*--------------------------------------------
| Name:        ior
| Description: Read a byte from I/O port
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
u8 ior(board_info_t *db, int reg){
   outb(reg, db->io_addr);
   return inb(db->io_data);
}

/*--------------------------------------------
| Name:        iow
| Description: Write a byte to I/O port
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
void iow(board_info_t *db, int reg, u8 value){
   outb(reg, db->io_addr);
   outb(value, db->io_data);
}

/*--------------------------------------------
| Name:        phy_read
| Description: Read a word from phyxcer
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static u16 phy_read(board_info_t *db, int reg)
{
   /* Fill the phyxcer register into REG_0C */
   iow(db, DM9KS_EPAR, DM9KS_PHY | reg);

   iow(db, DM9KS_EPCR, 0xc);            /* Issue phyxcer read command */
   udelay(100);                         /* Wait read complete */
   iow(db, DM9KS_EPCR, 0x0);            /* Clear phyxcer read command */

   /* The read data keeps on REG_0D & REG_0E */
   return ( ior(db, DM9KS_EPDRH) << 8 ) | ior(db, DM9KS_EPDRL);

}

/*--------------------------------------------
| Name:        phy_write
| Description: Write a word to phyxcer
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static void phy_write(board_info_t *db, int reg, u16 value)
{
   /* Fill the phyxcer register into REG_0C */
   iow(db, DM9KS_EPAR, (u8)(DM9KS_PHY | reg));

   /* Fill the written data into REG_0D & REG_0E */
   iow(db, DM9KS_EPDRL, (value & 0xff));
   iow(db, DM9KS_EPDRH, ( (value >> 8) & 0xff));

   iow(db, DM9KS_EPCR, 0xa);            /* Issue phyxcer write command */
   udelay(500);                         /* Wait write complete */
   iow(db, DM9KS_EPCR, 0x0);            /* Clear phyxcer write command */
}

/*--------------------------------------------
| Name:        read_srom_word
| Description: Read a word data from SROM
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static u16 read_srom_word(board_info_t *db, int offset){
   iow(db, DM9KS_EPAR, (u8)offset);
   iow(db, DM9KS_EPCR, 0x4);
   udelay(200);
   iow(db, DM9KS_EPCR, 0x0);
   return (ior(db, DM9KS_EPDRL) + (ior(db, DM9KS_EPDRH) << 8) );
}

/*--------------------------------------------
| Name:        dm9000_hash_table
| Description: Set DM9000A/DM9010 multicast address
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static void dm9000_hash_table(board_info_t *db){
//#if 0
//struct dev_mc_list *mcptr=(struct dev_mc_list*)0; /*= dev->mc_list;*/
   int mc_cnt = 0;      /*dev->mc_count;*/
   u32 hash_val;
   u16 i, oft, hash_table[4]={0};

   DMFE_DBUG(0, "dm9000_hash_table()", 0);

   /* Set Node address */
   for (i = 0, oft = 0x10; i < 6; i++, oft++)
      iow(db, oft, db->mac_addr[i]);

   /* Clear Hash Table */
   /*
for (i = 0; i < 4; i++)
           hash_table[i] = 0x0;
*/

   /* broadcast address */
   hash_table[3] = 0x8000;

   /* the multicast address in Hash Table : 64 bits */
   /*
        for (i = 0; i < mc_cnt; i++, mcptr = mcptr->next) {
                hash_val = cal_CRC((char *)mcptr->dmi_addr, 6, 0) & 0x3f;
                hash_table[hash_val / 16] |= (u16) 1 << (hash_val % 16);
        }
   */

   /* Write the hash table to MAC MD table */
   for (i = 0, oft = 0x16; i < 4; i++) {
      iow(db, oft++, hash_table[i] & 0xff);
      iow(db, oft++, (hash_table[i] >> 8) & 0xff);
   }
//#endif
}

/*--------------------------------------------
| Name:        cal_crc
| Description: Calculate the CRC valude of the Rx packet
| Parameters:  none
| Return Type: none
| Comments:    flag = 1 : return the reverse CRC (for the received packet CRC)
|                     0 : return the normal CRC (for Hash Table index)
| See:
----------------------------------------------*/
static unsigned long cal_crc(unsigned char * Data, unsigned int Len, u8 flag){
   u32 crc = 0; /*ether_crc_le(Len, Data)*/;
   //crc32 see  http://www.gelato.unsw.edu.au/lxr/source/lib/crc32.c#L59
   if (flag)
      return ~crc;

   return crc;
}

/*--------------------------------------------
| Name:        set_PHY_mode
| Description: Set PHY operationg mode
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static void set_PHY_mode(board_info_t *db)
{
#ifndef DM8606
   u16 phy_reg0 = 0x1200;               /* Auto-negotiation & Restart Auto-negotiation */
   u16 phy_reg4 = 0x01e1;               /* Default flow control disable*/

   if ( !(db->op_mode & DM9KS_AUTO) )      // op_mode didn't auto sense */
   {
      switch(db->op_mode) {
      case DM9KS_10MHD:  phy_reg4 = 0x21;
         phy_reg0 = 0x1000;
         break;
      case DM9KS_10MFD:  phy_reg4 = 0x41;
         phy_reg0 = 0x1100;
         break;
      case DM9KS_100MHD: phy_reg4 = 0x81;
         phy_reg0 = 0x3000;
         break;
      case DM9KS_100MFD: phy_reg4 = 0x101;
         phy_reg0 = 0x3100;
         break;
      default:
         break;
      }           // end of switch
   }      // end of if
   phy_write(db, 4, phy_reg4);
   phy_write(db, 0, phy_reg0);
#else
   /* Fiber mode */
   phy_write(db, 16, 0x4000);
   phy_write(db, 0, 0x2100);
#endif
}

/*--------------------------------------------
| Name:        dmfe_probe1
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dmfe_probe1(board_info_t* db){
   u32 id_val;
   u16 i, dm9000_found = FALSE;
   //u8 MAC_addr[6]={0x00,0x60,0x6E,0x33,0x44,0x55};
   u8 HasEEPROM=0;

   /* Search All DM9000 serial NIC */
   /* try multiple times, DM9000 sometimes gets the read wrong. patch from linux */
   for(i=0; i<8; i++) {
      outb(DM9KS_VID_L, db->io_addr);
      id_val = inb(db->io_data);
      outb(DM9KS_VID_H, db->io_addr);
      id_val |= inb(db->io_data) << 8;
      outb(DM9KS_PID_L, db->io_addr);
      id_val |= inb(db->io_data) << 16;
      outb(DM9KS_PID_H, db->io_addr);
      id_val |= inb(db->io_data) << 24;
   }
   //
   i=0;
   //
   if (id_val != DM9KS_ID && id_val != DM9010_ID)
      return -1;     //dm9000 not found;
   //
   dm9000_found = TRUE;
   //
   //get chip revision
   id_val = ior(db, DM9KS_CHIPR);

   switch (id_val) {
   case CHIPR_DM9000A:
      db->type = TYPE_DM9000A;
      break;

   case CHIPR_DM9000B:
      db->type = TYPE_DM9000B;
      break;

   default:
      db->type = TYPE_DM9000E;   //
      break;
   }

#if defined(CHECKSUM)
   dev->features |=  NETIF_F_IP_CSUM;
#endif
   /* Read SROM content */
   for (i=0; i<64; i++)
      ((u16 *)db->srom)[i] = read_srom_word(db, i);

   /* Detect EEPROM */
   for (i=0; i<6; i++)
      if(db->srom[i]) HasEEPROM=1;

   /* Set Node Address */
   for (i=0; i<6; i++)
   {
      if (HasEEPROM) /* use EEPROM */
         db->mac_addr[i] = db->srom[i];
      else      /* No EEPROM */
         db->mac_addr[i] = ADDMAC[i];
   }


   return dm9000_found ? 0 : -1;
}

/*--------------------------------------------
| Name:        dmfe_init_dm9000
| Description: Initilize dm9000 board
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static void dmfe_init_dm9000(board_info_t *db){
   //spin_lock_init(&db->lock);
#if 0
   /* set the internal PHY power-on, GPIOs normal, and wait 2ms */
   iow(db, DM9KS_GPR, 0);       /* GPR (reg_1Fh)bit GPIO0=0 pre-activate PHY */
   mdelay(20);                  /* wait for PHY power-on ready */
   iow(db, DM9KS_GPR, 1);       /* Power-Down PHY */
   mdelay(1000);                /* compatible with rtl8305s */
   mdelay(600);                 /* compatible with rtl8305s */
#endif
   /* GPIO0 on pre-activate PHY */
   //iow(db, DM9KS_GPR, 0); /* REG_1F bit0 activate phyxcer */
   //iow(db, DM9KS_GPCR, GPCR_GEP_CNTL);    /* Let GPIO0 output */
   //mdelay(20);    /* wait for PHY power-on ready */
   iow(db, DM9KS_GPR, 0);      /* GPR (reg_1Fh)bit GPIO0=0 pre-activate PHY */
   mdelay(20);                  /* wait for PHY power-on ready */

   /* do a software reset and wait 20us */
   iow(db, DM9KS_NCR, 3);
   udelay(20);                  /* wait 20us at least for software reset ok */
   iow(db, DM9KS_NCR, 3);       /* NCR (reg_00h) bit[0] RST=1 & Loopback=1, reset on */
   udelay(20);                  /* wait 20us at least for software reset ok */

   /* I/O mode */
   db->io_mode = ior(db, DM9KS_ISR) >> 6;      /* ISR bit7:6 keeps I/O mode */

   /* Set PHY */
   db->op_mode = media_mode;
   set_PHY_mode(db);

   /* Program operating register */
   iow(db, DM9KS_NCR, 0);
   iow(db, DM9KS_TCR, 0);               /* TX Polling clear */
   iow(db, DM9KS_BPTR, 0x3f);           /* Less 3kb, 600us */
   iow(db, DM9KS_SMCR, 0);              /* Special Mode */
   iow(db, DM9KS_NSR, 0x2c);            /* clear TX status */
   iow(db, DM9KS_ISR, 0x0f);            /* Clear interrupt status */
   iow(db, DM9KS_TCR2, 0x80);           /* Set LED mode 1 */
#if 0
   /* Data bus current driving/sinking capability  */
   iow(db, DM9KS_PBCR, 0x60);           /* default: 8mA */
#endif
#ifdef DM8606
   iow(db,0x34,1);
#endif

   /* Added by jackal at 03/29/2004 */
#if defined(CHECKSUM)
   iow(db, DM9KS_TCCR, 0x07);           /* TX UDP/TCP/IP checksum enable */
   iow(db, DM9KS_RCSR, 0x02);           /* RX checksum enable */
#endif

#if defined(ETRANS)
   iow(db, DM9KS_ETXCSR, 0x83);
#endif

   /* Set address filter table */
   dm9000_hash_table(db);

   /* Activate DM9000A/DM9010 */
   iow(db, DM9KS_RXCR, DM9KS_REG05|1);          /* RX enable*/
   iow(db, DM9KS_IMR, DM9KS_REGFF);             // Enable TX/RX interrupt mask

   /* Init Driver variable */
   db->tx_pkt_cnt          = 0;
   //netif_carrier_on(dev);
}

/*--------------------------------------------
| Name:        dmfe_open
| Description: Open the interface.
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static int dmfe_open(board_info_t *db){
   u8 reg_nsr;
   int i;


   /* Initilize DM910X board */
   dmfe_init_dm9000(db);
   /* Init driver variable */
   db->reset_counter       = 0;
   db->reset_tx_timeout    = 0;
   db->cont_rx_pkt_cnt     = 0;

   /* check link state and media speed */
   __set_eth_status(db->eth_stat,ETH_STAT_LINK_DOWN);
   db->Speed =10;
   i=0;
   do {
      reg_nsr = ior(db,DM9KS_NSR);
      if(reg_nsr & 0x40)           /* link OK!! */
      {
         /* wait for detected Speed */
         mdelay(200);
         reg_nsr = ior(db,DM9KS_NSR);
         if(reg_nsr & 0x80) {
            db->Speed =10;
            __set_eth_status(db->eth_stat,ETH_STAT_LINK_10);
         }else{
            db->Speed =100;
            __set_eth_status(db->eth_stat,ETH_STAT_LINK_100);
         }
         break;
      }
      i++;
      mdelay(1);
   } while(i<3000);     /* wait 3 second  */

   return 0;
}



/*--------------------------------------------
| Name:        dmfe_stop
| Description:   Stop the interface.
|                The interface is stopped when it is brought.
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static int dmfe_stop(board_info_t *db){
   DMFE_DBUG(0, "dmfe_stop", 0);
   /* free interrupt */
   //free_irq(dev->irq, dev);

   /* RESET devie */
   phy_write(db, 0x00, 0x8000);         /* PHY RESET */
   //iow(db, DM9KS_GPR, 0x01);  /* Power-Down PHY */
   iow(db, DM9KS_IMR, DM9KS_DISINTR);           /* Disable all interrupt */
   iow(db, DM9KS_RXCR, 0x00);           /* Disable RX */

   return 0;
}

/*--------------------------------------------
| Name:        dmfe_reset
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static void dmfe_reset(board_info_t *db){
   u8 reg_save;
   int i;
   /* Save previous register address */
   reg_save = inb(db->io_addr);

   db->reset_counter++;
   dmfe_init_dm9000(db);

   db->Speed =10;
   __set_eth_status(db->eth_stat,ETH_STAT_LINK_DOWN);
   for(i=0; i<1000; i++)      /*wait link OK, waiting time=1 second */
   {
      if(phy_read(db,0x1) & 0x4)           /*Link OK*/
      {
         if(phy_read(db,0)&0x2000) {
            db->Speed =100;
            __set_eth_status(db->eth_stat,ETH_STAT_LINK_100);
         }else{
            db->Speed =10;
            __set_eth_status(db->eth_stat,ETH_STAT_LINK_10);
         }
         break;
      }
      udelay(1000);
   }
   /* Restore previous register address */
   outb(reg_save, db->io_addr);
}

/*--------------------------------------------
| Name:        dmfe_timeout
| Description: Our watchdog timed out. Called by the networking layer
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static void dmfe_timeout(board_info_t *db){
   DMFE_DBUG(0, "dmfe_TX_timeout()", 0);
   db->reset_tx_timeout++;
   //db->stats.tx_errors++;
   dmfe_reset(db);
}

/*--------------------------------------------
| Name:        dmfe_timer
| Description:   A periodic timer routine
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static void dmfe_timer(board_info_t *db){
   DMFE_DBUG(0, "dmfe_timer()", 0);

   if (db->cont_rx_pkt_cnt>=CONT_RX_PKT_CNT) {
      db->cont_rx_pkt_cnt=0;
      iow(db, DM9KS_IMR, DM9KS_REGFF);
   }

   return;
}

/*--------------------------------------------
| Name:        dmfe_start_xmit
| Description: Hardware start transmission.
|              Send a packet to media from the upper layer.
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static int dmfe_start_xmit(board_info_t *db, u8* buf,int len){
   char * data_ptr;
   int i, tmplen;

   if(db->Speed == 10) {
      if (db->tx_pkt_cnt >= 1)
         return -1;
   }else{
      if (db->tx_pkt_cnt >= 2)
         return -1;
   }

   //db->stats.tx_packets++;
   //db->stats.tx_bytes+=skb->len;
   /*
        if (db->Speed == 10){
      if (db->tx_pkt_cnt >= 1)
         netif_stop_queue(db);
   }
        else{
      if (db->tx_pkt_cnt >= 2)
         netif_stop_queue(db);
   }*/

   /* Disable all interrupt */
   iow(db, DM9KS_IMR, DM9KS_DISINTR);

   /* packet counting */
   db->tx_pkt_cnt++;
   db->_output_w++;

   /* Set TX length to reg. 0xfc & 0xfd */
   iow(db, DM9KS_TXPLL, (len & 0xff));
   iow(db, DM9KS_TXPLH, (len >> 8) & 0xff);

   /* Move data to TX SRAM */
   data_ptr = (char *)buf;

   outb(DM9KS_MWCMD, db->io_addr);      // Write data into SRAM trigger
   switch(db->io_mode)
   {
   case DM9KS_BYTE_MODE:
      for (i = 0; i < len; i++)
         outb((data_ptr[i] & 0xff), db->io_data);
      break;
   case DM9KS_WORD_MODE:
      tmplen = (len + 1) / 2;
      for (i = 0; i < tmplen; i++)
         outw(((u16 *)data_ptr)[i], db->io_data);
      break;
   case DM9KS_DWORD_MODE:
      tmplen = (len + 3) / 4;
      for (i = 0; i< tmplen; i++)
         outl(((u32 *)data_ptr)[i], db->io_data);
      break;
   }

#if !defined(ETRANS)
   /* Issue TX polling command */
   iow(db, DM9KS_TCR, 0x1);      /* Cleared after TX complete*/
#endif

   /* Saved the time stamp */
   //dev->trans_start = jiffies;
   db->cont_rx_pkt_cnt =0;

   /* Free this SKB */
   //dev_kfree_skb(skb);

   /* Re-enable interrupt */
   iow(db, DM9KS_IMR, DM9KS_REGFF);

   return 0;
}

/*--------------------------------------------
| Name:        dmfe_tx_done
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static void dmfe_tx_done(board_info_t *db,unsigned long unused){
   int nsr;

   DMFE_DBUG(0, "dmfe_tx_done()", 0);

   //packet was sent or flush buffer because input packet buffer is overloaded?
   //see db->cont_rx_pkt_cnt>=CONT_RX_PKT_CNT
   if(db->_output_r!=db->_output_w) //protection if mutex was not used risk of dead lock in next dev_eth_dm9000a_write()
      kernel_pthread_mutex_unlock(&db->mutex);  //lock in dev_eth_dm9000a_write()
   //
   nsr = ior(db, DM9KS_NSR);
   if (nsr & 0x0c)
   {
      if(nsr & 0x04)
         db->tx_pkt_cnt--;
      if(nsr & 0x08)
         db->tx_pkt_cnt--;

      if(db->tx_pkt_cnt < 0)
      {
         //printk("[dmfe_tx_done] tx_pkt_cnt ERROR!!\n");
         while(ior(db,DM9KS_TCR) & 0x1) {}
         db->tx_pkt_cnt = 0;
      }else{
         db->_output_r++;
      }

   }else{
      while(ior(db,DM9KS_TCR) & 0x1) {}
      db->tx_pkt_cnt = 0;
   }

   if(db->tx_pkt_cnt <= 0 && _eth_dm9000a_desc_wr>=0) {
#if defined(__KERNEL_UCORE_EMBOS)
      __fire_io_int(ofile_lst[_eth_dm9000a_desc_wr].owner_pthread_ptr_write);
#elif defined(__KERNEL_UCORE_ECOS)
      __set_flag_fire_o_int();
#endif
   }

   //netif_wake_queue(dev);

   return;
}
/*--------------------------------------------
| Name:        check_rx_ready
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
#if !defined(CHECKSUM)
   #define check_rx_ready(a)       ((a) == 0x01)
#else
inline u8 check_rx_ready(u8 rxbyte)
{
   if (!(rxbyte & 0x01))
      return 0;
   return ((rxbyte >> 4) | 0x01);
}
#endif

/*--------------------------------------------
| Name:        dmfe_packet_receive
| Description: Received a packet and pass to upper layer
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static void dmfe_packet_receive(board_info_t *db){
   //struct sk_buff *skb;
   u8 rxbyte, val;
   u32 i, GoodPacket, tmplen = 0, MDRAH, MDRAL;
   u32 tmpdata;
   u32 _data=0;

   rx_t rx;

   u16 * ptr = (u16*)&rx;
   u8* rdptr;

   DMFE_DBUG(0, "dmfe_packet_receive()", 0);

   db->cont_rx_pkt_cnt=0;

   do {
      /*store the value of Memory Data Read address register*/
      MDRAH=ior(db, DM9KS_MDRAH);
      MDRAL=ior(db, DM9KS_MDRAL);

      ior(db, DM9KS_MRCMDX);                    /* Dummy read */
      rxbyte = inb(db->io_data);                /* Got most updated data */

      /* packet ready to receive check */
      if(!(val = check_rx_ready(rxbyte)))
         break;

      /* A packet ready now  & Get status/length */
      GoodPacket = TRUE;
      outb(DM9KS_MRCMD, db->io_addr);

      /* Read packet status & length */
      switch (db->io_mode) {
      case DM9KS_BYTE_MODE:
         *ptr = inb(db->io_data) +
                (inb(db->io_data) << 8);
         *(ptr+1) = inb(db->io_data) +
                    (inb(db->io_data) << 8);
         break;
      case DM9KS_WORD_MODE:
         *ptr = inw(db->io_data);
         *(ptr+1)    = inw(db->io_data);
         break;
      case DM9KS_DWORD_MODE:
         tmpdata  = inl(db->io_data);
         *ptr = (u16)tmpdata;
         *(ptr+1)    = (u16)(tmpdata >> 16);
         break;
      default:
         break;
      }

      /* Packet status check */
      if (rx.desc.status & 0xbf)
      {
         GoodPacket = FALSE;
         /*
                        if (rx.desc.status & 0x01)
                        {
                                db->stats.rx_fifo_errors++;
                                printk("<RX FIFO error>\n");
                        }
                        if (rx.desc.status & 0x02)
                        {
                                db->stats.rx_crc_errors++;
                                printk("<RX CRC error>\n");
                        }
                        if (rx.desc.status & 0x80)
                        {
                                db->stats.rx_length_errors++;
                                printk("<RX Length error>\n");
                        }
                        if (rx.desc.status & 0x08)
                                printk("<Physical Layer error>\n");
         */
      }

      if (!GoodPacket
          || _eth_dm9000a_desc_rd<0
          || ( (_eth_dm9000a_desc_rd>=0) && (ofile_lst[_eth_dm9000a_desc_rd].nb_reader<=0) ) ) {
         // drop this packet!!!
         switch (db->io_mode) {
          
         case DM9KS_BYTE_MODE:
            for (i=0; i<rx.desc.length; i++) {
               _data = inb(db->io_data);
            }
            break;
         case DM9KS_WORD_MODE:
            tmplen = (rx.desc.length + 1) / 2;
            for (i = 0; i < tmplen; i++) {
               _data = inw(db->io_data);
            }
            break;
         case DM9KS_DWORD_MODE:
            tmplen = (rx.desc.length + 3) / 4;
            for (i = 0; i < tmplen; i++) {
               _data = inl(db->io_data);
            }
            break;
         }
         continue;               /*next the packet*/
      }


      //skb = dev_alloc_skb(rx.desc.length+4);
      if (db->_ptr_eth_dm9000a_input_buffer == NULL) {
         ///re-load the value into Memory data read address register
         iow(db,DM9KS_MDRAH,(u8)MDRAH);
         iow(db,DM9KS_MDRAL,(u8)MDRAL);
         return;
      }else{
         u32 pos;
         u32 byte_no;
         u8 bit_no;
         u8 msk;
         int _tmp_w = ((db->_input_tmp_w+rx.desc.length)&(~ETH_DM9000_INPUT_BUFFER_SZ));

         if(db->_input_tmp_w<db->_input_r && _tmp_w>=db->_input_r)
            _tmp_w=0;  //error overrun

         // Read received packet from RX SARM
         rdptr = db->_ptr_eth_dm9000a_input_buffer;

         //
         if(_eth_dm9000a_desc_rd>=0) {
#if defined(__KERNEL_UCORE_EMBOS)
            __fire_io_int(ofile_lst[_eth_dm9000a_desc_rd].owner_pthread_ptr_read);
#elif defined(__KERNEL_UCORE_ECOS)
            __set_flag_fire_i_int();
#endif
         }
         //__fire_io_int(ofile_lst[_eth_dm9000a_desc_rd].owner_pthread_ptr_read);
         //
         switch (db->io_mode) {
         case DM9KS_BYTE_MODE:
            for (i=0; i<rx.desc.length; i++) {
               rdptr[db->_input_tmp_w]=inb(db->io_data);
               db->_input_tmp_w=((db->_input_tmp_w+1)&(~ETH_DM9000_INPUT_BUFFER_SZ));
            }
            break;
         case DM9KS_WORD_MODE:
            tmplen = (rx.desc.length + 1) / 2;
            for (i = 0; i < tmplen; i++) {
               ((u16 *)rdptr)[db->_input_tmp_w/2] = inw(db->io_data);
               db->_input_tmp_w=((db->_input_tmp_w+2)&(~ETH_DM9000_INPUT_BUFFER_SZ));
            }
            break;
         case DM9KS_DWORD_MODE:
            tmplen = (rx.desc.length + 3) / 4;
            for (i = 0; i < tmplen; i++) {
               ((u32 *)rdptr)[db->_input_tmp_w/4] = inl(db->io_data);
               db->_input_tmp_w=((db->_input_tmp_w+4)&(~ETH_DM9000_INPUT_BUFFER_SZ));
            }
            break;
         }

         pos = db->_input_tmp_w;
         byte_no = pos>>3;
         bit_no  = pos-(byte_no<<3);
         msk = 0x01<<bit_no;
         //
         db->ptr_eth_frame_vector[byte_no]|=msk; //valid this rtu frame

         /* Pass to upper layer */
         //skb->protocol = eth_type_trans(skb,dev);
#if defined(CHECKSUM)
         if (val == 0x01)
            skb->ip_summed = CHECKSUM_UNNECESSARY;
#endif
         db->cont_rx_pkt_cnt++;
#ifdef DBUG /* check RX FIFO pointer */
         u16 MDRAH1, MDRAL1;
         u16 tmp_ptr;
         MDRAH1 = ior(db,DM9KS_MDRAH);
         MDRAL1 = ior(db,DM9KS_MDRAL);
         tmp_ptr = (MDRAH<<8)|MDRAL;
         switch (db->io_mode) {
         case DM9KS_BYTE_MODE:
            tmp_ptr += rx.desc.length+4;
            break;
         case DM9KS_WORD_MODE:
            tmp_ptr += ((rx.desc.length+1)/2)*2+4;
            break;
         case DM9KS_DWORD_MODE:
            tmp_ptr += ((rx.desc.length+3)/4)*4+4;
            break;
         }
         if (tmp_ptr >=0x4000)
            tmp_ptr = (tmp_ptr - 0x4000) + 0xc00;
         if (tmp_ptr != ((MDRAH1<<8)|MDRAL1))
            printk("[dm9ks:RX FIFO ERROR\n");
#endif
         if (db->cont_rx_pkt_cnt>=CONT_RX_PKT_CNT) {
            dmfe_tx_done(db,0);
            break;
         }
      }

   } while((rxbyte & 0x01) == DM9KS_PKT_RDY);
   DMFE_DBUG(0, "[END]dmfe_packet_receive()", 0);

}

/*--------------------------------------------
| Name:        dmfe_interrupt
| Description: DM9000 insterrupt handler
| receive the packet to upper layer, free the transmitted packet
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
#if defined(__KERNEL_UCORE_EMBOS)
static void dmfe_interrupt(void) {
#elif defined(__KERNEL_UCORE_ECOS)
cyg_uint32 dev_eth_dm9000a_interrupt_isr(cyg_vector_t vector, cyg_addrword_t data) {
   cyg_interrupt_mask(vector);
#endif
   board_info_t * db= &g_board_info;         /* Point a board information structure */
   int int_status,i;
   u8 reg_save;

   //OS_EnterNestableInterrupt();see RTOSINIT_ATXXX.c
   //arm7
#if defined(__KERNEL_UCORE_EMBOS)
   *AT91C_AIC_IVR = 0; // Debug variant of vector read, protected mode is used.
   *AT91C_AIC_ICCR = 1 << AT91C_ID_IRQ1; // Clears INT1 interrupt.
#endif

   DMFE_DBUG(0, "dmfe_interrupt()", 0);

   /* A real interrupt coming */
   //spin_lock(&db->lock);

   /* Save previous register address */
   reg_save = inb(db->io_addr);

   /* Disable all interrupt */
   iow(db, DM9KS_IMR, DM9KS_DISINTR);

   /* Got DM9000A/DM9010 interrupt status */
   int_status = ior(db, DM9KS_ISR);                     /* Got ISR */
   iow(db, DM9KS_ISR, int_status);              /* Clear ISR status */

   /* Link status change */
   if(int_status & DM9KS_LINK_INTR) {
      //netif_stop_queue(dev);
      __set_eth_status(db->eth_stat,ETH_STAT_LINK_DOWN);
      for(i=0; i<500; i++) {
         /*wait link OK, waiting time =0.5s */
         phy_read(db,0x1);
         if(phy_read(db,0x1) & 0x4) {               /*Link OK*/
            /* wait for detected Speed */
            for(i=0; i<200; i++)
               udelay(/*1000*/ 50);
            /* set media speed */
            if(phy_read(db,0)&0x2000) {
               db->Speed =100;
               __set_eth_status(db->eth_stat,ETH_STAT_LINK_100);
            }else{
               db->Speed =10;
               __set_eth_status(db->eth_stat,ETH_STAT_LINK_10);
            }
            break;
         }
         udelay(1000);
      }
      //netif_wake_queue(dev);
      //printk("[INTR]i=%d speed=%d\n",i, (int)(db->Speed));
   }
   /* Received the coming packet */
   if (int_status & DM9KS_RX_INTR)
      dmfe_packet_receive(db);

   /* Transmit Interrupt check */
   if (int_status & DM9KS_TX_INTR)
      dmfe_tx_done(db,0);

   if (db->cont_rx_pkt_cnt>=CONT_RX_PKT_CNT) {
      iow(db, DM9KS_IMR, 0xa2);
   }else{
      /* Re-enable interrupt mask */
      iow(db, DM9KS_IMR, DM9KS_REGFF);
   }

   /* Restore previous register address */
   outb(reg_save, db->io_addr);
#if defined(__KERNEL_UCORE_EMBOS)
   *AT91C_AIC_EOICR = 0; // Signal end of interrupt to AIC.
#elif defined(__KERNEL_UCORE_ECOS)
   //spin_unlock(&db->lock);
   //OS_LeaveNestableInterrupt();see RTOSINIT_ATXXX.c
   cyg_interrupt_acknowledge(vector);
   //Informe kernel d'exécuter DSR
   return(CYG_ISR_HANDLED | CYG_ISR_CALL_DSR);
#endif
}

#if defined(__KERNEL_UCORE_ECOS)
void dev_eth_dm9000a_interrupt_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data) {
   board_info_t * db= &g_board_info;    /* Point a board information structure */
   //
   if(__is_set_flag_fire_i_int()) {
      __fire_io_int(ofile_lst[_eth_dm9000a_desc_rd].owner_pthread_ptr_read);
      __unset_flag_fire_i_int();
   }

   if(__is_set_flag_fire_o_int()) {
      __fire_io_int(ofile_lst[_eth_dm9000a_desc_wr].owner_pthread_ptr_write);
      __unset_flag_fire_o_int();
   }

   //autorise � nouveau les IT
   cyg_interrupt_unmask(vector);
}
#endif
/*-------------------------------------------
| Name:dev_eth_dm9000a_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eth_dm9000a_load(dev_io_info_t* p_dev_io_info){
   _eth_dm9000a_desc_rd = -1;
   _eth_dm9000a_desc_wr = -1;


   pthread_mutexattr_t mutex_attr=0;

   //init board information structure
   memset(&g_board_info, 0, sizeof(struct board_info));
   //
   kernel_pthread_mutex_init(&g_board_info.mutex,&mutex_attr);

   //reset status
   g_board_info.eth_stat=0;

   //get io info
   g_board_info.io_addr  = p_dev_io_info->io_addr;
   g_board_info.io_data  = p_dev_io_info->io_data;
   g_board_info.irq_no   = p_dev_io_info->irq_no;
   g_board_info.irq_prio = p_dev_io_info->irq_prio;

   //
   if(dmfe_probe1(&g_board_info)<0)
      return -1;

   g_board_info._ptr_eth_dm9000a_input_buffer =  (u8*) malloc(ETH_DM9000_INPUT_BUFFER_SZ+20);
   if(g_board_info._ptr_eth_dm9000a_input_buffer==(u8*)0)
      return -1;

   g_board_info.ptr_eth_frame_vector =  (u8*) malloc(__ETH_FRAME_VECTOR_SZ);
   if(g_board_info._ptr_eth_dm9000a_input_buffer==(u8*)0)
      return -1;

   memset(g_board_info.ptr_eth_frame_vector,0,__ETH_FRAME_VECTOR_SZ);
   g_board_info._input_r=0;
   g_board_info._input_w=0;
   g_board_info._input_tmp_w=0;
   g_board_info._output_w=0;
   g_board_info._output_r=0;

#if defined(__KERNEL_UCORE_EMBOS)
   // IRQ1  interrupt vector.
   AT91C_AIC_SVR[AT91C_ID_IRQ1] = (unsigned long)&dmfe_interrupt;
   // SRCTYPE=3, PRIOR=3. INT1 interrupt positive edge-triggered at prio 3.
   AT91C_AIC_SMR[AT91C_ID_IRQ1] = 0x63;

   //phlb modif 02/12/2008: now in device open driver interface
   //*AT91C_AIC_ICCR = 1 << AT91C_ID_IRQ1; // Clears INT1 interrupt.
   //*AT91C_AIC_IECR = 1 << AT91C_ID_IRQ1; // Enable INT1 interrupt.
#elif defined(__KERNEL_UCORE_ECOS)
   {
      cyg_interrupt_create(
         (cyg_vector_t)g_board_info.irq_no, (cyg_priority_t)g_board_info.irq_prio, 0,
         (p_dev_io_info->p_fct_isr ? p_dev_io_info->p_fct_isr : &
          dev_eth_dm9000a_interrupt_isr),
         &dev_eth_dm9000a_interrupt_dsr,
         &_eth_dm9000_handle, &_eth_dm9000_it);
      //
      cyg_interrupt_configure(g_board_info.irq_no, 0 /*edge*/, 1 /*rising*/ );
      //Liaison entre l'IT crée et le vecteur d'IT
      cyg_interrupt_attach(_eth_dm9000_handle);
   }
#endif

   return 0;
}

/*-------------------------------------------
| Name:dev_eth_dm9000a_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eth_dm9000a_open(desc_t desc, int o_flag){

   if(_eth_dm9000a_desc_rd<0 && _eth_dm9000a_desc_wr<0) {

#if defined(__KERNEL_UCORE_EMBOS)
      *AT91C_AIC_ICCR = 1 << AT91C_ID_IRQ1;    // Clears INT1 interrupt.
      *AT91C_AIC_IECR = 1 << AT91C_ID_IRQ1;    // Enable INT1 interrupt.
#elif defined(__KERNEL_UCORE_ECOS)
      cyg_vector_t _ecos_vector = g_board_info.irq_no;
      cyg_interrupt_acknowledge(_ecos_vector);
      cyg_interrupt_unmask(_ecos_vector);
#endif

      if(dmfe_open(&g_board_info)<0)
         return -1;

   }

   //
   if(o_flag & O_RDONLY) {
      if(_eth_dm9000a_desc_rd<0)
         _eth_dm9000a_desc_rd = desc;
      else
         return -1;  //already open

      //_eth_dm9000a_input_r = 0;
      //_eth_dm9000a_input_w = 0;

      if(!ofile_lst[desc].p)
         ofile_lst[desc].p=&g_board_info;
   }

   if(o_flag & O_WRONLY) {
      if(_eth_dm9000a_desc_wr<0)
         _eth_dm9000a_desc_wr = desc;
      else
         return -1;  //already open

      if(!ofile_lst[desc].p)
         ofile_lst[desc].p=&g_board_info;

      //_eth_dm9000a_output_r = 0xFFFF;
      //_eth_dm9000a_output_w = 0;
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_eth_dm9000a_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eth_dm9000a_close(desc_t desc){
   if(ofile_lst[desc].oflag & O_RDONLY) {
      if(!ofile_lst[desc].nb_reader) {
         _eth_dm9000a_desc_rd = -1;
      }
   }

   if(ofile_lst[desc].oflag & O_WRONLY) {
      if(!ofile_lst[desc].nb_writer) {
         _eth_dm9000a_desc_wr = -1;
      }
   }

   if(!ofile_lst[desc].nb_writer
      &&!ofile_lst[desc].nb_reader) {
      dmfe_stop(&g_board_info);
      *AT91C_AIC_IDCR = 1 << AT91C_ID_IRQ1; // Disable INT1 interrupt.
      *AT91C_AIC_ICCR = 1 << AT91C_ID_IRQ1; // Clears INT1 interrupt.
   }
   return 0;
}

/*-------------------------------------------
| Name:dev_eth_dm9000a_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eth_dm9000a_isset_read(desc_t desc){

   board_info_t * db=(board_info_t *)ofile_lst[desc].p;
   if(!db)
      return -1;

   if((db->_input_tmp_w!=db->_input_r)) {
      u32 _last_pos = db->_input_w>>3;
      u32 _byte     = _last_pos;
      u8 _bit      = 0;
      //
      do {
         if(db->ptr_eth_frame_vector[_byte])
            break;
      } while((_byte=((++_byte)%__ETH_FRAME_VECTOR_SZ))!=_last_pos);
      //
      if(_byte==_last_pos && !db->ptr_eth_frame_vector[_byte])
         return -1;

      //
      for(_bit=0; _bit<8; _bit++) {
         if( ((db->ptr_eth_frame_vector[_byte]>>_bit)&0x01) )
            break;
      }

      //unset bit in vector
      db->ptr_eth_frame_vector[_byte]&=~(0x01<<_bit);
      //phlb: bug test
      if(_byte>=__ETH_FRAME_VECTOR_SZ)
         return -1;

      db->_input_w = (_byte<<3)+_bit;
      return 0;
   }else{
      //reenable interrupt
      if (db->cont_rx_pkt_cnt>=CONT_RX_PKT_CNT) {
         db->cont_rx_pkt_cnt=0;
         iow(db, DM9KS_IMR, DM9KS_REGFF);
      }
      return -1;
   }
}

/*-------------------------------------------
| Name:dev_eth_dm9000a_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eth_dm9000a_isset_write(desc_t desc){
   board_info_t * db=(board_info_t *)ofile_lst[desc].p;
   if(!db)
      return -1;
   if(db->tx_pkt_cnt<=0)
      return 0;
   return -1;
}
/*-------------------------------------------
| Name:dev_eth_dm9000a_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eth_dm9000a_read(desc_t desc, char* buf,int size){

   u32 w = 0;
   u32 r = 0;
   u32 l = 0;
   int cb=0;
   board_info_t * db=(board_info_t *)ofile_lst[desc].p;
   if(!db)
      return -1;


   r= db->_input_r;
   w= db->_input_w;

   if(r==w)
      return 0;

   if(w>=r) {
      l=w-r;
      if(size<l)
         l=size;
      memcpy(buf,&db->_ptr_eth_dm9000a_input_buffer[r],l);
      db->_input_r =w;
      cb=l;
   }else{
      l=(ETH_DM9000_INPUT_BUFFER_SZ-r+w);
      if(size<l) {
         if(size<(ETH_DM9000_INPUT_BUFFER_SZ-r)) {
            cb=size;
            memcpy(buf,&db->_ptr_eth_dm9000a_input_buffer[r],cb);
            db->_input_r +=cb;
         }else{
            cb=(ETH_DM9000_INPUT_BUFFER_SZ-r);
            memcpy(buf,&db->_ptr_eth_dm9000a_input_buffer[r],cb);
            memcpy(buf+cb,&db->_ptr_eth_dm9000a_input_buffer[0],size-(ETH_DM9000_INPUT_BUFFER_SZ-r));
            db->_input_r =size-(ETH_DM9000_INPUT_BUFFER_SZ-r);
            cb=size;
         }
      }else{
         cb=(ETH_DM9000_INPUT_BUFFER_SZ-r);
         memcpy(buf,&db->_ptr_eth_dm9000a_input_buffer[r],cb);
         memcpy(buf+cb,&db->_ptr_eth_dm9000a_input_buffer[0],l-(ETH_DM9000_INPUT_BUFFER_SZ-r));
         db->_input_r =l-(ETH_DM9000_INPUT_BUFFER_SZ-r);
         cb=l;
      }
   }


   return cb;
}

/*-------------------------------------------
| Name:dev_eth_dm9000a_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eth_dm9000a_write(desc_t desc, const char* buf,int size){
   int r;
   board_info_t * db=(board_info_t *)ofile_lst[desc].p;
   if(!db)
      return -1;

   kernel_pthread_mutex_lock(&db->mutex);
   if(dmfe_start_xmit(db,(u8*)buf,size)<0) {
      kernel_pthread_mutex_unlock(&db->mutex);
      return -1;
   }

   //unlock in dmfe_tx_done()
   return size;
}

/*-------------------------------------------
| Name:dev_eth_dm9000a_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eth_dm9000a_seek(desc_t desc,int offset,int origin){
   return -1;
}

/*-------------------------------------------
| Name:dev_eth_dm9000a_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eth_dm9000a_ioctl(desc_t desc,int request,va_list ap){

   switch(request) {

   //reset interface
   case ETHRESET: {
      board_info_t * db=(board_info_t *)ofile_lst[desc].p;
      if(!db)
         return -1;
      dmfe_reset(db);
   }
   break;

   //status interface
   case ETHSTAT: {
      board_info_t* db=(board_info_t *)ofile_lst[desc].p;
      eth_stat_t* p_eth_stat = va_arg( ap, eth_stat_t*);
      if(!db || !p_eth_stat)
         return -1;
      //
      *p_eth_stat = db->eth_stat;
   }
   break;

   case ETHSETHWADDRESS: {
      board_info_t* db=(board_info_t *)ofile_lst[desc].p;
      unsigned char* p_eth_hwaddr = va_arg( ap, unsigned char*);
      if(!db || !p_eth_hwaddr)
         return -1;
      //stop ehternet interface
      dmfe_stop(db);
      /* Disable all interrupt */
      iow(db, DM9KS_IMR, DM9KS_DISINTR);
      //
      *AT91C_AIC_IDCR = 1 << AT91C_ID_IRQ1;    // Disable INT1 interrupt.
      *AT91C_AIC_ICCR = 1 << AT91C_ID_IRQ1;    // Clears INT1 interrupt.

      //set mac address
      db->mac_addr[0] = p_eth_hwaddr[0];
      db->mac_addr[1] = p_eth_hwaddr[1];
      db->mac_addr[2] = p_eth_hwaddr[2];
      db->mac_addr[3] = p_eth_hwaddr[3];
      db->mac_addr[4] = p_eth_hwaddr[4];
      db->mac_addr[5] = p_eth_hwaddr[5];

      //
      *AT91C_AIC_ICCR = 1 << AT91C_ID_IRQ1;    // Clears INT1 interrupt.
      *AT91C_AIC_IECR = 1 << AT91C_ID_IRQ1;    // Enable INT1 interrupt.
      //
      __flush_eth_frame_buffer(db);
      //reopen and restart ethernet interface
      dmfe_open(db);
   }
   break;

   case ETHGETHWADDRESS: {
      board_info_t* db=(board_info_t *)ofile_lst[desc].p;
      unsigned char* p_eth_hwaddr = va_arg( ap, unsigned char*);
      if(!db || !p_eth_hwaddr)
         return -1;
      p_eth_hwaddr[0] = db->mac_addr[0];
      p_eth_hwaddr[1] = db->mac_addr[1];
      p_eth_hwaddr[2] = db->mac_addr[2];
      p_eth_hwaddr[3] = db->mac_addr[3];
      p_eth_hwaddr[4] = db->mac_addr[4];
      p_eth_hwaddr[5] = db->mac_addr[5];
   }
   break;

   //
   default:
      return -1;
   }


   return 0;
}

/*============================================
| End of Source  : dev_eth_dm9000a.c
==============================================*/
