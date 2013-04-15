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

/*!
   \option
      -t timeout in ms
      -i stdin
      -o stdout
      -e stderr

   \brief
      init is first process at lepton startup.

   \use
      do not call in shell.
      kernel task, is the only task, which can launch init process.

*/

/*===========================================
Includes
=============================================*/

#include <stdlib.h>
#include <string.h>

#include "kernel/core/kernel.h"

#include "kernel/core/signal.h"
#include "kernel/core/libstd.h"
#include "kernel/core/stat.h"
#include "kernel/core/statvfs.h"
#include "kernel/core/devio.h"
#include "kernel/core/time.h"
#include "kernel/core/wait.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/ioctl.h"
#include "kernel/core/select.h"
#include "kernel/core/stropts.h"
//
#include "kernel/core/ioctl_lcd.h"
#include "kernel/core/ioctl_fb.h"
//
#include "lib/libc/stdio/stdio.h"


/*===========================================
Global Declaration
=============================================*/
static const char banner[] =
   "\
lepton (tauon) posix os\r\n\
$Revision: 1.3 $ $Date: 2009-06-18 13:43:22 $\r\n\
";


#define OPT_MSK_T 0x01 //-t timeout=5000;#5000ms (if timeout=0 then go=1;)
#define OPT_MSK_I 0x02 //-i stdin=/dev/ttys1
#define OPT_MSK_O 0x04 //-o stdout=/dev/ttys1
#define OPT_MSK_E 0x08 //-e stderr=/dev/ttys1 (if stderr not specified stderr = stdout)
#define OPT_MSK_S 0x10 //-s silent mode

/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:sigterm_handler
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static void sigterm_handler(int sig){
   printf("\r\nsending SIGTERM: shutdown all processes\r\n");
   kill(-1,SIGTERM);
   alarm(10);
}

