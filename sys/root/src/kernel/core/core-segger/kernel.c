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

#include "kernel/core/kernelconf.h"

#include "kernel/core/errno.h"
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernel.h"
#include "kernel/core/kernel_elfloader.h"
#include "kernel/core/kernel_object.h"
#include "kernel/core/syscall.h"
#include "kernel/core/process.h"
#include "kernel/core/signal.h"
#include "kernel/core/statvfs.h"
#include "kernel/core/bin.h"
#include "kernel/core/rttimer.h"
#include "kernel/core/time.h"
#include "kernel/core/flock.h"

//distrib: must be remove
#include "kernel/core/ioctl_board.h"
#include "kernel/core/ioctl_keyb.h"
#include "kernel/core/ioctl_hd.h"

#include "kernel/fs/vfs/vfs.h"
#include "kernel/fs/vfs/vfskernel.h"

#if defined (__KERNEL_NET_IPSTACK)
   #if defined(USE_UIP)
      #include "kernel/core/net/uip_core/uip_core.h"
   #endif
#endif

/*===========================================
Global Declaration
=============================================*/
//kernel compiler information
const char * _kernel_date = __DATE__;
const char * _kernel_time = __TIME__;

//from netBSD
#define BCD_TO_BIN(val) ((val)=((val)&15) + ((val)>>4)*10)
#define BIN_TO_BCD(val) ((val)=(((val)/10)<<4) + (val)%10)

volatile pid_t _syscall_owner_pid;
kernel_pthread_t* _syscall_owner_pthread_ptr;


volatile int _kernel_in_static_mode=1;

kernel_pthread_mutex_t kernel_mutex;
kernel_pthread_t kernel_thread;
tmr_t kernel_tmr;

//
#if ( (__tauon_compiler__==__compiler_iar_m16c__) && defined (__KERNEL_UCORE_EMBOS) && defined(CPU_M16C62))
   #define KERNEL_STACK_SIZE  1024 //1024//512 M16C
#elif ( (__tauon_compiler__==__compiler_iar_arm__) && defined (__KERNEL_UCORE_EMBOS) && defined(CPU_ARM7))
   #define KERNEL_STACK_SIZE  2048 //2048//ARM7
#elif ( (__tauon_compiler__==__compiler_iar_arm__) && defined (__KERNEL_UCORE_EMBOS) && defined(CPU_CORTEXM))
   #define KERNEL_STACK_SIZE  2048 //CORTEXM
#elif ( (__tauon_compiler__==__compiler_iar_arm__) && defined (__KERNEL_UCORE_EMBOS) && defined(CPU_ARM9))
   #define KERNEL_STACK_SIZE  2048 //2048//ARM9
#elif ( (__tauon_compiler__==__compiler_keil_arm__) && defined (__KERNEL_UCORE_EMBOS) )
//&& defined(CPU_CORTEXM))
   #define KERNEL_STACK_SIZE  2048 //CORTEXM
#elif WIN32
   #define KERNEL_STACK_SIZE  1024
#endif

#define KERNEL_PRIORITY    150
_macro_stack_addr char kernel_stack[KERNEL_STACK_SIZE];


#ifdef KERNEL_PROFILER
kernel_profiler_result_t kernel_profiler_result_lst[_SYSCALL_TOTAL_NB]={0};
io_profiler_result_t*      io_profiler_result_lst;
#endif

//cpu device
fdev_map_t*   __g_kernel_cpu;
desc_t __g_kernel_desc_cpu;

//i2c interface
fdev_map_t*    __g_kernel_if_i2c_master;
desc_t __g_kernel_desc_if_i2c_master;

//spi interface
fdev_map_t*    __g_kernel_if_spi_master;
desc_t __g_kernel_desc_if_spi_master;

//kernel errno
int __g_kernel_static_errno=0;


