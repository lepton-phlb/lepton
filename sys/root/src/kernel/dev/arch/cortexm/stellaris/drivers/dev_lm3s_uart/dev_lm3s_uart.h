/*--------------------------------------------
| Tachyon
|---------------------------------------------
| Project:         
| Project Manager: 
| Source:          dev_lm3s9b96_uart.h
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
| Compiler Directive   
==============================================*/
#ifndef _DEV_LM3S9B96_UART_H
#define _DEV_LM3S9B96_UART_H


/*============================================
| Includes 
==============================================*/
#include "driverlib/uart.h"
#include "driverlib/rom.h"
#include "driverlib/interrupt.h"

/*============================================
| Declaration  
==============================================*/
#define UART_BOARD_MAX 3
/**
 * @brief Holds the necessary information to use and configure a 
 *        UART peripheral.
 */
typedef struct
{
    //--------------------------------------------------------------
    //! Pointer to UART registers
    //--------------------------------------------------------------
    unsigned long  UARTBase;    
    // Stellaris interrupt assignments. (hw_ints.h)
    unsigned long  INTassign;     
    // Buffer size
    int            Tx_sz;
    int            Rx_sz;
}UARTStaticConf_t;
extern const UARTStaticConf_t UARTStaticConf[UART_BOARD_MAX];

typedef struct board_inf_uart_st{
    int             uart_id;
    unsigned long   base_address;
    
    desc_t          desc_r;
    desc_t          desc_w;
    
    char            *p_buff_out;
    char            *p_buff_in;
    unsigned int    read_head;
    unsigned int    read_tail;
    unsigned int    write_head;
    unsigned int    write_tail;
    unsigned int    data_to_read_max;
    unsigned int    input_loss; //count number of bytes lost because application didn't get data fast enought
    unsigned int    output_loss;
    
}board_inf_uart_t;

//*****************************************************************************
//
// Macros to determine number of free and used bytes in the transmit buffer.
//
//*****************************************************************************
#define UART_TX_BUFFER_SIZE     (UARTStaticConf[uart_id].Tx_sz)
#define TX_BUFFER_USED          (UART_GetBufferCount(p_board_inf_uart->write_tail,   \
                                                     p_board_inf_uart->write_head, \
                                                     UART_TX_BUFFER_SIZE))
#define TX_BUFFER_FREE          (UART_TX_BUFFER_SIZE - TX_BUFFER_USED)
#define TX_BUFFER_EMPTY         (UART_IsBufferEmpty(p_board_inf_uart->write_tail,   \
                                                    p_board_inf_uart->write_head))
#define TX_BUFFER_FULL          (UART_IsBufferFull(p_board_inf_uart->write_tail,   \
                                                   p_board_inf_uart->write_head, \
                                                   UART_TX_BUFFER_SIZE))
#define ADVANCE_TX_BUFFER_INDEX(Index) \
                                (Index) = ((Index) + 1) % UART_TX_BUFFER_SIZE

//*****************************************************************************
//
// Macros to determine number of free and used bytes in the receive buffer.
//
//*****************************************************************************
#define UART_RX_BUFFER_SIZE     (UARTStaticConf[uart_id].Rx_sz)
#define RX_BUFFER_USED          (UART_GetBufferCount(p_board_inf_uart->read_tail,   \
                                                     p_board_inf_uart->read_head, \
                                                     UART_RX_BUFFER_SIZE))
#define RX_BUFFER_FREE          (UART_RX_BUFFER_SIZE - RX_BUFFER_USED)
#define RX_BUFFER_EMPTY         (UART_IsBufferEmpty(p_board_inf_uart->read_tail,   \
                                                    p_board_inf_uart->read_head))
#define RX_BUFFER_FULL          (UART_IsBufferFull(p_board_inf_uart->read_tail,   \
                                                   p_board_inf_uart->read_head, \
                                                   UART_RX_BUFFER_SIZE))
#define ADVANCE_RX_BUFFER_INDEX(Index) \
                                (Index) = ((Index) + 1) % UART_RX_BUFFER_SIZE





extern board_inf_uart_t* p_board_inf_uart_list[UART_BOARD_MAX];


int dev_lm3s_uart_x_load         (int uart_id);
int dev_lm3s_uart_x_open         (desc_t, int);
int dev_lm3s_uart_x_close        (desc_t);
int dev_lm3s_uart_x_read         (desc_t, char *, int);
int dev_lm3s_uart_x_write        (desc_t, const char*, int);
int dev_lm3s_uart_x_isset_read   (desc_t);
int dev_lm3s_uart_x_isset_write  (desc_t);
int dev_lm3s_uart_x_seek         (desc_t, int, int);
int dev_lm3s_uart_x_ioctl        (desc_t, int, va_list ap);


inline int UART_IsBufferEmpty(unsigned int ulRead, unsigned int ulWrite) {
    return((ulWrite  == ulRead) ? true : false);
}

