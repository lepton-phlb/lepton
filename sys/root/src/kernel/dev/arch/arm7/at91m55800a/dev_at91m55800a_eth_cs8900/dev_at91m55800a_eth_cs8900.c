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
#include "kernel/core/ioctl_eth.h"

#include "lib/libc/termios/termios.h"

#include "kernel/fs/vfs/vfsdev.h"

/*============================================
| Global Declaration 
==============================================*/

//#define	IO_BASE_ADR    0x14180	// CS1 sur adresse $28000 + $300  <=> $14000 + $180 mots
//#define	MEM_CS8900_BASE_ADDR   0x14800	// CS1 sur adresse $28000 + $1000 <=> $14000 + $800 mots (A12=1)

//PHYCORE
#define	IO_CS8900_BASE_ADDR    0x20000300	// CS2 sur adresse $20000000 + $300
#define	MEM_CS8900_BASE_ADDR   0x20000000	// CS2 sur adresse $20000000 + $1000




/*  Registres en mode d'accès I/O  */
#define	CS89_RxTxData0	   0x0000 	// Receive/Transmit data Port 0
#define	CS89_RxTxData1	   0x0002 	// Receive/Transmit data Port 1
#define	CS89_TxCmd_IO  	0x0004 	// Transmit Command
#define	CS89_TxLength_IO  0x0006	// Transmit Length
#define	CS89_ISQ_IO	      0x0008	// Interrupt Status Queue
#define	CS89_PP_Ptr	      0x000A	// PacketPage Pointer
#define	CS89_PP_Data0	   0x000C	// PacketPage Data Port 0
#define	CS89_PP_Data1	   0x000E	// PacketPage Data Port 1



/*  Registres en mode d'accès MEMOIRE  */

// Bus Interface registers
//-------------------------//
#define	CS89_EISA	      0x0000  // EISA Registration number Crystal Semiconductor
#define	CS89_PID	         0x0002  // Product ID Number
#define	CS89_BaseIO	      0x0020  // I/O Base Address
#define	CS89_INT_NB	      0x0022  // Interrupt number (0,1,2, or 3)
#define  CS89_DMA_NB       0x0024  // DMA Channel number (0,1, or 2)
#define  CS89_DMA_SOF      0x0026  // DMA Start of Frame
#define  CS89_DMA_FCnt     0x0028  // DMA Frame count (12 bits)
#define  CS89_DMA_RxCnt    0x002A  // Rx DMA Byte count
#define	CS89_BaseMemory	0x002C  // Memory Base address register (20-bit)


// Configuration and control registers
//------------------------------------//
#define	CS89_RxCFG	      0x0102  // Receiver Configuration
#define SKIP_1             0x0040
#define Rx_STREAM_E        0x0080
#define Rx_OKi_E           0x0100
#define Rx_DMA_ONLY        0x0200
#define AUTORx_DMAE        0x0400
#define BUFFER_CRC         0x0800
#define Rx_CRC_ERROR_E     0x1000
#define Rx_RUNT_E          0x2000
#define Rx_EXTRA_DATA_E    0x4000

#define	CS89_RxCTL	         0x0104  // Receiver Control
#define Rx_IA_HASH_ACCEPT     0x0040
#define Rx_PROM_ACCEPT        0x0080
#define Rx_OK_ACCEPT          0x0100
#define Rx_MULTCAST_ACCEPT    0x0200
#define Rx_IA_ACCEPT          0x0400
#define Rx_BROADCAST_ACCEPT   0x0800
#define Rx_BAD_CRC_ACCEPT     0x1000
#define Rx_RUNT_ACCEPT        0x2000
#define Rx_EXTRA_DATA_ACCEPT  0x4000

#define	CS89_TxCFG	         0x0106  // Transmit Configuration
#define Tx_LOST_CRS_E         0x0040
#define Tx_SQE_ERROR_E        0x0080
#define Tx_OK_E               0x0100
#define Tx_LATE_COL_E         0x0200
#define Tx_JBR_E              0x0400
#define Tx_ANY_COL_E          0x0800
#define Tx_16_COL_E           0x8000

#define CS89_TxCmdRO	         0x0108	// Transmit Command Read Only Status
#define Tx_START_5_BYTES      0x0000
#define Tx_START_381_BYTES    0x0040
#define Tx_START_1021_BYTES   0x0080
#define Tx_START_ALL_BYTES    0x00C0
#define Tx_FORCE              0x0100
#define Tx_ONE_COL            0x0200
#define Tx_NO_CRC             0x1000
#define Tx_RUNT               0x2000

#define	CS89_BufCFG	         0x010A  // Buffer Configuration
#define GENERATE_SW_INT       0x0040
#define Rx_DMA_E              0x0080
#define READY_FOR_Tx_E        0x0100
#define Tx_UNDERRUN_E         0x0200
#define Rx_MISS_E             0x0400
#define Rx_128_BYTE_E         0x0800
#define Tx_COL_COUNT_OVRFL_E  0x1000
#define Rx_MISS_COUNT_OVRFL_E 0x2000
#define Rx_DEST_MATCH_E       0x8000