kernel_syscall_t const kernel_syscall_lst[] = {
   __add_syscall(_syscall_waitpid),       //0
   __add_syscall(_syscall_execve),        //1
   __add_syscall(_syscall_exit),          //2
   __add_syscall(_syscall_kill),          //3
   __add_syscall(_syscall_vfork),         //4
   __add_syscall(_syscall_sigprocmask),   //5
   __add_syscall(_syscall_sigpending),    //6
   __add_syscall(_syscall_sigaction),     //7
   __add_syscall(_syscall_alarm),         //8
   __add_syscall(_syscall_pause),         //9
   __add_syscall(_syscall_mount),         //10
   __add_syscall(_syscall_umount),
   __add_syscall(_syscall_opendir),
   __add_syscall(_syscall_closedir),
   __add_syscall(_syscall_readdir),
   __add_syscall(_syscall_rewinddir),
   __add_syscall(_syscall_telldir),
   __add_syscall(_syscall_seekdir),
   __add_syscall(_syscall_creat),
   __add_syscall(_syscall_open),
   __add_syscall(_syscall_close),         //20
   __add_syscall(_syscall_read),
   __add_syscall(_syscall_write),
   __add_syscall(_syscall_lseek),
   __add_syscall(_syscall_stat),
   __add_syscall(_syscall_mkdir),
   __add_syscall(_syscall_rmdir),
   __add_syscall(_syscall_chdir),
   __add_syscall(_syscall_dup),
   __add_syscall(_syscall_dup2),
   __add_syscall(_syscall_pipe),
   __add_syscall(_syscall_sigexit),
   __add_syscall(_syscall_mknod),
   __add_syscall(_syscall_ftruncate),
   __add_syscall(_syscall_truncate),
   __add_syscall(_syscall_rm),
   __add_syscall(_syscall_fstatvfs),
   __add_syscall(_syscall_statvfs),
   __add_syscall(_syscall_remove),
   __add_syscall(_syscall_gettimeofday),
   __add_syscall(_syscall_atexit),
   __add_syscall(_syscall_malloc),
   __add_syscall(_syscall_calloc),
   __add_syscall(_syscall_realloc),
   __add_syscall(_syscall_free),
   __add_syscall(_syscall_fcntl),
   __add_syscall(_syscall_getcwd),
   __add_syscall(_syscall_fstat),
   __add_syscall(_syscall_mkfs),
   __add_syscall(_syscall_setpgid),
   __add_syscall(_syscall_getpgrp),
   __add_syscall(_syscall_settimeofday),
   __add_syscall(_syscall_sync),
   __add_syscall(_syscall_fattach),
   __add_syscall(_syscall_fdetach),
   __add_syscall(_syscall_ioctl),
   __add_syscall(_syscall_sysctl),
   __add_syscall(_syscall_rename),
   __add_syscall(_syscall_pthread_create),
   __add_syscall(_syscall_pthread_cancel),
   __add_syscall(_syscall_pthread_exit),
   __add_syscall(_syscall_pthread_kill),
   __add_syscall(_syscall_pthread_mutex_init),
   __add_syscall(_syscall_pthread_mutex_destroy),
   __add_syscall(_syscall_pthread_cond_init),
   __add_syscall(_syscall_pthread_cond_destroy),
   __add_syscall(_syscall_timer_create),
   __add_syscall(_syscall_timer_delete),
   __add_syscall(_syscall_sem_init),
   __add_syscall(_syscall_sem_destroy)
};

//warning use sscanf from libc/stdio/scanf.c
extern int __sscanf(const char * sp, const char * fmt, ...);
#define sscanf       __sscanf

//
_kernel_syscall_trace_t _g_kernel_syscall_trace;

/*===========================================
Implementation
=============================================*/


