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


/*===========================================
Includes
=============================================*/
#include <stdlib.h>
#include <string.h>

#include "kernel/core/errno.h"
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/syscall.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"
#include "kernel/core/time.h"
#include "kernel/fs/vfs/vfs.h"
#include "kernel/fs/vfs/vfskernel.h"
#include "kernel/core/stat.h"
#include "kernel/core/devio.h"



/*===========================================
Global Declaration
=============================================*/




/*===========================================
Implementation
=============================================*/
/*--------------------------------------------
| Name:        _trylock_io
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _trylock_io(kernel_pthread_t* pthread_ptr, desc_t desc, int oflag){
   int r;
   __atomic_in();
   if( (r=__trylock_io(desc,oflag))!=EBUSY){
      pthread_ptr->io_desc = desc;
      /*kernel_pthread_mutex_lock(&ofile_lst[__desc__].mutex);*/\
   }
   __atomic_out();
   return r;
}

/*-------------------------------------------
| Name:select
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout){
   char b;
   char count=0;
   char wait=0;
   ldiv_t lr;
   pid_t pid;
   kernel_pthread_t* pthread_ptr;

   fd_set rfds=0;
   fd_set wfds=0;
   fd_set efds=0;

   fd_set unlock_rfds;
   fd_set unlock_wfds;
   fd_set unlock_efds;

   if(!(pthread_ptr = kernel_pthread_self()))
      return -1;

   if((pid= pthread_ptr->pid)<1)
      return -1;

   //
   rfds = readfds  ? *readfds  : 0;
   wfds = writefds ? *writefds : 0;
   efds = errorfds ? *errorfds : 0;

   //default timeout
   lr.quot = 10; //10 ms;

   //check and lock internal file descriptor
   for(b=0;b<sizeof(fd_set)|| b<nfds;b++){
      desc_t desc;

      //check
      if((desc = process_lst[pid]->desc_tbl[b])<0 )
         continue;
      /*
      if(!(ofile_lst[desc].oflag&O_NONBLOCK))
         continue;
      */

      //read
      if( ((rfds>>b) & 0x0001) ){
         pthread_ptr->io_desc = desc;
         if(_trylock_io(pthread_ptr,ofile_lst[desc].desc,O_RDONLY)!=EBUSY){
            //__atomic_in();
            {
               desc_t _desc=ofile_lst[desc].desc;
               //
               if(ofile_lst[_desc].owner_pthread_ptr_read!=pthread_ptr){
                  do{
                     //check
                     if(ofile_lst[_desc].used<=0){
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
                  }while((_desc=ofile_lst[_desc].desc_nxt[0])>=0);
               }
            }
            //__atomic_out();
         }else if (readfds){
            *readfds=(*readfds&(~(0x01<<b)));
         }
         continue;
      }

      //write
      if( ((wfds>>b) & 0x0001) ){
         pthread_ptr->io_desc = desc;
         if(_trylock_io(pthread_ptr,desc,O_WRONLY)!=EBUSY){
            __atomic_in();
            {
               desc_t _desc=ofile_lst[desc].desc;
               //
               if(ofile_lst[_desc].owner_pthread_ptr_write!=pthread_ptr){
                  do{
                     //check
                     if(ofile_lst[_desc].used<=0){
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
                  }while((_desc=ofile_lst[_desc].desc_nxt[1])>=0);
               }
            }
            __atomic_out();
         }else if(writefds){
            *writefds=(*writefds&(~(0x01<<b)));
         }
         continue;
      }

      //exception
      if( ((efds>>b) & 0x0001) ){
         pthread_ptr->io_desc = desc;
         if(_trylock_io(pthread_ptr,desc,O_WRONLY)!=EBUSY){
            //begin of section: protection from io interrupt
            __disable_interrupt_section_in();
            //
            ofile_lst[desc].owner_pid=pid;
            ofile_lst[desc].owner_pthread_ptr_write = pthread_ptr;
            //end of section: protection from io interrupt
            __disable_interrupt_section_out();
            //
         }else{
            *errorfds=(*errorfds&(~(0x01<<b)));
         }

         continue;
      }

   }

   //set file descriptor vector
   rfds = readfds  ? *readfds  : 0;
   wfds = writefds ? *writefds : 0;
   efds = errorfds ? *errorfds : 0;

   if(readfds)
      memset(readfds,0,sizeof(fd_set));
   if(writefds)
      memset(writefds,0,sizeof(fd_set));
   if(errorfds)
      memset(errorfds,0,sizeof(fd_set));

   //set unlock file descriptor vector
   unlock_rfds=rfds;
   unlock_wfds=wfds;
   unlock_efds=efds;

start:
   for(b=0;b<sizeof(fd_set)|| b<nfds;b++){
      desc_t desc;
      int isset=0;

      //check
      if((desc = process_lst[pid]->desc_tbl[b])<0 )
         continue;
      /*
      if(!(ofile_lst[desc].oflag&O_NONBLOCK))
         continue;
      */

      //read
      if( ((rfds>>b) & 0x0001) ){

         if(!ofile_lst[desc].pfsop->fdev.fdev_isset_read(desc)){
            isset=1;
            if(readfds)
               *readfds=(*readfds|((0x01<<b)));
         }else if(readfds){
            *readfds=(*readfds&(~(0x01<<b)));
         }
         //continue;
      }

      //write
      if( ((wfds>>b) & 0x0001) ){
         if(!ofile_lst[desc].pfsop->fdev.fdev_isset_write(desc)){
            isset=1;
            if(writefds)
               *writefds=(*writefds|((0x01<<b)));
         }else if(writefds){
            *writefds=(*writefds&(~(0x01<<b)));
         }
         //continue;
      }

      //exception
      if( ((efds>>b) & 0x0001) ){
         //to do: select on exception
         //continue;
      }

      if(isset)
         count++;
   }

   //wait
   if(!count && !wait){
      struct timespec abs_timeout;
      //
      if(timeout){
         abs_timeout.tv_sec   = timeout->tv_sec;
         abs_timeout.tv_nsec  = timeout->tv_usec*1000;
         __wait_io_int2(pthread_ptr,&abs_timeout);
      }else{
      /*   abs_timeout.tv_sec   = 0;
         abs_timeout.tv_nsec  = 0; */
    	 __wait_io_int2(pthread_ptr,NULL);
      }
      //

      //
      wait=1;
      goto start;
   }

   //unlock internal file descriptor
   for(b=0;b<sizeof(fd_set)|| b<nfds;b++){
      desc_t desc;
      int isset=0;

      //check
      if((desc = process_lst[pid]->desc_tbl[b])<0 )
         continue;
      /*
      if(!(ofile_lst[desc].oflag&O_NONBLOCK))
         continue;
      */

      //unlock descriptor
      if( ((unlock_rfds>>b) & 0x0001) ){
         __unlock_io(pthread_ptr,ofile_lst[desc].desc,O_RDONLY);//yes isset=1 or wait =1 then select is terminated
      }
      if( ((unlock_wfds>>b) & 0x0001) ){
         __unlock_io(pthread_ptr,ofile_lst[desc].desc,O_WRONLY);//yes isset=1 or wait =1 then select is terminated
      }
   }

   //reset file descriptor vector
   if(!count){
      if(readfds)
         memset(readfds,0,sizeof(fd_set));
      if(writefds)
         memset(writefds,0,sizeof(fd_set));
      if(errorfds)
         memset(errorfds,0,sizeof(fd_set));

   }

   return count;
}

/*-------------------------------------------
| Name:FD_CLR
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _FD_CLR(int fd, fd_set *fdset){
   *fdset=(*fdset&(~(0x01<<fd)));
}

/*-------------------------------------------
| Name:FD_ISSET
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _FD_ISSET(int fd, fd_set *fdset){
   if(fd<0)
      return 0;
   return ((*fdset) & ((0x01)<<fd));
}

/*-------------------------------------------
| Name:FD_SET
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _FD_SET(int fd, fd_set *fdset){
   *fdset=(*fdset|(0x01<<fd));
}

/*-------------------------------------------
| Name:FD_ZERO
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _FD_ZERO(fd_set *fdset){
   memset(fdset,0,sizeof(fd_set));
}


/*===========================================
End of Sourceselect.c
=============================================*/
