/*--------------------------------------------
| Copyright(C) 2005 CHAUVIN-ARNOUX
|---------------------------------------------
| Project:
| Project Manager:
| Source:          dev_at91sam9261_mci.c
| Path:
| Authors:
| Plateform:   	GNU/Linux
| Created:
| Revision/Date: 	$Revision:$ / $Date:$
| Description:
|---------------------------------------------*/


/*============================================
| Includes
==============================================*/
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"

#include "kernel/fs/vfs/vfsdev.h"

#include "lib/libc/ctype/ctype.h"
#include "kernel/core/types.h"
#include "kernel/core/time.h"
#include "kernel/core/devio.h"
#include "kernel/core/errno.h"

#include "kernel/dev/arch/at91/dev_at91_mci/dev_at91_mci.h"

#if defined(__KERNEL_UCORE_ECOS)
   #include "cyg/hal/at91sam9261.h"
#endif

#define  MCI_PINS    (AT91C_PA0_MCDA0 | AT91C_PA1_MCCDA | AT91C_PA2_MCCK | \
      AT91C_PA4_MCDA1 | AT91C_PA5_MCDA2 | AT91C_PA6_MCDA3)
#define  MCI_VECTOR  CYGNUM_HAL_INTERRUPT_MCI
/*============================================
| Global Declaration
==============================================*/
static const char dev_at91sam9261_mci_name[]="mci\0";

static int dev_at91sam9261_mci_load(void);
int dev_at91_mci_load(board_inf_mci_t * p_inf_mci, int master_clk);
extern int dev_at91_mci_open(desc_t desc, int o_flag);
extern int dev_at91_mci_close(desc_t desc);
extern int dev_at91_mci_read(desc_t desc, char* buf,int cb);
extern int dev_at91_mci_write(desc_t desc, const char* buf,int cb);
extern int dev_at91_mci_seek(desc_t desc,int offset,int origin);
extern int dev_at91_mci_ioctl(desc_t desc,int request,va_list ap);
//
dev_map_t dev_at91sam9261_mci_map={
      dev_at91sam9261_mci_name,
      S_IFBLK,
      dev_at91sam9261_mci_load,
      dev_at91_mci_open,
      dev_at91_mci_close,
      __fdev_not_implemented,
      __fdev_not_implemented,
      dev_at91_mci_read,
      dev_at91_mci_write,
      dev_at91_mci_seek,
      dev_at91_mci_ioctl,
};

//
static void _at91sam9261_mci_gpio_init(void);

//
static board_inf_mci_t p_g_board_mci __attribute__((section (".no_cache")));

/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name:dev_at91sam9261_mci_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam9261_mci_load(void) {
   _at91sam9261_mci_gpio_init();

   //enable MCI clock
   AT91C_BASE_PMC->PMC_PCER = (1 << MCI_VECTOR);

   //
   memset((void *)&p_g_board_mci, 0, sizeof(board_inf_mci_t));
   p_g_board_mci.base_addr = (volatile unsigned int *)0xfffa8000;
   p_g_board_mci.irq_no = MCI_VECTOR;
   p_g_board_mci.irq_prio = 3; //default

   //
   dev_at91_mci_load(&p_g_board_mci, CYGNUM_HAL_ARM_AT91SAM9261_CLOCK_SPEED);
}

/*-------------------------------------------
| Name:_at91sam9261_mci_gpio_init
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _at91sam9261_mci_gpio_init(void) {
   //Enable PIO B (0,1,2,4,5,6)
   AT91C_BASE_PIOA->PIO_IDR = MCI_PINS;
   AT91C_BASE_PIOA->PIO_BSR = MCI_PINS;
   AT91C_BASE_PIOA->PIO_PDR = MCI_PINS;
   AT91C_BASE_PIOA->PIO_PPUDR = MCI_PINS;
}
/*============================================
| End of Source  : dev_at91sam9261_mci.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
