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

/*===========================================
Includes
=============================================*/
#include <stdlib.h>
#include <string.h>

#include "kernel/core/kernelconf.h"
#include "kernel/core/errno.h"
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernel.h"
#include "kernel/core/kernel_object.h"
#include "kernel/core/kernel_sem.h"
#include "kernel/core/syscall.h"
#include "kernel/core/process.h"
#include "kernel/core/flock.h"
#include "kernel/core/system.h"
#include "kernel/core/time.h"
#include "kernel/core/systime.h"
#include "kernel/core/bin.h"
#include "kernel/core/ioctl.h"


#include "kernel/fs/vfs/vfs.h"
#include "kernel/fs/vfs/vfskernel.h"

#include "lib/libc/stdio/stdio.h"

#if defined(CPU_GNU32)
   #include <cyg/hal/hal_io.h>
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
#if !defined(USE_ECOS)
kernel_pthread_t kernel_thread;
#endif

//
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
   __add_syscall(_syscall_waitpid),
   __add_syscall(_syscall_execve),
   __add_syscall(_syscall_exit),
   __add_syscall(_syscall_kill),
   __add_syscall(_syscall_vfork), //OK
   __add_syscall(_syscall_sigprocmask), //5
   __add_syscall(_syscall_sigpending),
   __add_syscall(_syscall_sigaction),
#if defined(USE_SEGGER)
   __add_syscall(_syscall_alarm),
#endif
   __add_syscall(_syscall_pause),
   __add_syscall(_syscall_mount),
   __add_syscall(_syscall_umount), //10
   __add_syscall(_syscall_opendir),
   __add_syscall(_syscall_closedir),
   __add_syscall(_syscall_readdir),
   __add_syscall(_syscall_rewinddir),
   __add_syscall(_syscall_telldir), //15
   __add_syscall(_syscall_seekdir),
   __add_syscall(_syscall_creat),
   __add_syscall(_syscall_open),
   __add_syscall(_syscall_close),
   __add_syscall(_syscall_read), //20
   __add_syscall(_syscall_write),
   __add_syscall(_syscall_lseek),
   __add_syscall(_syscall_stat),
   __add_syscall(_syscall_mkdir),
   __add_syscall(_syscall_rmdir), //25
   __add_syscall(_syscall_chdir),
   __add_syscall(_syscall_dup),
   __add_syscall(_syscall_dup2),
   __add_syscall(_syscall_pipe),
   __add_syscall(_syscall_sigexit), //30
   __add_syscall(_syscall_mknod),
   __add_syscall(_syscall_ftruncate),
   __add_syscall(_syscall_truncate),
   __add_syscall(_syscall_rm),
   __add_syscall(_syscall_fstatvfs), //35
   __add_syscall(_syscall_statvfs),
   __add_syscall(_syscall_remove),
   __add_syscall(_syscall_gettimeofday),
#if defined(USE_SEGGER)
   __add_syscall(_syscall_atexit),
#endif
   __add_syscall(_syscall_malloc),
   __add_syscall(_syscall_calloc), //40
   __add_syscall(_syscall_realloc),
   __add_syscall(_syscall_free),
   __add_syscall(_syscall_fcntl),
   __add_syscall(_syscall_getcwd),
   __add_syscall(_syscall_fstat), //45
   __add_syscall(_syscall_mkfs),
   __add_syscall(_syscall_setpgid),
   __add_syscall(_syscall_getpgrp),
   __add_syscall(_syscall_settimeofday),
   __add_syscall(_syscall_sync), //50
   __add_syscall(_syscall_fattach),
   __add_syscall(_syscall_fdetach),
   __add_syscall(_syscall_ioctl),
   __add_syscall(_syscall_sysctl),
   __add_syscall(_syscall_rename), //55
   __add_syscall(_syscall_pthread_create),
   __add_syscall(_syscall_pthread_cancel),
   __add_syscall(_syscall_pthread_exit),
#if defined(USE_SEGGER)
   __add_syscall(_syscall_pthread_kill),
