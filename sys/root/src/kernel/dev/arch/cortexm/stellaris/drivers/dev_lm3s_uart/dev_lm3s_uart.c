/*--------------------------------------------
| Tachyon
|---------------------------------------------
| Project:         
| Project Manager: 
| Source:          dev_lm3s9b96_uart.c
| Path:        C:\tauon\sys\root\src\kernel\dev\arch\cortexM3\stellaris\lm3s9b96\dev_lm3s9b96_uart
| Authors:     
| Plateform:   
| Created:     
| Revision/Date: $Revision:$  $Date:$ 
| Description: 
|---------------------------------------------
| Historic:    
|---------------------------------------------
| Authors     | Date     | Comments  
| $Log:$
|---------------------------------------------*/


/*============================================
| Includes    
==============================================*/
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/malloc.h"
//#include "kernel/core/stat.h"
//#include "kernel/core/fcntl.h"
//
//#include "lib/libc/termios/termios.h"
//
//#include "kernel/fs/vfs/vfsdev.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"

#include "dev_lm3s_uart.h"

/*============================================
| Global Declaration 
==============================================*/

board_inf_uart_t* p_board_inf_uart_list[UART_BOARD_MAX]={0};

const UARTStaticConf_t UARTStaticConf[UART_BOARD_MAX] =
{  //REG BASE,  int number, Tx buffer, Rx buffer  (0 for forced synchronous mode)
    {UART0_BASE, INT_UART0, 2048, 2048/*0 or 32*/},
    {UART1_BASE, INT_UART1, 0, 0},
    {UART2_BASE, INT_UART2, 0, 0},
};

/*============================================
| Implementation 
==============================================*/

/*--------------------------------------------
| Name:        dev_lm3s_uart_x_load
| Description: Load device
| Parameters : uart number 
|   (index in UARTStaticConf[], which may not match the HW UART number)
| Return Type: integer -> driver loaded (=0)
| Comments:    
| See:         
----------------------------------------------*/
int dev_lm3s_uart_x_load(int uart_id){
    //  
    if(!p_board_inf_uart_list[uart_id])
    {
        p_board_inf_uart_list[uart_id] = (board_inf_uart_t*)_sys_malloc(sizeof(board_inf_uart_t));
        if(!p_board_inf_uart_list[uart_id])
        {
            return -1;
        }
        memset(p_board_inf_uart_list[uart_id], 0, sizeof(board_inf_uart_t));
    }
    p_board_inf_uart_list[uart_id]->uart_id = uart_id;
    p_board_inf_uart_list[uart_id]->base_address = UARTStaticConf[uart_id].UARTBase;
    p_board_inf_uart_list[uart_id]->data_to_read_max = 0;
    p_board_inf_uart_list[uart_id]->desc_r = INVALID_DESC;
    p_board_inf_uart_list[uart_id]->desc_w = INVALID_DESC;
    
    // Allocate reiceive buffer
    if(  (!p_board_inf_uart_list[uart_id]->p_buff_out)
       &&(UARTStaticConf[uart_id].Tx_sz) ) 
    {
        p_board_inf_uart_list[uart_id]->p_buff_out = (char *)_sys_malloc(UARTStaticConf[uart_id].Tx_sz);
    } else {
        p_board_inf_uart_list[uart_id]->p_buff_out = 0;
    }
    
    //Allocate transmit buffer
    if(  (!p_board_inf_uart_list[uart_id]->p_buff_in)
       &&(UARTStaticConf[uart_id].Rx_sz) ) 
    {
        p_board_inf_uart_list[uart_id]->p_buff_in = (char *)_sys_malloc(UARTStaticConf[uart_id].Rx_sz);
    } else {
        p_board_inf_uart_list[uart_id]->p_buff_in = 0;
    }
    
    //
    // Configure the UART for 115,200, 8-N-1 operation.
    // This function uses SysCtlClockGet() to get the system clock
    // frequency.  This could be also be a variable or hard coded value
    // instead of a function call.
    //
    ROM_UARTConfigSetExpClk(UARTStaticConf[uart_id].UARTBase, 
                            SysCtlClockGet(), 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));
    //Enable Rx and Tx in UART registers
    UARTTxIntModeSet(UARTStaticConf[uart_id].UARTBase, UART_TXINT_MODE_EOT);//UART_TXINT_MODE_FIFO (default)
    UARTFIFOEnable(UARTStaticConf[uart_id].UARTBase);
    ROM_UARTEnable(UARTStaticConf[uart_id].UARTBase);
    
    //
    //test
