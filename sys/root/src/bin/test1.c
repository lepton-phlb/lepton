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
#include "kernel/signal.h"
#include "kernel/libstd.h"
#include "kernel/stat.h"
#include "kernel/statvfs.h"
#include "kernel/devio.h"
#include "kernel/time.h"
#include "kernel/select.h"

#include "kernel/rttimer.h"
#include "kernel/rtevent.h"

#include "vfs/vfs.h"

#include "stdio/stdio.h"


/*===========================================
Global Declaration
=============================================*/


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:hand
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void sigpipe_hand(int sig){
   int i=0;

   printf("SIGPIPE sighandler(%d) %d\r\n",_sys_getpid(),sig);

   i++;
}

/*-------------------------------------------
| Name:vfs_test_win32
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void vfs_test_win32(void){
   char ref[16];
   desc_t fd;
   int cb;
   struct vfs_formatopt_t vfs_formatopt;
   struct statvfs statvfs;

   _vfs_ls("/");
   _vfs_ls("/dev");
   _vfs_ls("/bin");

   _vfs_mkdir("/mnt",0);
   _vfs_mkdir("/mnt2",0);
   _vfs_mkdir("/mnt/f0",0);
   _vfs_mkdir("/mnt2/fm2",0);

   _vfs_ls("/mnt");
   _vfs_ls("/mnt2");


   strcpy(ref,"/dev/rd0");
   printf("makefs : on %s \n",ref);
   vfs_formatopt.max_blk   = 20;
   vfs_formatopt.max_node  = 99;
   _vfs_makefs(fs_ufs,"/dev/rd0",&vfs_formatopt);

   strcpy(ref,"/dev/rd1");
   printf("makefs : on %s \n",ref);
   vfs_formatopt.max_blk   = 20;
   vfs_formatopt.max_node  = 99;
   _vfs_makefs(fs_ufs,"/dev/rd1",&vfs_formatopt);


   strcpy(ref,"/dev/rd0");
   printf("mount : %s on /mnt \n",ref);
   _vfs_mount(fs_ufs,"/dev/rd0","/mnt");

   _vfs_ls("/");
   _vfs_ls("/mnt");

   strcpy(ref,"/dev/rd1");
   printf("mount : %s on /mnt2 \n",ref);
   _vfs_mount(fs_ufs,"/dev/rd1","/mnt2");

   _vfs_ls("/");
   _vfs_ls("/mnt2");


   //
   _vfs_statvfs("/mnt",&statvfs );

   fd = _vfs_open("/mnt/f1",O_CREAT|O_WRONLY,0);
   _vfs_close(fd);

   fd = _vfs_open("/mnt/f2",O_CREAT|O_WRONLY,0);
   _vfs_close(fd);

   _vfs_statvfs("/mnt",&statvfs );

   //
   fd = _vfs_open("/mnt2/f21",O_CREAT|O_WRONLY,0);
   _vfs_close(fd);

   fd = _vfs_open("/mnt2/f22",O_CREAT|O_WRONLY,0);
   _vfs_close(fd);

   _vfs_mkdir("/mnt2/mnt3",0);

   _vfs_ls("/mnt");
   _vfs_ls("/mnt2");

   strcpy(ref,"/mnt");
   printf("umount : %s\n",ref);
   _vfs_umount("/mnt");

   _vfs_ls("/");
   _vfs_ls("/mnt");
   _vfs_ls("/mnt2");

   //remount test
   strcpy(ref,"/dev/rd0");
   printf("remount : %s on /mnt2/mnt3 \n",ref);
   _vfs_mount(fs_ufs,"/dev/rd0","/mnt2/mnt3");

   _vfs_ls("/");
   _vfs_ls("/mnt");
   _vfs_ls("/mnt2");
   _vfs_ls("/mnt2/mnt3");

   //remount test: dirty test becaur /dev/rd0 already mounted
   /*
   strcpy(ref,"/dev/rd0");
   printf("mount : %s on /mnt2/mnt3 \n",ref);
   _vfs_mount(fs_ufs,"/dev/rd0","/mnt2/mnt3");

   _vfs_ls("/mnt2/mnt3");
   */

   _vfs_statvfs("/mnt2/mnt3",&statvfs );

   fd = _vfs_open("/mnt2/mnt3/f23",O_CREAT|O_WRONLY,0);
   _vfs_statvfs("/mnt2/mnt3",&statvfs );
   _vfs_write(fd,"abcdefghijklmonopqrstuvwxyz0123456789",
              strlen("abcdefghijklmonopqrstuvwxyz0123456789"));
   _vfs_ftruncate(fd,4);
   _vfs_lseek(fd,0,SEEK_SET);
   while((cb=_vfs_read(fd,ref,14))>0) {
      ref[cb]=0x00;
      printf("read = %s\n",ref);
   }
   _vfs_close(fd);

   _vfs_statvfs("/mnt2/mnt3",&statvfs );

   _vfs_ls("/mnt2/mnt3");

   _vfs_remove("/mnt2/mnt3/f23");

   _vfs_statvfs("/mnt2/mnt3",&statvfs);

   _vfs_ls("/mnt2/mnt3");
}

