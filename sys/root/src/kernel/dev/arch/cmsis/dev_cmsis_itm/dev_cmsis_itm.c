

/*============================================
| Includes    
==============================================*/
#include "kernel/core/types.h"
//#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"
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
//#define ITM_CPU_MAX 1

cpu_inf_itm_t* p_cpu_inf_itm_list[ITM_CPU_MAX]={0};


/*============================================
| Implementation 
==============================================*/

/* ##################################### Debug In/Output function ########################################### */
/** \ingroup  CMSIS_Core_FunctionInterface
    \defgroup CMSIS_core_DebugFunctions CMSIS Core Debug Functions
  @{
 */

//volatile int32_t ITM_RxBuffer;                           /*!<  variable to receive characters                    */
#define                 ITM_RXBUFFER_EMPTY    0x5AA55AA5 /*!< value identifying ITM_RxBuffer is ready for next character */


/** \brief  ITM Send Character

    This function transmits a character via the selected ITM channel.
    It just returns when no debugger is connected that has booked the output.
    It is blocking when a debugger is connected, but the previous character send is not transmitted.

    \param [in]     ch  Character to transmit
    \param [in]   port  ITM channel to transmit on. (Port 0 is commony used.)
    \return             Character to transmit
 */
static __INLINE uint32_t ITMx_SendChar (uint32_t ch, uint32_t port)
{
  if ((CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)  &&      /* Trace enabled */
      (ITM->TCR & ITM_TCR_ITMENA_Msk)                  &&      /* ITM enabled */
      (ITM->TER & (1UL << port)        )                    )     /* ITM Port #0 enabled */
  {
    while (ITM->PORT[port].u32 == 0);
    ITM->PORT[port].u8 = (uint8_t) ch;
  }
  return (ch);
}


///** \brief  ITM Receive Character
//
//    This function inputs a character via external variable ITM_RxBuffer.
//    It just returns when no debugger is connected that has booked the output.
//    It is blocking when a debugger is connected, but the previous character send is not transmitted.
//
//    \return             Received character
//    \return         -1  No character received
// */
//static __INLINE int32_t ITM_ReceiveChar (void) {
//  int32_t ch = -1;                           /* no character available */
//
//  if (ITM_RxBuffer != ITM_RXBUFFER_EMPTY) {
//    ch = ITM_RxBuffer;
//    ITM_RxBuffer = ITM_RXBUFFER_EMPTY;       /* ready for next character */
//  }
//
//  return (ch);
//}


///** \brief  ITM Check Character
//
//    This function checks external variable ITM_RxBuffer whether a character is available or not.
//    It returns '1' if a character is available and '0' if no character is available.
//
//    \return          0  No character available
//    \return          1  Character available
// */
//static __INLINE int32_t ITM_CheckChar (void) {
//
//  if (ITM_RxBuffer == ITM_RXBUFFER_EMPTY) {
//    return (0);                                 /* no character available */
//  } else {
//    return (1);                                 /*    character available */
//  }
//}

/*@} end of CMSIS_core_DebugFunctions */



