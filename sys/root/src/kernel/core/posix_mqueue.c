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
| Includes
==============================================*/
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/syscall.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"
#include "kernel/fs/vfs/vfs.h"

#include "kernel/core/ioctl.h"
#include "kernel/core/kernel_sem.h"
#include "kernel/core/kernel_pthread_mutex.h"

#include "kernel/core/sys/mqueue.h"
#include "kernel/core/posix_mqueue.h"
/*============================================
| Global Declaration
==============================================*/
#define POSIX_MQUEUE_POOL_ALIGN 4 //alignement on 4 bytes

#define __mq_msg_align_sz(__size__) \
   ((__size__/POSIX_MQUEUE_POOL_ALIGN+((__size__%POSIX_MQUEUE_POOL_ALIGN)?1:0))*POSIX_MQUEUE_POOL_ALIGN)

int dev_posix_mqueue_load(void);
int dev_posix_mqueue_open(desc_t desc,int o_flag);
int dev_posix_mqueue_close(desc_t desc);
int dev_posix_mqueue_isset_read(desc_t desc);
int dev_posix_mqueue_isset_write(desc_t desc);
int dev_posix_mqueue_read(desc_t desc,char* buffer,int nbyte,...);
int dev_posix_mqueue_write(desc_t desc,const char* buffer,int nbyte,...);
int dev_posix_mqueue_seek(desc_t desc,int offset,int origin);
int dev_posix_mqueue_ioctl(desc_t desc,int request,va_list ap);


const char dev_posix_mqueue_name[]="mqueue";

dev_map_t dev_posix_mqueue_map={
   dev_posix_mqueue_name,
   S_IFCHR,
   dev_posix_mqueue_load,
   dev_posix_mqueue_open,
   dev_posix_mqueue_close,
   dev_posix_mqueue_isset_read,
   dev_posix_mqueue_isset_write,
   (fdev_read_t)dev_posix_mqueue_read,
   (fdev_write_t)dev_posix_mqueue_write,
   dev_posix_mqueue_seek,
   dev_posix_mqueue_ioctl
};



