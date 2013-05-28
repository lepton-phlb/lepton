

/*============================================
| Includes    
==============================================*/
#include "kernel/core/types.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"

#include "dev_os_debug.h"
#if defined(__KERNEL_UCORE_EMBOS)
#include "RTOS.h"
//void   OS_SendString(OS_ROM_DATA const char* s);  /* OSSend.c        */
//OS_RX_CALLBACK* OS_SetRxCallback(OS_RX_CALLBACK* cb);
#endif


/*============================================
| Global Declaration 
==============================================*/
os_inf_debug_t* p_os_inf_debug;

const char dev_os_debug_name[]="debug\0";

// Functions pointers
dev_map_t dev_os_debug_map={
    dev_os_debug_name,
    S_IFCHR,
    dev_os_debug_load,
    dev_os_debug_open,
    dev_os_debug_close,
    dev_os_debug_isset_read, //"data ready to read from app" 0:data available/ready to read, -1 no data to read 
    dev_os_debug_isset_write,//"data finished to send OR ready to send new data"  0:ready to send,  -1 otherwise (busy sending)
    dev_os_debug_read,
    dev_os_debug_write,
    dev_os_debug_seek,
    dev_os_debug_ioctl
};

void dev_os_debug_isr(char c);

/*============================================
| Implementation 
==============================================*/

/*--------------------------------------------
| Name:        dev_os_debug_load
| Description: Load device
| Parameters : none
| Return Type: integer -> driver loaded (=0)
| Comments:    
| See:         
----------------------------------------------*/
int dev_os_debug_load(void){//os_inf_debug_t *p_os_inf_debug){
//    int debug_no = 0;
    //  
    p_os_inf_debug = (os_inf_debug_t*)_sys_malloc(sizeof(os_inf_debug_t));//&os_inf_debug;
    if(!p_os_inf_debug){
        return -1;
    }
    p_os_inf_debug->desc_r = INVALID_DESC;
    p_os_inf_debug->desc_w = INVALID_DESC;
    p_os_inf_debug->data_to_read_max = 0;
    //
    //test
    #if defined(__KERNEL_UCORE_EMBOS)
    OS_SendString("Hello world!\r\n\0");
    #endif
    
    return 0;
}

/*--------------------------------------------
| Name:        dev_os_debug_open
| Description: Open device
| Parameters : desc   : descriptor
|              o_flag : integer (O_RDONLY or/and O_WRONLY device type)
| Return Type: integer (-1) already open
|                      (0) OK
| Comments:    
| See:         
----------------------------------------------*/
int dev_os_debug_open(desc_t desc, int o_flag){
    if(!p_os_inf_debug)
        return -1;
     
    // Read only Device opening
    if (o_flag & O_RDONLY) {
//        return -1; //Driver only support output
        if (p_os_inf_debug->desc_r >= 0) 
            return -1;  //already open: exclusive resource.
        p_os_inf_debug->read_head = 0;
        p_os_inf_debug->read_tail = 0;
        p_os_inf_debug->input_loss = 0;
        p_os_inf_debug->data_to_read = 0;
        OS_SetRxCallback(dev_os_debug_isr);
        p_os_inf_debug->desc_r = desc;        
    }
    
    // Write only Device opening
    if (o_flag & O_WRONLY) {
        if (p_os_inf_debug->desc_w >= 0) 
            return -1;  //already open: exclusive resource.
        
        p_os_inf_debug->desc_w = desc;
    }
//    p_os_inf_debug->o_flag |= o_flag; // set o_flag
  
    return 0;
}

