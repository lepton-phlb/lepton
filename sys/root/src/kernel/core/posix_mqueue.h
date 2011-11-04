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


/*============================================
| Compiler Directive   
==============================================*/
#ifndef _POSIX_MQUEUE_H
#define _POSIX_MQUEUE_H


/*============================================
| Includes 
==============================================*/


/*============================================
| Declaration  
==============================================*/

typedef struct dev_mq_msg_st {
   unsigned int  block_no;//msg number
	unsigned char priority; // 0 � 31.
	long size;
	struct dev_mq_msg_st * p_msg_next;
}dev_mq_msg_t;


typedef struct dev_mq_attr_st {
	struct mq_attr attr;
   //
   long            msg_align_sz;
   unsigned int    msg_vector_sz;
   unsigned int    msg_pool_sz;
   unsigned char*  p_msg_vector;
   unsigned char*  p_msg_pool;
	dev_mq_msg_t*   p_msg_head;
   //
   kernel_sem_t            kernel_sem;
   kernel_pthread_mutex_t  kernel_pthread_mutex;
}dev_mq_attr_t;

#endif
