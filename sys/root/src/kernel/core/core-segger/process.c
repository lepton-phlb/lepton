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


/*===========================================
Includes
=============================================*/
#include <stdlib.h>

#include "kernel/core/errno.h"
#include "kernel/core/signal.h"
#include "kernel/core/kernel_pthread.h"
#include "kernel/core/kernel_sem.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"
#include "kernel/core/system.h"
#include "kernel/core/systime.h"
#include "kernel/core/time.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/flock.h"
#include "kernel/core/bin.h"
#include "kernel/core/env.h"
#include "kernel/fs/vfs/vfs.h"
#include "kernel/core/lib.h"
#include "kernel/core/malloc.h"



/*===========================================
Global Declaration
=============================================*/

pid_t pid_counter;

//patch: optimization for code memory occupation.
#if defined (CPU_WIN32)
process_t* __process_lst[PROCESS_MAX]={0};
#elif defined (__IAR_SYSTEMS_ICC) || defined (__IAR_SYSTEMS_ICC__)
process_t* __process_lst[PROCESS_MAX];
#endif

process_t** process_lst=0;
kernel_pthread_t* process_thread_lst=0;

const char __fds_size = sizeof(fds_bits_t)*8;
const unsigned char __shl_fds_bits =   (sizeof(fds_bits_t)+2);

//
char env_path[PATH_MAX]={0}; 

//to remove:only for debug
#if 0   
void display_stack(HANDLE h, char* title)
{

   CONTEXT context;

   context.ContextFlags = CONTEXT_FULL|CONTEXT_DEBUG_REGISTERS|CONTEXT_FLOATING_POINT|CONTEXT_EXTENDED_REGISTERS;

   GetThreadContext( h,&context);

   printf("---------%s---------\n",title);
   printf("SegGs=0x%x\n",context.SegGs);
   printf("SegFs=0x%x\n",context.SegFs);
   printf("SegEs=0x%x\n",context.SegEs);
   printf("SegDs=0x%x\n",context.SegDs);

   printf("SegSs=0x%x\n",context.SegSs);
   printf("SegCs=0x%x\n",context.SegCs);

   printf("Esp=0x%x\n",context.Esp);
   printf("Ebp=0x%x\n",context.Ebp);
   printf("Eip=0x%x\n",context.Eip);

   printf("EFlags=0x%x\n",context.EFlags);

}
#endif

/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:_init_pid
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
/*! \fn void _pid(void);
    \brief initialise les m�canismes de gestion de processus
*/
void _pid(void){

   pid_t _pid=1;
   //init and shift
   process_lst= &__process_lst[0];
   process_lst--;
   //
   for(_pid=1;_pid<=PROCESS_MAX;_pid++){
      process_lst[_pid]=0;
   }

   pid_counter=0;//PROCESS_MAX-1;
}

/*-------------------------------------------
| Name:get_nextpid
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
/*! \fn int _nextpid(pid_t* pid)
    \brief obtention d'un num�ro de pid

    \param pid pointeur sur la variable quie va recevoir le num�ro de pid.

    \return EAGAIN si erreur sinon 0.
*/
int _nextpid(pid_t* pid){

   pid_t _pid=pid_counter;

   do{

      if(++_pid>PROCESS_MAX)
         _pid=1;

      if(!process_lst[_pid]){
         pid_counter=_pid;
         *pid   = _pid;
         return 0;
      }
      
   }while(_pid!=pid_counter);


   return -EAGAIN;
}

