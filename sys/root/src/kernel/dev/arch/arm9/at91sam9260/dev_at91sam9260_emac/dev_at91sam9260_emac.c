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
| Includes
==============================================*/

// Lepton
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/kernel_pthread.h"
#include "kernel/core/io.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "lib/libc/termios/termios.h"
#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/core/ioctl_eth.h"

// eCos
#include "cyg/hal/at91sam9260.h"
#include "cyg/hal/hal_platform_ints.h"
#include <string.h>
#include <stdlib.h>

// drivers
#include "dev_at91sam9260_phy.h"

/*============================================
| Global Declaration
==============================================*/
#define EMAC_PHY_ADDR               31

/// Number of buffer for RX, be carreful: MUST be 2^n
#define RX_BUFFERS  16
/// Number of buffer for TX, be carreful: MUST be 2^n
#define TX_BUFFERS   8

/// Buffer Size
#define EMAC_RX_UNITSIZE            128     /// Fixed size for RX buffer
#define EMAC_TX_UNITSIZE            1518    /// Size for ETH frame length

// The MAC can support frame lengths up to 1536 bytes.
#define EMAC_FRAME_LENTGH_MAX       1536

// Private Data structures
/// The buffer addresses written into the descriptors must be aligned so the
/// last few bits are zero.  These bits have special meaning for the EMAC
/// peripheral and cannot be used as part of the address.
#define EMAC_ADDRESS_MASK   ((unsigned int)0xFFFFFFFC)
#define EMAC_LENGTH_FRAME   ((unsigned int)0x0FFF)    /// Length of frame mask

// receive buffer descriptor bits
#define EMAC_RX_OWNERSHIP_BIT   (1 <<  0)
#define EMAC_RX_WRAP_BIT        (1 <<  1)
#define EMAC_RX_SOF_BIT         (1 << 14)
#define EMAC_RX_EOF_BIT         (1 << 15)

// Transmit buffer descriptor bits
#define EMAC_TX_LAST_BUFFER_BIT (1 << 15)
#define EMAC_TX_WRAP_BIT        (1 << 30)
#define EMAC_TX_USED_BIT        (1 << 31)

//-----------------------------------------------------------------------------
// Circular buffer management
//-----------------------------------------------------------------------------
// Return count in buffer
#define CIRC_CNT(head,tail,size) (((head) - (tail)) & ((size)-1))

// Return space available, 0..size-1
// We always leave one free char as a completely full buffer
// has head == tail, which is the same as empty
#define CIRC_SPACE(head,tail,size) CIRC_CNT((tail),((head)+1),(size))

// Return count up to the end of the buffer.
// Carefully avoid accessing head and tail more than once,
// so they can change underneath us without returning inconsistent results
#define CIRC_CNT_TO_END(head,tail,size) \
   ({int end = (size) - (tail); \
     int n = ((head) + end) & ((size)-1); \
     n < end ? n : end; })

// Return space available up to the end of the buffer
#define CIRC_SPACE_TO_END(head,tail,size) \
   ({int end = (size) - 1 - (head); \
     int n = (end + (tail)) & ((size)-1); \
     n <= end ? n : end+1; })

// Increment head or tail
#define CIRC_INC(headortail,size) \
   headortail++;             \
   if(headortail >= size) {  \
      headortail = 0;       \
   }

#define CIRC_EMPTY(circ)     ((circ)->head == (circ)->tail)
#define CIRC_CLEAR(circ)     ((circ)->head = (circ)->tail = 0)

/// Describes the type and attribute of Receive Transfer descriptor.
typedef struct _EmacRxTDescriptor {
   unsigned int addr;
   unsigned int status;
} __attribute__((packed, aligned(8))) EmacRxTDescriptor, *PEmacRxTDescriptor;

/// Describes the type and attribute of Transmit Transfer descriptor.
typedef struct _EmacTxTDescriptor {
   unsigned int addr;
   unsigned int status;
} __attribute__((packed, aligned(8))) EmacTxTDescriptor, *PEmacTxTDescriptor;

/// Descriptors for RX (required aligned by 8)
typedef struct {
   volatile EmacRxTDescriptor td[RX_BUFFERS];
   unsigned short idx;
} RxTd;