#define	CS89_LineCTL	      0x0112  // Line Control
#define SERIAL_Rx_ON          0x0040
#define SERIAL_Tx_ON          0x0080
#define AUI_ONLY              0x0100
#define AUTO_AUI_10BASET      0x0200
#define MODIFIED_BACKOFF      0x0800
#define NO_AUTO_POLARITY      0x1000
#define TWO_PART_DEFDIS       0x2000
#define LOW_Rx_SQUELCH        0x4000

#define	CS89_SelfCTL	      0x0114  // Self Control
#define POWER_ON_RESET        0x0040
#define SW_STOP               0x0100
#define SLEEP_ON              0x0200
#define AUTO_WAKEUP           0x0400
#define HCB0_ENBL             0x1000
#define HCB1_ENBL             0x2000
#define HCB0                  0x4000
#define HCB1                  0x8000

#define	CS89_BusCTL	         0x0116  // Bus Control
#define RESET_RX_DMA          0x0040
#define MEMORY_ON             0x0400
#define DMA_BURST_MODE        0x0800
#define IO_CHANNEL_READY_ON   0x1000
#define RX_DMA_SIZE_64K       0x2000
#define ENABLE_IRQ            0x8000

#define	CS89_TestCTL	      0x0118  // Test Control
#define LINK_OFF              0x0080
#define ENDEC_LOOPBACK        0x0200
#define AUI_LOOPBACK          0x0400
#define BACKOFF_OFF           0x0800
#define FDX_8900              0x4000


// Status and Event Registers
//---------------------------//
#define	CS89_ISQ	            0x0120  // Interrupt status queue
#define ISQ_RX_EVENT          0x0004
#define ISQ_TX_EVENT          0x0008
#define ISQ_BUFFER_EVENT      0x000C
#define ISQ_RX_MISS_EVENT     0x0010
#define ISQ_TX_COL_EVENT      0x0012
#define ISQ_EVENT_MASK        0x003F

#define	CS89_RxEvent	      0x0124  // Receiver Event
#define RX_IA_HASHED          0x0040
#define RX_DRIBBLE            0x0080
#define RX_OK                 0x0100
#define RX_HASHED             0x0200
#define RX_IA                 0x0400
#define RX_BROADCAST          0x0800
#define RX_CRC_ERROR          0x1000
#define RX_RUNT               0x2000
#define RX_EXTRA_DATA         0x4000
#define HASH_INDEX_MASK       0xFC00          // Hash-Table Index Mask (6 Bit)

#define	CS89_TxEvent	      0x0128  // Transmitter Event
#define TX_LOST_CRS           0x0040
#define TX_SQE_ERROR          0x0080
#define TX_OK                 0x0100
#define TX_LATE_COL           0x0200
#define TX_JBR                0x0400
#define TX_16_COL             0x8000
#define TX_COL_COUNT_MASK     0x7800

#define	CS89_BufEvent	      0x012C  // Buffer Event
#define SW_INTERRUPT          0x0040
#define RX_DMA                0x0080
#define READY_FOR_TX          0x0100
#define TX_UNDERRUN           0x0200
#define RX_MISS               0x0400
#define RX_128_BYTE           0x0800
#define TX_COL_OVRFLW         0x1000
#define RX_MISS_OVRFLW        0x2000
#define RX_DEST_MATCH         0x8000

#define	CS89_RxMiss	0x0130  // Receiver Miss Counter

#define	CS89_TxColl	0x0132  // Transmit Collision Counter

#define	CS89_LineStatus	   0x0134  // Line Status
#define LINK_OK               0x0080
#define AUI_ON                0x0100
#define TENBASET_ON           0x0200
#define POLARITY_OK           0x1000
#define CRS_OK                0x4000

#define	CS89_SelfStatus	   0x0136  // Self Status
#define ACTIVE_33V            0x0040
#define INIT_DONE             0x0080
#define SI_BUSY               0x0100
#define EEPROM_PRESENT        0x0200
#define EEPROM_OK             0x0400
#define EL_PRESENT            0x0800
#define EE_SIZE_64            0x1000

#define	CS89_BusStatus	   0x0138  // Bus Status
#define TX_BID_ERR	      0x0080
#define RDY4_TX_NOW		   0x0100


// Initiate Transmit Registers
//----------------------------//
#define	CS89_TxCmd	   0x0144  // Transmit Command
#define	CS89_TxLength	0x0146  // Transmit Length



// Adress Filter Registers
//-------------------------//
#define CS89_LAF     0x0150  // Logical Adress Filter (Hash Table)
#define	CS89_IA		0x0158  // Individual Address


// Frame Location
//---------------//
#define	CS89_RxStatus	0x0400  // Receive Status
#define	CS89_RxLength	0x0402  // Receive Length
#define	CS89_RxFrame	0x0404  // Receive Frame Offset
#define	CS89_TxFrame	0x0A00  // Transmit Frame Offset

//
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;


#pragma pack(push, 1)

static u16 *_ptr_base_addr_cs8900 = 0;	// Pointeur d'accés au controleur Ethernet

#pragma pack(pop)