/*-------------------------------------------
| Name:_kernel_syscall
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kernel_syscall(void){
   kernel_syscall_t kernel_syscall;
   kernel_pthread_t* pthread_ptr = g_pthread_lst;

   while(pthread_ptr) {
      if( (pthread_ptr->pid<=0) || (pthread_ptr->irq_nb!=KERNEL_INTERRUPT_NB) ) {
         pthread_ptr=pthread_ptr->gnext;
         continue;
      }

      _syscall_owner_pid = pthread_ptr->pid;
      _syscall_owner_pthread_ptr = pthread_ptr;
      //errno
      _syscall_owner_pthread_ptr->_errno = 0;

      //kernel trace for debug
      _g_kernel_syscall_trace._syscall_owner_pid = _syscall_owner_pid;
      _g_kernel_syscall_trace._syscall_owner_pthread_ptr = _syscall_owner_pthread_ptr;
      _g_kernel_syscall_trace.kernel_syscall_status = KERNEL_SYSCALL_STATUS_START;

      if((kernel_syscall.p_syscall=kernel_syscall_lst[pthread_ptr->reg.syscall].p_syscall)) {
         //kernel trace for debug
         memcpy(&_g_kernel_syscall_trace._kernel_syscall,&kernel_syscall,sizeof(kernel_syscall_t));
         //
         if(kernel_syscall.p_syscall(pthread_ptr,pthread_ptr->pid,pthread_ptr->reg.data)<0) {
            _g_kernel_syscall_trace.kernel_syscall_status = KERNEL_SYSCALL_STATUS_ENDERROR;
            __syscall_unlock();
            return -1; //stop kernel
         }
      }

      //
      _g_kernel_syscall_trace.kernel_syscall_status = KERNEL_SYSCALL_STATUS_END;
      __syscall_unlock();
      return 0;
   }



   //kernel panic!!!
   return -1; //stop kernel
}

/*-------------------------------------------
| Name:_kernel_mount
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kernel_mount(const char* argv[]){
   static const char* fstype_list[]={"rootfs","ufs","ufsx"};
   static const int fstype_list_size=sizeof(fstype_list)/sizeof(char*);
   fstype_t i;
   fstype_t fstype = -1;

   for(i=0;
       i<fstype_list_size;
       i++) {

      if(!strcmp(argv[0],fstype_list[i])) {
         fstype = i;
         break;
      }
   }

   if(fstype<0)
      return -1;

   return _vfs_mount(fstype,argv[1],argv[2]);
}

/*--------------------------------------------
| Name:        _kernel_warmup_profiler
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _kernel_warmup_profiler(void){
#ifdef KERNEL_PROFILER
   int i=0;
   for(i=0; i<_SYSCALL_TOTAL_NB; i++) {
      kernel_profiler_result_lst[i].pid=0;
      kernel_profiler_result_lst[i].counter=0;
      kernel_profiler_result_lst[i].pname = kernel_syscall_lst[i].p_syscall_name;
   }

   io_profiler_result_lst = _sys_malloc(sizeof(io_profiler_result_t)*max_dev);
   memset(io_profiler_result_lst,0,sizeof(io_profiler_result_t)*max_dev);

   __io_profiler_init();

#endif
   return 0;
}

/*-------------------------------------------
| Name:_kernel_warmup_rootfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _kernel_warmup_rootfs(void){
   //
   _vfs_rootmnt();

   //
   _vfs_mkdir("/dev",0);
   _vfs_mkdir("/dev/hd",0);

   //kernel
   _vfs_mkdir("/kernel",0);
   
   //kernel objects file system
   #if __KERNEL_VFS_SUPPORT_KOFS==1
      _vfs_mount(fs_kofs,(char*)0,"/kernel");
   #endif
   //

   //binary
   _vfs_mkdir("/bin",0);
   _vfs_mkdir("/usr",0);
   _vfs_mkdir("/etc",0);
   _vfs_mkdir("/var",0);
   _vfs_mkdir("/mnt",0);

   //make binary
   _mkbin("/bin/init","init");
   _mkbin("/bin/mkfs","mkfs");
   _mkbin("/bin/mount","mount");
   _mkbin("/bin/umount","umount");
   _mkbin("/bin/lsh","lsh");

}

/*--------------------------------------------
| Name:        _kernel_warmup_load_mount_cpufs
| Description:
| Parameters:  none
| Return Type: none
| Comments: load cpufs /dev/hd/hda device and mount on /usr
| See:
----------------------------------------------*/
void _kernel_warmup_load_mount_cpufs(void){
   char ref[16];
   dev_t dev;

   //create device
   for(dev=0; dev<max_dev; dev++) { //(MAX_DEV-1) last dev is NULL
      int len_dev_name =0;
      if(!pdev_lst[dev]) continue;
      len_dev_name = strlen(pdev_lst[dev]->dev_name);
      //
      if(pdev_lst[dev]->dev_name[len_dev_name+1]=='c'
         && pdev_lst[dev]->dev_name[len_dev_name+2]=='p'
         && pdev_lst[dev]->dev_name[len_dev_name+3]=='u'
         && pdev_lst[dev]->dev_name[len_dev_name+4]=='f'
         && pdev_lst[dev]->dev_name[len_dev_name+5]=='s') {
         //specific directory for disc /dev/hd.
         int len;
         strcpy(ref,"/dev/hd/");
         len = strlen(ref);

         ref[len+0] = 'h';
         ref[len+1] = 'd';
         ref[len+2] = 'a';
         ref[len+3] = '\0';

         //load dev
         if(pdev_lst[dev]->fdev_load)
            if(pdev_lst[dev]->fdev_load()<0)
               continue;

         //strcat(ref,pdev_lst[dev]->dev_name);
         _vfs_mknod(ref,(int16_t)pdev_lst[dev]->dev_attr,dev);
      }
   }

   //use hda for cpu flash rom disk(initial disk)
   _vfs_mount(fs_ufs,__BOOT_DEVICE,"/usr");

}