/// Descriptors for TX (required aligned by 8)
typedef struct {
   volatile EmacTxTDescriptor td[TX_BUFFERS];
   unsigned short head;            /// Circular buffer head pointer incremented by the upper layer (buffer to be sent)
   unsigned short tail;            /// Circular buffer head pointer incremented by the IT handler (buffer sent)
} TxTd;

//-----------------------------------------------------------------------------
/// Describes the statistics of the EMAC.
//-----------------------------------------------------------------------------
typedef struct _EmacStats {

   // TX errors
   unsigned int tx_packets;     /// Total Number of packets sent
   unsigned int tx_comp;        /// Packet complete
   unsigned int tx_errors;      /// TX errors ( Retry Limit Exceed )
   unsigned int collisions;     /// Collision
   unsigned int tx_exausts;     /// Buffer exhausted
   unsigned int tx_underruns;   /// Under Run, not able to read from memory
   // RX errors
   unsigned int rx_packets;     /// Total Number of packets RX
   unsigned int rx_eof;         /// No EOF error
   unsigned int rx_ovrs;        /// Over Run, not able to store to memory
   unsigned int rx_bnas;        /// Buffer is not available

} EmacStats, *PEmacStats;


static const cyg_uint8 default_mac_addr[6] = {0x00, 0x45, 0x56, 0x78, 0x9a, 0xcc};
static const char dev_at91sam9260_emac_name[]="eth0\0";

// Receive Transfer Descriptor buffer
static volatile RxTd rxTd __attribute__ ((aligned (4), section (".no_cache")));
// Transmit Transfer Descriptor buffer
static volatile TxTd txTd __attribute__ ((aligned (4), section (".no_cache")));
/// Send Buffer
// Section 3.6 of AMBA 2.0 spec states that burst should not cross 1K Boundaries.
// Receive buffer manager writes are burst of 2 words => 3 lsb bits of the address shall be set to 0
static volatile unsigned char pTxBuffer[TX_BUFFERS * EMAC_TX_UNITSIZE] __attribute__ ((aligned (4), section (".no_cache"))); //__attribute__((aligned(8)));
/// Receive Buffer
static volatile unsigned char pRxBuffer[RX_BUFFERS * EMAC_RX_UNITSIZE] __attribute__ ((aligned (4), section (".no_cache"))); //__attribute__((aligned(8)));
/// Statistics
static volatile EmacStats EmacStatistics __attribute__ ((aligned (4), section (".no_cache")));

typedef struct
{
   // trucs mac+phy
   cyg_uint8 mac_addr[6];
   phy_stuff_t dm9161;

   // machins eCos
   cyg_handle_t emac_handle;
   cyg_interrupt emac_it;

   // bidules lepton
   kernel_pthread_mutex_t mutex;
   desc_t desc_rd;
   desc_t desc_wr;
   desc_t desc_emac;

} at91_eth_priv_t;

// Interfaces du driver
static int dev_at91sam9260_emac_load(void);
static int dev_at91sam9260_emac_open(desc_t desc, int o_flag);
static int dev_at91sam9260_emac_close(desc_t desc);
static int dev_at91sam9260_emac_isset_read(desc_t desc);
static int dev_at91sam9260_emac_isset_write(desc_t desc);
static int dev_at91sam9260_emac_read(desc_t desc, char* buf,int size);
static int dev_at91sam9260_emac_write(desc_t desc, const char* buf,int size);
static int dev_at91sam9260_emac_seek(desc_t desc,int offset,int origin);
static int dev_at91sam9260_emac_ioctl(desc_t desc,int request,va_list ap);

// Fonctions internes
static bool at91_eth_init(at91_eth_priv_t *priv);
static cyg_uint32 at91_eth_isr (cyg_vector_t vector, cyg_addrword_t data);
static void at91_eth_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);
static void at91_set_mac(cyg_uint8 * enaddr);


dev_map_t dev_at91sam9260_eth0_map={
   dev_at91sam9260_emac_name,
   S_IFCHR,
   dev_at91sam9260_emac_load,
   dev_at91sam9260_emac_open,
   dev_at91sam9260_emac_close,
   dev_at91sam9260_emac_isset_read,
   dev_at91sam9260_emac_isset_write,
   dev_at91sam9260_emac_read,
   dev_at91sam9260_emac_write,
   dev_at91sam9260_emac_seek,
   dev_at91sam9260_emac_ioctl //ioctl
};


static at91_eth_priv_t g_board_info;