inline int UART_IsBufferFull(unsigned int ulRead, unsigned int ulWrite, unsigned int ulSize) {
    return((((ulWrite + 1) % ulSize) == ulRead) ? true : false);
}

inline unsigned int UART_GetBufferCount(unsigned int ulRead, unsigned int ulWrite, unsigned int ulSize) {
    return((ulWrite >= ulRead) ? (ulWrite - ulRead) :
                                 (ulSize - (ulRead - ulWrite)));
}

inline void UART_PrimeTransmit(int uart_id, board_inf_uart_t* p_board_inf_uart)
{
    //
    // Do we have any data to transmit?
    //
    if(!TX_BUFFER_EMPTY)
    {
        //
        // Disable the UART interrupt. If we don't do this there is a race
        // condition which can cause the read index to be corrupted.
        //
        ROM_IntDisable(UARTStaticConf[uart_id].INTassign);

        //
        // Yes - take some characters out of the transmit buffer and feed
        // them to the UART transmit FIFO.
        //
        while(ROM_UARTSpaceAvail(UARTStaticConf[uart_id].UARTBase) 
              && !TX_BUFFER_EMPTY)
        {
            ROM_UARTCharPutNonBlocking(UARTStaticConf[uart_id].UARTBase,
                                       p_board_inf_uart->p_buff_out[p_board_inf_uart->write_tail]);
            ADVANCE_TX_BUFFER_INDEX(p_board_inf_uart->write_tail);
        }

        //
        // Reenable the UART interrupt.
        //
        ROM_IntEnable(UARTStaticConf[uart_id].INTassign);
    }
}

inline void dev_lm3s_uart_x_isr  (int uart_id);
inline void dev_lm3s_uart_x_isr  (int uart_id)
{
    unsigned long ulStatus;
    char cChar;
    long lChar;
    //
    __hw_enter_interrupt();
    //
    board_inf_uart_t* p_board_inf_uart = p_board_inf_uart_list[uart_id];

    //
    // Get and clear the interrrupt status.
    //
    ulStatus = ROM_UARTIntStatus(UARTStaticConf[uart_id].UARTBase, true);
    ROM_UARTIntClear(UARTStaticConf[uart_id].UARTBase, ulStatus);
    
    //
    // Are we being interrupted because the TX FIFO has space available?
    //
    if( (ulStatus & UART_INT_TX)
       && UART_TX_BUFFER_SIZE ) // Ashync Tx mode activated for this UART?
    {
        //
        // Move as many bytes as we can into the transmit FIFO.
        //
        UART_PrimeTransmit(uart_id, p_board_inf_uart);

        //
        // If the output buffer is empty, turn off the transmit interrupt.
        //
        if(TX_BUFFER_EMPTY) {
            ROM_UARTIntDisable(UARTStaticConf[uart_id].UARTBase, UART_INT_TX);
//            __fire_io_int(ofile_lst[p_board_inf_uart->desc_w].owner_pthread_ptr_write);//
        }
        if(TX_BUFFER_FREE > (UART_TX_BUFFER_SIZE/2)){//Half free, ask for more from the app !
            __fire_io_int(ofile_lst[p_board_inf_uart->desc_w].owner_pthread_ptr_write);
        }
//        if(!TX_BUFFER_FULL) {
//            __fire_io_int(ofile_lst[p_board_inf_uart->desc_w].owner_pthread_ptr_write);//
//        }
    }
    //
    
    //
    // Are we being interrupted due to a received character?
    //
    if(  (ulStatus & (UART_INT_RX | UART_INT_RT))
       && UART_RX_BUFFER_SIZE ) //Push into buffer only if we have set one for this UART (otherwize shync mode is used)
    {
        //
        // Get all the available characters from the UART.
        //
        while(ROM_UARTCharsAvail(UARTStaticConf[uart_id].UARTBase))
        {
            //
            // Read a character
            //
            lChar = ROM_UARTCharGetNonBlocking(UARTStaticConf[uart_id].UARTBase);
            cChar = (unsigned char)(lChar & 0xFF);

            //
            // If there is space in the receive buffer, put the character
            // there, otherwise throw it away.
            //
            if(!RX_BUFFER_FULL)
            {
                //
                // Store the new character in the receive buffer
                //
                p_board_inf_uart->p_buff_in[p_board_inf_uart->read_head] = cChar;
                ADVANCE_RX_BUFFER_INDEX(p_board_inf_uart->read_head);
            }else{
                p_board_inf_uart->input_loss++;
            }
        }
        //
        // Tell the application there is data to get
        //
        if( p_board_inf_uart 
           && (p_board_inf_uart->desc_r >= 0) )
            __fire_io_int(ofile_lst[p_board_inf_uart->desc_r].owner_pthread_ptr_read);
    }
    __hw_leave_interrupt();
}



#endif