#endif
   __add_syscall(_syscall_pthread_mutex_init),
   __add_syscall(_syscall_pthread_mutex_destroy), //60
   __add_syscall(_syscall_pthread_cond_init),
   __add_syscall(_syscall_pthread_cond_destroy),
   __add_syscall(_syscall_timer_create),
   __add_syscall(_syscall_timer_delete),
   __add_syscall(_syscall_sem_init),
   __add_syscall(_syscall_sem_destroy)
};


//test backup_stack
//int backup_stack(kernel_pthread_t * pth);

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
   kernel_pthread_t* pthread_ptr = kernel_pthread_self(); /*g_pthread_lst;*/

   __set_syscall_owner_pid(pthread_ptr->pid);
   __set_syscall_owner_pthread_ptr(pthread_ptr);
   //errno
   _syscall_owner_pthread_ptr->_errno = 0;

   if((kernel_syscall.p_syscall=kernel_syscall_lst[pthread_ptr->reg.syscall].p_syscall)) {
      //
      if(kernel_syscall.p_syscall(pthread_ptr,pthread_ptr->pid,pthread_ptr->reg.data)<0) {
         return -1;
      }
   }
   return 0;
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
| Name:        _kernel_warmup_load_mount_cpufs
| Description:
| Parameters:  none
| Return Type: none
| Comments:
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

   //devices
   _vfs_mkdir("/dev",0);
   _vfs_mkdir("/dev/hd",0);

   //kernel
   _vfs_mkdir("/kernel",0);
#if !defined(CPU_CORTEXM)
   _vfs_mount(fs_kofs,(char*)0,"/kernel");
#endif

   //directories
   _vfs_mkdir("/bin",0);
   _vfs_mkdir("/usr",0);
   _vfs_mkdir("/etc",0);
   _vfs_mkdir("/var",0);
   _vfs_mkdir("/mnt",0);

   _vfs_ls("/");
#if defined(CPU_GNU32)
   //add sdcard periph
   _vfs_mkdir("/mem",0);
#endif

#if !defined(TAUON_BOOTLOADER)
   //make binaries
   _mkbin("/bin/init","init");
   _mkbin("/bin/mkfs","mkfs");
   _mkbin("/bin/mount","mount");
   _mkbin("/bin/umount","umount");
   _mkbin("/bin/lsh","lsh");
#endif
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

   io_profiler_result_lst = malloc(sizeof(io_profiler_result_t)*max_dev);
   memset(io_profiler_result_lst,0,sizeof(io_profiler_result_t)*max_dev);

   __io_profiler_init();

#endif
   return 0;
}