/*============================================
| Implementation
==============================================*/
/*--------------------------------------------
| Name:        _posix_mqueue_msg_get
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
dev_mq_msg_t* _posix_mqueue_msg_get(dev_mq_attr_t* p_mq_attr){

   unsigned int msg_vector_sz = p_mq_attr->msg_vector_sz;
   unsigned char*  p_msg_vector  = p_mq_attr->p_msg_vector;
   dev_mq_msg_t*   p_msg_new     = (dev_mq_msg_t*)0;

   unsigned int vector_block_no;
   unsigned int block_no;

   for(vector_block_no=0; vector_block_no<msg_vector_sz; vector_block_no++) {
      unsigned char b;
      unsigned char byte=p_msg_vector[vector_block_no];

      if (!byte)
         continue;

      for(b=0; b<8; b++) {
         char mask=(0x01<<b);

         if( (byte&mask) ) {
            char* p_new;
            p_msg_vector[vector_block_no]=byte&~mask;
            block_no=((vector_block_no<<3)+b);
            //
            if((long)block_no>=p_mq_attr->attr.mq_maxmsg)
               continue;
            //
            p_new=p_mq_attr->p_msg_pool+block_no*(p_mq_attr->msg_align_sz+sizeof(dev_mq_msg_t));

            p_msg_new=(dev_mq_msg_t*)(p_new+p_mq_attr->msg_align_sz);

            p_msg_new->block_no=block_no;
            p_msg_new->p_msg_next=(dev_mq_msg_t*)0;

            return p_msg_new;
         }
      }
   }

   return (dev_mq_msg_t*)0;
}

/*--------------------------------------------
| Name:        _posix_mqueue_msg_put
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
void _posix_mqueue_msg_put(dev_mq_attr_t* p_mq_attr,dev_mq_msg_t* p_msg){

   unsigned char*  p_msg_vector  = p_mq_attr->p_msg_vector;

   unsigned int block_no = p_msg->block_no;
   unsigned int vector_block_no=block_no>>3;

   char offset=(block_no-(vector_block_no<<3));
   //
   char mask=0x01<<offset;
   //
   p_msg_vector[vector_block_no]=p_msg_vector[vector_block_no]|(mask);
   //
   return;
}

/*--------------------------------------------
| Name:        _posix_mqueue_msg_read
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _posix_mqueue_msg_read(desc_t desc,void* buf,int size,unsigned int* priority){

   dev_mq_attr_t*  p_dev_mq_attr;
   unsigned long mq_msgsize;
   unsigned char*  p_msg_data;
   dev_mq_msg_t*   p_msg_head;
   dev_mq_msg_t*   p_msg;

   desc_t desc_head;

   //
   desc_head      = ofile_lst[desc].desc_prv;

   //
   p_dev_mq_attr =(dev_mq_attr_t*)ofile_lst[desc_head].p;
   mq_msgsize = p_dev_mq_attr->attr.mq_msgsize;


   //lock
   kernel_pthread_mutex_lock(&p_dev_mq_attr->kernel_pthread_mutex);
   //
   if(!p_dev_mq_attr->attr.mq_curmsgs) { //no msg available EAGAIN or ETIMEDOUT
      kernel_pthread_mutex_unlock(&p_dev_mq_attr->kernel_pthread_mutex);
      return -1;
   }
   //
   p_msg_head = p_dev_mq_attr->p_msg_head;
   p_msg      = p_msg_head;
   //
   if(!p_msg_head) { //corruption EBADMSG
      kernel_pthread_mutex_unlock(&p_dev_mq_attr->kernel_pthread_mutex);
      return -1;
   }
   //
   if(size<p_msg_head->size) { //size too small to copy data EMSGSIZE
      kernel_pthread_mutex_unlock(&p_dev_mq_attr->kernel_pthread_mutex);
      return -1;
   }
   //
   if(priority)
      *priority = p_msg_head->priority;
   //
   p_msg_data=(unsigned char*)(((unsigned char*)p_msg_head)- (p_dev_mq_attr->msg_align_sz) );
   //
   size=p_msg_head->size;
   memcpy(buf,p_msg_data,size);
   //
   p_dev_mq_attr->p_msg_head=p_msg_head->p_msg_next;
   //
   _posix_mqueue_msg_put(p_dev_mq_attr,p_msg_head);
   //
   p_dev_mq_attr->attr.mq_curmsgs--;
   //
   kernel_pthread_mutex_unlock(&p_dev_mq_attr->kernel_pthread_mutex);
   //put msg available with semaphore
   kernel_sem_post(&p_dev_mq_attr->kernel_sem);
   //
   return size;
}

/*--------------------------------------------
| Name:        _posix_mqueue_msg_write
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _posix_mqueue_msg_write(desc_t desc,const void* buf,int size,unsigned int priority, const struct timespec * abs_timeout){

   dev_mq_attr_t*  p_dev_mq_attr;
   unsigned long mq_msgsize = 0;
   unsigned char*  p_msg_data;
   dev_mq_msg_t*   p_msg_head;
   dev_mq_msg_t*   p_msg;
   dev_mq_msg_t*   p_msg_new;

   desc_t desc_head;
   desc_t desc_next_read;

   //
   desc_head      = ofile_lst[desc].desc_prv;
   if((desc_next_read = ofile_lst[desc_head].desc_nxt[0])<0)
      return -1;
   //
   p_dev_mq_attr =(dev_mq_attr_t*)ofile_lst[desc_head].p;
   mq_msgsize = p_dev_mq_attr->attr.mq_msgsize;

   //
   if((unsigned long)size>mq_msgsize)
      return -1;  //EMSGSIZE
   //
   //check msg availability with semaphore timed
   if((ofile_lst[desc].oflag&O_NONBLOCK) && kernel_sem_trywait(&p_dev_mq_attr->kernel_sem)<0)
      return -1;  //EAGAIN
   else if (kernel_sem_timedwait(&p_dev_mq_attr->kernel_sem,TIMER_ABSTIME,abs_timeout)<0)
      return -1;  //ETIMEDOUT

   //lock
   kernel_pthread_mutex_lock(&p_dev_mq_attr->kernel_pthread_mutex);
   //
   p_msg_head = p_dev_mq_attr->p_msg_head;
   p_msg      = (dev_mq_msg_t*)0; //p_msg_head;
   //
   if(!(p_msg_new=_posix_mqueue_msg_get(p_dev_mq_attr))) {
      //paranoiac protection
      kernel_pthread_mutex_unlock(&p_dev_mq_attr->kernel_pthread_mutex);
      return -1; //corruption EBADMSG panic!!!!
   }

   //
   p_msg_new->priority=(unsigned char)priority;
   p_msg_new->size=size;
   p_msg_data=(unsigned char*)(((unsigned char*)p_msg_new)- (p_dev_mq_attr->msg_align_sz) );
   memcpy(p_msg_data,buf,size);

   //
   while(p_msg_head && priority<=p_msg_head->priority) {
      p_msg=p_msg_head;
      p_msg_head=p_msg_head->p_msg_next;
   }

   if(p_msg) {
      ///bug fix; insert priority from fran�ois delaurat
      if(p_msg->p_msg_next)
         p_msg_new->p_msg_next=p_msg->p_msg_next;
      //
      p_msg->p_msg_next=p_msg_new;
   }else{
      p_msg_new->p_msg_next=p_msg_head;
      p_dev_mq_attr->p_msg_head=p_msg_new;
   }

   //if(p_dev_mq_attr->p_msg_head==p_msg)
   //p_dev_mq_attr->p_msg_head=p_msg_new;
   //
   p_dev_mq_attr->attr.mq_curmsgs++;
   //
   kernel_pthread_mutex_unlock(&p_dev_mq_attr->kernel_pthread_mutex);

   //prevent any operation on descriptor
   __atomic_in(); //__syscall_lock();
   //fire event to current writer
   if(ofile_lst[desc].owner_pthread_ptr_write)
      __fire_io(ofile_lst[desc].owner_pthread_ptr_write);
   //fire event to reader
   if(ofile_lst[desc_next_read].nb_reader>0 && ofile_lst[desc_next_read].owner_pthread_ptr_read)
      __fire_io(ofile_lst[desc_next_read].owner_pthread_ptr_read);
   //
   __atomic_out(); //__syscall_unlock();

   return 0;
}

/*-------------------------------------------
| Name:dev_posix_mqueue_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_posix_mqueue_load(void){
   return 0;
}

/*-------------------------------------------
| Name:dev_posix_mqueue_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/

int dev_posix_mqueue_open(desc_t desc,int o_flag){

   return 0;
}

/*-------------------------------------------
| Name:_posix_mqueue_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_posix_mqueue_close(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_posix_mqueue_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_posix_mqueue_isset_read(desc_t desc){
   desc_t desc_head = ofile_lst[desc].desc_prv;
   dev_mq_attr_t*  p_dev_mq_attr =(dev_mq_attr_t*)ofile_lst[desc_head].p;

   if(!p_dev_mq_attr)
      return -1;

   //lock
   kernel_pthread_mutex_lock(&p_dev_mq_attr->kernel_pthread_mutex);
   //
   if(p_dev_mq_attr->attr.mq_curmsgs) {
      //unlock
      kernel_pthread_mutex_unlock(&p_dev_mq_attr->kernel_pthread_mutex);
      return 0;
   }
   //unlock
   kernel_pthread_mutex_unlock(&p_dev_mq_attr->kernel_pthread_mutex);
   return -1;
}

/*-------------------------------------------
| Name:dev_posix_mqueue_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_posix_mqueue_isset_write(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:_posix_mqueue_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_posix_mqueue_read(desc_t desc,char* buffer,int nbyte,...){

   va_list ap;

   char* buf  = buffer;
   int size = nbyte;
   unsigned int* p_priority=(unsigned int*)0;

   va_start(ap, nbyte);

   if(!buf && !size) {
      buf        = va_arg(ap, char*);
      size       = va_arg(ap, int);
      p_priority = va_arg(ap, unsigned int*);
   }

   return _posix_mqueue_msg_read(desc,buf,size,p_priority);
}

/*-------------------------------------------
| Name:_posix_mqueue_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_posix_mqueue_write(desc_t desc,const char* buffer,int nbyte,...){

   va_list ap;

   const char* buf = buffer;
   int size      = nbyte;
   unsigned int priority = 0;
   struct timespec* abs_timeout= (struct timespec*)0;

   va_start(ap, nbyte);

   if(!buf && !size) {
      buf        = va_arg(ap, char*);
      size       = va_arg(ap, int);
      priority   = va_arg(ap, unsigned int);
      abs_timeout= va_arg(ap, struct timespec*);
   }

   return _posix_mqueue_msg_write(desc,buf,size,priority,abs_timeout);
}

/*-------------------------------------------
| Name:dev_posix_mqueue_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_posix_mqueue_seek(desc_t desc,int offset,int origin){
   return -1;
}

/*--------------------------------------------
| Name:        dev_posix_mqueue_ioctl
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_posix_mqueue_ioctl(desc_t desc,int request,va_list ap){

   switch(request) {
   //
   case I_LINK: {  //warning: in kernel context

      struct mq_attr* p_mq_attr;
      dev_mq_attr_t* p_dev_mq_attr;
      //
      desc_t desc_head = ofile_lst[desc].desc_prv;
      //
      if(ofile_lst[desc_head].p)   //attr already set.
         return 0;
      //third parameter //desc link
#if !defined(GNU_GCC)
      va_arg(ap, desc_t);
#else
      va_arg(ap, int);
#endif
      //fourth parameter
      p_mq_attr= va_arg(ap, struct mq_attr*);
      if(!p_mq_attr)
         return -1;
      //
      if(!(p_dev_mq_attr=_sys_malloc(sizeof(dev_mq_attr_t))))
         return -1;
      //
      memcpy(&p_dev_mq_attr->attr,p_mq_attr,sizeof(struct mq_attr));
      //
      //alignement
      p_dev_mq_attr->msg_align_sz = __mq_msg_align_sz(p_mq_attr->mq_msgsize);
      //
         p_dev_mq_attr->msg_pool_sz=p_mq_attr->mq_maxmsg*(p_dev_mq_attr->msg_align_sz+sizeof(dev_mq_msg_t));
      //
      if(!(p_dev_mq_attr->p_msg_pool=_sys_malloc(p_dev_mq_attr->msg_pool_sz))) {
         _sys_free(p_dev_mq_attr);
         return -1;
      }
      //
      p_dev_mq_attr->msg_vector_sz=(p_mq_attr->mq_maxmsg/8)+1;
      //
      if(!(p_dev_mq_attr->p_msg_vector=_sys_malloc(p_dev_mq_attr->msg_vector_sz))) {
         _sys_free(p_dev_mq_attr->p_msg_pool);
         _sys_free(p_dev_mq_attr);
         return -1;
      }
      //
      memset(p_dev_mq_attr->p_msg_vector,0xFF,p_dev_mq_attr->msg_vector_sz);
      //
      kernel_sem_init(&p_dev_mq_attr->kernel_sem,0,p_dev_mq_attr->attr.mq_maxmsg);
      //
      kernel_pthread_mutex_init(&p_dev_mq_attr->kernel_pthread_mutex,0);
      //
      p_dev_mq_attr->p_msg_head=(struct dev_mq_msg_st*)0;
      //
      p_dev_mq_attr->attr.mq_curmsgs=0;
      //
      ofile_lst[desc_head].p=p_dev_mq_attr;
      return 0;
   }
   break;

   //
   case I_UNLINK: {
   }
   break;

   //
   default:
      return -1;

   }
   return -1;
}

/*============================================
| End of Source  : posix_mqueue.c
==============================================*/
