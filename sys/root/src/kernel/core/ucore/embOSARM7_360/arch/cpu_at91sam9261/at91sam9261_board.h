/*--------------------------------------------
| Copyright(C) 2005 CHAUVIN-ARNOUX
|---------------------------------------------
| Project:         
| Project Manager: 
| Source:          at91sam9261_board.h
| Path:        X:\sources\embOSARM7_332
| Authors:     
| Plateform:   
| Created:     
| Revision/Date: $Revision: 1.1 $  $Date: 2010/02/08 13:28:28 $ 
| Description: 
|---------------------------------------------
| Historic:    
|---------------------------------------------
| Authors     | Date     | Comments  
| $Log: at91sam9261_board.h,v $
| Revision 1.1  2010/02/08 13:28:28  phlb
| at91sam9261 config and startup files for embos version 3.60.
|
| Revision 1.1  2009/03/30 15:48:59  jjp
| first import of tauon
|
| Revision 1.1  2009/03/30 11:18:55  jjp
| First import of tauon
|
| Revision 1.1  2008/06/16 09:30:20  phlb
| segger at91sam9261 specifics headers for configuration and startup.
|
|---------------------------------------------*/


/*============================================
| Compiler Directive   
==============================================*/
#ifndef _AT91SAM9261_BOARD_H
#define _AT91SAM9261_BOARD_H


/*============================================
| Includes 
==============================================*/


/*============================================
| Declaration  
==============================================*/
// Buttons
#define BUTTON1          (1UL << 20)
#define BUTTON2          (1UL << 19)
#define BUTTON3          (1UL << 18)
#define BUTTON4          (1UL << 17)
#define BUTTON5          (1UL << 16)
#define NB_BUTTONS       5
#define BUTTON_MASK      (BUTTON1|BUTTON2|BUTTON3|BUTTON4|BUTTON5)


// OSCILLATOR
#define SLOWCLOCK             (32768UL)      //* In Hz
#define MAIN_OSC_CLK          (18432000UL)   //* In Hz

// Configuration for a Quartz 18.432000 MHz
// 1MHz   <= PLL Input fequency   <= 32 MHz
// 80MHz  <= PLL Output fequency  <= 200 MHz (CKGR_PLL is 00)
// 190MHz <= PLL Output fequency  <= 240 MHz (CKGR_PLL is 10)

// PLLA 200.9088 MHz
#define MUL_PLLA    (55UL) //(55UL)    //(109UL)
#define DIV_PLLA    (10UL)
#define PLLA_FREQ   ((MAIN_OSC_CLK*MUL_PLLA)/DIV_PLLA)

// 47.9232 MHz
#define MUL_PLLB    (13UL)
#define DIV_PLLB    (5UL)
#define PLLB_FREQ   ((MAIN_OSC_CLK*MUL_PLLB)/DIV_PLLB)

// Processor Clock <= 238 MHz
#define Pclk        (PLLA_FREQ)

// Master Clock <= 119 MHz
#define Fmclk       (Pclk/2)

#define DELAY_PLL        100000
#define DELAY_MAIN_FREQ  100000

// USB Clock
#define USBclk      (PLLB_FREQ)

// System Timer period [us]
#define SYS_TMR_PER      (1000UL)

// SDRAM Base Address
#define AT91C_SDRAM      ((unsigned int *)0x20000000)

// Ethernet Base Address
#define BASE_DM9000A      ((unsigned short *)0x30000000)
#define DM9000A_DATA      0x1000



#endif /* __BOARD_H */