//============================================================================
// Interfaces Lepton
//

extern void emac_set_link_speed(unsigned char speed, unsigned char fullduplex);
extern void emac_enable_rmii( void );

static int dev_at91sam9260_emac_load(void)
{
   int i;
   volatile cyg_uint32 ncfg = 0;
   int Index;
   unsigned int Address;
   int errCount;
//   cyg_vector_t emac_vector = CYGNUM_HAL_INTERRUPT_EMAC;
//   cyg_priority_t emac_prior = 3;

   g_board_info.desc_rd = -1;
   g_board_info.desc_wr = -1;

   pthread_mutexattr_t mutex_attr = 0;
   kernel_pthread_mutex_init(&g_board_info.mutex,&mutex_attr);

   for(i=0; i<6; i++ ) {
      g_board_info.mac_addr[i] = default_mac_addr[i];
   }

   // Power ON
   AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_EMAC;
   // Disable TX & RX and more
   AT91C_BASE_EMACB->EMAC_NCR = 0;
   // disable
   AT91C_BASE_EMACB->EMAC_IDR = ~0;

   rxTd.idx = 0;
   CIRC_CLEAR(&txTd);

   // Setup the RX descriptors.
   for(Index = 0; Index < RX_BUFFERS; Index++) {

      Address = (unsigned int)(&(pRxBuffer[Index * EMAC_RX_UNITSIZE]));
      // Remove EMAC_RX_OWNERSHIP_BIT and EMAC_RX_WRAP_BIT
      rxTd.td[Index].addr = Address & EMAC_ADDRESS_MASK;
      rxTd.td[Index].status = 0;
   }
   rxTd.td[RX_BUFFERS - 1].addr |= EMAC_RX_WRAP_BIT;

   // Setup the TX descriptors.
   for(Index = 0; Index < TX_BUFFERS; Index++) {

      Address = (unsigned int)(&(pTxBuffer[Index * EMAC_TX_UNITSIZE]));
      txTd.td[Index].addr = Address;
      txTd.td[Index].status = EMAC_TX_USED_BIT;
   }
   txTd.td[TX_BUFFERS - 1].status = EMAC_TX_USED_BIT | EMAC_TX_WRAP_BIT;

   // Set the MAC address
   at91_set_mac(g_board_info.mac_addr);

   // Now setup the descriptors
   // Receive Buffer Queue Pointer Register
   AT91C_BASE_EMACB->EMAC_RBQP = (unsigned int) (rxTd.td);
   // Transmit Buffer Queue Pointer Register
   AT91C_BASE_EMACB->EMAC_TBQP = (unsigned int) (txTd.td);

   AT91C_BASE_EMACB->EMAC_NCR = AT91C_EMAC_CLRSTAT;

   // Clear all status bits in the receive status register.
   AT91C_BASE_EMACB->EMAC_RSR = (AT91C_EMAC_OVR | AT91C_EMAC_REC | AT91C_EMAC_BNA);

   // Clear all status bits in the transmit status register
   AT91C_BASE_EMACB->EMAC_TSR = ( AT91C_EMAC_UBR | AT91C_EMAC_COL | AT91C_EMAC_RLES
                                  | AT91C_EMAC_BEX | AT91C_EMAC_COMP
                                  | AT91C_EMAC_UND );

   // Clear interrupts
   AT91C_BASE_EMACB->EMAC_ISR;

   // Enable the copy of data into the buffers
   // ignore broadcasts, and don't copy FCS.
   AT91C_BASE_EMACB->EMAC_NCFGR |= (AT91C_EMAC_DRFCS | AT91C_EMAC_PAE);
/*
   if( enableCAF == EMAC_CAF_ENABLE ) {
      AT91C_BASE_EMACB->EMAC_NCFGR |= AT91C_EMAC_CAF;
   }
   if( enableNBC == EMAC_NBC_ENABLE ) {
      AT91C_BASE_EMACB->EMAC_NCFGR |= AT91C_EMAC_NBC;
   }
*/

   //phy initialization, return an error if no phy found
   // Init DM9161 driver
   dm9161_init(&g_board_info.dm9161, EMAC_PHY_ADDR);

   // PHY initialize
   if (!dm9161_init_phy(&g_board_info.dm9161)) {
      //printf("P: PHY Initialize ERROR!\n\r");
      return -1;
   }

   // Auto Negotiate
   if (!dm9161_auto_negotiate(&g_board_info.dm9161)) {
      //printf("P: Auto Negotiate ERROR!\n\r");
      return -1;
   }
   while( dm9161_get_link_speed(&g_board_info.dm9161, 1) == 0 ) {
      errCount++;
   }

//   cyg_interrupt_create(emac_vector, emac_prior, 0,
//         &at91_eth_isr, &at91_eth_dsr,
//         &g_board_info.emac_handle, &g_board_info.emac_it);
//   cyg_interrupt_attach(g_board_info.emac_handle);
//   cyg_interrupt_unmask(emac_vector);

   return 0;
}

