

/*============================================
| Includes    
==============================================*/
#include "kernel/core/types.h"
//#include "kernel/core/interrupt.h"
//#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
//#include "kernel/core/stat.h"
//#include "kernel/core/fcntl.h"

//#include "lib/libc/termios/termios.h"

//#include "kernel/fs/vfs/vfsdev.h"

#include "dev_cmsis_itm.h"

/*============================================
| Global Declaration 
==============================================*/
#define DEV_CMSIS_ITM_NO 0
static cpu_inf_itm_t cpu_inf_itm_0;

const char dev_cmsis_itm_0_name[]="itm0\0";

static int dev_cmsis_itm_0_load(void);
static int dev_cmsis_itm_0_open(desc_t desc, int o_flag);

// ITM Functions pointers
dev_map_t dev_cmsis_itm0_map={
    dev_cmsis_itm_0_name,
    S_IFCHR,
    dev_cmsis_itm_0_load,
    dev_cmsis_itm_0_open,
    dev_cmsis_itm_x_close,
    dev_cmsis_itm_x_isset_read, //"data ready to read from app" 0:data available/ready to read, -1 no data to read 
    dev_cmsis_itm_x_isset_write,//"data finished to send OR ready to send new data"  0:ready to send,  -1 otherwise (busy sending)
    dev_cmsis_itm_x_read,
    dev_cmsis_itm_x_write,
    dev_cmsis_itm_x_seek,
    dev_cmsis_itm_x_ioctl
};

/*============================================
| Implementation 
==============================================*/
/*--------------------------------------------
| Name:        dev_cmsis_itm_0_load
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    inspired from CMSIS 2.10 code.
| See:         
----------------------------------------------*/
static int dev_cmsis_itm_0_load(void){
    //GD-TODO: malloc()
    cpu_inf_itm_0.itm_no = DEV_CMSIS_ITM_NO;
    
    //
    if(dev_cmsis_itm_x_load(&cpu_inf_itm_0)<0)
        return -1;
    
    return 0;
}


/*--------------------------------------------
| Name:        dev_cmsis_itm_x_open
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static int dev_cmsis_itm_0_open(desc_t desc, int o_flag){
  
  ofile_lst[desc].p = (cpu_inf_itm_t *)&cpu_inf_itm_0;
    
   return dev_cmsis_itm_x_open(desc, o_flag);
}

//void UART0_IRQHandler()
//{
//    dev_cmsis_itm_x_isr(0);//&cpu_inf_itm_0);
//}
/*============================================
| End of Source  : dev_cmsis_itm_0.c
==============================================*/