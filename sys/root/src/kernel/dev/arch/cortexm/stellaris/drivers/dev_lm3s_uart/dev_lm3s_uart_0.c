/*--------------------------------------------
| Tachyon
|---------------------------------------------
| Project:         
| Project Manager: 
| Source:          dev_lm3s9b96_uart_0.c
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
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
//#include "driverlib/debug.h"
#include "driverlib/gpio.h"
//#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include "dev_lm3s_uart.h"

/*============================================
| Global Declaration 
==============================================*/
#define DEV_LM3S_UART_NO 0
//#define DEV_LM3S_UART_STATIC_MALLOC
#ifdef DEV_LM3S_UART_STATIC_MALLOC
static board_inf_uart_t board_inf_uart_0;
#endif

const char dev_lm3s_uart_0_name[]="ttys0\0";

static int dev_lm3s_uart_0_load(void);
static int dev_lm3s_uart_0_open(desc_t desc, int o_flag);

// uart Functions pointers
dev_map_t dev_lm3s_uart0_map={
    dev_lm3s_uart_0_name,
    S_IFCHR,
    dev_lm3s_uart_0_load,
    dev_lm3s_uart_0_open,
    dev_lm3s_uart_x_close,
    dev_lm3s_uart_x_isset_read,
    dev_lm3s_uart_x_isset_write,
    dev_lm3s_uart_x_read,
    dev_lm3s_uart_x_write,
    dev_lm3s_uart_x_seek,
    dev_lm3s_uart_x_ioctl
};

/*============================================
| Implementation 
==============================================*/
/*--------------------------------------------
| Name:        dev_lm3s_uart_0_load
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    inspired from embOS demo code "Start_LM3S9B96"
| See:         
----------------------------------------------*/
static int dev_lm3s_uart_0_load(void){
#ifdef DEV_LM3S_UART_STATIC_MALLOC
    board_inf_uart_0.uart_nb = DEV_LM3S_UART_NO;
    board_inf_uart_0.base_address = UART0_BASE;
    p_board_inf_uart_list[DEV_LM3S_UART_NO] = &board_inf_uart_0;
#endif
    //
    // Enable the peripherals used by this example.
    // The UART itself needs to be enabled, as well as the GPIO port
    // containing the pins that will be used.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    
    //
    // Configure the GPIO pin muxing for the UART function.
    // This is only necessary if your part supports GPIO pin function muxing.
    // Study the data sheet to see which functions are allocated per pin.
    // TODO: change this to select the port/pin you are using
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    
    //
    // Since GPIO A0 and A1 are used for the UART function, they must be
    // configured for use as a peripheral function (instead of GPIO).
    // TODO: change this to match the port/pin you are using
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    
    //
    if(dev_lm3s_uart_x_load(DEV_LM3S_UART_NO)<0)
        return -1;
    return 0;
}


/*--------------------------------------------
| Name:        dev_lm3s_uart_x_open
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static int dev_lm3s_uart_0_open(desc_t desc, int o_flag){  
    ofile_lst[desc].p = (board_inf_uart_t *)p_board_inf_uart_list[DEV_LM3S_UART_NO];
    return dev_lm3s_uart_x_open(desc, o_flag);
}

void UART0_IRQHandler() {
    dev_lm3s_uart_x_isr(0);
}
/*============================================
| End of Source  : dev_lm3s9b96_uart_0.c
==============================================*/