/*--------------------------------------------
| Name:        _sys_process_insert_pthread
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _sys_process_insert_pthread(process_t* process,kernel_pthread_t* p){
   if(!process->pthread_ptr){
      process->pthread_ptr=p;
   }else if(process->pthread_ptr->next){
      struct kernel_pthread_st* next = process->pthread_ptr->next;
      process->pthread_ptr->next=(struct kernel_pthread_st*)p;
      p->next=next;
   }else{
      process->pthread_ptr->next=(struct kernel_pthread_st*)p;
   }
   return 0;
}

/*--------------------------------------------
| Name:        _sys_process_remove_pthread
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _sys_process_remove_pthread(process_t* process,kernel_pthread_t* p){
   struct kernel_pthread_st* pthread_ptr = (struct kernel_pthread_st*)process->pthread_ptr; 

   if(p!=process->pthread_ptr){
      while(pthread_ptr && (pthread_ptr->next!=(struct kernel_pthread_st*)p)){
         pthread_ptr=pthread_ptr->next;
      }
      pthread_ptr->next=p->next;
   }else{
      process->pthread_ptr=p->next;
   }
   return 0;
}
/*-------------------------------------------
| Name:_sys_getpid
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
pid_t _sys_getpid(void){

   pid_t pid=-1;

   kernel_pthread_t* pthread = kernel_pthread_self();

   if(!pthread)
      return -1;

   if(!(pid= pthread->pid))
      return -1;
   
   if(!process_lst[pid])
      return -1;

   if( (pthread->stat&(PTHREAD_STATUS_ZOMBI)) )
      return -1;

   return pid;
}

/*-------------------------------------------
| Name:_sys_getppid
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
pid_t _sys_getppid(void){

   pid_t pid=-1;

   kernel_pthread_t* pthread = kernel_pthread_self();

   if(!pthread)
      return -1;

   if(!(pid= pthread->pid))
      return -1;
      
   if(!process_lst[pid])
      return -1;

   if( (pthread->stat&(PTHREAD_STATUS_ZOMBI)) )
      return -1;

   return process_lst[pid]->ppid;
}


/*-------------------------------------------
| Name:_get_fd
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _get_fd(pid_t pid,int limit){
   unsigned char i;
   unsigned char d;
   int fd=0;
   for(i=0;i<__FDSET_LONGS;i++){
      for(d=0;d<__fds_size;d++){
         if( !((process_lst[pid]->fds_bits[i]>>d)&0x01) ){
            fd = ((i<<__shl_fds_bits)+d); //i*8+d
            if(fd<limit) continue;
            //
            process_lst[pid]->fds_bits[i]|=(0x01<<d);
            return fd;
         }
      }

   }

   return -1;
}

/*-------------------------------------------
| Name:_put_fd
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _put_fd(pid_t pid,int fd){

   unsigned char i;
   unsigned char d;

   i = (fd>>__shl_fds_bits); // i/8
   d = fd-(i<<__shl_fds_bits);//i modulo 8

   if( !((process_lst[pid]->fds_bits[i]>>d)&0x01) )
      return -1;//already closed

   process_lst[pid]->fds_bits[i]&=(~(0x01<<d));

   return 0;
}

/*--------------------------------------------
| Name:        _set_cloexec
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _set_cloexec(pid_t pid,int fd){
   unsigned char i;
   unsigned char d;

   i = (fd>>__shl_fds_bits); // i/8
   d = fd-(i<<__shl_fds_bits);//i modulo 8

   if( !((process_lst[pid]->fds_bits[i]>>d)&0x01) )
      return -1;//already closed

   //set flag
   process_lst[pid]->clx_bits[i]|=((0x01<<d));
   return 0;
}

/*--------------------------------------------
| Name:        _unset_cloexec
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _unset_cloexec(pid_t pid,int fd){

   unsigned char i;
   unsigned char d;

   i = (fd>>__shl_fds_bits); // i/8
   d = fd-(i<<__shl_fds_bits);//i modulo 8

   if( !((process_lst[pid]->fds_bits[i]>>d)&0x01) )
      return -1;//already closed

   //set flag
   process_lst[pid]->clx_bits[i]&=(~(0x01<<d));
   return 0;
}

/*--------------------------------------------
| Name:        _is_cloexec
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _is_cloexec(pid_t pid,int fd){
   unsigned char i;
   unsigned char d;

   i = (fd>>__shl_fds_bits); // i/8
   d = fd-(i<<__shl_fds_bits);//i modulo 8
   //
   if( !((process_lst[pid]->fds_bits[i]>>d)&0x01) )
      return -1;//already closed
   //
   return process_lst[pid]->clx_bits[i]&((0x01<<d));
}

/*--------------------------------------------
| Name:        _cloexec_process_fd
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _cloexec_process_fd(pid_t pid){
   unsigned char i;
   unsigned char d;
   int fd;
   desc_t desc;
   for(i=0;i<__FDSET_LONGS;i++){
      for(d=0;d<__fds_size;d++){
         if(    ((process_lst[pid]->fds_bits[i]>>d)&0x01)
             && ((process_lst[pid]->clx_bits[i]>>d)&0x01) ){
            //
            fd = ((i<<__shl_fds_bits)+d); //i*8+d
            if( (desc = process_lst[pid]->desc_tbl[fd])<0 )
               continue;
            //
            process_lst[pid]->fds_bits[i]&=(~(0x01<<d));
            process_lst[pid]->clx_bits[i]&=(~(0x01<<d));

            //close all pid file descriptor with flag set to FD_CLOEXEC
            _vfs_close(desc);
            //
            process_lst[pid]->desc_tbl[fd]=INVALID_DESC;
            
            //printf("copy [%d] oflag=0x%x rd=%d wr=%d\n",desc,ofile_lst[desc].oflag,ofile_lst[desc].nb_reader,ofile_lst[desc].nb_writer);
         }
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:_copy_process_fd
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _copy_process_fd(pid_t pid, pid_t ppid,int oflags){

   unsigned char i;
   unsigned char d;
   int fd;
   desc_t desc;
   for(i=0;i<__FDSET_LONGS;i++){
      for(d=0;d<__fds_size;d++){
         if( ((process_lst[ppid]->fds_bits[i]>>d)&0x01) ){
            process_lst[ppid]->fds_bits[i]|=(0x01<<d);
            fd = ((i<<__shl_fds_bits)+d); //i*8+d
            desc = process_lst[ppid]->desc_tbl[fd];
            
            if((ofile_lst[desc].oflag&O_RDONLY)>0)
               ofile_lst[desc].nb_reader++;

            if((ofile_lst[desc].oflag&O_WRONLY)>0)
               ofile_lst[desc].nb_writer++;

            //printf("copy [%d] oflag=0x%x rd=%d wr=%d\n",desc,ofile_lst[desc].oflag,ofile_lst[desc].nb_reader,ofile_lst[desc].nb_writer);
         }
      }
   }

   //copy all file descriptor
   memcpy(process_lst[pid]->desc_tbl,process_lst[ppid]->desc_tbl,__FD_SETSIZE);
   memcpy(process_lst[pid]->fds_bits,process_lst[ppid]->fds_bits,__FDSET_LONGS);
   
   return 0;
}

/*-------------------------------------------
| Name:_close_process_fd
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _close_process_fd(pid_t pid){

   unsigned char i;
   unsigned char d;
   int fd;
   desc_t desc;

   for(i=0;i<__FDSET_LONGS;i++){
      for(d=0;d<__fds_size;d++){
         if( ((process_lst[pid]->fds_bits[i]>>d)&0x01) ){
            int ilck;
            process_lst[pid]->fds_bits[i]|=(0x01<<d);
            fd = ((i<<__shl_fds_bits)+d); //i*8+d
            desc = process_lst[pid]->desc_tbl[fd];

            //close all ppid file descriptor
            if(desc>=0)
               _vfs_close(desc);

            process_lst[pid]->fds_bits[i]&=(~(0x01<<d));

            //unlock file
            #ifdef __KERNEL_USE_FILE_LOCK
            if((ilck=_is_locked(ofile_lst[desc].inodenb))<0)
               continue;//not locked
            if( flock_lst[ilck]._flock.l_pid != pid)
               continue;//locked by another process: cannot be unlocked by the current process.

            _put_flock(ofile_lst[desc].inodenb);//is unlocked
            #endif

         }
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:_sys_dup
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_dup(pid_t pid,int fd,int limit){
   int _fd;
   desc_t desc;

   if(fd>=__FD_SETSIZE)
      return -1;

   if((_fd = _get_fd(pid,limit))<0)
      return -1;

   process_lst[pid]->desc_tbl[_fd]=process_lst[pid]->desc_tbl[fd];

   desc = process_lst[pid]->desc_tbl[_fd];

   if(ofile_lst[desc].oflag&O_RDONLY)
      ofile_lst[desc].nb_reader++;

   if(ofile_lst[desc].oflag&O_WRONLY)
      ofile_lst[desc].nb_writer++;

   //printf("dup [%d] rd=%d wr=%d\n",desc,ofile_lst[desc].nb_reader,ofile_lst[desc].nb_writer);

   return _fd;
}

/*-------------------------------------------
| Name:_sys_dup2
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_dup2(pid_t pid,int fd,int fd2){

   unsigned char i;
   unsigned char d;
   desc_t desc;

   if(fd>=__FD_SETSIZE || fd2>=__FD_SETSIZE)
      return -1;

   //fd2 is free
   i = (fd2>>__shl_fds_bits); // i/8
   d = fd2-(i<<__shl_fds_bits);//i modulo 8

   if( ((process_lst[pid]->fds_bits[i]>>d)&0x01) )//if not free
      _vfs_close(process_lst[pid]->desc_tbl[fd2]); //close fd2
   else
      process_lst[pid]->fds_bits[i]|=(0x01<<d);// it was free but now not.


   process_lst[pid]->desc_tbl[fd2]=process_lst[pid]->desc_tbl[fd];

   desc = process_lst[pid]->desc_tbl[fd2];

   if(ofile_lst[desc].oflag&O_RDONLY)
      ofile_lst[desc].nb_reader++;

   if(ofile_lst[desc].oflag&O_WRONLY)
      ofile_lst[desc].nb_writer++;
   
   return fd2;
}

/*-------------------------------------------
| Name:_sys_fcntl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_fcntl(pid_t pid,unsigned int fd, unsigned int cmd, unsigned int argc,void* argv[])
{
    int result;
    desc_t desc;

    if (fd >= OPEN_MAX 
       || fd<0
       || (desc=process_lst[pid]->desc_tbl[fd])<0)
	   return -EBADF;

    switch (cmd) {
      case F_DUPFD:{
         int limit=0;
         if(argc>0)
            limit = (int)argv[0];
	      result = _sys_dup(pid,fd,limit);
      }
	   break;

      case F_GETFD:
         //to do: fcntl: F_GETFD(close on exec)
	      //result = test_bit(fd, &fils->close_on_exec);
	   break;

      case F_SETFD:
         //fcntl: F_SETFD(close on exec)
         if(((unsigned int)argv[0])&FD_CLOEXEC)
            result = _set_cloexec(pid,fd);
         else
            result = _unset_cloexec(pid,fd);
	   break;

      case F_GETFL:
   	   result = ofile_lst[desc].oflag;
	   break;

      case F_SETFL:{
         int oflag=0;
         if(argc>0)
            oflag = (int)argv[0];

	      /*
	       * In the case of an append-only file, O_APPEND
	       * cannot be cleared
	       */
	      result = -EPERM;
	      if (ofile_lst[desc].oflag&O_APPEND || oflag & O_APPEND) {
	          ofile_lst[desc].oflag &= ~(O_APPEND | O_NONBLOCK);
	          ofile_lst[desc].oflag |= oflag & (O_APPEND | O_NONBLOCK);
	          result = 0;
         }else{
            ofile_lst[desc].oflag = oflag;
            result = 0;
         }
      }
	   break;

      default:
	      result = EINVAL;
	   break;
    }

    return result;
}

