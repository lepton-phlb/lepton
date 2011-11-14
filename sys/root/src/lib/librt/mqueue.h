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
#ifndef _LIBRT_MQUEUE_H
#define _LIBRT_MQUEUE_H


/*============================================
| Includes 
==============================================*/

#include "kernel/core/sys/mqueue.h"

/*============================================
| Declaration  
==============================================*/

typedef int mqd_t;

#ifdef __cplusplus
extern "C" {
#endif

mqd_t    _mq_open(const char* name,int oflag,...);
int      _mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned int msg_prio,const struct timespec *abs_timeout);
ssize_t  _mq_timedreceive(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned int* msg_prio,const struct timespec *abs_timeout);
int      _mq_getattr(mqd_t mqdes,struct mq_attr* attr);

#ifdef __cplusplus
}
#endif

#define mq_open _mq_open
#define mq_send(__mqdes__,__msg_ptr__,__msg_len__,__msg_prio__) _mq_timedsend(__mqdes__,__msg_ptr__,__msg_len__,__msg_prio__,(const struct timespec *)0)
#define mq_receive(__mqdes__,__msg_ptr__,__msg_len__,__msg_prio__) _mq_timedreceive(__mqdes__,__msg_ptr__,__msg_len__,__msg_prio__,(const struct timespec *)0)

#define mq_timedsend(__mqdes__,__msg_ptr__,__msg_len__,__msg_prio__,__abs_timeout__) _mq_timedsend(__mqdes__,__msg_ptr__,__msg_len__,__msg_prio__,__abs_timeout__)
#define mq_timedreceive(__mqdes__,__msg_ptr__,__msg_len__,__msg_prio__,__abs_timeout__) _mq_timedreceive(__mqdes__,__msg_ptr__,__msg_len__,__msg_prio__,__abs_timeout__)

#define mq_getattr(__mqdes__,__attr__) _mq_getattr(__mqdes__,__attr__)

#endif