/*-------------------------------------------
| Name:init_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int initd_main(int argc,char* argv[]){
   int fd=-1;
   int status=0;
   time_t cur_time;
   pid_t pid=0;
   int oflag=0;
   int c,i;
   int timeout=0;
   struct timeval time_out;
   char go = 0;
   fd_set readfs;
   struct sigaction sa;
   char * _stdin=(char*)0;
   char * _stdout=(char*)0;
   char * _init_script_path=(char*)0;
   unsigned char opt=0;

   //pid=1 ppid=0 pgid=1
   setpgid(0,0);

   //get option
   for(i=1; i<argc; i++) {
      if(argv[i][0]=='-') {
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1; c<l; c++) {
            switch(argv[i][c]) {
            case 't':   //timeout
               opt |= OPT_MSK_T;
               break;

            case 's':   //
               opt |= OPT_MSK_S;
               break;

            case 'i':   //stdin
               opt |= OPT_MSK_I;
               break;

            case 'o':   //stdout
               opt |= OPT_MSK_O;   //ms
               break;

            case 'e':   //stderr
               opt |= OPT_MSK_E;   //ms
               break;

            }
         }
      }else{
         if(!argv[i])
            return -1;

         if(opt&OPT_MSK_T) {
            timeout = atoi(argv[i])/10; //timebase in select());
         }else if(opt&OPT_MSK_I) {
            _stdin = argv[i];
         }else if(opt&OPT_MSK_O) {
            _stdout = argv[i];
         }else if(opt&OPT_MSK_E) {
         }else if(opt&OPT_MSK_S) {
         }else{
            _init_script_path=argv[i];
         }

         opt=0;
      }
   }

   //SIGTERM interception
   sa.sa_handler=sigterm_handler;

   sigaction(SIGTERM,&sa,NULL);

   //stdin fd =0
   if(!_stdin)
      return -1;
   fd= open(_stdin,O_RDONLY,0);
   if(fd<0)
      return -1;

   //stdout fd =1
   if(!_stdout)
      return -1;
   if(!(opt&OPT_MSK_S))
      fd= open(_stdout,O_WRONLY,0);
   else
      fd= open("/dev/null",O_WRONLY,0);
   if(fd<0)
      return -1;

   //open stderr: use dup(fd) fd is ttys1 in O_WRONLY
   //stderr fd =2
   fd  = dup(fd);
   if(fd<0)
      return -1;

   //be aware like jcvd ;)!!!! test console system
#if (defined(EVAL_BOARD) || defined(CPU_GNU32)) && !defined(USE_KERNEL_STATIC)

   #if defined(USE_NANOX)
   {
      int fd_fb=-1;
      int fd_fb0=-1;
      int fd_fb1=-1;

      fb_info_t fb_info  = {0,0,0,0,0,0,NULL,0,-1,NULL};
      fb_info_t fb0_info = {0,0,0,0,0,0,NULL,0,-1,NULL};
      fb_info_t fb1_info = {0,0,0,0,0,0,NULL,0,-1,NULL};

      fd=open("/dev/lcd0",O_WRONLY,0);
      //get lcd data addr
      ioctl(fd, LCDGETVADDR, &fb_info.smem_start);
      //get lcd info
      ioctl(fd, FBIOGET_DISPINFO, &fb_info);
      fb_info.cmap=(void*)0; //color map from screen
      fb_info.cmap_len=0; //color map length
      fb_info.desc_w=-1;
      fb_info.next=(void*)0; //next framebuffer data

      //set framebuffer size
      memcpy(&fb0_info,&fb_info,sizeof(fb_info_t));
      fb0_info.y_res=fb_info.y_res/2;
      fb0_info.smem_start=0;
      fb0_info.smem_len=0;
      //
      memcpy(&fb1_info,&fb_info,sizeof(fb_info_t));
      fb1_info.y_res=fb_info.y_res/2;
      fb1_info.smem_start=0;
      fb1_info.smem_len=0;

      //open fd and link it to create generic fb
      fd_fb=open("/dev/fb/fb", O_WRONLY,0);
      //fb_info.smem_start = (unsigned long)lcd_addr;
      //set cmap from lcd
      ioctl(fd, FBIOGETCMAP, &fb_info.cmap_len, &fb_info.cmap);
      //link generic framebuffer to lcd
      ioctl(fd_fb, I_LINK, fd, &fb_info);
      //ioctl(fd_fb, FBIOPUT_INIT, &fb_info);

      //attach it to 2 fd framebuffer fildes
      fattach(fd_fb, "/dev/fb/fb0");
      fattach(fd_fb, "/dev/fb/fb1");
      close(fd_fb);
      close(fd);

      //open it
      fd_fb0 = open("/dev/fb/fb0", O_WRONLY,0);
      fd_fb1 = open("/dev/fb/fb1", O_WRONLY,0);

      //configure it
      ioctl(fd_fb0,FBIOPUT_MODEINFO,&fb0_info,1);
      ioctl(fd_fb1,FBIOPUT_MODEINFO,&fb1_info,2);

      close(fd_fb1);
      ioctl(STDOUT_FILENO,I_LINK,fd_fb0);
   }
   #else
   fd=open("/dev/lcd0",O_WRONLY,0);
   ioctl(STDOUT_FILENO,I_LINK,fd);
   #endif
#endif
   //

   //print kernel information
   printf(banner);
   printf("\r\n\nversion %s kernel compilation date: %s - %s\r\n",__LEPTON_VERSION,__KERNEL_DATE,
          __KERNEL_TIME);
   cur_time = time(&cur_time);
   printf("\r\n%s\r\n\r\n",ctime(&cur_time));

   //run script ?
   printf("\r\ntype ctrl-x: .init script not run or any key to continue\r\n");
   //set oflag|O_NONBLOCK on stdin
   oflag = fcntl(STDIN_FILENO,F_GETFL);
   fcntl(STDIN_FILENO,F_SETFL,oflag|O_NONBLOCK);
   //async
   FD_ZERO(&readfs);
   //
   if(!timeout)
      go = 1;
   while(!go) {
      //
      time_out.tv_sec = timeout/1000;
      time_out.tv_usec = (timeout%1000)*1000;
      //
      FD_SET(STDIN_FILENO,&readfs); //stdin

      switch( select(STDIN_FILENO+1,&readfs,0,0,&time_out) ) {
      case 0:
         if(!(timeout%100))
            printf("elapse time:%d s.\r\n",timeout/100);
         if(--timeout<0)
            go = 1;
         break;

      default:
         //
         if(FD_ISSET(STDIN_FILENO,&readfs)) {   //stdin
            c = getchar();
            if(c =='\x18')    //ctrl-x
               go = 2;
            else
               go = 1;
         }
         break;
      }

   }
   //restore old oflag
   fcntl(STDIN_FILENO,F_SETFL,oflag);
   printf("\r\nlepton start!\r\n\r\n\r\n");

   //to remove: only for test.
   //Sleep(5000);
   //printf("\r\n test printf %f!\r\n\r\n\r\n",1.5);
   //printf("\r\n test printf %+05.2f!\r\n\r\n\r\n",1.2345);

   //start shell with '.init' script?
   if((pid=vfork())==0) {
      if(opt&OPT_MSK_S) {
         close(STDOUT_FILENO);
         open(_stdout,O_WRONLY,0);
      }
      if(go == 1) {
         if(_init_script_path)
            execl("/usr/sbin/lsh","/usr/sbin/lsh",_init_script_path,NULL);
         else
            execl("/usr/sbin/lsh","/usr/sbin/lsh","/usr/etc/.init",NULL);
      }else if(go == 2) {
         execl("/usr/sbin/lsh","/usr/sbin/lsh",NULL);
      }
      exit(0);
   }

   //
   while((pid=waitpid((pid_t)-1,&status,0))>=0) {
      if(!pid) continue;
      printf(":[%d] done(%d)\r\n",pid,status);
   };

   if(!pid)
      printf(":error\r\n");

   //to do: execute shutdown script /usr/etc/.shutdown
   //usleep(10000000);

   printf("bye!\r\n");

   return 0;
}


/*===========================================
End of Source init.c
=============================================*/
