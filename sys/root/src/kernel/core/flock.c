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
| Includes    
==============================================*/
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/syscall.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/flock.h"
#include "kernel/fs/vfs/vfskernel.h"


/*============================================
| Global Declaration 
==============================================*/
#ifdef __KERNEL_USE_FILE_LOCK
flock_t flock_lst[MAX_OPEN_FILE];
#endif

/*============================================
| Implementation 
==============================================*/

/*--------------------------------------------
| Name:        _get_flock
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _get_flock(inodenb_t inodenb){
#ifdef __KERNEL_USE_FILE_LOCK
   int i;
   for(i=0;i<MAX_OPEN_FILE;i++)
      if(flock_lst[i].inodenb<0){
         flock_lst[i].inodenb = inodenb;//now is locked
         return i;
      }
   return -1;//already locked
#else         
   return -1;//already locked
#endif
}

/*--------------------------------------------
| Name:        _put_flock
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _put_flock(inodenb_t inodenb){
#ifdef __KERNEL_USE_FILE_LOCK
   int i;
   for(i=0;i<MAX_OPEN_FILE;i++)
      if(flock_lst[i].inodenb==inodenb){
         flock_lst[i].inodenb = -1;//now is unlocked
         return i;
      }

   return -1;
#else
   return -1;
#endif
}

/*--------------------------------------------
| Name:        _is_locked
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _is_locked(inodenb_t inodenb){
#ifdef __KERNEL_USE_FILE_LOCK
   int i;
   for(i=0;i<MAX_OPEN_FILE;i++)
      if(flock_lst[i].inodenb!=inodenb)
         continue;
      else
         break;

   if(i==MAX_OPEN_FILE)
      return -1;//not locked
         
   return i;
#else
   return -1;
#endif
}

/*--------------------------------------------
| Name:        _sys_lock
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _sys_lock(kernel_pthread_t* pthread_ptr, unsigned int fd, unsigned int cmd, struct flock* p_flock){
   desc_t  desc;
   inodenb_t inodenb;
   int i;
   pid_t pid;
#ifdef __KERNEL_USE_FILE_LOCK
   pid = pthread_ptr->pid;
   if(pid<=0)
      return -1;

   desc=process_lst[pid]->desc_tbl[fd];
   inodenb = ofile_lst[desc].inodenb;

   //F_GETLK
   if(cmd==F_GETLK){
      if((i=_is_locked(inodenb))<0){
         //not locked
         p_flock->l_type = F_UNLCK;
      }else{
         //is locked
         p_flock->l_pid    = flock_lst[i]._flock.l_pid;
         p_flock->l_type   = flock_lst[i]._flock.l_type;
         p_flock->l_whence = flock_lst[i]._flock.l_whence;
         p_flock->l_start  = flock_lst[i]._flock.l_start;
         p_flock->l_len    = flock_lst[i]._flock.l_len ;
      }
      return 0;
   }

   //F_SETLK and F_SETLKW
   switch(p_flock->l_type){
      case F_RDLCK:
      case F_WRLCK:
         if(_is_locked(inodenb)<0){
            //not locked
            i = _get_flock(inodenb);
            if(i<0)
               return -1;

            //now is locked
            flock_lst[i]._flock.l_pid     = p_flock->l_pid;
            flock_lst[i]._flock.l_type    = p_flock->l_type;
            flock_lst[i]._flock.l_whence  = p_flock->l_whence;
            flock_lst[i]._flock.l_start   = p_flock->l_start;
            flock_lst[i]._flock.l_len     = p_flock->l_len;
            return 0;
         }
      
      //
      case F_UNLCK:{
         if((i=_is_locked(inodenb))<0)
            return -1;//not locked
         if( flock_lst[i]._flock.l_pid != p_flock->l_pid)
            return -1;//locked by another process: cannot be unlocked by the current process.
         if(_put_flock(inodenb)<0)
            return -1;
      }
      return 0;
      

   }
#endif
   return -1;
}

/*--------------------------------------------
| Name:        _sys_unlock
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _sys_unlockw(void){
#ifdef __KERNEL_USE_FILE_LOCK
   //read process list. take the highest priority process 
   //with lock call on the same node
   kernel_pthread_t* pthread_ptr;
   //
   pthread_ptr = g_pthread_lst;
   //
   while(pthread_ptr){
      if(pthread_ptr
         && (pthread_ptr->pid>0)
         && (pthread_ptr->stat&PTHREAD_STATUS_STOP)
         && !(pthread_ptr->stat&PTHREAD_STATUS_ZOMBI)
         && (pthread_ptr->reg.syscall==_SYSCALL_FCNTL) ){
            fcntl_t* _fcntl_dt = (fcntl_t*)pthread_ptr->reg.data;
            struct flock* _p_flock = (struct flock*)_fcntl_dt->argv[0];
            if(_fcntl_dt->cmd != F_SETLKW){//blocking call?
               pthread_ptr = pthread_ptr->gnext;
               continue;
            }// to do : bug fix: pthread_ptr = pthread_ptr->gnext;
            _fcntl_dt->ret = _sys_lock(pthread_ptr,_fcntl_dt->fd,_fcntl_dt->cmd,_p_flock);
            if(_fcntl_dt->ret<0){
               pthread_ptr = pthread_ptr->gnext;
               continue;
            }
            pthread_ptr->stat&=(~PTHREAD_STATUS_STOP);
            __kernel_pthread_resume(pthread_ptr);
            __kernel_ret_int(pthread_ptr); // locked ok, wakeup process
            break;
      }//endif
      pthread_ptr = pthread_ptr->gnext;
   }
   //
   return 0;
#else
   return -1;
#endif
}

/*--------------------------------------------
| Name:        _flocks()
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void _flocks(void){
#ifdef __KERNEL_USE_FILE_LOCK
   int i;
   for(i=0;i<MAX_OPEN_FILE;i++)
      flock_lst[i].inodenb=-1;
#endif
}

/*============================================
| End of Source  : flock.c
==============================================*/