#ifndef RELEASE
//    ROM_UARTCharPut(UARTStaticConf[uart_id].UARTBase, 'O');
//    ROM_UARTCharPut(UARTStaticConf[uart_id].UARTBase, 'K');
//    ROM_UARTCharPut(UARTStaticConf[uart_id].UARTBase, '\n');
#endif
#ifdef __KERNEL_UCORE_EMBOS
    //
    //Fast interrupts with Cortex M3 / M4 and M4F
    //Instead of disabling interrupts when embOS does atomic operations, the interrupt
    //level of the CPU is set to 128. Therefore all interrupt priorities higher than 128 can
    //still be processed. Please note, that lower priority numbers define a higher priority.
    //All interrupts with priority level from 0 to 127 are never disabled. These interrupts
    //are named Fast interrupts. You must not execute any embOS function from within a
    //fast interrupt function.    
    //
    ROM_IntPrioritySet(UARTStaticConf[uart_id].INTassign, 129);//priority >128 for embOS on Cortex-M, see above  //GD-TODO: make plan for interrupt levels with orther drivers/interrupts...
#endif
    //
    // Enable the UART interrupt.
    //
    ROM_IntEnable(UARTStaticConf[uart_id].INTassign);
    
    return 0;
}

/*--------------------------------------------
| Name:        dev_lm3s_uart_x_open
| Description: Open device
| Parameters : desc   : descriptor
|              o_flag : integer (O_RDONLY or/and O_WRONLY device type)
| Return Type: integer (-1) already open
|                      (0) OK
| Comments:    
| See:         
----------------------------------------------*/
int dev_lm3s_uart_x_open(desc_t desc, int o_flag){
    //
    board_inf_uart_t *p_board_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
    if(!p_board_inf_uart)
        return -1;
    
    // Read only Device opening
    if (o_flag & O_RDONLY) {
        if (p_board_inf_uart->desc_r >= 0) 
            return -1;  //already open: exclusive resource.        
        p_board_inf_uart->desc_r = desc;       
        p_board_inf_uart->input_loss = 0;
        p_board_inf_uart->read_head = 0;
        p_board_inf_uart->read_tail = 0;
        //At last, enable RX interrupts
        ROM_UARTIntEnable(p_board_inf_uart->base_address, UART_INT_RX | UART_INT_RT);
    }
    
    // Write only Device opening
    if (o_flag & O_WRONLY) {
        if (p_board_inf_uart->desc_w >= 0) 
            return -1;  //already open: exclusive resource.
        p_board_inf_uart->desc_w = desc;
        p_board_inf_uart->output_loss = 0;
        p_board_inf_uart->write_head = 0;
        p_board_inf_uart->write_tail = 0;
    }
//    p_board_inf_uart->o_flag |= o_flag; // set o_flag
  
    return 0;
}

/*--------------------------------------------
| Name:        dev_lm3s_uart_x_close
| Description: Called by the kernel
| Parameters : desc : descriptor
| Return Type: integer 
| Comments:    
| See:         
----------------------------------------------*/
int dev_lm3s_uart_x_close(desc_t desc){
  
  board_inf_uart_t *p_board_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
   
  if(ofile_lst[desc].oflag & O_RDONLY){
      if(!ofile_lst[desc].nb_reader){
          ROM_UARTIntDisable(p_board_inf_uart->base_address, UART_INT_RX | UART_INT_RT);
//          UARTFIFODisable(p_board_inf_uart->base_address);
          p_board_inf_uart->desc_r = INVALID_DESC;
      }
   }
   
   if(ofile_lst[desc].oflag & O_WRONLY){
      if(!ofile_lst[desc].nb_writer){
          ROM_UARTIntDisable(p_board_inf_uart->base_address, UART_INT_TX );
          p_board_inf_uart->desc_w = INVALID_DESC;
      }
   }

   return 0;
}

/*--------------------------------------------
| Name:        dev_lm3s_uart_x_isset_read
| Description: Called by Read POSIX interface
| Parameters : desc  : descriptor
| Return Type: integer (0)  : wait incomming data   
|                      (-1) : end of waiting
| Comments:    
| See:         
----------------------------------------------*/
int dev_lm3s_uart_x_isset_read(desc_t desc){    
    board_inf_uart_t *p_board_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
    if(!p_board_inf_uart)
        return -1;
    
    if(!p_board_inf_uart->p_buff_in) {
        // Synchronous mode
        if(ROM_UARTCharsAvail(p_board_inf_uart->base_address))
            return 0; //Data to read
        else
            return -1; //No data to read
    } else {
        //Asynchronous mode
        return(RX_BUFFER_EMPTY ? -1 : 0);
    }
}