static int dev_at91sam9260_emac_open(desc_t desc, int o_flag)
{
   cyg_vector_t emac_vector = CYGNUM_HAL_INTERRUPT_EMAC;
   cyg_priority_t emac_prior = 3;

   ofile_lst[desc].p = &g_board_info;

   // Switch on o_flag type and save it
   if (o_flag & O_RDONLY) {
      if(g_board_info.desc_rd<0) {
         g_board_info.desc_rd = desc;
      } else {
         return -1; //already open
      }
   }

   if (o_flag & O_WRONLY) {
      if(g_board_info.desc_wr<0) {
         g_board_info.desc_wr = desc;
      } else {
         return -1; //already open
      }
   }

   // save descriptor
   g_board_info.desc_emac = desc;

   // Enable Rx and Tx, plus the stats register.
   AT91C_BASE_EMACB->EMAC_NCR |= (AT91C_EMAC_TE | AT91C_EMAC_RE | AT91C_EMAC_WESTAT);

   // Setup the interrupts for Rx, TX and errors
   AT91C_BASE_EMACB->EMAC_IER = AT91C_EMAC_RXUBR
                                | AT91C_EMAC_TUNDR
                                | AT91C_EMAC_RLEX
                                | AT91C_EMAC_TXERR
                                | AT91C_EMAC_TCOMP
                                | AT91C_EMAC_RCOMP
                                | AT91C_EMAC_ROVR
                                | AT91C_EMAC_HRESP;

   //
   cyg_interrupt_create(emac_vector, emac_prior, 0,
                        &at91_eth_isr, &at91_eth_dsr,
                        &g_board_info.emac_handle, &g_board_info.emac_it);
   cyg_interrupt_attach(g_board_info.emac_handle);
   cyg_interrupt_unmask(emac_vector);

   return 0;
}

static int dev_at91sam9260_emac_close(desc_t desc)
{
   // Disable TX & RX and more
   AT91C_BASE_EMACB->EMAC_NCR = 0;
   // disable
   AT91C_BASE_EMACB->EMAC_IDR = ~0;

   if(ofile_lst[desc].oflag & O_RDONLY) {
      if(!ofile_lst[desc].nb_reader) {
         g_board_info.desc_rd = -1;
      }
   }
   if(ofile_lst[desc].oflag & O_WRONLY) {
      if(!ofile_lst[desc].nb_writer) {
         g_board_info.desc_wr = -1;
      }
   }

   return 0;
}

static int dev_at91sam9260_emac_isset_read(desc_t desc)
{
   unsigned int tmpIdx = rxTd.idx;
   volatile EmacRxTDescriptor *pRxTd = rxTd.td + rxTd.idx;

   // Process received RxTd
   while ((pRxTd->addr & EMAC_RX_OWNERSHIP_BIT) == EMAC_RX_OWNERSHIP_BIT) {
      // An end of frame has been received
      if ((pRxTd->status & EMAC_RX_EOF_BIT) == EMAC_RX_EOF_BIT) {
         return 0;
      }
      // Increment the pointer
      CIRC_INC(tmpIdx, RX_BUFFERS);
      // Process the next buffer
      pRxTd = rxTd.td + tmpIdx;
   }
   return -1;
}

static int dev_at91sam9260_emac_isset_write(desc_t desc)
{
   // Si plus de place, on sort
   if( CIRC_SPACE(txTd.head, txTd.tail, TX_BUFFERS) == 0) {
      return -1;
   } else {
      return 0;
   }
}