static const char dev_at91m55800a_eth_cs8900_name[]="eth0\0";

static int dev_at91m55800a_eth_cs8900_load(void);
static int dev_at91m55800a_eth_cs8900_open(desc_t desc, int o_flag);
static int dev_at91m55800a_eth_cs8900_close(desc_t desc);
static int dev_at91m55800a_eth_cs8900_isset_read(desc_t desc);
static int dev_at91m55800a_eth_cs8900_isset_write(desc_t desc);
static int dev_at91m55800a_eth_cs8900_read(desc_t desc, char* buf,int size);
static int dev_at91m55800a_eth_cs8900_write(desc_t desc, const char* buf,int size);
static int dev_at91m55800a_eth_cs8900_seek(desc_t desc,int offset,int origin);
static int dev_at91m55800a_eth_cs8900_ioctl(desc_t desc,int request,va_list ap);

dev_map_t dev_at91m55800a_eth_cs8900_map={
   dev_at91m55800a_eth_cs8900_name,
   S_IFCHR,
   dev_at91m55800a_eth_cs8900_load,
   dev_at91m55800a_eth_cs8900_open,
   dev_at91m55800a_eth_cs8900_close,
   dev_at91m55800a_eth_cs8900_isset_read,
   dev_at91m55800a_eth_cs8900_isset_write,
   dev_at91m55800a_eth_cs8900_read,
   dev_at91m55800a_eth_cs8900_write,
   dev_at91m55800a_eth_cs8900_seek,
   dev_at91m55800a_eth_cs8900_ioctl //ioctl
};


static volatile int _at91m55800a_eth_cs8900_desc_rd = -1;
static volatile int _at91m55800a_eth_cs8900_desc_wr = -1;

static unsigned int g_status_cs8900 = 0;

OS_CSEMA segger_csema_txok_cs8900;

#define LWIP_ETHADDR0    0x00
#define LWIP_ETHADDR1    0xbd
#define LWIP_ETHADDR2    0x3b
#define LWIP_ETHADDR3    0x33
#define LWIP_ETHADDR4    0x05
#define LWIP_ETHADDR5    0x71

//
static const char ADDMAC[6] = {   LWIP_ETHADDR0,
                           LWIP_ETHADDR1,
                           LWIP_ETHADDR2,
                           LWIP_ETHADDR3,
                           LWIP_ETHADDR4,
                           LWIP_ETHADDR5 };


//lwip
static unsigned char eth_cs8900_ethaddr[6]={  LWIP_ETHADDR0,
                                       LWIP_ETHADDR1,
                                       LWIP_ETHADDR2,
                                       LWIP_ETHADDR3,
                                       LWIP_ETHADDR4,
                                       LWIP_ETHADDR5 };


#define ETH_CS8900A_INPUT_BUFFER_SZ     (8*1024) /*8192*///(2048) //warning!!!! must be 2^N
#define __ETH_FRAME_VECTOR_SZ   (ETH_CS8900A_INPUT_BUFFER_SZ/8+2)

#define __set_eth_frame_vector(db,pos){\
   unsigned char byte_no = pos>>3;\
   unsigned char bit_no  = pos-(byte_no<<3);\
   unsigned char msk = 0x01<<bit_no;\
   db->ptr_eth_frame_vector[byte_no]|=msk;\
}

#define __unset_eth_frame_vector(db,pos){\
   unsigned char byte_no = pos>>3;\
   unsigned char bit_no  = pos-(byte_no<<3);\
   unsigned char msk = 0x01<<bit_no;\
   db->ptr_eth_frame_vector[byte_no]&=~(msk);\
}

#define __flush_eth_frame_buffer(db){\
   memset(db->ptr_eth_frame_vector,0,__ETH_FRAME_VECTOR_SZ);\
   db->_input_r=0;\
   db->_input_w=0;\
   db->_input_tmp_w=0;\
   db->_output_r=0;\
   db->_output_w=0;\
}

//stat struc
typedef struct net_stats_st{
   u32 tx_errors;
   u32 rx_errors;
}net_stats_t;

// Structure/enum 
typedef struct board_info {
   
   u32 _input_r;
   u32 _input_w;
   u32 _input_tmp_w;

   u32 _output_r;
   u32 _output_w;

   //input buffer
   u8* _ptr_eth_input_buffer;
   //output buffer
   u8* _ptr_eth_output_buffer;
   //frame vector;
   u8* ptr_eth_frame_vector;

   // 
	u32 reset_counter;      /* counter: RESET */ 
	u32 reset_tx_timeout;   /* RESET caused by TX Timeout */ 

	u32 io_addr;   /* Register I/O base address */
	u32 io_data;	/* Data I/O address */
	int tx_pkt_cnt;

	u8 op_mode;    /* PHY operation mode */
	u8 io_mode;		/* 0:word, 2:byte */
	u8 device_wait_reset;	/* device state */
	u8 Speed;/* current speed */

	int cont_rx_pkt_cnt; /* current number of continuos rx packets  */
	//struct timer_list timer;
	//struct net_device_stats stats;
	unsigned char srom[128];

	//spinlock_t lock;
   kernel_pthread_mutex_t     mutex;
   //
   char mac_addr[6];

   //stats
   net_stats_t stats;

} board_info_t;