/*--------------------------------------------
| Name:        _sys_pthread_create
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _sys_pthread_create(kernel_pthread_t** new_kernel_pthread,
                        kernel_pthread_t* parent_pthread_ptr,
                        pthread_attr_t *attr,
                        void *(*start_routine)(void*),void *arg, pid_t pid){

   kernel_pthread_t* pthread_ptr;
   //malloc kernel_pthread_t
   if( !(*new_kernel_pthread = (kernel_pthread_t*)_sys_malloc(sizeof(kernel_pthread_t))) )
      return -1;//kernel panic!!!

   //malloc stack
   if( !(attr->stackaddr = (char*)_sys_malloc(attr->stacksize)) )
      return -1;//kernel panic!!!

   memset((void *)(*new_kernel_pthread), 0, sizeof(kernel_pthread_t));
   //disable context switch
   __atomic_in();
   //
   pthread_ptr = (*new_kernel_pthread);
   //set pthread name
   attr->name = process_lst[pid]->arg;
   //create
   kernel_pthread_create((*new_kernel_pthread),attr,start_routine,arg);
   //attach pthread with process pid
   (*new_kernel_pthread)->pid = pid;
   //insert in process container
   _sys_process_insert_pthread(process_lst[pid],(*new_kernel_pthread));

   //signal handler inherit from parent pthread
   memcpy(pthread_ptr->sigaction_lst,parent_pthread_ptr->sigaction_lst,sizeof(sigaction_dfl_lst));

   //pthread sigqueue
   #ifdef __KERNEL_POSIX_REALTIME_SIGNALS
   memcpy(&pthread_ptr->kernel_sigqueue,&_kernel_sigqueue_initializer,sizeof(kernel_sigqueue_t));
   pthread_ptr->kernel_sigqueue.constructor(&process_lst[pid]->kernel_object_head, &pthread_ptr->kernel_sigqueue);
   #endif

   //load static library
   #ifdef __KERNEL_LOAD_LIB
      #if __KERNEL_LOAD_LIB_PTHREAD
         load_lib(pthread_ptr);
      #endif
   #endif

   //backup pthread start context (see vfork() backup stack operation
   __bckup_thread_start_context(pthread_ptr->start_context,pthread_ptr);
   //
   _dbg_printf("process(%d) create thread id=%d\n",(*new_kernel_pthread)->pid,(*new_kernel_pthread)->id);
   //enable context switch
   __atomic_out();
   return 0;
}

/*--------------------------------------------
| Name:        _sys_pthread_cancel
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _sys_pthread_cancel(kernel_pthread_t* kernel_pthread,pid_t pid){

   //access denied ?
   if(kernel_pthread->pid!=pid)
      return -1;

   //disable context switch
   __atomic_in();

   _dbg_printf("process(%d) cancel thread id=%d\n",kernel_pthread->pid,kernel_pthread->id);

   //unlock io desc if needed
   if(kernel_pthread->io_desc!=-1){
      //kernel_pthread_mutex_owner_destroy(kernel_pthread,&ofile_lst[kernel_pthread->io_desc].mutex);
      //to do: check if another procress took the sem and take the value to reinit the sem with this value
      kernel_sem_destroy(&ofile_lst[kernel_pthread->io_desc].sem_read);
      kernel_sem_destroy(&ofile_lst[kernel_pthread->io_desc].sem_write);
      kernel_sem_init(&ofile_lst[kernel_pthread->io_desc].sem_read,0,1);
      kernel_sem_init(&ofile_lst[kernel_pthread->io_desc].sem_write,0,1);
   }
   //cancel
   kernel_pthread_cancel(kernel_pthread);
   //remove from process container
   _sys_process_remove_pthread(process_lst[pid],kernel_pthread);
   //free stack
   _sys_free(kernel_pthread->attr.stackaddr);
   //detach pthread
   kernel_pthread->pid=(pid_t)-1;
   //free kernel_pthread_t
   _sys_free(kernel_pthread);
   //enable context switch
   __atomic_out();
   return 0;
}

/*--------------------------------------------
| Name:        _sys_pthread_cancel_all_except
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _sys_pthread_cancel_all_except(pid_t pid,kernel_pthread_t* except_pthread_ptr){
   kernel_pthread_t* pthread_ptr=process_lst[pid]->pthread_ptr;
   
   //disable context switch
   __atomic_in();
   //
   while(pthread_ptr){
      kernel_pthread_t* next_pthread_ptr=pthread_ptr->next;
      if(pthread_ptr!=except_pthread_ptr){
         _sys_pthread_cancel(pthread_ptr,pid);
      }
      //next
      pthread_ptr=next_pthread_ptr;
   }
   //enable context switch
   __atomic_out();
   //
   return 0;
}

/*-------------------------------------------
| Name:process_routine
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void     _system_exit(int status);
void* process_routine(void* arg){

   pid_t pid=0;

   _dbg_printf("__process_routine\r\n");

   pid=_sys_getpid();

   process_lst[pid]->status = process_lst[pid]->process_routine(process_lst[pid]->argc,process_lst[pid]->argv);
   process_lst[pid]->pthread_ptr->exit=NULL;

   _dbg_printf("__exit(%d)\n",pid);
   //call kernel. signal thread termination
   _system_exit(process_lst[pid]->status);

   //thread destroy
   return NULL;
}

/*-------------------------------------------
| Name:find_exec
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int find_exec(const char* path,exec_file_t* exec_file){
   int cb;
   desc_t desc;
   int i;

   //
   desc=_vfs_open((char*)path,O_RDONLY,0);
   if(desc<0){//error: file not exist
      for(i=0;i<max_env;i++){
         
         strcpy(env_path,__kernel_env[i]);
         
         if(path[0]!='/'){
            int len = strlen(env_path);
            env_path[len]='/';
            env_path[len+1]='\0';
         }
         //to do: chek length of env_path+path
         strcat(env_path,path);
         if((desc = _vfs_open((char*)env_path,O_RDONLY,0))>=0){
            //test if ref is a directory
            if(!(ofile_lst[desc].attr&S_IFDIR))
               break;

            _vfs_close(desc);
         }
      }
      if(desc<0)
         return -1;
   }
   cb=sizeof(exec_file_t);
   cb=_vfs_read(desc,(char*)exec_file,cb);
   _vfs_close(desc);

   return 0;
}

/*-------------------------------------------
| Name:_sys_krnl_exec
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
pid_t _sys_krnl_exec(const char* path, 
                  const char* argv[], 
                  const char* envp, 
                  pid_t ppid,
                  pid_t pid){

   pid_t _pid  = 0;
   void* arg   = (void*)argv;
   pthread_attr_t attr={0};
   exec_file_t    exec_file;
   int argc;
   int l;
   char* p;
   struct __timeval tv;

   //open binary file
   if(find_exec(path,&exec_file)<0)
      return -1;//cannot open binary file

   //check binary signature
   if(exec_file.signature!=EXEC_SIGNT)
      return -1;

   //get new pid
   if(_nextpid(&_pid)==-EAGAIN)
      return -ENOMEM;
   
   attr.priority  = bin_lst[exec_file.index].priority;

   //alloc process_t control block for the new process
   p = _sys_malloc(sizeof(process_t));
   if(!p)//kernel panic!!!
      return -ENOMEM;

   //
   process_lst[_pid] = (process_t*)p;
   memset(process_lst[_pid],0,sizeof(process_t));

   //alloc main thread stack for the new process
   attr.stackaddr = _sys_malloc(bin_lst[exec_file.index].stacksize); //(p + sizeof(process_t));
   if(!attr.stackaddr)
      return -ENOMEM;//kernel panic!!!

   attr.stacksize = bin_lst[exec_file.index].stacksize;
   attr.timeslice = bin_lst[exec_file.index].timeslice;
   
   process_lst[_pid]->process_routine = bin_lst[exec_file.index].start_routine;
   process_lst[_pid]->pthread_ptr=(kernel_pthread_t*)_sys_malloc(sizeof(kernel_pthread_t));//kernel panic!!!
   if(!process_lst[_pid]->pthread_ptr)
      return -ENOMEM;//kernel panic!!!

   memset(process_lst[_pid]->pthread_ptr,0,sizeof(kernel_pthread_t));

   //to remove:
   _dbg_printf("kernel execve _pid:%d\n",_pid);

   //reset flag CLOEXEC
   memset(process_lst[_pid]->clx_bits,0,__FDSET_LONGS);
   //reset fd bitmap
   memset(process_lst[_pid]->fds_bits,0,__FDSET_LONGS);
   //
   memset(process_lst[_pid]->desc_tbl,-1,__FD_SETSIZE*sizeof(desc_t));
   //
  
   //argument parser
   //to do: test len of argument: warning exception!!!.
   //reset argument list
   for(argc=0;argc<ARG_MAX;argc++){
      process_lst[_pid]->argv[argc]=(char*)0;
   }

   strcpy(process_lst[_pid]->arg,path);
   for(argc=1;argc<ARG_MAX;argc++){
      strcat(process_lst[_pid]->arg," ");
      if(!argv || !argv[argc]) break;
      strcat(process_lst[_pid]->arg,argv[argc]);
   }
   //
   argc=0;
   process_lst[_pid]->argc=0;
   
   //
   p = process_lst[_pid]->arg;
   process_lst[_pid]->argv[argc]=p;
   l = strlen(path);
   process_lst[_pid]->argv[argc][l]='\0';
   while(argv[++argc]){
      p = p+l+1;
      process_lst[_pid]->argv[argc]= p/*strtok(0," ")*/;
      l = strlen(argv[argc]);
      process_lst[_pid]->argv[argc][l]='\0';
   }
   process_lst[_pid]->argc=argc;

   //heritage  pid <- ppid
   if(ppid){
      process_lst[_pid]->inode_curdir = process_lst[ppid]->inode_curdir;
      process_lst[_pid]->pgid = process_lst[ppid]->pgid;
   }else{
      process_lst[_pid]->pgid = ppid;//in this case 0 it's a kernel process daemon
      process_lst[_pid]->inode_curdir = 0;
   }

   // set thread name
   attr.name = process_lst[_pid]->arg;
   //create thread
   if(kernel_pthread_create(process_lst[_pid]->pthread_ptr, &attr,process_routine,arg)<0)
      return -ENOMEM;

   //attach thread with process
   process_lst[_pid]->pthread_ptr->pid = _pid;
   //fill process stucture
   _sys_gettimeofday(&tv,NULL);
   process_lst[_pid]->pid     = _pid;
   process_lst[_pid]->ppid    = ppid;
   process_lst[_pid]->start_time    = tv.tv_sec;
   //prepare kernel object chained list (see _sys_vfork() )
   process_lst[_pid]->kernel_object_head = (kernel_object_t*)0;
   //
   process_lst[_pid]->pthread_ptr->stat=PTHREAD_STATUS_NULL;
   //
   process_lst[_pid]->pthread_ptr->time_out  = (time_t)-1;//for alarm()
   //
   process_lst[_pid]->pthread_ptr->parent_pthread_ptr = (kernel_pthread_t*)0;
   //restore default sig handler
   memcpy(process_lst[_pid]->pthread_ptr->sigaction_lst,sigaction_dfl_lst,sizeof(sigaction_dfl_lst));

   //thread sigqueue
   #ifdef __KERNEL_POSIX_REALTIME_SIGNALS
   memcpy(&process_lst[_pid]->pthread_ptr->kernel_sigqueue,&_kernel_sigqueue_initializer,sizeof(kernel_sigqueue_t));
   process_lst[_pid]->pthread_ptr->kernel_sigqueue.constructor(&process_lst[_pid]->kernel_object_head, &process_lst[_pid]->pthread_ptr->kernel_sigqueue);
   #endif

   //atexit registred functions
   #if ATEXIT_MAX>0
      process_lst[_pid]->p_atexit_func  = (atexit_func_t*) kernel_pthread_alloca( process_lst[_pid]->pthread_ptr,(ATEXIT_MAX+1)*sizeof(atexit_func_t));
   #endif 

   //load static library
   #ifdef __KERNEL_LOAD_LIB
   load_lib(process_lst[_pid]->pthread_ptr);
   #endif

   //
   #ifdef KERNEL_PROCESS_VFORK_CLRSET_IRQ
   __clr_irq();
   #endif
   __bckup_thread_start_context(process_lst[_pid]->pthread_ptr->start_context,process_lst[_pid]->pthread_ptr);
   #ifdef KERNEL_PROCESS_VFORK_CLRSET_IRQ
   __set_irq();
   #endif

   return _pid;
}

