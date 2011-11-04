/*
The contents of this file are subject to the Mozilla Public License Version 1.1 
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
specific language governing rights and limitations under the License.

The Original Code is ______________________________________.

The Initial Developer of the Original Code is ________________________.
Portions created by ______________________ are Copyright (C) ______ _______________________.
All Rights Reserved.

Contributor(s): ______________________________________.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license 
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable 
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under 
the MPL, indicate your decision by deleting  the provisions above and replace 
them with the notice and other provisions required by the [eCos GPL] License. 
If you do not delete the provisions above, a recipient may use your version of this file under 
either the MPL or the [eCos GPL] License."
*/

#ifndef VIRTUAL_CPU_H_
#define VIRTUAL_CPU_H_

//struct to send command
typedef struct {
   int hdwr_id;
   int cmd;
}virtual_cmd_t;

//new struct for hardware
//Serial
#define SHM_SERIAL_SIZE       2048//128
#define SHM_SERIAL_IOCTL      64

typedef struct __attribute__((packed)){
   unsigned char  data_in[SHM_SERIAL_SIZE];
   unsigned int   size_in;
   unsigned char  data_out[SHM_SERIAL_SIZE];
   unsigned int   size_out;
   unsigned char  data_ioctl[SHM_SERIAL_IOCTL];
}virtual_serial_t;

//Ethernet
#define SHM_ETH_SIZE          1600
#define SHM_ETH_IOCTL         16

typedef struct __attribute__((packed)){
   unsigned char  data_in[SHM_ETH_SIZE];
   unsigned int   size_in;
   unsigned char  data_out[SHM_ETH_SIZE];
   unsigned int   size_out;
   unsigned char  data_ioctl[SHM_ETH_IOCTL];
}virtual_eth_t;

//Keyboard
#define SHM_KB_IOCTL          8

typedef struct __attribute__((packed)){
   unsigned char  data_in;
   unsigned char  data_ioctl[SHM_KB_IOCTL];
}virtual_kb_t;

//Leds
#define SHM_LEDS_MAX          32
typedef struct __attribute__((packed)){
   unsigned char  data_out[SHM_LEDS_MAX];
}virtual_leds_t;

//Screen
#define SHM_SCRN_IOCTL        32

//now we have a pointer on it
typedef struct __attribute__((packed)){
   //on load
   //data_ioctl[0] is x_res
   //data_ioctl[1] is y_res
   //data_ioctl[2] is bpp
   unsigned char  data_ioctl[SHM_SCRN_IOCTL];
   //get a pointer on virtual_cpu screen
   unsigned char  * data_out;
}virtual_screen_t;


//Put All shared devices in that struct
typedef struct __attribute__((packed)) {
   virtual_serial_t  ttys0;
   virtual_serial_t  ttys1;
   virtual_serial_t  ttypt;
   virtual_eth_t     eth0;
   virtual_kb_t      kb0;
   virtual_leds_t    leds;
   virtual_serial_t  rtu0;
   virtual_serial_t  rtu1;
   virtual_screen_t  screen0;
}virtual_shared_dev_t;

//offset in shared device structure
#define TTYS0_OFFSET    0
#define TTYS1_OFFSET    (TTYS0_OFFSET+sizeof(virtual_serial_t))
#define TTYPT_OFFSET    (TTYS1_OFFSET+sizeof(virtual_serial_t))
#define ETH0_OFFSET     (TTYPT_OFFSET+sizeof(virtual_serial_t))
#define KB0_OFFSET      (ETH0_OFFSET+sizeof(virtual_eth_t))
#define LEDS_OFFSET     (KB0_OFFSET+sizeof(virtual_kb_t))
#define RTU0_OFFSET     (LEDS_OFFSET+sizeof(virtual_leds_t))
#define RTU1_OFFSET     (RTU0_OFFSET+sizeof(virtual_serial_t))
#define SCRN_OFFSET     (RTU1_OFFSET+sizeof(virtual_serial_t))


//size of shared memory segment
#define VIRTUAL_SHM_SIZE      2048*1024 //2M
#define VIRTUAL_SHM_KEY       0x1000

//list of hardware ID
enum hdwr_id {
   CLOCK,
   SERIAL_0,
   SERIAL_1,
   SERIAL_PT,
   ETH_0,
   KB,
   LEDS,
   RTU_0,
   RTU_1,
   SCREEN
};


//list of ops of hardware
enum hdrw_ops {
   OPS_LOAD,
   OPS_OPEN,
   OPS_CLOSE,
   OPS_READ,
   OPS_WRITE,
   OPS_SEEK,
   OPS_IOCTL,
   ACK
};

//
#define VMAX(X,Y) ( (X) > (Y) ? (X) : (Y))

#define DEBUG  1

#include <stdio.h>
#if defined(DEBUG)
#define DEBUG_TRACE(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG_TRACE(...)
#endif /* DEBUG */

//
int init_hardware(void * arg);
int decode_cmd(virtual_cmd_t *c);
#endif /* VIRTUAL_CPU_H_ */
