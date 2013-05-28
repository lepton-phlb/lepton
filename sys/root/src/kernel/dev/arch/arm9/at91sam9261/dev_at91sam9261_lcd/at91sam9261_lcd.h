/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Philippe Le Boulanger.
Portions created by Philippe Le Boulanger are Copyright (C) 2011 <lepton.phlb@gmail.com>.
All Rights Reserved.

Contributor(s): Jean-Jacques Pitrolle <lepton.jjp@gmail.com>.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under
the MPL, indicate your decision by deleting  the provisions above and replace
them with the notice and other provisions required by the [eCos GPL] License.
If you do not delete the provisions above, a recipient may use your version of this file under
either the MPL or the [eCos GPL] License."
*/




/*============================================
| Compiler Directive
==============================================*/
#ifndef _AT91SAM9261_LCD_H
#define _AT91SAM9261_LCD_H


/*============================================
| Includes
==============================================*/
#include "kernel/core/etypes.h"

#if defined(__KERNEL_UCORE_ECOS)
   #include "cyg/hal/at91sam9261.h"
#endif
/*============================================
| Declaration
==============================================*/
#define AT91C_MASTER_CLOCK              DEV_AT91SAM9261_MASTER_CLOCK
#define AT91C_MASTER_CLOCK_FOR_I2S      DEV_AT91SAM9261_MASTER_CLOCK

#define AT91C_BAUD_RATE                 115200

#define AT91C_VERSION   "VER 1.0"


#define LCD_MONOCHROME       0
#define LCD_COLOR2              1
#define LCD_COLOR4              2
#define LCD_COLOR8              3
#define LCD_COLOR16             4

#define AT91C_FRAME_BUFFER_0    0x22000000L    // Adresse du Frame buffer ds la SDRAM
#define AT91C_FRAME_BUFFER_1    0x22080000L     // Adresse du Frame buffer ds la SDRAM
#define MEMO_ECRAN            0x22100000L    // Adresse de la recopie d'cran ds la SDRAM

/*   Frame buffer memory information    */
#if defined(__KERNEL_UCORE_EMBOS)
extern char *lcd_base;               // Start of framebuffer memory
extern char *lcd_console_address;       // Start of console buffer
extern short console_col;
extern short console_row;
extern char lcd_is_enabled;
extern int lcd_line_length;
extern int lcd_color_fg;
extern int lcd_color_bg;
#endif
//#include "main.h"

/*   Infos on LCDC for AT91SAM9261 like frame buffer address     */
struct lcdc_info
{
   AT91PS_LCDC lcdc;
   unsigned long frame_buffer;
};

/** LCD Controller structure for AT91SAM9261 **/
typedef struct dev_panel_info
{
   uint16_t vl_col;             // Number of columns (i.e. 640)
   uint16_t vl_row;             // Number of rows (i.e. 480)
   uint16_t vl_width;                // Width of display area in millimeters
   uint16_t vl_height;          // Height of display area in millimeters
   uint32_t vl_pixclock;        // pixel clock in ps

   /* LCD configuration register */
   uchar8_t vl_clkp;                 // Clock polarity
   uchar8_t vl_oep;             // Output Enable polarity
   uchar8_t vl_hsp;             // Horizontal Sync polarity
   uchar8_t vl_vsp;             // Vertical Sync polarity
   uchar8_t vl_dp;              // Data polarity
   uchar8_t vl_bpix;                 // Bits per pixel, 0 = 1, 1 = 2, 2 = 4, 3 = 8, 4 = 16
   uchar8_t vl_lbw;             // LCD Bus width, 0 = 4, 1 = 8
   uchar8_t vl_splt;                 // Split display, 0 = single-scan, 1 = dual-scan
   uchar8_t vl_clor;                 // Color, 0 = mono, 1 = color
   uchar8_t vl_tft;             // 0 = passive, 1 = TFT

   /* Horizontal control register. Timing from data sheet */
   uint16_t vl_hpw;             // Horz sync pulse width
   uchar8_t vl_blw;             // Wait before of line
   uchar8_t vl_elw;             // Wait end of line

   /* Vertical control register. */
   uchar8_t vl_vpw;             // Vertical sync pulse width
   uchar8_t vl_bfw;             // Wait before of frame
   uchar8_t vl_efw;             // Wait end of frame

   /* PXA LCD controller params */
   struct       lcdc_info controller;
} dev_panel_info_t;