/*-------------------------------------------
| Name:_sys_exec
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
pid_t _sys_exec(const char* path, 
                  const char* argv[], 
                  const char* envp, 
                  kernel_pthread_t* pthread_ptr){

   void* arg   = (void*)argv;
   pthread_attr_t attr;
   exec_file_t    exec_file;
   int argc;
   int l;
   char* p=0;
   pid_t pid = 0;
   pid_t ppid = 0;
   struct __timeval tv;

   //profiler
   __kernel_profiler_start();
   //

   //get pid
   pid = pthread_ptr->pid;
   ppid = process_lst[pid]->ppid;

   //open binary file
   if(find_exec(path,&exec_file)<0)
      return -1;//cannot open binary file

    //check binary signature
   if(exec_file.signature!=EXEC_SIGNT)
      return -1;

   //argument parser
   //to do: test len of argument: warning exception!!!.
   //reset argument list
   for(argc=0;argc<ARG_MAX;argc++){
      process_lst[pid]->argv[argc]=(char*)0;
   }

   strcpy(process_lst[pid]->arg,path);
   for(argc=1;argc<ARG_MAX;argc++){
      strcat(process_lst[pid]->arg," ");
      if(!argv || !argv[argc]) break;
      strcat(process_lst[pid]->arg,argv[argc]);
   }
   //
   argc=0;
   process_lst[pid]->argc=0;
   
   p = process_lst[pid]->arg;
   process_lst[pid]->argv[argc]=p;
   l = strlen(path);
   process_lst[pid]->argv[argc][l]='\0';
   while(argv[++argc]){
      p = p+l+1;
      process_lst[pid]->argv[argc]= p/*strtok(0," ")*/;
      l = strlen(argv[argc]);
      process_lst[pid]->argv[argc][l]='\0';
   }
   process_lst[pid]->argc=argc;

   //to do: replace by heap check function
   //check heap available
   /*
   attr.stackaddr = _sys_malloc(bin_lst[exec_file.index].stacksize); 
   if(!attr.stackaddr)
      return -ENOMEM;
   //free imedialtly: reduce external fragmentation
   free(attr.stackaddr);
   */

   //WARNING!!!!!! call stop_sched()
   //restart_sched is call in _syscall_execve after _sys_exec() 
   //see kernel/syscall.c
   //minimize suspended time of scheduler
   __atomic_in();
   __stop_sched()

   //destroy previous process
   if(process_lst[pid]->pthread_ptr->parent_pthread_ptr && process_lst[pid]->pthread_ptr->parent_pthread_ptr->stat&PTHREAD_STATUS_FORK){
      //
      kernel_pthread_t* pthread_ptr = process_lst[pid]->pthread_ptr;
      kernel_pthread_t* backup_parent_pthread_ptr = process_lst[pid]->pthread_ptr->parent_pthread_ptr;

      //put all kernel object from chained list (see _sys_vfork() and _sys_vfork_exit() )
      //must be use before all pthreads stack destroyed and local variable too.
      kernel_object_manager_put_all(&process_lst[pid]->kernel_object_head);

      //to do: cancel all annexe thread in pid process not the main thread
      _sys_pthread_cancel_all_except(pid,process_lst[pid]->pthread_ptr);

      //preserve glist
      backup_parent_pthread_ptr->gnext = pthread_ptr->gnext;
      backup_parent_pthread_ptr->gprev = pthread_ptr->gprev;
      //fix problem: patch version 3.0.0.4
      //backup_parent_pthread_ptr->next  = pthread_ptr->next;

      //unswicth pthread_ptr
      memcpy(pthread_ptr,backup_parent_pthread_ptr,sizeof(kernel_pthread_t));
      process_lst[pid]->pthread_ptr = backup_parent_pthread_ptr;

      //re-insert pthread from the pthread list of father process
      _sys_process_insert_pthread(process_lst[ppid],pthread_ptr);
      
      //see vfork() in fork.c. heap external fragmentation.
      //restore parent pthread stack
      __rstr_stack(pthread_ptr);
      //__rstr_stack(pthread_ptr,&pthread_ptr->bckup_context,pthread_ptr->bckup_stack);

      //restore parent pthread context 
      __rstr_context(pthread_ptr->bckup_context,pthread_ptr);
      
      //cancel pseudo process
      //restore status on parent thread
      pthread_ptr->stat &= (~PTHREAD_STATUS_FORK);
      
   }else{
      //put all kernel object from chained list (see _sys_vfork_exit() )
      //must be use before all pthreads stack destroyed and local variable too.
      kernel_object_manager_put_all(&process_lst[pid]->kernel_object_head);
      //cancel all annexe and main thread in pid process
      _sys_pthread_cancel_all_except(pid,(kernel_pthread_t*)0);
   }

   attr.stackaddr = _sys_malloc(bin_lst[exec_file.index].stacksize); 
   if(!attr.stackaddr){//kernel panic!!!
      __restart_sched();
      __atomic_out();
      return -ENOMEM;
   }

   attr.priority  = bin_lst[exec_file.index].priority;
   attr.stacksize = bin_lst[exec_file.index].stacksize;
   attr.timeslice = bin_lst[exec_file.index].timeslice;
   
   process_lst[pid]->process_routine = bin_lst[exec_file.index].start_routine;

   /*
   process_lst[pid]->pthread_ptr=(kernel_pthread_t*)_sys_malloc(sizeof(kernel_pthread_t));
   if(!process_lst[pid]->pthread_ptr)
      return -ENOMEM;//kernel panic!!!
   */
   //use pthread container alocated in vfork();
   memset(process_lst[pid]->pthread_ptr,0,sizeof(kernel_pthread_t));

   //to remove:
   _dbg_printf("execve pid:%d\n",pid);

   //to remove :not reset fd bitmap: use current copy
   //memset(process_lst[pid]->fds_bits,0,__FDSET_LONGS);
   
   // set thread name
   attr.name = process_lst[pid]->arg;
   //create thread
   if(kernel_pthread_create(process_lst[pid]->pthread_ptr, &attr,process_routine,arg)<0){
      __restart_sched();
      __atomic_out();
      return -ENOMEM;//to do: free memory attr stack addr
   }
   //attach thread with process
   process_lst[pid]->pthread_ptr->pid = pid;
   process_lst[pid]->pid           = pid;
   process_lst[pid]->ppid          = ppid;
   //fill process stucture
   _sys_gettimeofday(&tv,NULL);
   process_lst[pid]->start_time    = tv.tv_sec;
   //prepare kernel object chained list (see _sys_vfork() )
   process_lst[pid]->kernel_object_head = (kernel_object_t*)0;
   //must de reset to null (see vfork() and __syscall_exec() )
   process_lst[pid]->pthread_ptr->parent_pthread_ptr = (kernel_pthread_t*)0;  
   //restore default sig handler
   memcpy(process_lst[pid]->pthread_ptr->sigaction_lst,sigaction_dfl_lst,sizeof(sigaction_dfl_lst));
   //
   process_lst[pid]->pthread_ptr->time_out  = (time_t)-1;//for alarm()
   //
   process_lst[pid]->pthread_ptr->stat=PTHREAD_STATUS_NULL;

   //thread sigqueue
   #ifdef __KERNEL_POSIX_REALTIME_SIGNALS
   memcpy(&process_lst[pid]->pthread_ptr->kernel_sigqueue,&_kernel_sigqueue_initializer,sizeof(kernel_sigqueue_t));
   process_lst[pid]->pthread_ptr->kernel_sigqueue.constructor(&process_lst[pid]->kernel_object_head, &process_lst[pid]->pthread_ptr->kernel_sigqueue);
   #endif

   //atexit registred functions
   #if ATEXIT_MAX>0
      process_lst[pid]->p_atexit_func  = (atexit_func_t*) kernel_pthread_alloca( process_lst[pid]->pthread_ptr,(ATEXIT_MAX+1)*sizeof(atexit_func_t));
   #endif 

   //load static library
   #ifdef __KERNEL_LOAD_LIB
   load_lib(process_lst[pid]->pthread_ptr);
   #endif

   //
   #ifdef KERNEL_PROCESS_VFORK_CLRSET_IRQ
   __clr_irq();
   #endif
   __bckup_thread_start_context(process_lst[pid]->pthread_ptr->start_context,process_lst[pid]->pthread_ptr);
   #ifdef KERNEL_PROCESS_VFORK_CLRSET_IRQ
   __set_irq();
   #endif

   //profiler
   __kernel_profiler_stop(process_lst[pid]->pthread_ptr);
   __profiler_add_result(process_lst[pid]->pthread_ptr,_SYSCALL_EXECVE,__kernel_profiler_get_counter(process_lst[pid]->pthread_ptr));
   
   return pid;
}