/*--------------------------------------------
| Name:        _kernel_warmup_i2c
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
void _kernel_warmup_i2c(void){
   desc_t desc;
   char ref[16];
   dev_t dev;

   //reset i2c interface
   __set_if_i2c_master((fdev_map_t*)0);

   for(dev=0; dev<max_dev; dev++) { //(MAX_DEV-1) last dev is NULL
      if(!pdev_lst[dev]) continue;
      //
      if(pdev_lst[dev]->dev_name[0]=='i'
         && pdev_lst[dev]->dev_name[1]=='2'
         && pdev_lst[dev]->dev_name[2]=='c'
         && pdev_lst[dev]->dev_name[3]=='0') {


         //load dev
         if(pdev_lst[dev]->fdev_load)
            if(pdev_lst[dev]->fdev_load()<0)
               continue;


         //i2c interface
         strcpy(ref,"/dev/");
         strcat(ref,pdev_lst[dev]->dev_name);
         _vfs_mknod(ref,(int16_t)pdev_lst[dev]->dev_attr,dev);
         __set_if_i2c_master((fdev_map_t*)pdev_lst[dev]);
      }
   }
   //i2c master interface
   if( (desc = _vfs_open("/dev/i2c0",O_RDWR,0))<0)
      return;  //i2c interface not available

   __set_if_i2c_master_desc(desc);
}

/*--------------------------------------------
| Name:        _kernel_warmup_spi
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
void _kernel_warmup_spi(void){
   desc_t desc;
   char ref[16];
   dev_t dev;

   //reset spi interface
   __set_if_spi_master((fdev_map_t*)0);

   for(dev=0; dev<max_dev; dev++) { //(MAX_DEV-1) last dev is NULL
      if(!pdev_lst[dev]) continue;
      //
      if(   pdev_lst[dev]->dev_name[0]=='s'
            && pdev_lst[dev]->dev_name[1]=='p'
            && pdev_lst[dev]->dev_name[2]=='i'
            && pdev_lst[dev]->dev_name[3]=='0') {

         //load dev
         if(pdev_lst[dev]->fdev_load)
            if(pdev_lst[dev]->fdev_load()<0)
               continue;

         //spi interface
         strcpy(ref,"/dev/");
         strcat(ref,pdev_lst[dev]->dev_name);
         _vfs_mknod(ref,(int16_t)pdev_lst[dev]->dev_attr,dev);
         __set_if_spi_master((fdev_map_t*)pdev_lst[dev]);
      }
   }
   //spi master interface
   if( (desc = _vfs_open("/dev/spi0",O_RDWR,0))<0)
      return;  //spi interface not available

   __set_if_spi_master_desc(desc);
}

/*--------------------------------------------
| Name:        _kernel_warmup_dev
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
void _kernel_warmup_dev(void){
   char ref[PATH_MAX+1];
   dev_t dev;
   char hd_cpt=0;

   //create device
   for(dev=0; dev<max_dev; dev++) { //(MAX_DEV-1) last dev is NULL
      if(!pdev_lst[dev]) continue;

      //
      if(pdev_lst[dev]->dev_name[0]=='h'
         && pdev_lst[dev]->dev_name[1]=='d') {
         //specific directory for disc /dev/hd.
         int len;
         len = strlen(pdev_lst[dev]->dev_name);
         //
         if(pdev_lst[dev]->dev_name[len+1]=='c'
            && pdev_lst[dev]->dev_name[len+2]=='p'
            && pdev_lst[dev]->dev_name[len+3]=='u'
            && pdev_lst[dev]->dev_name[len+4]=='f'
            && pdev_lst[dev]->dev_name[len+5]=='s')
            continue;  //it's already install and mounted ( see _kernel_warmup_load_mount_cpufs() );

         strcpy(ref,"/dev/hd/");
         len = strlen(ref);

         ref[len+0] = 'h';
         ref[len+1] = 'd';
         ref[len+2] = hd_cpt + 97+1; //hd 'b' to 'z'. hda is reserved for cpufs (see _kernel_warmup_load_mount_cpufs() );
         ref[len+3] = '\0';

         //load dev
         if(pdev_lst[dev]->fdev_load)
            if(pdev_lst[dev]->fdev_load()<0)
               continue;

         hd_cpt++;

         //strcat(ref,pdev_lst[dev]->dev_name);
         _vfs_mknod(ref,(int16_t)pdev_lst[dev]->dev_attr,dev);
      }else if(pdev_lst[dev]->dev_name[0]=='i'
               && pdev_lst[dev]->dev_name[1]=='2'
               && pdev_lst[dev]->dev_name[2]=='c'
               && pdev_lst[dev]->dev_name[3]=='0') {
         //already mount see _kernel_warmup_i2c
      }/*else if(pdev_lst[dev]->dev_name[0]=='s'
         && pdev_lst[dev]->dev_name[1]=='p'
         && pdev_lst[dev]->dev_name[2]=='i'
         && pdev_lst[dev]->dev_name[3]=='0'){
         //already mount see _kernel_warmup_spi
      }*/else if(pdev_lst[dev]->dev_name[0]=='c'
              && pdev_lst[dev]->dev_name[1]=='p'
              && pdev_lst[dev]->dev_name[2]=='u'
              && pdev_lst[dev]->dev_name[3]=='0') {
         //cpu device

         //load dev
         if(pdev_lst[dev]->fdev_load)
            if(pdev_lst[dev]->fdev_load()<0)
               continue;

         strcpy(ref,"/dev/");
         strcat(ref,pdev_lst[dev]->dev_name);

         _vfs_mknod(ref,(int16_t)pdev_lst[dev]->dev_attr,dev);
         __set_cpu((fdev_map_t*)pdev_lst[dev]);

      }else{
         //load dev
         char *p_ref=ref;
         if(pdev_lst[dev]->fdev_load)
            if(pdev_lst[dev]->fdev_load()<0)
               continue;

         strcpy(ref,"/dev/");
         strcat(ref,pdev_lst[dev]->dev_name);
         //
         while((*(++p_ref))!='\0') {
            if(*p_ref=='/') {
               *p_ref='\0';
               _vfs_mkdir(ref,0);
               *p_ref='/';
            }
         }

         //
         _vfs_mknod(ref,(int16_t)pdev_lst[dev]->dev_attr,dev);
      }
   }
}