/*--------------------------------------------
| Name:        dev_os_debug_close
| Description: Called by the kernel
| Parameters : desc : descriptor
| Return Type: integer 
| Comments:    
| See:         
----------------------------------------------*/
int dev_os_debug_close(desc_t desc){  
    if(!p_os_inf_debug)
        return -1;
    if(ofile_lst[desc].oflag & O_RDONLY){
        if(!ofile_lst[desc].nb_reader){
            OS_SetRxCallback(0);//cancel the call back
            p_os_inf_debug->read_head = 0;
            p_os_inf_debug->read_tail = 0;
            p_os_inf_debug->data_to_read = 0;
            p_os_inf_debug->desc_r = INVALID_DESC;
        }
    }
    if(ofile_lst[desc].oflag & O_WRONLY){
        if(!ofile_lst[desc].nb_writer){
            p_os_inf_debug->desc_w = INVALID_DESC;
        }
    }
    return 0;
}

/*--------------------------------------------
| Name:        dev_os_debug_isset_read
| Description: Called by Read POSIX interface
| Parameters : desc  : descriptor
| Return Type: integer (0)  : wait incomming data   
|                      (-1) : end of waiting
| Comments:    
| See:         
----------------------------------------------*/
int dev_os_debug_isset_read(desc_t desc){    
//return 0; //Data to read
//    return -1; //No data to read
#if defined(__KERNEL_UCORE_EMBOS)
    if(!p_os_inf_debug)
        return -1;
    if(p_os_inf_debug->data_to_read)
        return 0;
    else 
        return -1; //No data to read
#else
    return -1;//no data
#endif
}

/*--------------------------------------------
| Name:        dev_os_debug_isset_write
| Description: Called by write POSIX interface
| Parameters : desc  : descriptor
| Return Type: int 
| Comments:    
| See:         
----------------------------------------------*/
int dev_os_debug_isset_write (desc_t desc){
      return 0;//0==no Tx data (transmit finished/ready to Tx);  0!=data to Tx  //-1;
}

/*--------------------------------------------
| Name:        dev_os_debug_isset_write
| Description: Called by Read Posix interface
| Parameters : desc : descriptor
|              buf  : pointer on read buffer
|              size : size 
| Return Type: integer : number of bytes read
| Comments:    
| See:         
----------------------------------------------*/
int dev_os_debug_read(desc_t desc, char* buf,int size){
//    os_inf_debug_t *p_os_inf_debug = (os_inf_debug_t *)ofile_lst[desc].p;
//    *buf = ITM_ReceiveChar();//(p_os_inf_debug->base_address);
    
#if defined(__KERNEL_UCORE_EMBOS)
    uint16_t head;
    uint16_t tail;
    int len = 0;
    int cb = 0;
   
    if(!p_os_inf_debug)
        return -1;
    //Snapshot head that may be moved at any time from the interrupt
    head = p_os_inf_debug->read_head;
    tail = p_os_inf_debug->read_tail;
    len = 0;
    cb = 0;
    //
    if(!p_os_inf_debug->data_to_read)
        return 0;
    
    if(head <= tail) {
        //Copy first segment
        len = OS_DEBUG_IN_BUFF_SZ - tail;
        if(len > size)
            len = size;
        cb = len;
        memcpy(buf, &p_os_inf_debug->os_debug_in_buff[tail], len);
        if(size==cb){
            p_os_inf_debug->read_tail = tail+cb;
            return cb;
        }
        //Copy second segment
        len = head;
        if(cb+len > size)
            len = size - cb;
        cb += len;
        memcpy(buf+cb, &p_os_inf_debug->os_debug_in_buff[0], len);
        if(size==cb){
            tail += cb;
            if(tail >= OS_DEBUG_IN_BUFF_SZ)
                tail -= OS_DEBUG_IN_BUFF_SZ;
            p_os_inf_debug->read_tail = tail;
            p_os_inf_debug->data_to_read -= cb;
            return cb;
        }
    } else {
        len = head - tail;
        if(len > size)
            len = size;
        cb = len;
        memcpy(buf, &p_os_inf_debug->os_debug_in_buff[tail], len);
        if(size==cb){
            p_os_inf_debug->read_tail = tail+cb;
            p_os_inf_debug->data_to_read -= cb;
            return cb;
        }
    }
    p_os_inf_debug->read_tail = p_os_inf_debug->read_head;
    p_os_inf_debug->data_to_read -= cb;
    return cb;
    
#else
    return -1;
#endif
//    return 1;
}