/*--------------------------------------------
| Name:        _kernel_warmup_boot
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _kernel_warmup_boot(void)
{
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
               st=2;                         //st=4;
            continue;                    //*(pbuf)=';';
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
               st=-1;                         //st=4;
               pbuf++;                         //*(pbuf)='\0';
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
            case 0: {                   //open dev
               char* dev=buf;
               desc_t _desc;
               st=1;                         //st=3;
               //
               if((_desc = _vfs_open(dev,O_RDONLY,0))<0)
                  break;
               _vfs_ioctl(_desc,IOCTL_MULTIBOOT_GETVAL,&kb_val);
               _vfs_close(_desc);
            }
            break;

            case 1: {                   //delay
               unsigned int delay=atoi(buf);
               st =2;
               //to do: usleep cannot be use kernel is not started
               /*if(delay)
    usleep(delay);*/
            }
            break;
            //
            case 2: {                   //match kb val
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

            case 4: {                   //exec bin
               char* argv[ARG_MAX]={0};
               int argc=0;
               argv[argc] = strtok( buf," ");
               while( argv[argc++] != NULL )
                  argv[argc] = strtok( NULL," ");                              //Get next token:
               _sys_krnl_exec(argv[0],(const char **)argv,0,0,0);
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

   }
   else{
      //const char* argv[ARG_MAX]={"init","-i","/dev/ttydbgu","-o","/dev/tty0"};
      const char* argv[ARG_MAX]={"initd","-i","/dev/ttys0","-o","/dev/ttys0"};
      //_sys_krnl_exec("/usr/bin/init",(const char **)argv,0,0,0);
      _sys_krnl_exec("/usr/sbin/initd",(const char **)argv,0,0,0);
   }
   return 0;
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
| Name:        _kernel_warmup_cpu
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
void _kernel_warmup_cpu(void)
{
   desc_t desc;
   if((desc = _vfs_open("/dev/cpu0",O_RDWR,0))<0)
      return;  //cpu device not available
   __set_cpu_desc(desc);
}


/*--------------------------------------------
| Name:        _kernel_warmup_rtc
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _kernel_warmup_rtc(void)
{
   desc_t desc = -1;
   //specific rtc
   //set kernel time from rtc
   if((desc = _vfs_open("/dev/rtc0",O_RDONLY,0))<0)      //ST m41t81
      desc = _vfs_open("/dev/rtc1",O_RDONLY,0);

   if(desc>=0) {
      char buf[8]={0};
      struct tm _tm={ 0, 0, 12, 28, 0, 103 };          //init for test
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
   if((desc = _vfs_open("/dev/rtt0",O_RDONLY,0))<0)      //ST m41t81
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

#if defined(USE_KERNEL_STATIC)
void __wrpr_kernel_dev_gettime(desc_t __desc, char * __buf, int __size){
   if(ofile_lst[__desc].pfsop->fdev.pfdev_ext)
      ((fdev_rtc_t*)(ofile_lst[__desc].pfsop->fdev.pfdev_ext))->fdev_rtc_gettime(__desc,__buf,
                                                                                 __size);
}
#endif

/*-------------------------------------------
| Name:_kernel_mount
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kernel_mount(const char* argv[]){
   static const char* fstype_list[]={"rootfs","ufs","ufsx","kofs","msdos","vfat"};
   static const int fstype_list_size=sizeof(fstype_list)/sizeof(char*);
   fstype_t i;
   fstype_t fstype = -1;

   if(!argv[1]) return -1;

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
| Name:        _kernel_warmup_mount
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
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
            _kernel_mount((const char **)argv);
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
         _kernel_mount((const char **)argv);
      }
      _vfs_close(desc);
   }
   return 0;
}


/*--------------------------------------------
| Name:        _kernel_routine
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
void _kernel_routine(void* arg){
   int ret;
   kernel_pthread_t* pthread_ptr;

#if !defined(CPU_CORTEXM)
   __set_irq();
#endif
   __syscall_lock();

   ret = _kernel_syscall();
   pthread_ptr = _syscall_owner_pthread_ptr;

   //wait on a child
   if(pthread_ptr->stat&PTHREAD_STATUS_STOP) {
      __syscall_unlock();
      //bug fix waitpid event
      if(pthread_ptr->reg.syscall == _SYSCALL_WAITPID) {
         kernel_sem_post(&pthread_ptr->sem_wait);
      }
      __kernel_pthread_suspend(pthread_ptr);

#if !defined(CPU_CORTEXM)
      __clr_irq();
#endif
      __kernel_ret_syscall(pthread_ptr);
   }
   else {
      __syscall_unlock();
#if !defined(CPU_CORTEXM)
      __clr_irq();
#endif
      __kernel_ret_syscall(pthread_ptr);
   }
}

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
   //
   __kernel_static_mode_in();
   //init kernel system
   //init gestion processus
   _pid();
   //init filesystem
   _vfs();
   //init mutex for stdin, stdout, stderr
   __stdio_init();
   //init locks
   _flocks();

   //init syscall
#if !defined(TAUON_BOOTLOADER)
   _init_syscall();
#endif

   kernel_pthread_mutex_init(&kernel_mutex,&mutex_attr);

   _kernel_warmup_profiler();

   _kernel_warmup_rootfs();

   _kernel_warmup_load_mount_cpufs();

   _kernel_warmup_i2c();

   _kernel_warmup_dev();

   _kernel_warmup_object_manager();

   _kernel_warmup_cpu();

   _kernel_warmup_rtc();

   _kernel_warmup_mount();

   _kernel_warmup_boot();

   __kernel_static_mode_out();
}

/*-------------------------------------------
| Name: _init_syscall
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _init_syscall(void)
{
#if defined(CPU_GNU32) && !defined(USE_KERNEL_STATIC)
   struct cyg_hal_sys_sigaction action;
   struct cyg_hal_sys_sigset_t blocked;

   CYG_HAL_SYS_SIGFILLSET(&blocked);
   CYG_HAL_SYS_SIGADDSET(&blocked, KERNEL_INTERRUPT_NB);
   if (0 !=
       cyg_hal_sys_sigprocmask(CYG_HAL_SYS_SIG_UNBLOCK, &blocked, (cyg_hal_sys_sigset_t*) 0)) {
      //diag_printf("Failed to initialize sigprocmask");
   }

   action.hal_mask     = 0;
   action.hal_flags    = CYG_HAL_SYS_SA_SIGINFO;
   action._hal_u.hal_sigaction = &_kernel_syscall_handler_synth;
   action.hal_restorer = (void (*)(void)) 0;

   if (0 !=
       cyg_hal_sys_sigaction(KERNEL_INTERRUPT_NB,  &action, (struct cyg_hal_sys_sigaction*) 0)) {
      //diag_printf("Failed to install signal handler for CYG_HAL_SYS_SIGUSR1");
   }

#elif defined(CPU_ARM7) || defined(CPU_ARM9)
   /*variables necessaires pour gestion syscall*/
   cyg_exception_handler_t *old_handler;
   cyg_addrword_t old_data;
   /*Installation de l'exception*/
   cyg_exception_set_handler(CYGNUM_HAL_VECTOR_SOFTWARE_INTERRUPT,
                             &_kernel_syscall_handler, 0, &old_handler, &old_data);
#else

#endif
}