#define LCD_BPP  LCD_COLOR8
//#define LCD_BPP LCD_MONOCHROME

#define FB_SYNC_HOR_HIGH_ACT         1  /* horizontal sync high active	*/
#define FB_SYNC_VERT_HIGH_ACT        2  /* vertical sync high active	*/

/* More or less configurable parameters for LCDC controller*/
#define SIDSAFB_FIFO_SIZE            512
#define SIDSAFB_DMA_BURST_LEN        127
#define SIDSAFB_CRST_VAL           0xc8   // 0xda

#define AT91C_PMC_HCK1        ((unsigned int) 0x1 << 17) // (PMC) AHB LCDCK Clock Output

//AT91PS_SYS  AT91_SYS = (AT91PS_SYS)AT91C_BASE_SYS;


/* Video functions */

void    lcd_putc        (const char c);
void    lcd_puts        (const char *s);
void    lcd_printf      (const char *fmt, ...);



/*  Information about displays we are using. This is for configuring
 *  the LCD controller and memory allocation. Someone has to know what
 *  is connected, as we can't autodetect anything.
*/
#define CFG_HIGH    0    // Pins are active high
#define CFG_LOW     1    // Pins are active low

/*----------------------------------------------------------------------*/
/* Default to 8bpp if bit depth not specified */
#ifndef LCD_BPP
   # define LCD_BPP  LCD_COLOR8
#endif

#ifndef LCD_DF
   # define LCD_DF   1
#endif

/* Calculate nr. of bits per pixel  and nr. of colors */
#define NBITS(bit_code)         (1 << (bit_code))
#define NCOLORS(bit_code)       (1 << NBITS(bit_code))

/***********************************/
/*        CONSOLE CONSTANTS        */
/***********************************/
#define  CONSOLE_COLOR_BLACK      0
#define  CONSOLE_COLOR_RED        1
#define  CONSOLE_COLOR_GREEN      2
#define  CONSOLE_COLOR_YELLOW     3
#define  CONSOLE_COLOR_BLUE       4
#define  CONSOLE_COLOR_MAGENTA    5
#define  CONSOLE_COLOR_CYAN       6
#define  CONSOLE_COLOR_GREY       14
#define  CONSOLE_COLOR_WHITE      255   // Must remain last / highest


/************************************************************************/
#ifndef PAGE_SIZE
   # define PAGE_SIZE     4096
#endif

/**************************************************/
/*        CONSOLE DEFINITIONS & FUNCTIONS         */
/**************************************************/
#define   CONSOLE_ROWS          (panel_info.vl_row / VIDEO_FONT_HEIGHT)
#define   CONSOLE_COLS        (panel_info.vl_col / VIDEO_FONT_WIDTH)
#define   CONSOLE_ROW_SIZE    (VIDEO_FONT_HEIGHT * lcd_line_length)
#define   CONSOLE_ROW_FIRST   (lcd_console_address)
#define   CONSOLE_ROW_SECOND  (lcd_console_address + CONSOLE_ROW_SIZE)
#define   CONSOLE_ROW_LAST    (lcd_console_address + CONSOLE_SIZE - CONSOLE_ROW_SIZE)
#define   CONSOLE_SIZE        (CONSOLE_ROW_SIZE * CONSOLE_ROWS)
#define   CONSOLE_SCROLL_SIZE (CONSOLE_SIZE - CONSOLE_ROW_SIZE)

#define COLOR_MASK(c)           (c)


#define   N25    0
#define   N35    1
#define   B25    2
#define   B35    3
#define   I25    4
#define   I35    5
#define   B35H   6


//lcd low level api
extern int at91sam9261_lcd_ctrl_init (dev_panel_info_t* p_panel_info);
extern void at91sam9261_lcd_set_lut(const PALETTEENTRY *palette);
extern unsigned long at91sam9261_lcd_switch_active_frame_buffer(void);
extern unsigned long at91sam9261_lcd_get_current_frame_buffer(void);
extern unsigned long at91sam9261_lcd_get_shadow_frame_buffer(void);
extern unsigned long at91sam9261_lcd_calc_frame_buffer_size(void);
extern PALETTEENTRY * at91sam9261_lcd_get_palette(void);
extern dev_panel_info_t* at91sam9261_lcd_get_panel_info(void);

#endif  /* _LCD_H_ */
