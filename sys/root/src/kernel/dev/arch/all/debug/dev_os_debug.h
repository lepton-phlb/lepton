

/*============================================
| Compiler Directive   
==============================================*/
#ifndef _DEV_OS_DEBUG_H
#define _DEV_OS_DEBUG_H


/*============================================
| Includes 
==============================================*/


/*============================================
| Declaration  
==============================================*/

#define OS_DEBUG_OUT_BUFF_SZ 32
#define OS_DEBUG_IN_BUFF_SZ 16

typedef struct os_inf_debug_st{
//    int debug_no;
    desc_t desc_r;
    desc_t desc_w;
    unsigned short  read_head;
    unsigned short  read_tail;
    int             data_to_read;
    int             data_to_read_max;
    int             input_loss; //count number of bytes lost because application didn't get data fast enought
    char            os_debug_out_buff[OS_DEBUG_OUT_BUFF_SZ];
    char            os_debug_in_buff[OS_DEBUG_IN_BUFF_SZ];
}os_inf_debug_t;


int dev_os_debug_load         (void);//(os_inf_debug_t *);
int dev_os_debug_open         (desc_t, int);
int dev_os_debug_close        (desc_t);
int dev_os_debug_read         (desc_t, char *, int);
int dev_os_debug_write        (desc_t, const char*, int);
int dev_os_debug_isset_read   (desc_t);
int dev_os_debug_isset_write  (desc_t);
int dev_os_debug_seek         (desc_t, int, int);
int dev_os_debug_ioctl        (desc_t, int, va_list ap);

//void dev_os_debug_isr         (char);//void dev_os_debug_isr         (board_inf_uart_t*);

#endif