static board_info_t g_board_info;

/*============================================
| Implementation 
==============================================*/

/*--------------------------------------------
| Name:        _read_io
| Description: Lecture d'un mot de 16 bit dans
|              le controleur en mode I/O
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static unsigned short _read_io(unsigned long addr){
 *(_ptr_base_addr_cs8900 + CS89_PP_Ptr/2) = addr;    // Positionnement du pointeur d'adresse
 return(*(_ptr_base_addr_cs8900 + CS89_PP_Data0/2));
}

/*--------------------------------------------
| Name:        _write_io
| Description: Ecriture d'un mot de 16 bit dans 
|              le controleur en mode I/O
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static void _write_io(unsigned long addr, unsigned short data){
 *(_ptr_base_addr_cs8900 + CS89_PP_Ptr/2) = addr;    // Positionnement du pointeur d'adresse
 *(_ptr_base_addr_cs8900 + CS89_PP_Data0/2) = data;
}

/*--------------------------------------------
| Name:        _init_cs8900
| Description: Procedure d'Initialisation du 
|              controleur Ethernet
| Parameters:  none
| Return Type: none
| Comments: 
|              return(0)  => problème d'initialisation
|              return(1)  => initialisation correcte      
|            
| See:         
----------------------------------------------*/
static int _init_cs8900(void){
   unsigned int i,j,k;
/*
 CSR.1 = 1;         // Validation de CS1
 CSR.5 = 0;	    // wait state sur CS1
 CSE.2 = 0;
 CSE.3 = 1;         // 3 wait state (indispensable a 20 MHz)
*/

 _ptr_base_addr_cs8900 = 0;
 _ptr_base_addr_cs8900 = _ptr_base_addr_cs8900 + (IO_CS8900_BASE_ADDR/2);

 i = _read_io(CS89_EISA);    // Accés bidon (Active AEN en // avec SHBE) => passage en mode 16 bit
 i = _read_io(CS89_EISA);    // Lecture Identification du controleur
 if(i != 0x630E)                // Lecture Code ID = 0x630E si controleur OK
    return(0);                  // Défaut d'initialisation du contrôleur

 _write_io(CS89_SelfCTL, POWER_ON_RESET);   // Reset du contrôleur
 j = 10000;
 do
    {j--;                                     // time out
     for(k=0;k<1000;k++)
        ;                                     // Tempo
     i = _read_io(CS89_SelfStatus);
    }
 while( ((i & INIT_DONE)==0) && j );        // Attente fin du Reset du contrôleur ou time out

 if(j){
     _write_io(CS89_IA  , ADDMAC[0] + 256*ADDMAC[1]);
     _write_io(CS89_IA+2, ADDMAC[2] + 256*ADDMAC[3]);
     _write_io(CS89_IA+4, ADDMAC[4] + 256*ADDMAC[5]);  // Chargement de l'adresse MAC


     _write_io(CS89_RxCTL,Rx_OK_ACCEPT|Rx_IA_ACCEPT|Rx_BROADCAST_ACCEPT|Rx_MULTCAST_ACCEPT|Rx_CRC_ERROR_E|Rx_RUNT_E|0x0005);

     /*
     _write_io(CS89_RxCFG, Rx_OKi_E | 0x0003);    // IRQ sur reception trame
     _write_io(CS89_BufCFG, Rx_MISS_E|READY_FOR_Tx_E| 0x000B);  // IRQ si au moins 1 trame est perdue en reception
     _write_io(CS89_TxCFG  , Tx_OK_E | Tx_JBR_E | Tx_16_COL_E | 0x0007); // IRQ sur emission trame Ok ou Collision ou Trans > 26 ms
      */
     
     
     _write_io(CS89_RxCFG, 0x0003);     // pas d'IRQ sur reception trame
     _write_io(CS89_BufCFG,0x000B);     // pas d'IRQ si au moins 1 trame est perdue en reception
     _write_io(CS89_TxCFG  , 0x0007);   // pas d'IRQ sur emission trame Ok ou Collision ou Trans > 26 ms
     
          

     _write_io(CS89_INT_NB, 0x0000);              // IRQ0 active
     _write_io(CS89_LineCTL, SERIAL_Rx_ON | SERIAL_Tx_ON | 0x0013);   // Receiver & Transmitter enabled

     _write_io(CS89_BaseMemory, 0x00);          // Ecriture Memory Base Adress = $1000 (décodage avec A12=1)
     _write_io(CS89_BusCTL, MEMORY_ON | 0x0017);  // Validation de l'accés en Memory Mode
     i=_read_io(CS89_BusCTL);

     _ptr_base_addr_cs8900 = 0;
     _ptr_base_addr_cs8900 = _ptr_base_addr_cs8900 + (MEM_CS8900_BASE_ADDR/2);
     i = *(_ptr_base_addr_cs8900 + CS89_EISA/2);    // Lecture Identification du controleur en Memory Mode
     if(i != 0x630E)                    // Lecture Code ID = 0x630E si controleur OK
        return(0);                      // Défaut d'initialisation du contrôleur
     else{
         //*(_ptr_base_addr_cs8900 + CS89_BusCTL/2) = MEMORY_ON | ENABLE_IRQ | 0x0017;   // Memory Mode & IRQ demasquée
         return(1);      // Initialisation correcte du contrôleur
        }
    }

 else
     return(0);      // Défaut d'initialisation du contrôleur
}

