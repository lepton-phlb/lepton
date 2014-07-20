/*--------------------------------------------
| Copyright(C) 2010 CHAUVIN-ARNOUX
|---------------------------------------------
| Project: tauon
| Project Manager:
| Source: dev_at91sam9261-ek_dm9000.c
| Path: /tauon-project/lepton/sys/user/tauon-basic/src/dev/board_atmel_at91sam9261-ek/dev_at91sam9261-ek_dm9000.c        
| Authors: cle_d_anton
| Plateform:
| Created:  10 sept. 2010
| Revision/Date: $Revision: 1.3 $  $Date: 2010-02-08 09:17:04 $
| Description:
|---------------------------------------------*/


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

#include "lib/libc/termios/termios.h"

#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/core/ioctl_eth.h"

#include "kernel/dev/arch/all/eth/dev_eth_dm9000a/dev_eth_dm9000a.h"
#include "kernel/dev/arch/arm9/at91sam9261/dev_at91sam9261_eth_dm9000a/dev_at91sam9261_eth_dm9000a.h"


#if defined(USE_ECOS)
   #include "cyg/hal/at91sam9261.h"
#endif

/*============================================
| Global Declaration
==============================================*/
//#define USE_BOARD_A25400
/*
#define USE_BOARD_AT91SAM9261_EK
#if defined(USE_BOARD_AT91SAM9261_EK)
   #define DM9KS_DATA_IO    0x04
   #define DM9KS_IRQ_NO    CYGNUM_HAL_INTERRUPT_PIOC //(voir cyg/hal/hal_platform_ints.h)
#elif defined(USE_BOARD_A25400)
   #define DM9KS_DATA_IO    0x1000
   #define DM9KS_IRQ_NO    CYGNUM_HAL_INTERRUPT_IRQ1 //(voir cyg/hal/hal_platform_ints.h)
#endif
*/

extern AT91PS_SYS  AT91_SYS;

#define DM9KS_MIN_IO    0x30000000
#define DM9KS_DATA_IO   0x04
#define CYGNUM_HAL_H_PRIOR 3

static int dev_at91sam9261_ek_eth_dm9000_load(void);

dev_map_t dev_at91sam9261_ek_eth_dm9000_map={
   dev_eth_dm9000a_name,
   S_IFCHR,
   dev_at91sam9261_ek_eth_dm9000_load,
   dev_at91sam9261_eth_dm9000a_open,
   dev_at91sam9261_eth_dm9000a_close,
   dev_eth_dm9000a_isset_read,
   dev_eth_dm9000a_isset_write,
   dev_eth_dm9000a_read,
   dev_eth_dm9000a_write,
   dev_eth_dm9000a_seek,
   dev_eth_dm9000a_ioctl //ioctl
};
/*===========================================
Implementation
=============================================*/

/*--------------------------------------------
| Name:        dev_at91sam9261_ek_eth_dm9000_load
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static int dev_at91sam9261_ek_eth_dm9000_load(void){

   dev_io_info_t dev_io_info;
   dev_io_info.io_addr  = DM9KS_MIN_IO;
   dev_io_info.io_data  = DM9KS_MIN_IO+DM9KS_DATA_IO;
   
#if defined(__KERNEL_UCORE_ECOS)
   dev_io_info.irq_no   = CYGNUM_HAL_INTERRUPT_PIOC;//(voir cyg/hal/hal_platform_ints.h)
   dev_io_info.irq_prio = CYGNUM_HAL_H_PRIOR;
#elif defined(__KERNEL_UCORE_EMBOS) || defined(__KERNEL_UCORE_FREERTOS) 
   dev_io_info.irq_no   = AT91C_ID_PIOC;
#endif
   
   //disable hard reset on dm9000 chip for atmel at91sam9261-ek board.
   AT91_SYS->PMC_PCER |= (1 << (4)); //PIOC ID = 4 // Switch on clock to portC
   AT91_SYS->PIOC_PER |= (AT91C_PIO_PC10); //enable PC10 as IO
   AT91_SYS->PIOC_OER |= (AT91C_PIO_PC10); //enable PC10 as Output
   AT91_SYS->PIOC_CODR|= (AT91C_PIO_PC10); //clear PC10 line. reset is inactive.
   //enable PC11 FIQ for dm9000 chip for for atmel at91sam9261-ek board.
   return dev_at91sam9261_eth_dm9000a_load(&dev_io_info);
}


/*============================================
| End of Source  : dev_at91sam9261-ek_dm9000.c
==============================================*/

/*--------------------------------------------
| Created:  10 sept. 2010
| Revision/Date: $Revision: 1.3 $  $Date: 2010-02-08 09:17:04 $
| Description:
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
|
|---------------------------------------------*/