/*-------------------------------------------
| Name: _kernel_syscall_handler_synth
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
#if defined(CPU_GNU32) && !defined(USE_KERNEL_STATIC)

void _kernel_syscall_handler_synth(int sig, cyg_hal_sys_siginfo_t * info, void *ptr)
{
   k_handler_context_t * kh_context;
   //normalement recherche du pid du thread appelant
   kernel_pthread_t* pthread_ptr = kernel_pthread_self();
   if((pthread_ptr->stat&PTHREAD_STATUS_KERNEL) == PTHREAD_STATUS_KERNEL) {
      /*changement de mode*/
      __kernel_end_syscall(pthread_ptr)
      kh_context = (k_handler_context_t *)ptr;
      /*recopie pour reprendre le flux d'exe normal*/
      memcpy((void *)&(kh_context->hal_uc_mcontext.hal_gregs), (void *)&(pthread_ptr->bckup_context),
             sizeof(context_t));

   }

   //thread en mode user
   else if(((pthread_ptr->stat ==  PTHREAD_STATUS_NULL ||
             (pthread_ptr->stat&PTHREAD_STATUS_SIGHANDLER) == PTHREAD_STATUS_SIGHANDLER))
           && pthread_ptr->pid>0) {
      int addr_k_entry = (int) _kernel_routine;
      //changement de mode
      __kernel_begin_syscall(pthread_ptr)
      kh_context = (k_handler_context_t *)ptr;
      //sauvegarde de contexte dans structure
      memcpy((void *)&(pthread_ptr->bckup_context), (void *)&(kh_context->hal_uc_mcontext.hal_gregs),
             sizeof(context_t));
      //empilage de l'adresse de _kernel_routine sur la pile noyau du thread
      memcpy((void *)(pthread_ptr->kernel_stack + KERNEL_STACK - sizeof(int)),
             (void *)&addr_k_entry, sizeof(int));
      //empilage de ebx sur la pile noyau
      memcpy((void *)(pthread_ptr->kernel_stack + KERNEL_STACK - 2*sizeof(int)),
             (void *)&kh_context->hal_uc_mcontext.hal_gregs[S_REG_EBX], sizeof(int));
      //faire pointer le sp au bon endroit
      kh_context->hal_uc_mcontext.hal_gregs[S_REG_ESP] =
         (int)(pthread_ptr->kernel_stack + KERNEL_STACK
               - 2*sizeof(int));
   }
}