//-----------------------------------------------------------------------------
/// Receive a packet with EMAC
/// If not enough buffer for the packet, the remaining data is lost but right
/// frame length is returned.
/// \param pFrame           Buffer to store the frame
/// \param frameSize        Size of the frame
/// \param pRcvSize         Received size
/// \return                 OK, no data, or frame too small
//-----------------------------------------------------------------------------
/*
unsigned char EMAC_Poll(unsigned char *pFrame,
                        unsigned int frameSize,
                        unsigned int *pRcvSize)
*/
static int dev_at91sam9260_emac_read(desc_t desc, char* buf,int size)
{
   unsigned short bufferLength;
   unsigned int tmpFrameSize=0;
   char  *pTmpFrame=0;
   unsigned int tmpIdx = rxTd.idx;
   volatile EmacRxTDescriptor *pRxTd = rxTd.td + rxTd.idx;
   int pRcvSize;

   //  ASSERT(pFrame, "F: EMAC_Poll\n\r");

   char isFrame = 0;
   // Set the default return value
   pRcvSize = 0;

   // Process received RxTd
   while ((pRxTd->addr & EMAC_RX_OWNERSHIP_BIT) == EMAC_RX_OWNERSHIP_BIT) {

      // A start of frame has been received, discard previous fragments
      if ((pRxTd->status & EMAC_RX_SOF_BIT) == EMAC_RX_SOF_BIT) {
         // Skip previous fragment
         while (tmpIdx != rxTd.idx) {
            pRxTd = rxTd.td + rxTd.idx;
            pRxTd->addr &= ~(EMAC_RX_OWNERSHIP_BIT);
            CIRC_INC(rxTd.idx, RX_BUFFERS);
         }
         // Reset the temporary frame pointer
         pTmpFrame = buf;
         tmpFrameSize = 0;
         // Start to gather buffers in a frame
         isFrame = 1;
      }

      // Increment the pointer
      CIRC_INC(tmpIdx, RX_BUFFERS);

      // Copy data in the frame buffer
      if (isFrame) {
         if (tmpIdx == rxTd.idx) {
            //TRACE_INFO("no EOF (Invalid of buffers too small)\n\r");
            do {
               pRxTd = rxTd.td + rxTd.idx;
               pRxTd->addr &= ~(EMAC_RX_OWNERSHIP_BIT);
               CIRC_INC(rxTd.idx, RX_BUFFERS);
            } while(tmpIdx != rxTd.idx);
            return -1;
         }
         // Copy the buffer into the application frame
         bufferLength = EMAC_RX_UNITSIZE;
         if ((tmpFrameSize + bufferLength) > size) {
            bufferLength = size - tmpFrameSize;
         }

         memcpy(pTmpFrame, (void*)(pRxTd->addr & EMAC_ADDRESS_MASK), bufferLength);
         pTmpFrame += bufferLength;
         tmpFrameSize += bufferLength;

         // An end of frame has been received, return the data
         if ((pRxTd->status & EMAC_RX_EOF_BIT) == EMAC_RX_EOF_BIT) {
            // Frame size from the EMAC
            pRcvSize = (pRxTd->status & EMAC_LENGTH_FRAME);

            // Application frame buffer is too small all data have not been copied
            if (tmpFrameSize < pRcvSize) {
               //printf("size req %d size allocated %d\n\r", *pRcvSize, size);
               return tmpFrameSize;
            }

            //  TRACE_DEBUG("packet %d-%d (%d)\n\r", rxTd.idx, tmpIdx, pRcvSize);
            // All data have been copied in the application frame buffer => release TD
            while (rxTd.idx != tmpIdx) {
               pRxTd = rxTd.td + rxTd.idx;
               pRxTd->addr &= ~(EMAC_RX_OWNERSHIP_BIT);
               CIRC_INC(rxTd.idx, RX_BUFFERS);
            }
            EmacStatistics.rx_packets++;
            return pRcvSize;
         }
      }
      // SOF has not been detected, skip the fragment
      else {
         pRxTd->addr &= ~(EMAC_RX_OWNERSHIP_BIT);
         rxTd.idx = tmpIdx;
      }

      // Process the next buffer
      pRxTd = rxTd.td + tmpIdx;
   }

   //TRACE_DEBUG("E");
   // return EMAC_RX_NO_DATA;
   return -1;
}