/*-------------------------------------------
| Name:_sys_adopt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_adopt(pid_t pid){
   pid_t _pid;
   pid_t ppid = 0;
   //procesus adoption
   for(_pid=1;_pid<=PROCESS_MAX;_pid++){
      if(!process_lst[_pid] || process_lst[_pid]->ppid!=pid)continue;

      ppid = process_lst[pid]->ppid;
      if(ppid){
         process_lst[_pid]->ppid=ppid;
         //parent thread of main thread always null except in vfork()
         process_lst[_pid]->pthread_ptr->parent_pthread_ptr = (kernel_pthread_t*)0;
      }else{
         process_lst[_pid]->ppid=0;
         //parent thread of main thread always null except in vfork()
         process_lst[_pid]->pthread_ptr->parent_pthread_ptr = (kernel_pthread_t*)0;
      }

      if(process_lst[pid]->pgid==pid)//lead process
         process_lst[_pid]->pgid = _pid;//become lead process
      
   }
   return 0;
}

/*-------------------------------------------
| Name:_sys_setpgid
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
pid_t _sys_setpgid(pid_t pid, pid_t id_grp){
   if(!process_lst[pid]) return (pid_t)-1;

   if(!id_grp)
      process_lst[pid]->pgid = pid;
   else
      process_lst[pid]->pgid = id_grp;

   return process_lst[pid]->pgid;
}

/*-------------------------------------------
| Name:_sys_getpgrp
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
pid_t _sys_getpgrp(pid_t pid){
   if(!process_lst[pid]) return (pid_t)-1;
   return process_lst[pid]->pgid;
}

/*-------------------------------------------
| Name:_sys_exit
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _sys_exit(pid_t pid,int status){

   //put all kernel object from chained list (see _sys_vfork_exit() )
   //must be use before all pthreads stack destroyed and local variable too.
   kernel_object_manager_put_all(&process_lst[pid]->kernel_object_head);

   //cancel all annexe thread in pid process main thread will be cancelled in _sys_waitpid()
   _sys_pthread_cancel_all_except(pid,process_lst[pid]->pthread_ptr);
   //
   process_lst[pid]->pthread_ptr->stat|=PTHREAD_STATUS_ZOMBI;
   //
   _dbg_printf("exit _pid:%d\n",pid);
   //
   _sys_adopt(pid);
   /*
   if(process_lst[pid]->pstat==PSTAT_NULL){
      //dynamic allocation:free
      free(process_lst[pid]);
      process_lst[pid]= 0;
   }*/
}