/*--------------------------------------------
| Name:        _rcv_irq
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static void _rcv_irq(unsigned char v){
   if(v)
    *(_ptr_base_addr_cs8900 + CS89_RxCFG/2) |= (Rx_OKi_E|Rx_CRC_ERROR_E|Rx_RUNT_E);
   else
    *(_ptr_base_addr_cs8900 + CS89_RxCFG/2) &= ~(Rx_OKi_E|Rx_CRC_ERROR_E|Rx_RUNT_E);
}

/*--------------------------------------------
| Name:        _snd_irq
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static void _snd_irq(unsigned char v){
   if(v)
    *(_ptr_base_addr_cs8900 + CS89_TxCFG/2) |= Tx_OK_E | Tx_JBR_E | Tx_16_COL_E;
   else
    *(_ptr_base_addr_cs8900 + CS89_TxCFG/2) &= ~(Tx_OK_E | Tx_JBR_E | Tx_16_COL_E);
}

/*--------------------------------------------
| Name:        _cs8900_irq
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static void _cs8900_irq(unsigned char v){
   if(v)
      *(_ptr_base_addr_cs8900 + CS89_BusCTL/2) |= ENABLE_IRQ;   // IRQ demasquée
   else
      *(_ptr_base_addr_cs8900 + CS89_BusCTL/2) &= ~(ENABLE_IRQ);   //IRQ masquée
}

/*--------------------------------------------
| Name:        _snd_cs8900
| Description: Procedure d'écriture d'une trame Ethernet
| Parameters:  none
| Return Type: none
| Comments:   
|           return( 1) => Transmission effectuée correctement
|           return( 0) => Erreur, liaison physique en défaut
|           return(-1) => Erreur, Nb d'octets à transmettre trop grand
|           return(-2) => Erreur, Controleur pas prêt pour émission
|           Remarque: le préambule de 8 caractères et le CRC sont générés en
|                     automatique par le controleur CS8900A      
| See:         
----------------------------------------------*/
static int _start_snd_cs8900(board_info_t *db,const u8 *buf, u16 size){
   int i=0;
   int len=size;
   int tmplen=0;
   char * data_ptr;

   if(*(_ptr_base_addr_cs8900 + CS89_LineStatus/2) & LINK_OK == 0)
      return 0;      // Liaison physique en défaut

   
	// 16 collisions max, CRC ok, TxPadDis = 0
   if(len < 60)            // 60 car CRC valide => +4 octets = 64 octets
      len = 60;		    // Nombre d'octets minimum à emettre


   //ready to send packet
   //block all interrupts!!!
   __clr_irq();
   *(_ptr_base_addr_cs8900 + CS89_TxCmd/2) = Tx_START_ALL_BYTES /*| 0x0009*/;	// Emission de la trame lorsque la trame complète est ds le CS8900
   *(_ptr_base_addr_cs8900 + CS89_TxLength/2) = len;	// Nombre d'octets à emettre
   
   //
   if(*(_ptr_base_addr_cs8900 + CS89_BusStatus/2) & TX_BID_ERR){
      //restore all interrupts!!!
      __set_irq();
      return -1;      // Nb d'octets à transmettre trop grand ( >1518 ou >1514 avec CRC)
   }

   // tx ready to send. Essai 20 fois
   for(i=0;i<20;i++){
      if( (*(_ptr_base_addr_cs8900 + CS89_BusStatus/2) & RDY4_TX_NOW)!=0 )
         break;
      i++;
   };	// CS8900 prêt a accepter une trame en emission ?
   //
   if( i>=20 ){
      //restore all interrupts!!!
      __set_irq();
      return -1;//not ready
   }

   
   //
   db->tx_pkt_cnt++;
   db->_output_w++;
   //copy packet to ethernet device
   //restore all interrupts!!!
   __set_irq();

   data_ptr = (char *)buf;
   tmplen = (len + 1) / 2;
   for (i = 0; i < tmplen; i++)
      if(size<60 && (i>=(size+1)/2) )//paddding
         *(_ptr_base_addr_cs8900 + CS89_TxFrame/2)=(u16)(0);
      else
         *(_ptr_base_addr_cs8900 + CS89_TxFrame/2)=((u16 *)data_ptr)[i];
   

   /*
   for(i=0;i<len;i=i+2){
      if((len - i) == 1){
         // Nb d'octets de donnée impair
         *(_ptr_base_addr_cs8900 + CS89_TxFrame/2) = *(buf+i);
         len++;
      }
      else{
         *(_ptr_base_addr_cs8900 + CS89_TxFrame/2) = *(buf+i) + *(buf+i+1)*256;
      }
   }
   //
   for(i=len;i<60;i=i+2)
      *(_ptr_base_addr_cs8900 + CS89_TxFrame/2) = 0;
   */

   //
   return 0;
  
}

