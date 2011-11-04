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

#include "lwip/opt.h"

#if LWIP_DNS && LWIP_SOCKET

#include <stddef.h> /* for size_t */

#include "lwip/sockets.h"

/* some rarely used options */
#ifndef LWIP_DNS_API_DECLARE_H_ERRNO
#define LWIP_DNS_API_DECLARE_H_ERRNO 1
#endif

#ifndef LWIP_DNS_API_DEFINE_ERRORS
#define LWIP_DNS_API_DEFINE_ERRORS 1
#endif

#ifndef LWIP_DNS_API_DECLARE_STRUCTS
#define LWIP_DNS_API_DECLARE_STRUCTS 1
#endif

#if LWIP_DNS_API_DEFINE_ERRORS
/** Errors used by the DNS API functions, h_errno can be one of them */
#define EAI_NONAME      200
#define EAI_SERVICE     201
#define EAI_FAIL        202
#define EAI_MEMORY      203

#define HOST_NOT_FOUND  210
#define NO_DATA         211
#define NO_RECOVERY     212
#define TRY_AGAIN       213
#endif /* LWIP_DNS_API_DEFINE_ERRORS */

#if LWIP_DNS_API_DECLARE_STRUCTS
struct hostent {
    char  *h_name;      /* Official name of the host. */
    char **h_aliases;   /* A pointer to an array of pointers to alternative host names,
                           terminated by a null pointer. */
    int    h_addrtype;  /* Address type. */
    int    h_length;    /* The length, in bytes, of the address. */
    char **h_addr_list; /* A pointer to an array of pointers to network addresses (in
                           network byte order) for the host, terminated by a null pointer. */
#define h_addr h_addr_list[0] /* for backward compatibility */
};

struct addrinfo {
    int               ai_flags;      /* Input flags. */
    int               ai_family;     /* Address family of socket. */
    int               ai_socktype;   /* Socket type. */
    int               ai_protocol;   /* Protocol of socket. */
    socklen_t         ai_addrlen;    /* Length of socket address. */
    struct sockaddr  *ai_addr;       /* Socket address of socket. */
    char             *ai_canonname;  /* Canonical name of service location. */
    struct addrinfo  *ai_next;       /* Pointer to next in list. */
};
#endif /* LWIP_DNS_API_DECLARE_STRUCTS */

#if LWIP_DNS_API_DECLARE_H_ERRNO
/* application accessable error code set by the DNS API functions */
extern int h_errno;
#endif /* LWIP_DNS_API_DECLARE_H_ERRNO*/

struct hostent *lwip_gethostbyname(const char *name);
int lwip_gethostbyname_r(const char *name, struct hostent *ret, char *buf,
                size_t buflen, struct hostent **result, int *h_errnop);
void lwip_freeaddrinfo(struct addrinfo *ai);
int lwip_getaddrinfo(const char *nodename,
       const char *servname,
       const struct addrinfo *hints,
       struct addrinfo **res);

#if LWIP_COMPAT_SOCKETS
#define gethostbyname(name) lwip_gethostbyname(name)
#define gethostbyname_r(name, ret, buf, buflen, result, h_errnop) \
       lwip_gethostbyname_r(name, ret, buf, buflen, result, h_errnop)
#define freeaddrinfo(addrinfo) lwip_freeaddrinfo(addrinfo)
#define getaddrinfo(nodname, servname, hints, res) \
       lwip_getaddrinfo(nodname, servname, hints, res)
#endif /* LWIP_COMPAT_SOCKETS */

#endif /* LWIP_DNS && LWIP_SOCKET */