/*-------------------------------------------
| Name:_sys_waitpid
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
pid_t _sys_waitpid(pid_t pid,pid_t child_pid,int options,int* status){
   int r = -1;

   *status = 0;
   if(child_pid==0){
      //check if process has child.
      pid_t _pid;
      //check all child procesus (_pid)
      for(_pid=1;_pid<=PROCESS_MAX;_pid++){
         if(!process_lst[_pid]
            || process_lst[_pid]->pgid!=process_lst[pid]->pgid
            || process_lst[_pid]->ppid!=pid)continue;

         if( (process_lst[_pid]->pthread_ptr) && (process_lst[_pid]->pthread_ptr->stat&PTHREAD_STATUS_ZOMBI)){
            *(((char*)status)+1) = (char)(process_lst[_pid]->status);
            //see _sys_vfork_exit() and _sys_exit()
            //free main pthread of cureent process
            _sys_pthread_cancel_all_except(_pid,(kernel_pthread_t*)0);
            //free process
            _sys_free(process_lst[_pid]);
            process_lst[_pid]= (process_t*)0;
            _dbg_printf("free(%d)\n",_pid);
            return _pid;
         }else{
            r = 0;
         }
        
      }
      //if r=-1 then no child process
      //if r=0 childs processes still running: no zombi.
      return r;
   }else if(child_pid<0){
      //check if process has child.
      pid_t _pid;
      //procesus adoption
      for(_pid=1;_pid<=PROCESS_MAX;_pid++){
         if(!process_lst[_pid]
            || process_lst[_pid]->ppid!=pid )continue;

         if( (process_lst[_pid]->pthread_ptr) && (process_lst[_pid]->pthread_ptr->stat&PTHREAD_STATUS_ZOMBI)){
            *(((char*)status)+1) = (char)(process_lst[_pid]->status);
            //see _sys_vfork_exit() and _sys_exit()
            //free main pthread of cureent process
            _sys_pthread_cancel_all_except(_pid,(kernel_pthread_t*)0);
            //free process
            _sys_free(process_lst[_pid]);
            process_lst[_pid]= (process_t*)0;
            _dbg_printf("free(%d)\n",_pid);
            return _pid;
         }else{
            r = 0;
         }
      }
      //if r=-1 then no child process
      //if r=0 childs processes still running: no zombi.
      return r;
   }else if(child_pid>0){
      if( (!process_lst[child_pid])){
         _dbg_printf("no child(%d)\n",child_pid);
         return -1;
      }else if(process_lst[child_pid]->ppid==pid
         && (process_lst[child_pid]->pthread_ptr)
         && !(process_lst[child_pid]->pthread_ptr->stat&PTHREAD_STATUS_ZOMBI)){
         _dbg_printf("still running(%d)\n",child_pid);
         return 0;//child process still running.
      }
      else if( (process_lst[child_pid]->ppid==pid)
         && (process_lst[child_pid]->pthread_ptr)
         && (process_lst[child_pid]->pthread_ptr->stat&PTHREAD_STATUS_ZOMBI)){
         *(((char*)status)+1) = (char)(process_lst[child_pid]->status);
         //see _sys_vfork_exit() and _sys_exit()
         //free main pthread of cureent process
         _sys_pthread_cancel_all_except(child_pid,(kernel_pthread_t*)0);
         //free process
         _sys_free(process_lst[child_pid]);
         process_lst[child_pid]= (process_t*)0;
         _dbg_printf("free(%d)\n",child_pid);
         return child_pid;
      }
   }

   return -1;
}

/*-------------------------------------------
| Name:_sys_sigprocmask
| Description:
| Parameters:
| Return Type:
| Comments: set sig mask
| See:
---------------------------------------------*/
int _sys_sigprocmask(kernel_pthread_t* pthread_ptr,int how,sigset_t* set,sigset_t* oset){

   if(!set && !oset) return -EINVAL;

   switch(how){
      //
      case SIG_SETMASK:
         if(oset){
            oset->std = pthread_ptr->sig_mask.std;
            oset->rt = pthread_ptr->sig_mask.rt;
         }

         if(set){
            pthread_ptr->sig_mask.std = set->std; 
            pthread_ptr->sig_mask.rt = set->rt; 
         }

      break;

      //
      case SIG_BLOCK:
         if(oset!=NULL){
            oset->std = pthread_ptr->sig_mask.std;
            oset->rt = pthread_ptr->sig_mask.rt;
         }

         if(set){
            pthread_ptr->sig_mask.std |= set->std; // set U oset
            pthread_ptr->sig_mask.rt |= set->rt; // set U oset
         }
      break;

      //
      case SIG_UNBLOCK:
         if(oset!=NULL){
            oset->std = pthread_ptr->sig_mask.std;
            oset->rt = pthread_ptr->sig_mask.rt;
         }

         if(set){
            pthread_ptr->sig_mask.std &= ~(set->std); // set - oset
            pthread_ptr->sig_mask.rt &= ~(set->rt); // set - oset
         }
      break;

      //
      default:
         return -EINVAL;

   }

   return 0;
}