/*--------------------------------------------
| Name:        _snd_cs8900_done
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static void _snd_cs8900_done(board_info_t *db){
   db->tx_pkt_cnt--;
   db->_output_r++;
   if( (db->tx_pkt_cnt <= 0) && (_at91m55800a_eth_cs8900_desc_wr>=0) )
      //if(db->_output_w==db->_output_r)
         __fire_io_int(ofile_lst[_at91m55800a_eth_cs8900_desc_wr].owner_pthread_ptr_write);

   db->tx_pkt_cnt=0;
   //lock in dev_eth_cs8900_write()
   kernel_pthread_mutex_unlock(&db->mutex);
	
	return;
}

/*--------------------------------------------
| Name:        _rcv_cs8900
| Description: Procedure de lecture d'une trame Ethernet  
| Parameters:  none
| Return Type: none
| Comments:    
|           return( >= 1) => Nombre d'octets recus correctement
|           return ( 0)   => Aucun octet recu
|           return (-1)   => Erreur de reception
|           Remarque : le préambule de 8 caractères et le CRC sont enlevés en
|                      automatique par le controleur CS8900A   
| See:         
----------------------------------------------*/
static int _rcv_cs8900(board_info_t *db)
{
   u32 i;
   u16 k;
   u32 len =*(_ptr_base_addr_cs8900 + CS89_RxLength/2);// Longueur de la trame reçue;

   u32 pos ;
   u32 byte_no ;
   u8 bit_no ;
   u8 msk;

   u8* rdptr = db->_ptr_eth_input_buffer;
  

   //signal 
   if((_at91m55800a_eth_cs8900_desc_rd>=0))
      __fire_io_int(ofile_lst[_at91m55800a_eth_cs8900_desc_rd].owner_pthread_ptr_read);
   
   //copy data (16 bits mode)
   //memcpy( trame, (const int*)(_ptr_base_addr_cs8900 + CS89_RxFrame/2), len );
   for(i=0;i<((len+1)>>1);i++){
      k = *(_ptr_base_addr_cs8900 + CS89_RxFrame/2 + i);
		((u16 *)rdptr)[db->_input_tmp_w/2] = k;
      db->_input_tmp_w=((db->_input_tmp_w+2)&(~ETH_CS8900A_INPUT_BUFFER_SZ));
   }

   pos = db->_input_tmp_w;
   byte_no = pos>>3;
   bit_no  = pos-(byte_no<<3);
   msk = 0x01<<bit_no;
   //
   db->ptr_eth_frame_vector[byte_no]|=msk;//valid this rtu frame
   	
   return len;     // Nb de caractères recus
}

/*--------------------------------------------
| Name:        dev_at91m55800a_eth_cs8900_interrupt
| Description: Programme d'interruption de 
|              gestion du controleur Ethernet
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static void dev_at91m55800a_eth_cs8900_interrupt(void){

   unsigned int _cb;
   board_info_t * db= &g_board_info;    /* Point a board information structure */

   //__hw_enter_interrupt();
   __AIC_IVR = 0; // Debug variant of vector read, protected mode is used.

   //disable receive and send irq
   //_rcv_irq(0);
   //_snd_irq(0);
   _cs8900_irq(0);

   //
   __AIC_ICCR = 1 << IRQ2; // Clears IRQ2 interrupt.
   //
   while ( (g_status_cs8900 = *(_ptr_base_addr_cs8900 + CS89_ISQ/2)) ){	    // Lecture Interrupt status queue (Raz de l'IRQ)

      switch((g_status_cs8900 & ISQ_EVENT_MASK)){

         //rcv
         case ISQ_RX_EVENT:{
            u16 _rx_status = *(_ptr_base_addr_cs8900 + CS89_RxStatus/2);
            if(!(_rx_status&RX_OK)){//error invalid frame
               break;
            }
            _rcv_cs8900(db);
         }
         break;

         //snd
         case ISQ_TX_EVENT:{
            _snd_cs8900_done(db);
            if ((g_status_cs8900 & TX_OK) == 0) 
               db->stats.tx_errors++;
			   if (g_status_cs8900 & TX_LOST_CRS) 
               db->stats.tx_errors++;
			   if (g_status_cs8900 & TX_SQE_ERROR) 
               db->stats.tx_errors++;
			   if (g_status_cs8900 & TX_LATE_COL) 
               db->stats.tx_errors++;
			   if (g_status_cs8900 & TX_16_COL) 
               db->stats.tx_errors++;
         }
         break;

         //
         case ISQ_BUFFER_EVENT:{
			   if (g_status_cs8900 & READY_FOR_TX) {
               _snd_cs8900_done(db);
				   /* we tried to transmit a packet earlier,
               but inexplicably ran out of buffers.
               That shouldn't happen since we only ever
               load one packet.  Shrug.  Do the right
               thing anyway. */
            }
         }
         break;

         //
         case ISQ_RX_MISS_EVENT:
         break;

         case ISQ_TX_COL_EVENT:
         break;

         default:
         break;

      }
      
   }

   //re-enable receive and send irq
   _cs8900_irq(1);
   //_rcv_irq(1);
   //_snd_irq(1);

   //
   __AIC_EOICR = 0; // Signal end of interrupt to AIC.