#elif defined(CPU_ARM7) || defined(CPU_ARM9)

void _kernel_syscall_handler(cyg_addrword_t data,
                             cyg_code_t number,
                             cyg_addrword_t info)
{
   context_t * tmp_save_context;
   //normalement recherche du pid du thread appelant
   kernel_pthread_t* pthread_ptr = kernel_pthread_self();
   //
   if((pthread_ptr->stat&PTHREAD_STATUS_KERNEL) == PTHREAD_STATUS_KERNEL) {
      __kernel_end_syscall(pthread_ptr);
      /*restauration contexte*/
      tmp_save_context = &(pthread_ptr->bckup_context);
      //dummy test
      //pthread_ptr->tcb->stack_ptr = pthread_ptr->bckup_context.sp;
      tmp_save_context->pc = tmp_save_context->lr;

      memcpy((void *)info,(void *) tmp_save_context,sizeof(context_t));
   }
   /*thread en mode user (1er passage dans la routine)*/
   else if(pthread_ptr->stat ==  PTHREAD_STATUS_NULL ||
           (pthread_ptr->stat&PTHREAD_STATUS_SIGHANDLER) == PTHREAD_STATUS_SIGHANDLER) {
      //changement de mode
      __kernel_begin_syscall(pthread_ptr)
      tmp_save_context = (context_t *)info;
      /*sauvegarde contexte*/
      memcpy((void *)&(pthread_ptr->bckup_context),(void *)tmp_save_context,sizeof(context_t));
      /*copie de tmp_save_context dans pile en mode noyau en queue de pile*/
      memcpy((void *)(pthread_ptr->kernel_stack + KERNEL_STACK - sizeof(context_t)),
             (void *)tmp_save_context, sizeof(context_t));
      tmp_save_context->lr = (cyg_uint32)_kernel_routine;
      tmp_save_context->pc = tmp_save_context->lr;
      /*modification pointeur de pile*/
      tmp_save_context->sp = (cyg_uint32)(pthread_ptr->kernel_stack
                                          + KERNEL_STACK - sizeof(context_t));
   }
}

#elif defined(CPU_CORTEXM)
void do_swi(void) {
   int a=(int)_kernel_syscall_handler;
   //put handler addr in r3
   __asm__("str r3, %0\n\t"
           "swi 0\n\t"
           : "=m" (a)
           );
}

///DUMMY
kernel_pthread_t* _kernel_pthread_self_swi(void){
   kernel_pthread_t* p;
   p=g_pthread_lst;
   while(p) {
      if( __is_thread_self(p) ) {
         return p;
      }
      p=p->gnext;
   }

   return (kernel_pthread_t*)0;
}
///

