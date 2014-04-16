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


/*============================================
| Includes
==============================================*/
#include <stdarg.h>
#include "kernel/core/types.h"
#include "kernel/core/kal.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/ioctl.h"
#include "kernel/core/stat.h"
#include "kernel/core/devio.h"
#include "kernel/core/stropts.h"
#include "kernel/core/timer.h"

#include "kernel/core/sys/mqueue.h"
#include "kernel/core/posix_mqueue.h"

#include "kernel/fs/vfs/vfs.h"

#include "lib/librt/mqueue.h"


/*============================================
| Global Declaration
==============================================*/
extern dev_map_t dev_posix_mqueue_map;


/*============================================
| Implementation
==============================================*/
/*--------------------------------------------
| Name:        mq_open
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
mqd_t _mq_open(const char* name,int oflag,...){
   va_list ap;
   mode_t mode=0;
   struct mq_attr* p_attr=(struct mq_attr*)0;

   int fd;

   if(!name)
      return -1;

   if(oflag&O_CREAT) {

      int fd_head;
      int fd_mqueue_read;
      int fd_mqueue_write;

      int r=0;

      va_start(ap, oflag);

#if !defined(GNU_GCC)
      mode     = va_arg(ap, mode_t);
#else
      mode     = va_arg(ap, int);
#endif
      p_attr   = va_arg(ap, struct mq_attr*);

      va_end(ap);

      if(!p_attr)
         return -1;

      //
      if((fd_head=open("/dev/head",O_RDWR,0))<0)
         return -1;

      if((fd_mqueue_read=open("/dev/mqueue",O_RDONLY,0))<0) {
         close(fd_head);
         return -1;
      }

      if((fd_mqueue_write=open("/dev/mqueue",O_WRONLY,0))<0) {
         close(fd_head);
         close(fd_mqueue_read);
         return -1;
      }

      //
      oflag&=~(O_CREAT|O_EXCL);
      p_attr->mq_flags=oflag;

      if(ioctl(fd_head,I_LINK,fd_mqueue_read,p_attr)<0) {
         close(fd_head);
         close(fd_mqueue_read);
         close(fd_mqueue_write);
         return -1;
      }

      if(ioctl(fd_head,I_LINK,fd_mqueue_write,p_attr)<0) {
         close(fd_head);
         close(fd_mqueue_read);
         close(fd_mqueue_write);
         return -1;
      }

      //
      r=fattach(fd_head,name);

      close(fd_head);
      close(fd_mqueue_read);
      close(fd_mqueue_write);

      if(r<0)
         return -1;

   }

   //
   oflag&=~(O_CREAT|O_EXCL);
   //
   if((fd=open(name,oflag,mode))<0)
      return -1;

   return fd;
}

/*--------------------------------------------
| Name:        _mq_receive
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
ssize_t _mq_timedreceive(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned int* msg_prio,
                         const struct timespec *abs_timeout){
   pid_t pid;
   kernel_pthread_t* pthread_ptr;
   desc_t desc;
   desc_t desc_link;
   int cb;

   if(mqdes<0)
      return -1;

   if(msg_len<=0)
      return -1;

   if(!(pthread_ptr = kernel_pthread_self()))
      return -1;

   if((pid= pthread_ptr->pid)<1)
      return -1;

   desc = process_lst[pid]->desc_tbl[mqdes];

   if(desc<0)
      return -1;

   if(!(ofile_lst[desc].oflag&O_RDONLY))
      return -1;

   if(ofile_lst[desc].attr&(S_IFREG|S_IFDIR|S_IFBLK)) {
      return -1;
   }

   //is not implemented for this device dev?
   if(!ofile_lst[desc].pfsop->fdev.fdev_read)
      return -1;

   //check is mqueue?
   desc_link=ofile_lst[desc].desc; //head
   desc_link=ofile_lst[desc_link].desc_nxt[0]; //mqueue
   if(ofile_lst[desc_link].pfsop!=(pfsop_t)&dev_posix_mqueue_map)
      return -1;

   //
   __lock_io(pthread_ptr,ofile_lst[desc].desc,O_RDONLY);
   //check thread owner
   __atomic_in();
   {
      desc_t _desc=ofile_lst[desc].desc;
      //
      if(ofile_lst[_desc].owner_pthread_ptr_read!=pthread_ptr) {
         do {
            //check
            if(ofile_lst[_desc].used<=0) {
               __atomic_out();
               __unlock_io(pthread_ptr,ofile_lst[desc].desc,O_RDONLY);
               return -1; //error, stream not coherent :(
            }
            //
            //begin of section: protection from io interrupt
            __disable_interrupt_section_in();
            //
            ofile_lst[_desc].owner_pthread_ptr_read=pthread_ptr;
            ofile_lst[_desc].owner_pid=pid;
            //end of section: protection from io interrupt
            __disable_interrupt_section_out();
            //
            //aware: continue operation on original desc (see fattach() and _vfs_open() note 1)
         } while((_desc=ofile_lst[_desc].desc_nxt[0])>=0);
      }
   }
   __atomic_out();

   //
   if((ofile_lst[desc_link].attr&S_IFCHR) && !(ofile_lst[desc].oflag&O_NONBLOCK)) {
      fdev_posix_mqueue_read_t fdev_posix_mqueue_read =
         (fdev_posix_mqueue_read_t)ofile_lst[desc_link].pfsop->fdev.fdev_read;

      while(ofile_lst[desc_link].pfsop->fdev.fdev_isset_read
            && ofile_lst[desc_link].pfsop->fdev.fdev_isset_read(desc_link)) {
         if(__wait_io_int_abstime(pthread_ptr,abs_timeout)<0)   //wait incomming data
            break;
      }
      //
      cb=fdev_posix_mqueue_read(desc_link,(void*)0,0,(void*)msg_ptr,msg_len,msg_prio,abs_timeout);
      //profiler
      __io_profiler_stop(desc);
      __io_profiler_add_result(desc,O_RDONLY,msg_len,__io_profiler_get_counter(desc));
      //
      __unlock_io(pthread_ptr,ofile_lst[desc].desc,O_RDONLY);
      return cb;
   }else if((ofile_lst[desc_link].attr&S_IFCHR) && (ofile_lst[desc].oflag&O_NONBLOCK)) {
      fdev_posix_mqueue_read_t fdev_posix_mqueue_read =
         (fdev_posix_mqueue_read_t)ofile_lst[desc_link].pfsop->fdev.fdev_read;
      //profiler
      __io_profiler_start(desc);
      cb=fdev_posix_mqueue_read(desc_link,(void*)0,0,(void*)msg_ptr,msg_len,msg_prio,abs_timeout);
      __unlock_io(pthread_ptr,ofile_lst[desc].desc,O_RDONLY);
      //profiler
      __io_profiler_stop(desc);
      __io_profiler_add_result(desc,O_RDONLY,msg_len,__io_profiler_get_counter(desc));
      //
      return cb;
   }
   __unlock_io(pthread_ptr,ofile_lst[desc].desc,O_RDONLY);
   return -1;
}

/*--------------------------------------------
| Name:        _mq_send
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned int msg_prio,
                  const struct timespec *abs_timeout){

   pid_t pid;
   kernel_pthread_t* pthread_ptr;
   desc_t desc;
   desc_t desc_link;
   int cb;

   if(mqdes<0)
      return -1;

   if(msg_len<=0)
      return -1;

   if(!(pthread_ptr = kernel_pthread_self()))
      return -1;

   if((pid= pthread_ptr->pid)<1)
      return -1;

   desc = process_lst[pid]->desc_tbl[mqdes];

   if(desc<0)
      return -1;

   if(!(ofile_lst[desc].oflag&O_WRONLY))
      return -1;

   if(ofile_lst[desc].attr&(S_IFREG|S_IFDIR|S_IFBLK)) {
      return -1;
   }

   //is not implemented for this device dev?
   if(!ofile_lst[desc].pfsop->fdev.fdev_write)
      return -1;

   //check is mqueue?
   desc_link=ofile_lst[desc].desc; //head
   desc_link=ofile_lst[desc_link].desc_nxt[1]; //mqueue
   if(ofile_lst[desc_link].pfsop!=(pfsop_t)&dev_posix_mqueue_map)
      return -1;

   //
   __lock_io(pthread_ptr,ofile_lst[desc].desc,O_WRONLY);
   //check thread owner
   __atomic_in();
   {
      desc_t _desc=ofile_lst[desc].desc;
      //
      if(ofile_lst[_desc].owner_pthread_ptr_write!=pthread_ptr) {
         do {
            //check
            if(ofile_lst[_desc].used<=0) {
               __atomic_out();
               __unlock_io(pthread_ptr,ofile_lst[desc].desc,O_WRONLY);
               return -1; //error, stream not coherent :(
            }
            //
            //begin of section: protection from io interrupt
            __disable_interrupt_section_in();
            //
            ofile_lst[_desc].owner_pthread_ptr_write=pthread_ptr;
            ofile_lst[_desc].owner_pid=pid;
            //end of section: protection from io interrupt
            __disable_interrupt_section_out();
            //
            //aware: continue operation on original desc (see fattach() and _vfs_open() note 1)
         } while((_desc=ofile_lst[_desc].desc_nxt[1])>=0);
      }
   }
   __atomic_out();

   //
   if((ofile_lst[desc_link].attr&S_IFCHR) && !(ofile_lst[desc].oflag&O_NONBLOCK)) {
      fdev_posix_mqueue_write_t fdev_posix_mqueue_write =
         (fdev_posix_mqueue_write_t)ofile_lst[desc_link].pfsop->fdev.fdev_write;
      //profiler
      __io_profiler_start(desc);
      //
      if((cb=
             fdev_posix_mqueue_write(desc_link,(void*)0,0,(void*)msg_ptr,msg_len,msg_prio,
                                     abs_timeout))<0) {
         //profiler
         __io_profiler_stop(desc);
         __io_profiler_add_result(desc,O_WRONLY,0,0);
         //
         __unlock_io(pthread_ptr,ofile_lst[desc].desc,O_WRONLY);
         return -1;
      }

      do {
         __wait_io_int(pthread_ptr); //wait all data are transmitted
      } while(ofile_lst[desc_link].pfsop->fdev.fdev_isset_write
              && ofile_lst[desc_link].pfsop->fdev.fdev_isset_write(desc_link));
      //profiler
      __io_profiler_stop(desc);
      __io_profiler_add_result(desc,O_WRONLY,msg_len,__io_profiler_get_counter(desc));
      //
      //printf("__wait_io_int ok\n");
   }else if((ofile_lst[desc_link].attr&S_IFCHR) && (ofile_lst[desc].oflag&O_NONBLOCK)) {
      fdev_posix_mqueue_write_t fdev_posix_mqueue_write =
         (fdev_posix_mqueue_write_t)ofile_lst[desc_link].pfsop->fdev.fdev_write;
      //profiler
      __io_profiler_start(desc);
      //
      cb=fdev_posix_mqueue_write(desc_link,(void*)0,0,(void*)msg_ptr,msg_len,msg_prio,abs_timeout);
      //profiler
      __io_profiler_stop(desc);
      __io_profiler_add_result(desc,O_WRONLY,msg_len,__io_profiler_get_counter(desc));
   }

   //
   __unlock_io(pthread_ptr,ofile_lst[desc].desc,O_WRONLY);
   //
   return cb;
}

/*--------------------------------------------
| Name:        _mq_getattr
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _mq_getattr(mqd_t mqdes,struct mq_attr* attr){
   pid_t pid;
   kernel_pthread_t* pthread_ptr;
   desc_t desc;
   desc_t desc_head;
   desc_t desc_link;
   dev_mq_attr_t* p_dev_mq_attr;

   if(mqdes<0)
      return -1;

   if(!attr)
      return -1;

   if(!(pthread_ptr = kernel_pthread_self()))
      return -1;

   if((pid= pthread_ptr->pid)<1)
      return -1;

   desc = process_lst[pid]->desc_tbl[mqdes];

   if(desc<0)
      return -1;

   if(ofile_lst[desc].attr&(S_IFREG|S_IFDIR|S_IFBLK)) {
      return -1;
   }

   //check is mqueue?
   desc_head=ofile_lst[desc].desc; //head
   if(desc_head<0)
      return -1;

   if((ofile_lst[desc].oflag&O_RDONLY))
      desc_link = ofile_lst[desc_head].desc_nxt[0];
   else if((ofile_lst[desc].oflag&O_WRONLY))
      desc_link = ofile_lst[desc_head].desc_nxt[1];

   if(desc_link<0 || ofile_lst[desc_link].pfsop!=(pfsop_t)&dev_posix_mqueue_map)
      return -1;

   //get mq device attr
   p_dev_mq_attr=(dev_mq_attr_t*)ofile_lst[desc_head].p; //mqueue
   if(!p_dev_mq_attr)
      return -1;

   //lock
   kernel_pthread_mutex_lock(&p_dev_mq_attr->kernel_pthread_mutex);
   //
   attr->mq_curmsgs = p_dev_mq_attr->attr.mq_curmsgs;
   attr->mq_flags   = p_dev_mq_attr->attr.mq_flags;
   attr->mq_maxmsg  = p_dev_mq_attr->attr.mq_maxmsg;
   attr->mq_msgsize = p_dev_mq_attr->attr.mq_msgsize;
   //unlock
   kernel_pthread_mutex_unlock(&p_dev_mq_attr->kernel_pthread_mutex);
   //
   return 0;
}

/*============================================
| End of Source  : mq.c
==============================================*/