// __hw_leave_interrupt();
}


/*-------------------------------------------
| Name:dev_at91m55800a_eth_cs8900_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_at91m55800a_eth_cs8900_load(void){
  _at91m55800a_eth_cs8900_desc_rd = -1;
  _at91m55800a_eth_cs8900_desc_wr = -1;
  
   pthread_mutexattr_t  mutex_attr=0;
   kernel_pthread_mutex_init(&g_board_info.mutex,&mutex_attr);
	

   g_board_info._ptr_eth_input_buffer =  (u8*) malloc(ETH_CS8900A_INPUT_BUFFER_SZ+20);
   if(g_board_info._ptr_eth_input_buffer==(u8*)0)
      return -1;

   g_board_info.ptr_eth_frame_vector =  (u8*) malloc(__ETH_FRAME_VECTOR_SZ);
   if(g_board_info._ptr_eth_input_buffer==(u8*)0)
      return -1;

   memset(g_board_info.ptr_eth_frame_vector,0,__ETH_FRAME_VECTOR_SZ);
   g_board_info._input_r=0;
   g_board_info._input_w=0;
   g_board_info._input_tmp_w=0;
   g_board_info._output_w=0;
   g_board_info._output_r=0;

 
    //init hardware
  __EBI_CSR2 = 0x20003431;//init base address at 0x20000000, 5 wait state, enable chip select
  //__EBI_CSR2=0x2000342D;//init base address at 0x20000000, 4 wait state, enable chip select

  __PIO_PDR |= (1<<11);//IRQ2 on PA11

  __AIC_SVR27 = (unsigned long)&dev_at91m55800a_eth_cs8900_interrupt; // Usart 0 interrupt vector.
  __AIC_SMR27 = 0x63; // SRCTYPE=3, PRIOR=3. IRQ2 interrupt (eth0) positive edge-triggered at prio 3.
  __AIC_ICCR = 1 << IRQ2; // Clears IRQ2 interrupt.
  __AIC_IECR = 1 << IRQ2; // Enable IRQ2 interrupt.
 

  return (_init_cs8900()?0:-1);
}

/*-------------------------------------------
| Name:dev_at91m55800a_eth_cs8900_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_at91m55800a_eth_cs8900_open(desc_t desc, int o_flag){

   //
   if(o_flag & O_RDONLY){
      if(_at91m55800a_eth_cs8900_desc_rd<0)
         _at91m55800a_eth_cs8900_desc_rd = desc;
      else
         return -1; //already open

      //
      _rcv_irq(1);
      //
      if(!ofile_lst[desc].p)
         ofile_lst[desc].p=&g_board_info;

      g_board_info.stats.rx_errors=0;
   }

   if(o_flag & O_WRONLY){
      if(_at91m55800a_eth_cs8900_desc_wr<0)
         _at91m55800a_eth_cs8900_desc_wr = desc;
      else
         return -1; //already open
      //
      _snd_irq(1);
      //
      if(!ofile_lst[desc].p)
         ofile_lst[desc].p=&g_board_info;

      g_board_info.tx_pkt_cnt=0;
      g_board_info.stats.tx_errors=0;
   } 

   //to do: interrupt configuration
   //INT1IC = 0x02;// Niveau priorité=2, front descendant sur INT1
   __PIO_IER |= (1<<11);//enable IRQ2 on PA11
   //
   _cs8900_irq(1);//enable irq on ethernet chip

   return 0;
}

/*-------------------------------------------
| Name:dev_at91m55800a_eth_cs8900_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_at91m55800a_eth_cs8900_close(desc_t desc){
   if(ofile_lst[desc].oflag & O_RDONLY){
      if(!ofile_lst[desc].nb_reader){
         _at91m55800a_eth_cs8900_desc_rd = -1;
         _rcv_irq(0);
      }
   }

   if(ofile_lst[desc].oflag & O_WRONLY){
      if(!ofile_lst[desc].nb_writer){
         _at91m55800a_eth_cs8900_desc_wr = -1;
         _snd_irq(0);
      }
   }

   if(!ofile_lst[desc].nb_writer
      &&!ofile_lst[desc].nb_reader){
      _cs8900_irq(0);//disable irq on ethernet chip
      //to do: disable interrupt

      //to do: disable interrupt 
      __PIO_IDR |= (1<<11);//enable IRQ2 on PA11
      //INT1IC = 0x00;//set to 0 ???

   }
   return 0;
}

/*-------------------------------------------
| Name:dev_at91m55800a_eth_cs8900_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_at91m55800a_eth_cs8900_isset_read(desc_t desc){
   board_info_t * db=(board_info_t *)ofile_lst[desc].p;
   if(!db)
      return -1;

   if((db->_input_tmp_w!=db->_input_r)){
      u32 _last_pos = db->_input_w>>3;
      u32 _byte     = _last_pos;
      u8 _bit      = 0;
      //
      do{
         if(db->ptr_eth_frame_vector[_byte])
            break;
      }while((_byte=((++_byte)%__ETH_FRAME_VECTOR_SZ))!=_last_pos);
      //
      if(_byte==_last_pos && !db->ptr_eth_frame_vector[_byte])
         return -1;

      //
      for(_bit=0;_bit<8;_bit++){
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
   }/*else if((ttys_termios.c_cc[VTIME] && !inter_char_timer) ){
      inter_char_timer=ttys_termios.c_cc[VTIME];
      return 0;
   }*/else{
      return -1;
   }
   
}