void _kernel_syscall_handler(void) {
   //normalement recherche du pid du thread appelant
   kernel_pthread_t* pthread_ptr = _kernel_pthread_self_swi();     //kernel_pthread_self();
   //R12 contain stack addr
   __asm__("str r12, %0" : "=m" (pthread_ptr->bckup_context.svc_r12));
   pthread_ptr->bckup_context.svc_regs = (svc_reg_t*)pthread_ptr->bckup_context.svc_r12;
   //
   if((pthread_ptr->stat&PTHREAD_STATUS_KERNEL) == PTHREAD_STATUS_KERNEL) {
      __kernel_end_syscall(pthread_ptr);
      //restore previous context
      memcpy((void *)pthread_ptr->bckup_context.user_stack_addr,
             (void *)pthread_ptr->bckup_context.svc_regs,
             sizeof(svc_reg_t));
      //modify return addr to back in user function
      pthread_ptr->bckup_context.svc_regs = (svc_reg_t*)pthread_ptr->bckup_context.user_stack_addr;
      pthread_ptr->bckup_context.svc_regs->ret_addr = pthread_ptr->bckup_context.save_user_ret_addr;
      pthread_ptr->bckup_context.svc_regs->r14 = pthread_ptr->bckup_context.save_user_r14;
      //
      __asm__("ldr r4,%0\n\t"
              "msr psp,r4\n\t"
              :
              : "m" (pthread_ptr->bckup_context.user_stack_addr));

      __asm__("mrs r4,msp\n\t"
              "str r4,%0\n\t"
              : "=m" (pthread_ptr->bckup_context.ret_addr));
      *(pthread_ptr->bckup_context.ret_addr+
        2)=pthread_ptr->bckup_context.user_stack_addr+sizeof(svc_reg_t);
   }
   /*thread en mode user (1er passage dans la routine)*/
   else if(pthread_ptr->stat ==  PTHREAD_STATUS_NULL ||
           (pthread_ptr->stat&PTHREAD_STATUS_SIGHANDLER) == PTHREAD_STATUS_SIGHANDLER) {
      //changement de mode
      __kernel_begin_syscall(pthread_ptr)
      //align 8 bytes
      pthread_ptr->bckup_context.kernel_stack_addr =
         (unsigned int)(pthread_ptr->kernel_stack+KERNEL_STACK);
      pthread_ptr->bckup_context.kernel_stack_addr &= KERNEL_STACK_ALIGN_MASK;
      //copy save registers in kernel_stack-8*4
      pthread_ptr->bckup_context.kernel_stack_addr -= sizeof(svc_reg_t);
      memcpy((void *)pthread_ptr->bckup_context.kernel_stack_addr,
             (void *)pthread_ptr->bckup_context.svc_regs,
             sizeof(svc_reg_t));
      //do it twice to save previous return addr to back in user mode
      //or save user link register
      pthread_ptr->bckup_context.save_user_ret_addr = pthread_ptr->bckup_context.svc_regs->ret_addr;
      pthread_ptr->bckup_context.save_user_r14 = pthread_ptr->bckup_context.svc_regs->r14;

      //modify return addr to execute kernel_routine
      pthread_ptr->bckup_context.svc_regs =
         (svc_reg_t*)pthread_ptr->bckup_context.kernel_stack_addr;
      pthread_ptr->bckup_context.svc_regs->ret_addr = (unsigned int)_kernel_routine;
      pthread_ptr->bckup_context.svc_regs->r14 = 0;
      //save previous user stack
      pthread_ptr->bckup_context.user_stack_addr =
         (unsigned int)(pthread_ptr->bckup_context.svc_r12);
      //modify stack
      __asm__("ldr r4,%0\n\t"
              "msr psp,r4\n\t"
              :
              : "m" (pthread_ptr->bckup_context.kernel_stack_addr));

   }
}
#endif

/*===========================================
| End of Source : kernel.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
| Revision 1.7  2010/03/03 09:27:12  jjp
| version 3.0.2.6 see change-log.txt
|
| Revision 1.6  2010/02/12 14:53:50  jjp
| add sem_init and sem_destroy syscall + mount kofs + add errno
|
| Revision 1.5  2009/11/17 11:14:20  jjp
| Use __set_syscall_owner_pid and __set_syscall_owner_pthread_ptr macros
|
| Revision 1.4  2009/11/13 12:03:18  jjp
| improve syscall mechanism
|
| Revision 1.3  2009/07/28 12:19:07  jjp
| add sdcard device for synthetic target
|
| Revision 1.2  2009/06/09 10:15:43  jjp
| add pthread_cond_init and pthread_cond_destroy syscall
|
| Revision 1.1  2009/03/30 15:48:57  jjp
| first import of tauon
|
| Revision 1.1  2009/03/30 11:18:54  jjp
| First import of tauon
|
| Revision 1.1.1.1  2009/02/05 15:29:49  jjp
| First import in CVS
|
|---------------------------------------------
=============================================*/