/*--------------------------------------------
| Name:        dev_cmsis_itm_x_load
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
//#define ITM_Port32(n) (*((volatile unsigned int *)(0xE0000000+4*n)))
int dev_cmsis_itm_x_load(cpu_inf_itm_t *p_cpu_inf_itm){
    int itm_no = p_cpu_inf_itm->itm_no;
    //  
    p_cpu_inf_itm_list[itm_no] = p_cpu_inf_itm;
    p_cpu_inf_itm->desc_r = -1;
    p_cpu_inf_itm->desc_w = -1;
    
    //
    //test
//    while (ITM_Port32(0) == 0);
//    ITM_Port32(0) = 'a';
//    ITM_SendChar('O');
//    ITM_SendChar('k');
//    ITM_SendChar('\n');
    ITMx_SendChar('O',itm_no);
    ITMx_SendChar('k',itm_no);
    ITMx_SendChar('\n',itm_no);
    
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
int dev_cmsis_itm_x_open(desc_t desc, int o_flag){
  
    cpu_inf_itm_t *p_cpu_inf_itm = (cpu_inf_itm_t *)ofile_lst[desc].p;
    
    // Read only Device opening
    if (o_flag & O_RDONLY)
    {
        return -1; //Driver only support output
//        if (p_cpu_inf_itm->desc_r >= 0) 
//            return -1;  //already open: exclusive resource.
//        
//        p_cpu_inf_itm->desc_r = desc;        
    }
    
    // Write only Device opening
    if (o_flag & O_WRONLY)
    {
        if (p_cpu_inf_itm->desc_w >= 0) 
            return -1;  //already open: exclusive resource.
        
        p_cpu_inf_itm->desc_w = desc;
    }
    
//    p_cpu_inf_itm->o_flag |= o_flag; // set o_flag
  
    return 0;
}

/*--------------------------------------------
| Name:        dev_cmsis_itm_x_close
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_cmsis_itm_x_close(desc_t desc){  
  cpu_inf_itm_t *p_cpu_inf_itm = (cpu_inf_itm_t *)ofile_lst[desc].p;
   
  if(ofile_lst[desc].oflag & O_RDONLY){
      if(!ofile_lst[desc].nb_reader){
        
      }
   }
   
   if(ofile_lst[desc].oflag & O_WRONLY){
      
      if(!ofile_lst[desc].nb_writer){
          p_cpu_inf_itm->desc_w = INVALID_DESC;
      }
   }

   return 0;
}

/*--------------------------------------------
| Name:        dev_cmsis_itm_x_isset_read
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_cmsis_itm_x_isset_read(desc_t desc){    
//    return !ITM_CheckChar();
//return 0; //Data to read
    return -1; //No data to read
}

/*--------------------------------------------
| Name:        dev_cmsis_itm_x_isset_write
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_cmsis_itm_x_isset_write (desc_t desc){
      return 0;//0==no Tx data (transmit finished/ready to Tx);  0!=data to Tx  //-1;
}

/*--------------------------------------------
| Name:        dev_cmsis_itm_x_isset_write
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_cmsis_itm_x_read(desc_t desc, char* buf,int size){
//    cpu_inf_itm_t *p_cpu_inf_itm = (cpu_inf_itm_t *)ofile_lst[desc].p;
//    *buf = ITM_ReceiveChar();//(p_cpu_inf_itm->base_address);
//    __fire_io_int(ofile_lst[p_cpu_inf_itm->desc_r].owner_pthread_ptr_read);
//    return 1;
    return -1;
}

/*--------------------------------------------
| Name:        dev_cmsis_itm_x_write
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_cmsis_itm_x_write(desc_t desc, const char* buf,int size){
    cpu_inf_itm_t *p_cpu_inf_itm = (cpu_inf_itm_t *)ofile_lst[desc].p;
    int count = size;
    while(count)
    {
        count--;
//            ITM_SendChar(*buf++);//(p_cpu_inf_itm->base_address)
        ITMx_SendChar(*buf++,p_cpu_inf_itm->itm_no);
    }
    __fire_io(ofile_lst[p_cpu_inf_itm->desc_w].owner_pthread_ptr_write);
    return (size-count);
}

/*--------------------------------------------
| Name:        dev_cmsis_itm_x_seek
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_cmsis_itm_x_seek(desc_t desc,int offset,int origin){
   return 0;
}

/*--------------------------------------------
| Name:        dev_cmsis_itm_x_ioctl
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_cmsis_itm_x_ioctl(desc_t desc, int request, va_list ap)
{
 
   return 0;
}


void dev_cmsis_itm_x_isr(int itm_no)//cpu_inf_itm_t* p_cpu_inf_itm)
{
//    unsigned long ulStatus;
//    cpu_inf_itm_t* p_cpu_inf_itm = p_cpu_inf_itm_list[itm_no];
//
//    __fire_io_int(ofile_lst[p_cpu_inf_itm->desc_w].owner_pthread_ptr_write);
}
/*============================================
| End of Source  : dev_cmsis_itm.c
==============================================*/