/*--------------------------------------------
| Name:        _kernel_warmup_stream
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
void _kernel_warmup_stream(void){
   /*
   desc_t desc_1;
   desc_t desc_2;
   //to remove: test stream with ftl
   if((desc_1 = _vfs_open("/dev/ftl",O_RDWR,0))<0)
      return;

   if((desc_2 = _vfs_open("/dev/hd/hdd",O_RDWR,0))<0)
      return;

   if(_vfs_ioctl(desc_1,I_LINK,desc_2)<0)
      return;

   if(_vfs_fattach(desc_1,"/dev/hd/hdd0")<0)
      return;

   */

}

/*--------------------------------------------
| Name:        _kernel_warmup_object_manager
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _kernel_warmup_object_manager(void){
   int kernel_object_no = __KERNEL_OBJECT_POOL_MAX;

   return kernel_object_manager_pool(kernel_object_no);
}

/*--------------------------------------------
| Name:        _kernel_warmup_cpu
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
void _kernel_warmup_cpu(void){
   desc_t desc;
   if((desc = _vfs_open("/dev/cpu0",O_RDWR,0))<0)
      return;  //cpu device not available
   __set_cpu_desc(desc);
}


/*-------------------------------------------
| Name:_kernel_warmup_rtc
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kernel_warmup_rtc(void){
   desc_t desc = -1;
   //specific rtc
   //set kernel time from rtc
   if((desc = _vfs_open("/dev/rtc0",O_RDONLY,0))<0) //ST m41t81
      desc = _vfs_open("/dev/rtc1",O_RDONLY,0);

   if(desc>=0) {
      char buf[8]={0};
      struct tm _tm={ 0, 0, 12, 28, 0, 103 }; //init for test
      time_t time=0;

      /*buf[0] = _tm.tm_sec;
      buf[1] = _tm.tm_min;
      buf[2] = _tm.tm_hour;
      buf[3] = _tm.tm_mday;
      buf[4] = _tm.tm_mon;
      buf[5] = _tm.tm_year;

      __kernel_dev_settime(desc,buf,8);*/

      //
      memset(&_tm,0,sizeof(struct tm));
      //

      __kernel_dev_gettime(desc,buf,6);
      _tm.tm_sec  = buf[0];
      _tm.tm_min  = buf[1];
      _tm.tm_hour = buf[2];
      _tm.tm_mday = buf[3];
      _tm.tm_mon  = buf[4];
      _tm.tm_year = buf[5];

      //
      time = mktime(&_tm);
      xtime.tv_usec=0;
      xtime.tv_sec = time;
      _vfs_close(desc);

      return 0;
   }

   //specific rtt
   //set kernel time from rtt
   if((desc = _vfs_open("/dev/rtt0",O_RDONLY,0))<0) //ST m41t81
      return -1;
   if(desc>=0) {
      time_t time=0;

      _vfs_read(desc,(char*)&time,sizeof(time_t));

      xtime.tv_usec=0;
      xtime.tv_sec = time;
      _vfs_close(desc);

      return 0;
   }

   return 0;
}

