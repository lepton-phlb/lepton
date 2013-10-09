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

/**
 * \defgroup uipsplit uIP TCP throughput booster hack
 * @{
 *
 * The basic uIP TCP implementation only allows each TCP connection to
 * have a single TCP segment in flight at any given time. Because of
 * the delayed ACK algorithm employed by most TCP receivers, uIP's
 * limit on the amount of in-flight TCP segments seriously reduces the
 * maximum achievable throughput for sending data from uIP.
 *
 * The uip-split module is a hack which tries to remedy this
 * situation. By splitting maximum sized outgoing TCP segments into
 * two, the delayed ACK algorithm is not invoked at TCP
 * receivers. This improves the throughput when sending data from uIP
 * by orders of magnitude.
 *
 * The uip-split module uses the uip-fw module (uIP IP packet
 * forwarding) for sending packets. Therefore, the uip-fw module must
 * be set up with the appropriate network interfaces for this module
 * to work.
 */


/**
 * \file
 * Module for splitting outbound TCP segments in two to avoid the
 * delayed ACK throughput degradation.
 * \author
 * Adam Dunkels <adam@sics.se>
 *
 */

#ifndef __UIP_SPLIT_H__
#define __UIP_SPLIT_H__

/**
 * Handle outgoing packets.
 *
 * This function inspects an outgoing packet in the uip_buf buffer and
 * sends it out using the uip_fw_output() function. If the packet is a
 * full-sized TCP segment it will be split into two segments and
 * transmitted separately. This function should be called instead of
 * the actual device driver output function, or the uip_fw_output()
 * function.
 *
 * The headers of the outgoing packet is assumed to be in the uip_buf
 * buffer and the payload is assumed to be wherever uip_appdata
 * points. The length of the outgoing packet is assumed to be in the
 * uip_len variable.
 *
 */
void uip_split_output(void);

#endif /* __UIP_SPLIT_H__ */

/** @} */
/** @} */
