/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Chauvin-Arnoux.
Portions created by Chauvin-Arnoux are Copyright (C) 2011. All Rights Reserved.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under
the MPL, indicate your decision by deleting  the provisions above and replace
them with the notice and other provisions required by the [eCos GPL] License.
If you do not delete the provisions above, a recipient may use your version of this file under
either the MPL or the [eCos GPL] License."
*/

/*===========================================
Includes
=============================================*/
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/cpu.h"
#include "kernel/core/core_rttimer.h"
#include "kernel/fs/vfs/vfsdev.h"

#include "kernel/dev/arch/cortexm/k60n512/common/dev_k60n512_sim.h"
#include "kernel/dev/arch/cortexm/k60n512/common/dev_k60n512_gpio.h"
/*===========================================
Global Declaration
=============================================*/
const char dev_twrk60n512_gpio_leds_name[]="gleds\0";

static int dev_twrk60n512_gpio_leds_load(void);
static int dev_twrk60n512_gpio_leds_open(desc_t desc, int o_flag);

static int dev_twrk60n512_gpio_leds_load(void);
static int dev_twrk60n512_gpio_leds_open(desc_t desc, int o_flag);
static int dev_twrk60n512_gpio_leds_close(desc_t desc);
static int dev_twrk60n512_gpio_leds_read(desc_t desc, char* buf,int cb);
static int dev_twrk60n512_gpio_leds_write(desc_t desc, const char* buf,int cb);
static int dev_twrk60n512_gpio_leds_ioctl(desc_t desc,int request,va_list ap);

dev_map_t dev_twrk60n512_gpio_leds_map={
   dev_twrk60n512_gpio_leds_name,
   S_IFBLK,
   dev_twrk60n512_gpio_leds_load,
   dev_twrk60n512_gpio_leds_open,
   dev_twrk60n512_gpio_leds_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_twrk60n512_gpio_leds_read,
   dev_twrk60n512_gpio_leds_write,
   __fdev_not_implemented,
   dev_twrk60n512_gpio_leds_ioctl
};

#define  NUM_LEDS       4


#define  LED_BLUE_PORTA_PIN      10
#define  LED_ORANGE_PORTA_PIN    11
#define  LED_GREEN_PORTA_PIN     28
#define  LED_YELLOW_PORTA_PIN    29

#define LED_BLUE CYGHWR_HAL_KINETIS_PIN(A, LED_BLUE_PORTA_PIN, 1, 0)
#define LED_ORANGE CYGHWR_HAL_KINETIS_PIN(A, LED_ORANGE_PORTA_PIN, 1, 0)
#define LED_GREEN CYGHWR_HAL_KINETIS_PIN(A, LED_GREEN_PORTA_PIN, 1, 0)
#define LED_YELLOW CYGHWR_HAL_KINETIS_PIN(A, LED_YELLOW_PORTA_PIN, 1, 0)

static unsigned int gpio_base_addr = 0x400ff000;

static unsigned char _twrk60n512_gpio_leds_pin_tabs[] = {
   LED_ORANGE_PORTA_PIN,
   LED_GREEN_PORTA_PIN,
   LED_YELLOW_PORTA_PIN,
   LED_BLUE_PORTA_PIN,
};
/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:dev_twrk60n512_gpio_leds_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_twrk60n512_gpio_leds_load(void){
   volatile unsigned int reg_val = 0;

   hal_set_pin_function(LED_BLUE);
   hal_set_pin_function(LED_ORANGE);
   hal_set_pin_function(LED_GREEN);
   hal_set_pin_function(LED_YELLOW);

   //enable clock gating (SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK)
   HAL_READ_UINT32(REG_SIM_SCGC5_ADDR, reg_val);
   reg_val |= REG_SIM_SCGC5_PORTA_MASK;
   HAL_WRITE_UINT32(REG_SIM_SCGC5_ADDR, reg_val);

   //set gpio as output
   HAL_WRITE_UINT32(gpio_base_addr + REG_GPIO_PDDR, (1<<LED_BLUE_PORTA_PIN) |
                    (1<<LED_ORANGE_PORTA_PIN) | (1<<LED_GREEN_PORTA_PIN) | (1<<LED_YELLOW_PORTA_PIN));

   return 0;
}

/*-------------------------------------------
| Name:dev_twrk60n512_gpio_leds_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_twrk60n512_gpio_leds_open(desc_t desc, int o_flag){
   HAL_WRITE_UINT32(gpio_base_addr + REG_GPIO_PSOR, (1<<LED_BLUE_PORTA_PIN) |
                    (1<<LED_ORANGE_PORTA_PIN) | (1<<LED_GREEN_PORTA_PIN) | (1<<LED_YELLOW_PORTA_PIN));

   return 0;
}

/*-------------------------------------------
| Name:dev_twrk60n512_gpio_leds_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_twrk60n512_gpio_leds_close(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_twrk60n512_gpio_leds_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_twrk60n512_gpio_leds_read(desc_t desc, char* buf,int size){
   return size;
}

/*-------------------------------------------
| Name:dev_twrk60n512_gpio_leds_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_twrk60n512_gpio_leds_write(desc_t desc, const char* buf,int size){
   int i=0;

   //buf[0] ORANGE
   //buf[1] GREEN
   //buf[2] YELLOW
   //buf[3] BLUE
   if(size!=4) {
      return -1;
   }

   for(i=0; i<NUM_LEDS; i++) {
      if(buf[i]) {
         HAL_WRITE_UINT32(gpio_base_addr + REG_GPIO_PCOR, (1 << _twrk60n512_gpio_leds_pin_tabs[i]));
      }
      else {
         HAL_WRITE_UINT32(gpio_base_addr + REG_GPIO_PSOR, (1 << _twrk60n512_gpio_leds_pin_tabs[i]));
      }
   }

   return size;
}

/*-------------------------------------------
| Name:dev_twrk60n512_gpio_leds_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_twrk60n512_gpio_leds_seek(desc_t desc,int offset,int origin){
   return 0;
}

/*-------------------------------------------
| Name:dev_twrk60n512_gpio_leds_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_twrk60n512_gpio_leds_ioctl(desc_t desc,int request,va_list ap) {
   return 0;
}

/*============================================
| End of Source  : dev_k60n512_gpio_leds.c
==============================================*/