/*-------------------------------------------
| Name:_kernel_warmup_mount
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kernel_warmup_mount(void){

   desc_t desc = -1;

   //.mount
   if((desc=_vfs_open("/usr/etc/.mount",O_RDONLY,0))>=0) {
      char buf[ARG_LEN_MAX];
      char *pbuf=buf;
      int cb=0;
      while((cb =_vfs_read(desc,pbuf,1))>0) {
         *(pbuf+1)='\0';
         if(*pbuf!='\r' && *pbuf!='\n') {
            pbuf++;
            continue;
         }
         *(pbuf)='\0';
         if(strlen(buf)) {
            char* argv[ARG_MAX]={0};
            int argc=0;


            argv[argc] = strtok( buf," ");
            while( argv[argc++] != NULL )
               argv[argc] = strtok( NULL," ");  //Get next token:
            _kernel_mount(argv);
         }
         pbuf=buf;
      }
      if(strlen(buf)) {
         char* argv[ARG_MAX]={0};
         int argc=0;
         int cb=0;

         argv[argc] = strtok( buf," ");
         while( argv[argc++] != NULL )
            argv[argc] = strtok( NULL," ");  //Get next token:
         _kernel_mount(argv);
      }
      _vfs_close(desc);
   }
   return 0;
}

/*-------------------------------------------
| Name:_kernel_warmup_boot
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kernel_warmup_boot(void){
   desc_t desc = -1;
   int l;
   //.boot
   if((desc=_vfs_open("/usr/etc/.boot",O_RDONLY,0))>=0) {
      char buf[ARG_LEN_MAX];
      char *pbuf=buf;
      int cb=0;
      int kb_val=0;
      int st=0;
      while((cb =_vfs_read(desc,pbuf,1))>0) {
         if(*pbuf=='\r' || *pbuf=='\n') {
            if(st<0)
               st=2;
            continue;
         }
         //
         *(pbuf+1)='\0';

         //
         if(*pbuf!=':' && *pbuf!=';' && *pbuf!='!') {
            pbuf++;
            continue;
         }

         //
         if( (st==3) && ( (*pbuf=='!') || (*pbuf==';') )  ) {
            if((*pbuf==';')) {
               st=-1;
               pbuf++;
               continue;
            }
            //
            st=4;
            pbuf=buf;
            *(pbuf)='\0';
            continue;
         }

         //
         *(pbuf)='\0';
         if((l=strlen(buf))) {
            switch(st) {
            //
            case 0: {  //open dev
               char* dev=buf;
               desc_t _desc;
               st=1;
               //
               if((_desc = _vfs_open(dev,O_RDONLY,0))<0)
                  break;
               _vfs_ioctl(_desc,IOCTL_MULTIBOOT_GETVAL,&kb_val);
               _vfs_close(_desc);
            }
            break;

            case 1: {  //delay
               unsigned int delay=atoi(buf);
               st =2;
               //to do: usleep cannot be use kernel is not started
               /*if(delay)
                  usleep(delay);*/
            }
            break;

            //
            case 2: {  //match kb val
               int _kb_val = 0;
               unsigned int coef = 1;

               while((--l)>0) {
                  char v=0;
                  if(buf[l]=='X' || buf[l]=='x') {
                     break;
                  }else if(buf[l]>='0' && buf[l]<='9') {
                     v=buf[l]-'0';
                  }else if(buf[l]>='a' && buf[l]<='f') {
                     v=buf[l]-'a'+10;
                  }else if(buf[l]>='A' && buf[l]<='F') {
                     v=buf[l]-'A'+10;
                  }else{
                     _kb_val = 0;
                     break;
                  }

                  _kb_val+= (v*coef);
                  if(!(coef = coef << 4))
                     break;
               }


               if(!_kb_val || kb_val==_kb_val)
                  st=4;
               else if(kb_val!=_kb_val)
                  st=3;
               else
                  st=-1;
               break;
            }
            break;

            case 4: {  //exec bin
               char* argv[ARG_MAX]={0};
               int argc=0;
               argv[argc] = strtok( buf," ");
               while( argv[argc++] != NULL )
                  argv[argc] = strtok( NULL," ");   //Get next token:
               _sys_krnl_exec(argv[0],argv,0,0,0);
               st=2;
            }
            break;

            default:
               break;
            }

         }
         pbuf=buf;
      }
      _vfs_close(desc);

   }else{
      //warning!!!:/dev/ttyp0 only for win32 version
      char* argv[ARG_MAX]={"-t","5000","-i","/dev/ttyp0","-o","/dev/ttyp0"};
      _sys_krnl_exec("/bin/init",(char**)argv,0,0,0);
   }

   return 0;
}