static int dev_at91sam9260_emac_write(desc_t desc, const char* buf,int size)
{
   volatile EmacTxTDescriptor *pTxTd;
   // volatile EMAC_TxCallback   *pTxCb;

   kernel_pthread_mutex_lock(&g_board_info.mutex);
   //TRACE_DEBUG("EMAC_Send\n\r");

   // Check parameter
   if (size > EMAC_TX_UNITSIZE) {

      //  TRACE_ERROR("EMAC driver does not split send packets.");
      //  TRACE_ERROR(" It can send %d bytes max in one packet (%d bytes requested)\n\r",
      //     EMAC_TX_UNITSIZE, size);
      //return EMAC_TX_INVALID_PACKET;
      kernel_pthread_mutex_unlock(&g_board_info.mutex);
      return -1;
   }

   // If no free TxTd, buffer can't be sent, schedule the wakeup callback
   if( CIRC_SPACE(txTd.head, txTd.tail, TX_BUFFERS) == 0) {
      //return EMAC_TX_BUFFER_BUSY;
      kernel_pthread_mutex_unlock(&g_board_info.mutex);
      return -1;
   }

   // Pointers to the current TxTd
   pTxTd = txTd.td + txTd.head;
//   pTxCb = txTd.txCb + txTd.head;

   // Sanity check
//   ASSERT((pTxTd->status & EMAC_TX_USED_BIT) != 0,
//      "-F- Buffer is still under EMAC control\n\r");

   // Setup/Copy data to transmition buffer
   if (buf && size) {
      // Driver manage the ring buffer
      memcpy((void *)pTxTd->addr, buf, size);
   }

   // Tx Callback
   //  *pTxCb = fEMAC_TxCallback;

   // Update TD status
   // The buffer size defined is length of ethernet frame
   // so it's always the last buffer of the frame.
   if (txTd.head == (TX_BUFFERS-1) ) {
      pTxTd->status =
         (size & EMAC_LENGTH_FRAME) | EMAC_TX_LAST_BUFFER_BIT | EMAC_TX_WRAP_BIT;
   } else {
      pTxTd->status = (size & EMAC_LENGTH_FRAME) | EMAC_TX_LAST_BUFFER_BIT;
   }

   CIRC_INC(txTd.head, TX_BUFFERS)

   // Tx packets count
   EmacStatistics.tx_packets++;

   // Now start to transmit if it is not already done
   AT91C_BASE_EMACB->EMAC_NCR |= AT91C_EMAC_TSTART;

   kernel_pthread_mutex_unlock(&g_board_info.mutex);
   return size;
}

static int dev_at91sam9260_emac_seek(desc_t desc,int offset,int origin)
{
   return -1;
}

static int dev_at91sam9260_emac_ioctl(desc_t desc,int request,va_list ap)
{
   switch(request) {
   //reset interface
   case ETHRESET: {
      return -1;
   }
   break;

   //status interface
   case ETHSTAT: {
      return -1;
   }
   break;

   case ETHSETHWADDRESS: {
      return -1;
   }
   break;

   case ETHGETHWADDRESS: {
      unsigned char* p_eth_hwaddr = va_arg( ap, unsigned char*);
      if(!p_eth_hwaddr)
         return -1;
      p_eth_hwaddr[0] = g_board_info.mac_addr[0];
      p_eth_hwaddr[1] = g_board_info.mac_addr[1];
      p_eth_hwaddr[2] = g_board_info.mac_addr[2];
      p_eth_hwaddr[3] = g_board_info.mac_addr[3];
      p_eth_hwaddr[4] = g_board_info.mac_addr[4];
      p_eth_hwaddr[5] = g_board_info.mac_addr[5];
   }
   break;

   //
   default:
      return -1;
   }
   return 0;
}

//======================================================================
// Initialization code


// Set a specific address match to a given address. Packets received which
// match this address will be passed on.
static void at91_set_mac(cyg_uint8 * enaddr)
{
   AT91C_BASE_EMACB->EMAC_SA1L = ( ((unsigned int)enaddr[3] << 24)
                                   | ((unsigned int)enaddr[2] << 16)
                                   | ((unsigned int)enaddr[1] << 8 )
                                   | enaddr[0] );

   AT91C_BASE_EMACB->EMAC_SA1H = ( ((unsigned int)enaddr[5] << 8 )
                                   | enaddr[4] );
}


//======================================================================

