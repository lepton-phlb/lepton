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
#ifndef __LWIP_INET_H__
#define __LWIP_INET_H__

#include "lwip/opt.h"
#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

u16_t inet_chksum(void *data, u16_t len);
u16_t inet_chksum_pbuf(struct pbuf *p);
u16_t inet_chksum_pseudo(struct pbuf *p,
       struct ip_addr *src, struct ip_addr *dest,
       u8_t proto, u32_t proto_len);

u32_t inet_addr(const char *cp);
s8_t inet_aton(const char *cp, struct in_addr *addr);

#ifndef _MACHINE_ENDIAN_H_
#ifndef _NETINET_IN_H
#ifndef _LINUX_BYTEORDER_GENERIC_H
u16_t htons(u16_t n);
u16_t ntohs(u16_t n);
u32_t htonl(u32_t n);
u32_t ntohl(u32_t n);
#endif /* _LINUX_BYTEORDER_GENERIC_H */
#endif /* _NETINET_IN_H */
#endif /* _MACHINE_ENDIAN_H_ */

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_INET_H__ */