/*--------------------------------------------
| Name:        dev_os_debug_write
| Description: Write buffer 
|              Called by Write Posix Interface
| Parameters : descriptor (desc_t)
|              buffer adress (char *)
|              buffer size
| Return Type: Integer : number of bytes written
| Comments:    
| See:         
----------------------------------------------*/
int dev_os_debug_write(desc_t desc, const char* buf,int size){
    int count;
    int cb;
    #if defined(__KERNEL_UCORE_EMBOS)
    if(!p_os_inf_debug)
        return -1;
    //
    count = size;
    cb = size;
    //
    while(size)
    {
        if(size > (OS_DEBUG_OUT_BUFF_SZ - 1))//max size -1
            count = OS_DEBUG_OUT_BUFF_SZ - 1;//max size -1
        else
            count = size;
        size -= count;
        memcpy(p_os_inf_debug->os_debug_out_buff, buf, count);
        buf += count;
        p_os_inf_debug->os_debug_out_buff[count] = 0;
        OS_SendString(p_os_inf_debug->os_debug_out_buff);
    }
    __fire_io_int(ofile_lst[p_os_inf_debug->desc_w].owner_pthread_ptr_write);
    return (cb);
    #else
    return -1;
    #endif
}

/*--------------------------------------------
| Name:        dev_os_debug_seek
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_os_debug_seek(desc_t desc,int offset,int origin){
   return 0;
}

/*--------------------------------------------
| Name:        dev_os_debug_ioctl
| Description: Called by ioctl
| Parameters :
|             descriptor desc_t  desc
|             int     request // $BM
|             va_list ap
| Return Type: int (-1 :)
| Comments:    
| See:         
----------------------------------------------*/
int dev_os_debug_ioctl(desc_t desc, int request, va_list ap){
   return 0;
}

/*--------------------------------------------
| Name       : dev_os_debug_isr
| Description: Generic Interrupt function for
|              reading bytes from input (the 
|              OS/debugger terminal I/O)
| Parameters : descriptor  (desc_t)
| Return Type: none
| Comments   : -   
| See        : -        
----------------------------------------------*/
void dev_os_debug_isr(char c){
   uint16_t tail; 
   uint16_t head;
#if defined(__KERNEL_UCORE_EMBOS)
    if(!p_os_inf_debug)
        return;
    if (p_os_inf_debug->desc_r < 0) 
        return;//Invalid descriptor (device not openned)
    //Snapshot at least tail which is subject to be moved by next read
    tail = p_os_inf_debug->read_tail; 
    head = p_os_inf_debug->read_head;
    
    if(head <= tail){
        if( (OS_DEBUG_IN_BUFF_SZ + head - tail) <= 1){ //"1" is the size of incomming data size
            p_os_inf_debug->input_loss++;
            OS_SendString("DEBUG input overflow\n\0");
            return; //buffer full
        }
    } else {
        if( (head != tail) && ((head - tail) <= 1) ){ //"1" is the size of incomming data size
            p_os_inf_debug->input_loss++;
            OS_SendString("DEBUG input overflow\n\0");
            return; //buffer full
        }
    }
    p_os_inf_debug->os_debug_in_buff[p_os_inf_debug->read_head++] = c;
    if(p_os_inf_debug->read_head >= OS_DEBUG_IN_BUFF_SZ)
        p_os_inf_debug->read_head = 0;//roll over ring buffer
    p_os_inf_debug->data_to_read++;
    if(p_os_inf_debug->data_to_read > p_os_inf_debug->data_to_read_max)
        p_os_inf_debug->data_to_read_max++;
    __fire_io_int(ofile_lst[p_os_inf_debug->desc_r].owner_pthread_ptr_read);
    #endif
}
/*============================================
| End of Source  : dev_os_debug.c
==============================================*/