static cyg_uint32 at91_eth_isr (cyg_vector_t vector, cyg_addrword_t data)
{
   cyg_uint32 ret;
   cyg_uint32 isr;

   cyg_interrupt_mask(vector);
   /* Get the interrupt status */
   isr = AT91C_BASE_EMACB->EMAC_ISR;
   ret = CYG_ISR_HANDLED;

   //TODO: We should probably be handling some of the error interrupts as well
   if(isr & AT91C_EMAC_TCOMP) {
      ret |= CYG_ISR_CALL_DSR;
   }
   if(isr & AT91C_EMAC_RCOMP) {
      ret |= CYG_ISR_CALL_DSR;
   }
   cyg_interrupt_acknowledge(vector);
   return(ret);
}


static void at91_eth_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
   volatile EmacTxTDescriptor *pTxTd;
   unsigned int isr;
   unsigned int rsr;
   unsigned int tsr;
   unsigned int rxStatusFlag;
   unsigned int txStatusFlag;

   //TRACE_DEBUG("EMAC_Handler\n\r");
   isr = AT91C_BASE_EMACB->EMAC_ISR & AT91C_BASE_EMACB->EMAC_IMR;
   rsr = AT91C_BASE_EMACB->EMAC_RSR;
   tsr = AT91C_BASE_EMACB->EMAC_TSR;

   // RX packet
   if ((isr & AT91C_EMAC_RCOMP) || (rsr & AT91C_EMAC_REC)) {
      rxStatusFlag = AT91C_EMAC_REC;
      // Frame received
      EmacStatistics.rx_packets++;

      // Check OVR
      if (rsr & AT91C_EMAC_OVR) {
         rxStatusFlag |= AT91C_EMAC_OVR;
         EmacStatistics.rx_ovrs++;
      }
      // Check BNA
      if (rsr & AT91C_EMAC_BNA) {
         rxStatusFlag |= AT91C_EMAC_BNA;
         EmacStatistics.rx_bnas++;
      }
      // Clear status
      AT91C_BASE_EMACB->EMAC_RSR |= rxStatusFlag;

      // Invoke callbacks
      //if (rxTd.rxCb) {
      //   rxTd.rxCb(rxStatusFlag);
      //}
      if(g_board_info.desc_rd >= 0)
         __fire_io_int(ofile_lst[g_board_info.desc_rd].owner_pthread_ptr_read);
   }

   // TX packet
   if ((isr & AT91C_EMAC_TCOMP) || (tsr & AT91C_EMAC_COMP)) {
      txStatusFlag = AT91C_EMAC_COMP;
      EmacStatistics.tx_comp++;

      // A frame transmitted
      // Check RLE
      if (tsr & AT91C_EMAC_RLES) {
         txStatusFlag |= AT91C_EMAC_RLES;
         EmacStatistics.tx_errors++;
      }
      // Check COL
      if (tsr & AT91C_EMAC_COL) {
         txStatusFlag |= AT91C_EMAC_COL;
         EmacStatistics.collisions++;
      }
      // Check BEX
      if (tsr & AT91C_EMAC_BEX) {
         txStatusFlag |= AT91C_EMAC_BEX;
         EmacStatistics.tx_exausts++;
      }
      // Check UND
      if (tsr & AT91C_EMAC_UND) {
         txStatusFlag |= AT91C_EMAC_UND;
         EmacStatistics.tx_underruns++;
      }
      // Clear status
      AT91C_BASE_EMACB->EMAC_TSR |= txStatusFlag;

      // Sanity check: Tx buffers have to be scheduled
      //  ASSERT(!CIRC_EMPTY(&txTd),
      //      "-F- EMAC Tx interrupt received meanwhile no TX buffers has been scheduled\n\r");

      // Check the buffers
      while (CIRC_CNT(txTd.head, txTd.tail, TX_BUFFERS)) {
         pTxTd = txTd.td + txTd.tail;

         // Exit if buffer has not been sent yet
         if ((pTxTd->status & EMAC_TX_USED_BIT) == 0) {
            break;
         }

         // Notify upper layer that packet has been sent
         //if (*pTxCb) {
         //    (*pTxCb)(txStatusFlag);
         //}
         if(g_board_info.desc_wr >= 0)
            __fire_io_int(ofile_lst[g_board_info.desc_wr].owner_pthread_ptr_write);

         CIRC_INC( txTd.tail, TX_BUFFERS );
      }
   }
   cyg_interrupt_unmask(vector);
}


/*============================================
| End of Source  : dev_at91sam9260_emac.c
==============================================*/