/*-------------------------------------------
| Name:_kernel_powerdown
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kernel_powerdown(void){
   desc_t desc = -1;

   //power down
   if((desc=_vfs_open("/dev/board",O_RDONLY,0))>=0) {
      _vfs_ioctl(desc,BRDPWRDOWN,(long*)0);
      _vfs_close(desc);
   }

   return 0;
}

/*-------------------------------------------
| Name:kernel_thread
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void* kernel_routine(void* arg){

   //
   for(;; ) {
      __kernel_wait_int();
      if(_kernel_syscall()<0)
         break;
   }
   __restart_sched();
   __atomic_out();

   //umount all
   _vfs_sync();

   //
   _kernel_powerdown();

   __atomic_in();
   __stop_sched();

   return NULL;
}

/*-------------------------------------------
| Name:_kernel_timer
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _kernel_timer(void){
   pid_t _pid;
   kill_t kill_dt;

   kernel_pthread_t* pthread_ptr;
   //
   pthread_ptr = g_pthread_lst;
   //
   while(pthread_ptr) {
      if(pthread_ptr
         && (pthread_ptr->pid>0)
         && ((signed long)(pthread_ptr->time_out)>=0L)) {
         //
         if(pthread_ptr->time_out>0L) {
            if( pthread_ptr->time_out-- ) {
               pthread_ptr=pthread_ptr->gnext;
               continue;
            }
         }
         //try make sytem call
         if(__syscall_trylock()==-EBUSY) {
            pthread_ptr=pthread_ptr->gnext;
            continue;
         }
         //prepare to kill pthread
         pthread_ptr->time_out = -1;
         kill_dt.pid = pthread_ptr->pid;
         kill_dt.sig = SIGALRM;
         kill_dt.atomic = 0; //__clrirq(), __setirq() not used.
         //send SIGALRM to pthread
         _syscall_kill(pthread_ptr,pthread_ptr->pid,&kill_dt);
         __set_active_pthread(pthread_ptr);

         //end of sys call
         __syscall_unlock();
      }

      pthread_ptr=pthread_ptr->gnext;
   }

   for(_pid=1; _pid<=PROCESS_MAX; _pid++) {
      if(!process_lst[_pid]) continue;


   }

   rttmr_restart(&kernel_tmr);

}

#include "lib/libc/stdio/stdio.h"
/*-------------------------------------------
| Name:start_kernel
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _start_kernel(char* arg){

   pthread_mutexattr_t mutex_attr=0;
   pthread_attr_t thread_attr;
   rttmr_attr_t rttmr_attr={0};

   //
   __kernel_static_mode_in();

   //init kernel system
   _pid();
   //
   _vfs();
   //
   _flocks();

   //
   kernel_pthread_mutex_init(&kernel_mutex,&mutex_attr);

//   rttmr_attr.tm_msec=__KERNEL_ALARM_TIMER;
//   rttmr_attr.func = _kernel_timer;
//   rttmr_create(&kernel_tmr,&rttmr_attr);
//   rttmr_start(&kernel_tmr);

   //stdio init(mutex for stdin, stdout, stderr)
   __stdio_init();

   //
   thread_attr.stacksize = KERNEL_STACK_SIZE;
   thread_attr.stackaddr = (void*)&kernel_stack;
   thread_attr.priority  = KERNEL_PRIORITY;
   thread_attr.timeslice = 0;
   thread_attr.name ="kernel_thread";

   kernel_pthread_create(&kernel_thread,&thread_attr,kernel_routine,arg);
   //
   _kernel_warmup_profiler();
   //create directory /dev, /bin, /usr, /etc.
   //detect and create devices in /dev
   _kernel_warmup_rootfs();
   //
   _kernel_warmup_load_mount_cpufs();
   //
   _kernel_warmup_i2c();
   //
   //_kernel_warmup_spi();
   //
   _kernel_warmup_dev();
   //
   _kernel_warmup_stream();
   //
   _kernel_warmup_object_manager();
   //
   _kernel_warmup_cpu();
   //
   _kernel_warmup_rtc();
   //
   _kernel_warmup_mount();
   //
   _kernel_warmup_boot();
   //only in bootstrap configuration
   //_kernel_warmup_elfloader();
   //
   #if defined (__KERNEL_NET_IPSTACK)
      #if defined(USE_UIP)
         uip_core_run();
      #endif
   #endif
   //
   __kernel_static_mode_out();
}

/*===========================================
End of Sourcekernel.c
=============================================*/
