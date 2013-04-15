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


/*===========================================
Includes
=============================================*/
#include "kernel/core/kernel.h"
#include "kernel/core/net/uip_core/uip_core.h"

//#include "kernel/net/uip0.9/net/uip.h"
//#include "kernel/net/uip1.0/net/uip.h"
//#include "net/uip.h"

#if USE_UIP_VER == 1000 
#pragma message ("uip 1.0")
#include "kernel/net/uip1.0/net/uip.h"
#include "kernel/net/uip1.0/net/uip_arch.h"
#endif
#if USE_UIP_VER == 2500 
#pragma message ("uip 2.5")
#include "kernel/net/uip2.5/contiki-conf.h"
#include "kernel/net/uip2.5/net/uip.h"
#include "kernel/net/uip2.5/net/uip_arch.h"
#endif

/*===========================================
Global Declaration
=============================================*/
/* SLIP special character codes*/

#define END            192 /*0300*/    /* indicates end of packet */
#define ESC            219 /*0333*/    /* indicates byte stuffing */
#define ESC_END        220 /*0334*/    /* ESC ESC_END means END data byte */
#define ESC_ESC        221 /*0335*/    /* ESC ESC_ESC means ESC data byte */


/*===========================================
Implementation
=============================================*/


/*-------------------------------------------
| Name:_slip_send_packet
| Description:
| Parameters:
| Return Type:
| Comments:sends a packet of length "len", starting at
| location "p".
| See:
---------------------------------------------*/
int _slip_send_packet(desc_t desc, const unsigned char *p, int len)
{
   /* send an initial END character to flush out any data that may
   * have accumulated in the receiver due to line noise
   */
   _uip_core_send_char(desc,END);

   /* for each byte in the packet, send the appropriate character
   * sequence
   */
   while(len--) {
      switch(*p) {
      /* if it's the same code as an END character, we send a
      * special two character code so as not to make the
      * receiver think we sent an END
      */
      case END:
         _uip_core_send_char(desc,ESC);
         _uip_core_send_char(desc,ESC_END);
         break;

      /* if it's the same code as an ESC character,
      * we send a special two character code so as not
      * to make the receiver think we sent an ESC
      */
      case ESC:
         _uip_core_send_char(desc,ESC);
         _uip_core_send_char(desc,ESC_ESC);
         break;

      /* otherwise, we just send the character
      */
      default:
         _uip_core_send_char(desc,*p);
         break;
      }
      p++;
   }

   /* tell the receiver that we're done sending the packet
    */
   _uip_core_send_char(desc,END);
   return len;
}


/*-------------------------------------------
| Name:_slip_recv_packet
| Description:
| Parameters:
| Return Type:
| Comments:receives a packet into the buffer located at "p".
|      If more than len bytes are received, the packet will
|      be truncated.
|      Returns the number of bytes stored in the buffer.
| See:
---------------------------------------------*/
int _slip_recv_packet(desc_t desc, unsigned char *p, int len)
{
   unsigned char c;
   int received = 0;

   /* sit in a loop reading bytes until we put together
   * a whole packet.
   * Make sure not to copy them into the packet if we
   * run out of room.
   */
   while(1) {
      /* get a character to process
      */
      c = _uip_core_recv_char(desc);

      /* handle bytestuffing if necessary
      */
      switch(c) {

      /* if it's an END character then we're done with
      * the packet
      */
      case END:
         /* a minor optimization: if there is no
         * data in the packet, ignore it. This is
         * meant to avoid bothering IP with all
         * the empty packets generated by the
         * duplicate END characters which are in
         * turn sent to try to detect line noise.
         */
         if(received)
            return received;
         else
            break;

      /* if it's the same code as an ESC character, wait
      * and get another character and then figure out
      * what to store in the packet based on that.
      */
      case ESC:
         c = _uip_core_recv_char(desc);

         /* if "c" is not one of these two, then we
         * have a protocol violation.  The best bet
         * seems to be to leave the byte alone and
         * just stuff it into the packet
         */
         switch(c) {
         case ESC_END:
            c = END;
            break;

         case ESC_ESC:
            c = ESC;
            break;
         }

      /* here we fall into the default handler and let
      * it store the character for us
      */
      default:
         if(received < len)
            p[received++] = c;
      }
   }

   return received;
}

/*===========================================
End of Source uip_slip.c
=============================================*/