/*--------------------------------------------
| Name:        dev_lm3s_uart_x_isset_write
| Description: Called by write POSIX interface
| Parameters : desc  : descriptor
| Return Type: int 
| Comments:    
| See:         
----------------------------------------------*/
int dev_lm3s_uart_x_isset_write (desc_t desc){
    board_inf_uart_t *p_board_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
    int uart_id = p_board_inf_uart->uart_id;
    if(!p_board_inf_uart)
        return -1;
    
    if(!p_board_inf_uart->p_buff_out) {
        // Synchronous mode
        return 0;//0==no Tx data (transmit finished);  0!=data to Tx  //-1;
    }else{
        //Asynchronous mode
//        return(TX_BUFFER_EMPTY ? 0 : -1); //Only accept new data if previous write is finished.
        return(TX_BUFFER_FULL ? -1 : 0); //Accept data if at least one byte is available in the output buffer
    }
}

/*--------------------------------------------
| Name:        dev_lm3s_uart_x_isset_write
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_lm3s_uart_x_read(desc_t desc, char* buf,int size){
    board_inf_uart_t *p_board_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
    int uart_id = p_board_inf_uart->uart_id;
    int cb = 0;
    int sz = 0;
    if(!p_board_inf_uart)
        return -1;
    
    if(!p_board_inf_uart->p_buff_in) {
        // Synchronous mode
        //
        // Loop while there are characters in the receive FIFO.
        //
        while(ROM_UARTCharsAvail(p_board_inf_uart->base_address) && (cb < size))
        {
            //
            // Read the next character from the UART
            //
            *buf++ = ROM_UARTCharGetNonBlocking(p_board_inf_uart->base_address);
            cb++;
        }
        //    *buf = UARTCharGet(p_board_inf_uart->base_address);
        //__fire_io_int(ofile_lst[p_board_inf_uart->desc_r].owner_pthread_ptr_read);
        return cb;//1;
    } else {
        //Asynchronous mode
        sz = RX_BUFFER_USED;
        if(sz > p_board_inf_uart->data_to_read_max) {
            p_board_inf_uart->data_to_read_max = sz;
        }
        //
        // Process characters untill input buffer empty or user buffer full.
        //
        while( /*(!RX_BUFFER_EMPTY)*/sz-- && (cb < size) )
        {
            //
            // Read the next character from the receive buffer.
            //
            *buf++ = p_board_inf_uart->p_buff_in[p_board_inf_uart->read_tail];
            ADVANCE_RX_BUFFER_INDEX(p_board_inf_uart->read_tail);
            cb++;
        }
        return cb;
    }
}

/*--------------------------------------------
| Name:        dev_lm3s_uart_x_write
| Description: Write buffer 
|              Called by Write Posix Interface
| Parameters : descriptor (desc_t)
|              buffer adress (char *)
|              buffer size
| Return Type: Integer : number of bytes written
| Comments:    
| See:         
----------------------------------------------*/
int dev_lm3s_uart_x_write(desc_t desc, const char* buf,int size){
    board_inf_uart_t *p_board_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
    int uart_id = p_board_inf_uart->uart_id;
    int count = size;
    if(!p_board_inf_uart->p_buff_out) {
        // Synchronous mode
        while(count)
        {
            count--;
            ROM_UARTCharPut(p_board_inf_uart->base_address, *buf++);
        }
        __fire_io_int(ofile_lst[p_board_inf_uart->desc_w].owner_pthread_ptr_write);
        return (size-count);
    } else {
        //Asynchronous mode
        
        // Send the characters
        for(count = 0; count < size; count++) {
            // Send the character to the UART output.
            if(!TX_BUFFER_FULL) {
                p_board_inf_uart->p_buff_out[p_board_inf_uart->write_head] = *buf++;
                ADVANCE_TX_BUFFER_INDEX(p_board_inf_uart->write_head);
            }else{
                // Buffer is full - return the actual count of written characters.
                p_board_inf_uart->output_loss += size - count;
                break;
            }
        }
        
        // If we have anything in the buffer, make sure that the UART is set
        // up to transmit it.
        if(!TX_BUFFER_EMPTY)
        {
            UART_PrimeTransmit(uart_id, p_board_inf_uart);
            ROM_UARTIntEnable(p_board_inf_uart->base_address, UART_INT_TX);
        }
        //
////        if(!TX_BUFFER_FULL)
////        {
////            __fire_io_int(ofile_lst[p_board_inf_uart->desc_w].owner_pthread_ptr_write);
////        }
        
        // Return the number of characters written.
        return(count);
    }
}

/*--------------------------------------------
| Name:        dev_lm3s_uart_x_seek
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_lm3s_uart_x_seek(desc_t desc,int offset,int origin){
   return 0;
}

/*--------------------------------------------
| Name:        dev_lm3s_uart_x_ioctl
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_lm3s_uart_x_ioctl(desc_t desc, int request, va_list ap)
{
 
   return 0;
}

/*============================================
| End of Source  : dev_lm3s9b96_uart.c
==============================================*/