/*-------------------------------------------
| Name:_sys_sigpending
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_sigpending(kernel_pthread_t* pthread_ptr,sigset_t* set){

   if(!set) return -EINVAL;

   set->std = pthread_ptr->sig_mask.rt & pthread_ptr->sig_pending.std;
   set->rt = pthread_ptr->sig_mask.rt & pthread_ptr->sig_pending.rt;

   return 0;
}

/*-------------------------------------------
| Name:_sys_sigaction
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_sigaction(kernel_pthread_t* pthread_ptr,int sig,struct sigaction* act,struct sigaction* oact){

   if(!sig || (sig>NSIG && sig<SIGRTMIN) || sig>SIGRTMAX ) return -EINVAL;

   if(oact)
      memcpy(oact,&pthread_ptr->sigaction_lst[sig],sizeof(struct sigaction));

   if(act)
      memcpy(&pthread_ptr->sigaction_lst[sig],act,sizeof(struct sigaction));


   return 0;
}

/*-------------------------------------------
| Name:_sys_kill
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_kill(kernel_pthread_t* pthread_ptr,int sig,int atomic){

   pid_t pid=pthread_ptr->pid;

   if(pthread_ptr->stat&PTHREAD_STATUS_ZOMBI)//cannot kill zombi process!.
      return -1;

   
   if(sig<NSIG){
      //standard signal

      //sig ignore?
      if((unsigned long)(pthread_ptr->sigaction_lst[sig].sa_handler)==SIG_IGN)
         return 0;

      //to do: support for SIGSTOP(suspend process) and SIGCONT(resume process)
      //       see sighandler in signal.c
      if(pthread_ptr->stat&PTHREAD_STATUS_SIGHANDLER){
         //already in sighandler
         pthread_ptr->sig_pending.std|= (1 << (sig-1));
         return 0;
      }

      //sig is blocked?
      if( (((pthread_ptr->sig_mask.std)>>(sig-1))&0x01) )
         return 0; //yes, sig is blocked

      //BEGIN OF ATOMIC SECTION
      #ifdef KERNEL_PROCESS_VFORK_CLRSET_IRQ
      if(atomic){
         __clr_irq();
      }
      #endif
   
      //block sig
      pthread_ptr->sig_mask.std|= (1 << (sig-1));
   }else if(sig>=SIGRTMIN && sig<SIGRTMAX){
      //realtime signal

      //sig ignore?
      if((unsigned long)(pthread_ptr->sigaction_lst[sig].sa_handler)==SIG_IGN)
         return 0;
     
      //sig is blocked ?
      if( (((pthread_ptr->sig_mask.rt)>>(sig-SIGRTMIN))&0x01) ){
         kernel_sigevent_t kernel_sigevent={0};
         
         //put signal in queue only if it's blocked sa_flags==SA_SIGINFO
         if( (pthread_ptr->sigaction_lst[sig].sa_flags&SA_SIGINFO) ){
            //init sigevent
            kernel_sigevent.from=(unsigned long)0x00;
            kernel_sigevent.si_code = SI_USER; 
            kernel_sigevent._sigevent.sigev_signo=sig;
            pthread_ptr->kernel_sigqueue.send(pthread_ptr,&kernel_sigevent);
         }
         return 0; //sig is blocked
      }

      //see sighandler in signal.c
      if(pthread_ptr->stat&PTHREAD_STATUS_SIGHANDLER){
         //already in sighandler
         pthread_ptr->sig_pending.rt|= (1 << (sig-SIGRTMIN));
         return 0;
      }

      //BEGIN OF ATOMIC SECTION
      #ifdef KERNEL_PROCESS_VFORK_CLRSET_IRQ
      if(atomic){
         __clr_irq();
      }
      #endif
   
   }else{
      return -1;
   }

   
   //back_up pid context
   //__bckup_context(pthread_ptr->bckup_context,pthread_ptr);
   __bckup_context(pthread_ptr->bckup_context,pthread_ptr);

   //set siginfo for handler
   pthread_ptr->siginfo.si_signo = sig;
   pthread_ptr->siginfo.si_code = SI_USER;

   //route execution flow (sighandler: see kernel/signal.c)
   __swap_signal_handler(pthread_ptr,sighandler);

   //set process status
   pthread_ptr->stat=pthread_ptr->stat | PTHREAD_STATUS_SIGHANDLER;

   //END OF ATOMIC SECTION
   #ifdef KERNEL_PROCESS_VFORK_CLRSET_IRQ
   if(atomic)
      __set_irq();
   #endif
   
   return 0;
}

/*-------------------------------------------
| Name:_sys_kill_exit
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_kill_exit(kernel_pthread_t* pthread_ptr){
   int sig;

   #ifdef KERNEL_PROCESS_VFORK_CLRSET_IRQ
   __clr_irq();
   #endif
   
   //restore pid context
   __rstr_context(pthread_ptr->bckup_context,pthread_ptr);
   
   //reset process status
   pthread_ptr->stat=(pthread_ptr->stat &(~PTHREAD_STATUS_SIGHANDLER) );
   //standard sig/realtime signal
   if(pthread_ptr->siginfo.si_signo<NSIG){
      //standard signal
      //unblock sig
      pthread_ptr->sig_mask.std &= ~(1 << (pthread_ptr->siginfo.si_signo-1));
      //reset sig_info
      pthread_ptr->siginfo.si_signo=0;
      //
      if(!pthread_ptr->sig_pending.std){
         //no pending signal
          /*modif phlb 19/01/2009*/
         __set_active_pthread(pthread_ptr);
         #ifdef KERNEL_PROCESS_VFORK_CLRSET_IRQ
         __set_irq();
         #endif
         return 0;
      }
      //to do: unqueue sig pending not yet tested
      for(sig=1;sig<NSIG;sig++){
         if(((pthread_ptr->sig_pending.std>>(sig-1))&0x01)){
            pthread_ptr->sig_pending.std &= ~(1 << (sig-1));
            return _sys_kill(pthread_ptr,sig,0);
         }
      }
   }else if(pthread_ptr->siginfo.si_signo>=SIGRTMIN && pthread_ptr->siginfo.si_signo<SIGRTMAX){
      //realtime signal
      //reset sig_info
      pthread_ptr->siginfo.si_signo=0;
      //
      if(!pthread_ptr->sig_pending.rt){
         //no pending signal
         /*modif phlb 19/01/2009*/
         __set_active_pthread(pthread_ptr);
         #ifdef KERNEL_PROCESS_VFORK_CLRSET_IRQ
         __set_irq();
         #endif
         return 0;
      }
      //to do: unqueue sig pending not yet tested
      for(sig=SIGRTMIN;sig<SIGRTMAX;sig++){
         if(((pthread_ptr->sig_pending.rt>>(sig-SIGRTMIN))&0x01)){
            pthread_ptr->sig_pending.rt&= ~(1 << (sig-SIGRTMIN));
            return _sys_kill(pthread_ptr,sig,0);
         }
      }

   }
   //no pending signal
    /*modif phlb 19/01/2009*/
   __set_active_pthread(pthread_ptr);
   #ifdef KERNEL_PROCESS_VFORK_CLRSET_IRQ
   __set_irq();
   #endif

   return 0;
}

/*-------------------------------------------
| Name:_sys_raise
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_raise(pid_t pid, int sig){
   return 0;
}

/*===========================================
End of Sourceprocess.c
=============================================*/