/*-------------------------------------------
| Name:vfs_test_m16c
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void vfs_test_m16c(void){
   char ref[16];
   desc_t fd;
   int cb;
   struct vfs_formatopt_t vfs_formatopt;
   struct statvfs _statvfs;

   _vfs_ls("/");
   _vfs_ls("/dev");
   _vfs_ls("/bin");

   mkdir("/mnt",0);
   mkdir("/mnt2",0);
   mkdir("/mnt/f0",0);
   mkdir("/mnt2/fm2",0);

   _vfs_ls("/mnt");
   _vfs_ls("/mnt2");


   //test filecpu

   printf("mount : /dev/rd2 on /mnt \n");
   mount(fs_ufs,"/dev/rd2","/mnt");

   _vfs_ls("/mnt");

   printf("umount : /mnt\n");
   umount("/mnt");


   printf("mount : /dev/rd0 on /mnt \n");
   mount(fs_ufs,"/dev/rd0","/mnt");

   _vfs_ls("/mnt");

   printf("umount : /mnt\n");
   umount("/mnt");

   //
   strcpy(ref,"/dev/rd0");
   printf("makefs : on %s \n",ref);
   vfs_formatopt.max_blk   = 20;
   vfs_formatopt.max_node  = 99;
   _vfs_makefs(fs_ufs,"/dev/rd0",&vfs_formatopt);

   //
   strcpy(ref,"/dev/rd0");
   printf("mount : %s on /mnt \n",ref);
   mount(fs_ufs,"/dev/rd0","/mnt");

   _vfs_ls("/");
   _vfs_ls("/mnt");
   _vfs_ls("/mnt2");



   //
   statvfs("/mnt",&_statvfs );

   fd = open("/mnt/f1",O_CREAT|O_WRONLY,0);
   close(fd);

   fd = open("/mnt/f2",O_CREAT|O_WRONLY,0);
   close(fd);

   statvfs("/mnt",&_statvfs );


   strcpy(ref,"/mnt");
   printf("umount : %s\n",ref);
   umount("/mnt");

   _vfs_ls("/");
   _vfs_ls("/mnt");

   //remount test
   strcpy(ref,"/dev/rd0");
   printf("remount : %s on /mnt2 \n",ref);
   mount(fs_ufs,"/dev/rd0","/mnt2");

   _vfs_ls("/");
   _vfs_ls("/mnt");
   _vfs_ls("/mnt2");


   //remount test: dirty test because /dev/rd0 already mounted
   statvfs("/mnt2",&_statvfs );

   fd = open("/mnt2/f23",O_CREAT|O_WRONLY,0);
   statvfs("/mnt2",&_statvfs );
   write(fd,"abcdefghijklmonopqrstuvwxyz0123456789",strlen("abcdefghijklmonopqrstuvwxyz0123456789"));
   lseek(fd,0,SEEK_SET);
   while((cb=read(fd,ref,14))>0) {
      ref[cb]=0x00;
      printf("read = %s\n",ref);
   }
   ftruncate(fd,4);
   lseek(fd,0,SEEK_SET);
   while((cb=read(fd,ref,14))>0) {
      ref[cb]=0x00;
      printf("read = %s\n",ref);
   }
   close(fd);

   statvfs("/mnt2",&_statvfs );

   _vfs_ls("/mnt2");

   remove("/mnt2/f23");

   statvfs("/mnt2",&_statvfs);

   _vfs_ls("/mnt2");

   printf("chdir /mnt2\n");
   chdir("/mnt2");

   _vfs_ls(".");

   printf("mkdir dir2\n");
   mkdir("dir2",0);

   _vfs_ls(".");

   printf("chdir dir2\n");
   chdir("dir2");

   _vfs_ls(".");
   _vfs_ls("..");

   printf("chdir ..\n");
   chdir("..");

   _vfs_ls(".");

   printf("chdir ..\n");
   chdir("..");

   _vfs_ls(".");


}


/*-------------------------------------------
| Name:async_test
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void async_test(void){

   int fd0;
   int fd1;

   int cb,len=0;

   fd_set readfs;
   fd_set writefs;

   char buf[8]={0};

   char status=0; //0 read; 1:write

   fd0=open("/dev/ttys0",O_RDONLY|O_NONBLOCK,0);
   fd1=open("/dev/ttys0",O_WRONLY|O_NONBLOCK,0);


   FD_ZERO(&readfs);
   FD_ZERO(&writefs);

   for(;; ) {
      FD_SET(fd0,&readfs);
      FD_SET(fd1,&writefs);

      switch( select(fd1+1,&readfs,&writefs,0,0) ) {
      case 0:
         //timeout
         break;

      default:
         //readfs
         if(status==0 && FD_ISSET(fd0,&readfs)) {
            cb=read(fd0,buf,5);
            len=write(fd1,buf,cb);
            status=1;

         }

         //writefs
         if(status==1 && FD_ISSET(fd1,&writefs)) {
            if(len<cb)
               len+=write(fd1,buf+len,cb-len);
            else
               status=0;
         }

         break;


      }

   }

}

/*-------------------------------------------
| Name:test_rtxxx
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static tmr_t init_tmr;
static rtevt_t rtevt;

void _init_timer(void){
   _rtevt_signal(&rtevt,0x02,0);
}
//
void test_rtxxx(void){
   char buf[6];
   rttmr_attr_t rttmr_attr;

   _rtevt_create(&rtevt,0);

   rttmr_attr.tm_msec=5000; //5 sec
   rttmr_attr.func = _init_timer;
   rttmr_create(&init_tmr,&rttmr_attr);
   rttmr_start(&init_tmr);

   _rtevt_wait(&rtevt,0x02,0);

   alarm(5);
   if(vfork()==0)
      execve("/usr/sbin/lsh",0,"");

   read(0,buf,5);
}

/*-------------------------------------------
| Name:init_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int test1_main(int argc,char* argv[]){

   //char path[16]={0};
   char buf[8]={0};
   int i=0;
   int cb;
   int fd0=-1;
   int fd1=-1;
   int fd_lcd=-1;

   int p[2];
   int np;
   struct sigaction sa;
   time_t temps;
   //int ticks;



   pid_t pid=_sys_getpid();

   //
   //async_test();

   //

   //
   fd0= open("/dev/ttys0",O_RDONLY,0);
   fd1= open("/dev/ttys0",O_WRONLY,0);

   printf("open /dev/ttys0 O_WRONLY\r\n");
   printf("open /dev/ttys0 O_RDONLY\r\n");

   sa.sa_handler=sigpipe_hand;

   sigaction(SIGPIPE,&sa,NULL);

   kill(pid,SIGPIPE);

   raise(SIGPIPE);


   printf("%s(%d) started ok. argv[0]=%s\r\n",argv[0],pid,argv[0]);

   //lcd test
   /*
   fd_lcd= open("/dev/lcd0",O_RDONLY,0);
   if(fd_lcd>0){
      buf[0]=2;
      buf[1]=5;
      buf[2]=8;
      buf[3]=6;
      lseek(fd_lcd,110,SEEK_SET);
      write(fd_lcd,buf,4);
   }
   */

   //vfs test
   //vfs_test_m16c();

   //time test
   /*
   for(;;){
      temps = time(&temps);
      ticks = __get_timer_ticks();
      printf("%d : %s\n",ticks,ctime(&temps));

      OS_Delay(1000);
   }
   */


   //remove("/mnt2/mnt3/f22");


   //echo test
   while ((cb=read(STDIN_FILENO,buf,5))!=0) { //read ttys0
      temps = time(&temps);
      printf("\r\n%s\r\n",ctime(&temps));
      cb=write(STDOUT_FILENO,buf,cb); //write ttys0
   }

   //named pipe
   mknod("/dev/pipe0",S_IFIFO,0);

   // pipe
   pipe(p);

   printf("pipe ok\r\n");
   //
   switch(vfork()) {
   case 0:   //son
   {
      close(STDIN_FILENO);
      dup(p[0]);
      close(p[0]);
      close(p[1]);

      execve("/usr/sbin/tst/test2",0,0);
      printf("error!!!");
   }
   break;

   default:   //father
      close(STDOUT_FILENO);
      dup(p[1]);
      close(p[1]);
      close(p[0]);

      //printf("open /dev/pipe0 O_RDONLY...\n");//warning! dead lock(normal) with shell and named pipe
      np = open("/dev/pipe0",O_RDONLY,0);
      //printf("open /dev/pipe0 O_RDONLY ok\n");
      //printf("open /dev/pipe0 O_RDONLY...\n");//warning! dead lock(normal) with shell and named pipe
      //printf("open /dev/pipe0 O_RDONLY ok\n");

      while ((cb=read(STDIN_FILENO,buf,5))!=0) {  //read ttys0

         write(STDOUT_FILENO,buf,cb);   //write in pipe similar to printf (pipe == stdout)

         //read from named pipe
         cb=read(np,buf,cb);
         for(i=0; i<cb; i++) {
            //printf("%c",buf[i]);
            if(buf[i]=='q') {
               //close(STDOUT_FILENO); //close pipe
               kill(3,SIGUSR1);
            }
         }
      }
      break;
   }


   close(fd0);
   close(fd1);

   return 0;
}



/*===========================================
End of Sourcetest1.c
=============================================*/