/*-------------------------------------------
| Name:dev_at91m55800a_eth_cs8900_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_at91m55800a_eth_cs8900_isset_write(desc_t desc){
   board_info_t * db=(board_info_t *)ofile_lst[desc].p;
   if(!db)
      return -1;

   if(/*db->_output_w==db->_output_r &&*/ db->tx_pkt_cnt<=0)
      return 0;
   return -1;
}
/*-------------------------------------------
| Name:dev_at91m55800a_eth_cs8900_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_at91m55800a_eth_cs8900_read(desc_t desc, char* buf,int size){
   u32 w = 0 ;
   u32 r = 0 ; 
   u32 l = 0;
   int cb=0;
   board_info_t * db=(board_info_t *)ofile_lst[desc].p;
   if(!db)
      return -1;

   
   r= db->_input_r;
   w= db->_input_w;
   
   if(r==w)
      return 0;
   
   if(w>=r){
      l=w-r;
      if(size<l)
         l=size;
      memcpy(buf,&db->_ptr_eth_input_buffer[r],l);
      db->_input_r =w;
      cb=l;
   }else{
      l=(ETH_CS8900A_INPUT_BUFFER_SZ-r+w);
      if(size<l){
         if(size<(ETH_CS8900A_INPUT_BUFFER_SZ-r)){
            cb=size;
            memcpy(buf,&db->_ptr_eth_input_buffer[r],cb);
            db->_input_r +=cb;
         }else{
             cb=(ETH_CS8900A_INPUT_BUFFER_SZ-r);
             memcpy(buf,&db->_ptr_eth_input_buffer[r],cb);        
             memcpy(buf+cb,&db->_ptr_eth_input_buffer[0],size-(ETH_CS8900A_INPUT_BUFFER_SZ-r));
             db->_input_r =size-(ETH_CS8900A_INPUT_BUFFER_SZ-r);
             cb=size;
         }
      }else{
         cb=(ETH_CS8900A_INPUT_BUFFER_SZ-r);
         memcpy(buf,&db->_ptr_eth_input_buffer[r],cb);        
         memcpy(buf+cb,&db->_ptr_eth_input_buffer[0],l-(ETH_CS8900A_INPUT_BUFFER_SZ-r));
         db->_input_r =l-(ETH_CS8900A_INPUT_BUFFER_SZ-r);
         cb=l;
      }
   }
   

   return cb;
}

/*-------------------------------------------
| Name:dev_at91m55800a_eth_cs8900_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_at91m55800a_eth_cs8900_write(desc_t desc, const char* buf,int size){
    int r;
   board_info_t * db=(board_info_t *)ofile_lst[desc].p;
   if(!db)
      return -1;
   kernel_pthread_mutex_lock(&db->mutex);
   if(_start_snd_cs8900(db,(const u8*)buf,size)<0){
      kernel_pthread_mutex_unlock(&db->mutex);
      return -1;
   }
   //unlock in dmfe_tx_done()
   return size;
}

/*-------------------------------------------
| Name:dev_at91m55800a_eth_cs8900_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_at91m55800a_eth_cs8900_seek(desc_t desc,int offset,int origin){
   return -1;
}

/*-------------------------------------------
| Name:dev_at91m55800a_eth_cs8900_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_at91m55800a_eth_cs8900_ioctl(desc_t desc,int request,va_list ap){
  switch(request){

      //reset interface
      case ETHRESET:{
        return -1;
      }
      break;

      //status interface
      case ETHSTAT:{
        return -1;
      }
      break;

      case ETHSETHWADDRESS:{
         return -1;
      }
      break;

      case ETHGETHWADDRESS:{
         unsigned char* p_eth_hwaddr = va_arg( ap, unsigned char*);
         if(!p_eth_hwaddr)
            return -1;
         p_eth_hwaddr[0] = eth_cs8900_ethaddr[0];
         p_eth_hwaddr[1] = eth_cs8900_ethaddr[1]; 
         p_eth_hwaddr[2] = eth_cs8900_ethaddr[2];
         p_eth_hwaddr[3] = eth_cs8900_ethaddr[3];
         p_eth_hwaddr[4] = eth_cs8900_ethaddr[4];
         p_eth_hwaddr[5] = eth_cs8900_ethaddr[5];
      }
      break;

      //
      default:
      return -1;
    }

}
/*============================================
| End of Source  : dev_at91m55800a_eth_cs8900.c
==============================================*/
