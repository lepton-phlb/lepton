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
#ifndef _SELECT_H
#define _SELECT_H


/*===========================================
Includes
=============================================*/
#include "kernel/core/types.h"

/*===========================================
Declaration
=============================================*/
#if defined(__GNUC__)
//typedef struct timeval timeval;
   #include "kernel/core/time.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout);
void  _FD_CLR(int fd, fd_set *fdset);
int   _FD_ISSET(int fd, fd_set *fdset);
void  _FD_SET(int fd, fd_set *fdset);
void  _FD_ZERO(fd_set *fdset);

#define FD_ISSET _FD_ISSET

#define  FD_SET(fd,fdset) *(fdset)=(*(fdset)|(0x01<<fd))
#define  FD_CLR(fd,fdset) *(fdset)=(*(fdset)&(~(0x01<<fd)))
#define  FD_ZERO(fdsetp) memset(fdsetp,0,sizeof(fd_set))

#ifdef __cplusplus
}
#endif

#endif
