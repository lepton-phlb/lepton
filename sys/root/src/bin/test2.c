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

#include "kernel/core/kernel.h"
#include "kernel/core/signal.h"
#include "kernel/core/libstd.h"
#include "kernel/core/devio.h"
#include "kernel/core/wait.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/time.h"

#include "lib/libc/stdio/stdio.h"
#include "lib/libc/termios/termios.h"

#include "kernel/core/net/socket.h"

#include "lib/pthread/pthread.h"
#include "lib/librt/mqueue.h"
#include "lib/librt/semaphore.h"





/*
#define LWIP_COMPAT_SOCKETS 1
#include "lwip/sockets.h"*/
//#include "memstruc.i"
//extern char PTR_ATTRIBUTE *_last_heap_object;
//for cygwin unix test
/*
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <io.h>
#include <stdio.h>
*/


/*===========================================
Global Declaration
=============================================*/

//
#define STOPSERVER_TIMEOUT          10

#define ERR_NO_SOCKET		-1
#define ERR_BIND_FAILED		-2
#define ERR_LISTEN_FAILED	-3
#define ERR_ACCEPT_FAILED	-4
#define ERR_CONNECT_FAILED	-5
#define ERR_NO_HOSTNAME		-6


#define TCP_PORT_SERVER    2000
#define TCP_PORT_CLIENT    2000

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
void hand(int sig){
   int i=0;

   printf("sighandler(%d) %d\n",getpid(),sig);

   i++;

}
/*-------------------------------------------
| Name:socket_client_tcp
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
#define  XML_REQUEST           "\
<?xml version=\"1.0\"?>\
<releve-cpl version=\"1.0\">\
	<telerel-rq ident=\"040000000001\">\
		<read-rq>\
			<exp-cmd>\
				<dia-stat/>\
			</exp-cmd>\
		</read-rq>\
	</telerel-rq>\
</releve-cpl>"

#define MAX_LEN      512

char str[MAX_LEN];     /* string to send */

void socket_client_tcp(void)
{
  int sock;                   /* socket descriptor */

  struct sockaddr_in mc_addr; /* socket address structure */
  int    send_len;               /* length of string to send */
  char   mc_addr_str[32];          /*  IP address */
  unsigned short mc_port;     /*  port */
  int    res;

   strcpy (mc_addr_str, "192.168.200.1");
   mc_port     = 80;

  /* create a socket for sending to the multicast address */
  if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    printf("socket() failed");
    exit(1);
  }
    
  /* construct a tcpip address structure */
  memset(&mc_addr, 0, sizeof(mc_addr));
  mc_addr.sin_family      = AF_INET;
  mc_addr.sin_addr.s_addr = inet_addr(mc_addr_str);
  mc_addr.sin_port        = htons(mc_port);

  res =sizeof(struct sockaddr_in);
  // Connection to the server
  res = connect ( sock,
                 (struct sockaddr *) &mc_addr,
                 sizeof(struct sockaddr_in) );
   if (res == -1) {
      close(sock); 
      return;
      // Error: connection with server is aborted!!!! ADU
   }
   res = connect ( sock,
                 (struct sockaddr *) &mc_addr,
                 sizeof(struct sockaddr_in) );
   if (res == -1) {
      close(sock); 
      return;
      // Error: connection with server is aborted!!!! ADU
   }
  /* clear send buffer */
  memset(str, 0, sizeof(str)); 
  strcpy (str, XML_REQUEST);
  send_len = strlen(str);

    /* send string to multicast address */
 res = sendto(sock, str, send_len, 0, 
         (struct sockaddr *) &mc_addr, 
         sizeof(mc_addr));

    if (res != send_len) {
      printf("sendto() sent incorrect number of bytes");
      exit(1);
    }

    /* clear send buffer */
    memset(str, 0, sizeof(str));
}


/*-------------------------------------------
| Name:socket_test
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int socket_test(void){
   int sock;
   int nRc;
   struct sockaddr_in addr;
   char buffer[100];
   int newSock;
   struct sockaddr acc_sin;
   int sin_len=sizeof(struct sockaddr);

   sock=socket(PF_INET,SOCK_STREAM,0);
   if(sock<0)
   {
		return(ERR_NO_SOCKET);
   }

   memset(&addr,0,sizeof(addr));
   addr.sin_family=PF_INET;
   addr.sin_port=htons(TCP_PORT_SERVER);
   //addr.sin_addr.s_addr=INADDR_ANY;
   //addr.sin_addr.s_addr=inet_addr("14.2.211.157");
   addr.sin_addr.s_addr=inet_addr("128.1.1.2");


   nRc=bind(sock,(struct sockaddr*)&addr,sizeof(struct sockaddr));
   if(nRc!=0){
     close(sock);
	  return(ERR_BIND_FAILED);
	}

   nRc=listen(sock,/*SOMAXCONN*/0);
   if(nRc!=0){
		close(sock);
		return(ERR_LISTEN_FAILED);
	}


   //
   newSock = accept( sock,(struct sockaddr*) &acc_sin,(int*) &sin_len);
   for(;;){
      int cb;
      int i=0;
      if((cb=recv(newSock,buffer,sizeof(buffer)-1,0))<=0)break;
      for(i=0;i<cb;i++){
         //printf("buf[%d]=%c\n",i,buffer[i]);
      }
      send(newSock,buffer,cb,0);
   }

   pause();

   return 0;
}

/*--------------------------------------------
| Name:        lwip_socket_test
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
/*
char sock_buf[400];
static void
lwip_socket_test(void *arg)
{
	int sock,s;
	int len;
	struct sockaddr_in addr,rem;
	sock = socket(AF_INET,SOCK_STREAM,0);	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(7);
	addr.sin_addr.s_addr = INADDR_ANY;

   printf("socket app started\n");

	bind(sock,(struct sockaddr *)&addr,sizeof(addr));
			
	listen(sock,5);
	while(1) {	
		len = sizeof(rem);
		s = accept(sock,(struct sockaddr*)&rem,&len);
		while((len = read(s,sock_buf,400)) > 0)
			write(s,sock_buf,len);
		close(s);
	}	
}
*/
/*-------------------------------------------
| Name:basic_test
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void basic_test(void){

   pid_t pid=getpid();
   int i=0;

   /*
   strcpy(path,"/bin/sh");
   pid=execve(path,"shell2","env");

   if(pid!=ENOMEM)
      printf("exeve pid=%d\n",pid);
*/
/*
   pid=vfork();
   if(pid>0){
      while(1){
         printf("Father(%d):%d\n",_sys_getpid(),i++);
         #ifdef WIN32
            Sleep(1000);
         #endif
      }
   }else if(pid==0){

      pid_t pid;

      #ifdef WIN32
         Sleep(5000);
      #endif

      strcpy(path,"/bin/sh");

      pid=execve(path,"shell2","env");
      if(pid!=ENOMEM)
         printf("exeve(%d) pid=%d\n",_sys_getpid(),pid);
      else
         printf("exeve Error(%d):%d\n",_sys_getpid(),i++);

      while(1){
         if(i>5) break;
         printf("Son(%d):%d\n",_sys_getpid(),i++);
         #ifdef WIN32
            Sleep(1000);
         #endif
      }
   }
   else
      printf("vfork Error(%d):%d\n",_sys_getpid(),i++);
*/


   if(!(pid=vfork())){
      i++;
      printf("son(%d):%d\r\n",getpid(),++i);
      exit(0);
   }
   printf("father(%d):%d\r\n",getpid(),++i);

 /*
   pid=vfork();
   if(pid!=0){
      i++;
      printf("Father(%d):%d\r\n",getpid(),++i);

   }else{
      i++;

      printf("Son(%d):%d\r\n",getpid(),++i);
      //Sleep(1000);
      pid=vfork();
      if(pid!=0){
         printf("Father(%d):%d\r\n",getpid(),++i);
         //Sleep(1000);
      }else{
         printf("Son(%d):%d\r\n",getpid(),++i);
         //Sleep(1000);
      }

   }
*/
   exit(0);

}

/*-------------------------------------------
| Name:sh_ls
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int sh_ls(char* path){

   return 0;
}

/*-------------------------------------------
| Name:pipe_test
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int pipe_test(int argc,char* argv[]){
   char path[16]={0};
   char buf[10]={0};
   int i=0;
   int cb;
   int fd0=STDIN_FILENO;
   int fd1=STDOUT_FILENO;
   char c=0;
   int np;

   pid_t pid=getpid();

   struct sigaction sa;

   printf("%s(%d) started ok. argv[0]=%s\n",argv[0],pid,argv[0]);

   sa.sa_handler=hand;

   sigaction(SIGUSR1,&sa,NULL);

   kill(pid,SIGUSR1);

   printf("open /dev/pipe0 O_WRONLY...\r\n");
   np = open("/dev/pipe0",O_WRONLY,0);
   printf("open /dev/pipe0 O_WRONLY ok\r\n");

   while (1){

      cb=read(STDIN_FILENO,buf,5);//read pipe
      if(cb<=0){
         printf("\r\n no writer\r\n");
#ifdef WIN32
         Sleep(10);
#endif
      }

      for(i=0;i<cb;i++){
         //printf("%c",buf[i]);//write ttys0
         if(buf[i]=='q'){
            //close(STDIN_FILENO); //close pipe: SIGPIPE to writer
         }
      }

      if(cb>0)
       write(STDOUT_FILENO,buf,cb);//write ttys0

      //write to named pipe
      write(np,buf,cb);
   }



   close(fd0);
   close(fd1);

   //basic_test();


   printf("exit(%d)\n",getpid());
   return 0;
}

/*-------------------------------------------
| Name:file_test
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void file_test(){
   char buf[80]={0};//64+16
   int i =0;
   int j =0;
   int cb = 0;
   int fd = -1;

   printf("fill buffer\r\n");
   //
   for(i = 0; i<sizeof(buf); i++){
      buf[i]='a'+ (j++);
      printf("%c",buf[i]);

      if(buf[i]=='z'){
         j=0;
         printf("\r\n");
      }


   }

   //
   printf("\r\n");
   //write
   printf("write buffer\r\n");
   fd=open("/etc/ftst",O_CREAT|O_WRONLY,0);
   if(fd<0){
      printf("test error\r\n");
      return;
   }

   lseek(fd,(off_t)0,SEEK_SET);

   cb  = write(fd,buf,sizeof(buf));
   printf("write %d bytes ok\r\n",cb);

   lseek(fd,(off_t)0,SEEK_SET);
   cb  = read(fd,buf,sizeof(buf));
   printf("read %d bytes ok\r\n",cb);
   for(i = 0; i<sizeof(buf); i++){
      printf("%c",buf[i]);

      if(buf[i]=='z'){
         printf("\r\n");
      }

   }

   close(fd);

   printf("reopen\r\n",cb);
   fd=open("/etc/ftst",O_RDONLY,0);
   if(fd<0){
      printf("test error\r\n");
      return;
   }

   lseek(fd,(off_t)0,SEEK_SET);

   //reset buffer
   memset(buf,0,sizeof(buf));
   printf("reset buffer\r\n",cb);
   j=0;
   for(i = 0; i<sizeof(buf); i++){
      printf("%c",buf[i]+'0');

      if(!((++j)%26)){
         j=0;
         printf("\r\n");
      }

   }

   //read
   cb  = read(fd,buf,sizeof(buf));
   printf("read %d bytes ok\r\n",cb);
   for(i = 0; i<sizeof(buf); i++){
      printf("%c",buf[i]);

      if(buf[i]=='z'){
         printf("\r\n");
      }

   }

   printf("\r\n");
   printf("\r\n");

}

/*-------------------------------------------
| Name:test_waitpid
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void test_waitpid(void){

   pid_t pid = 0;
   int status;
   printf("father:pid[%d]\r\n",getpid());

   if(!(pid=vfork())){
      printf("son:pid[%d]\r\n",getpid());
      //Sleep(3000);
      setpgid(0,0);
      printf("son:pid[%d] setpgid()\r\n",getpid());
      //Sleep(3000);
      printf("son:pid[%d] exit()\r\n",getpid());
      exit(0);
   }

   while((pid=waitpid((pid_t)0,&status,0))>0){
      printf(":[%d] done(%d)\r\n",pid,status);
   };

   if(pid<0)
      printf(":error\r\n");

}

/*-------------------------------------------
| Name:fread_test
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void fread_test(void){
   char buf[10]={0};
   int size = 3;
   int i=0;

   //size = fread( buf, (size_t)1, (size_t)3, stdin );
/*
   buf[0] = fgetc(stdin);
   buf[0] = fgetc(stdin);
   buf[0] = fgetc(stdin);

   while(size>0){
      size = fread( buf, (size_t)1, (size_t)9, stdin );
      for(i=0;i<size;i++){
         printf("%.2x;",buf[i]);
      }
      printf("\r\n");
   }
   */
}

/*-------------------------------------------
| Name:fread_test
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void write_test(void){
   char buf[64]={0};
   int i=0;
   int cb;
   int fd;
   fd=open("/etc/f1",O_CREAT|O_WRONLY,0);
   for(i=0;i<2;i++){
      /*if(i==546)
         printf("bug");*/
      cb=write(fd,buf,sizeof(buf));
   }
   close(fd);
}

/*--------------------------------------------
| Name:        lock_test
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void lock_test(void){
   char buf[32];
   struct flock flock1;
   pid_t pid = getpid();
   int fd = open("/usr/bin/tst/tstkb",O_RDONLY,0);
   
   //
   fcntl(fd,F_GETLK,&flock1);
   if(flock1.l_type!=F_UNLCK){
      sprintf(buf,"pid=%d lock by pid=%d\r\n",pid,flock1.l_pid);
      write(1,buf,strlen(buf));
   }

   //
   sprintf(buf,"pid=%d lock...\r\n",pid);
   write(1,buf,strlen(buf));
   //
   flock1.l_type = F_WRLCK;
   fcntl(fd,F_SETLKW,&flock1);
   //
   sprintf(buf,"pid=%d lock ok\r\n",pid);
   write(1,buf,strlen(buf));

   //wait 10 s
   usleep(10000000);

   //
   /*
   sprintf(buf,"pid=%d unlock...\r\n",pid);
   write(1,buf,strlen(buf));

   flock1.l_type = F_UNLCK;
   fcntl(fd,F_SETLK,&flock1);
   */

   sprintf(buf,"pid=%d close file\r\n",pid);
   write(1,buf,strlen(buf));
   close(fd);

   //wait 10 s
   usleep(10000000);

   sprintf(buf,"pid=%d quit ok\r\n",pid);
   write(1,buf,strlen(buf));
}


/*--------------------------------------------
| Name:        test_eth
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void test_eth(void){
   const static unsigned char _dummy_tcp[64] = {0x00,0x02,0xF2,0x00,0xAA,0x55,0x00,0x02,0xF2,0x00,0x03,0x7B,0x08,0x00,0x44,0x45,
                0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x60,0x61,
                0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
                0x78,0x79,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x90,0x91,0x92,0x93};

   char buf[512]={0};
   int fd;
   int cb;

   fd = open("/dev/eth0",O_RDWR,0);
   for(;;){
      printf("wait packet...");
      cb=read(fd,buf,sizeof(buf));
      if(cb<0)
         break;
      //printf("ethernet frame size cb=%d\r\n",cb);
      /*for(i=0;i<cb;i++)
         printf("byte[%d]=%X\r\n",i,buf[i]);*/
      //printf("\r\n");
      printf("ok len=%d\r\n",cb);

      write(fd,_dummy_tcp,sizeof(_dummy_tcp));
   }
   close(fd);
}

/*--------------------------------------------
| Name:        test_alarm
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static int alarm_fd;
int alarm_cpt=0;

void alarm_handler(int sig){
   char buf[48]={0};
   int i=0;
   int cb=0;
   
   alarm(1);
   printf("alarm sighandler(%d) %d\r\n",getpid(),alarm_cpt++);
      
   //cb=write(alarm_fd,buf,sizeof(buf));
   //if(cb<sizeof(buf))
      //exit(0);
}

void alarm_test(){
   struct sigaction sa;
   alarm_fd=open("/etc/f1",O_CREAT|O_WRONLY,0);
   
   sa.sa_handler=alarm_handler;

   sigaction(SIGALRM,&sa,NULL);
   alarm(1);
   for(;;)
      pause();
}

/*--------------------------------------------
| Name:        test_cal_asyc3
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void test_cal_asyc3(void){
#define EEPROM_ADDRESS 0x77F0
   char buf[32]={0};
   int fd_src;
   int fd_dst;

   //remove("/etc/calib");
   //test
   fd_src=open("/dev/hd/hdb",O_RDONLY,0);//eeprom
   fd_dst=open("/etc/calib",O_CREAT|O_WRONLY,0);//fichier de calibration

   lseek(fd_src,EEPROM_ADDRESS,SEEK_SET);
   read(fd_src,buf,sizeof(buf));
   lseek(fd_dst,0,SEEK_SET);
   write(fd_dst,buf,sizeof(buf));

   close(fd_dst);
   close(fd_src); 

}

/*--------------------------------------------
| Name:        test_modbus_eth
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void test_modbus_eth(void){
   char buf[32];
   static char eth_buf[1500];
   char* p_eth_buf = eth_buf;

   int cb;
   unsigned int i, l;
   static unsigned int cpt=0;
   while(1){
      //printf("%03d:",cpt++);
      cb = read(0,buf,sizeof(buf));
      for(i=0;i<(unsigned)cb;i++){
         sprintf(p_eth_buf,"0x%02x ",buf[i]);
         l = strlen(p_eth_buf);
         p_eth_buf+=l;
         cpt+=l;
      }
      
      sprintf(p_eth_buf,"\r\n");
      l = strlen(p_eth_buf);
      p_eth_buf+=l;
      cpt+=l;
      
      //
      if(cpt<450)
         continue;
      //send
      l=cpt;
      cpt=0;
      while(cpt<l){
         cb = write(1,eth_buf+cpt,l-cpt);
         cpt+=cb;
      }

      //
      p_eth_buf = eth_buf;
      cpt=0;
     
   }
}

/*--------------------------------------------
| Name:        test_modbus
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void test_modbus(void){
   char buf[64];
   int cb;
   int i ;
   unsigned char cpt=0;
   struct termios _termios;

   tcgetattr(STDIN_FILENO,&_termios);
   _termios.c_cc[VTIME]=20;//read timeout 20*0.1s=2 seconds
   tcsetattr(STDIN_FILENO,TCSANOW,&_termios);

   while(1){
      printf("%03d:",cpt++);
      
      if((cb = read(0,buf,sizeof(buf)))<0){
         printf("crc error\r\n");
         continue;
      }
      if(!cb)
         printf("timeout\r\n");

      for(i=0;i<cb;i++)
         printf("0x%02x ",buf[i]);
      printf("\r\n");
   }
}

/*--------------------------------------------
| Name:        test_echo
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void test_rs_asyc3_cal(void){
   int cb;
   int w;
   char buffer[128];

   for(;;){
      cb=0;
      printf("wait...");
      while( (cb+=read(0,buffer+cb,sizeof(buffer)-cb))<(sizeof(buffer)-2));
      printf("ok\r\n");
      printf("test_cal_asyc3...");
      test_cal_asyc3();
      printf("ok\r\n");

      w=0;
      while(w<cb)
         w+=write(1,buffer+w,cb-w);
   }
}

/*--------------------------------------------
| Name:        test_pthread1
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void* my_pthread_routine(void* arg){
   int i=0;
   for(;;){
      if(i++==10){
         //pthread_exit(0);
         pthread_cancel(pthread_self());
      }
      printf_r("thread %ul\r\n",pthread_self());
      usleep(1000000);
   }

}

void test_pthread1(void){
   int i=0;
   pthread_t my_pthread1;
   pthread_t my_pthread2;
   pthread_attr_t thread_attr;
   
   //
   thread_attr.stacksize = 2048;
   thread_attr.stackaddr = NULL;
   thread_attr.priority  = 100;
   thread_attr.timeslice = 1;

   pthread_create(&my_pthread1, &thread_attr,my_pthread_routine,NULL);
   printf("thread 1 %ul\r\n",my_pthread1);
   pthread_create(&my_pthread2, &thread_attr,my_pthread_routine,NULL);
   printf("thread 2 %ul\r\n",my_pthread2);

   for(;;){
      if(i++==2)
         exit(0);
      printf("main %u\r\n",pthread_self());
      usleep(1000000);
   }

}

/*--------------------------------------------
| Name:        test_pthread_mutex1
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static volatile int shared_var = 0;

void* my_pthread_mutex1_routine(void* arg){
   int i=0;
   
   pthread_mutex_t* mutex=(pthread_mutex_t*)arg;

   for(;;){

      if(i++==20){
         //pthread_exit(0);
         pthread_cancel(pthread_self());
      }
      //
      if(pthread_mutex_lock(mutex)<0)
         return (void*)0;
      shared_var++;
      printf_r("thread %ul var=%d\r\n",pthread_self(),shared_var);
      if(pthread_mutex_unlock(mutex)<0)
         return (void*)0;
      //
      //usleep(500000);
      
   }
}

void test_pthread_mutex1(void){
   int i=0;
   pthread_t my_pthread1;
   pthread_attr_t thread_attr;

   //
   pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

   //
   pthread_mutex_init(&mutex,(pthread_mutexattr_t*)0);
   
   //
   thread_attr.stacksize = 2048;
   thread_attr.stackaddr = NULL;
   thread_attr.priority  = 100;
   thread_attr.timeslice = 1;

   pthread_create(&my_pthread1, &thread_attr,my_pthread_mutex1_routine,&mutex);
   printf("thread 1 %ul\r\n",my_pthread1);

   for(;;){

      if(i++==20)
         exit(0);
      //
      if(pthread_mutex_lock(&mutex)<0)
         return;
      shared_var++;
      printf("main %ul var=%d\r\n",pthread_self(),shared_var);
      //
      //usleep(1000000);
      //
      if(pthread_mutex_unlock(&mutex)<0)
         return;

      //usleep(2);

   }

}

/*--------------------------------------------
| Name:        test_pthread_cond1
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* my_pthread_cond1_routine(void* arg){
   int i=0;
   
   pthread_mutex_t* mutex=(pthread_mutex_t*)arg;

   for(;;){

      //
      printf_r("thread[%ul]  wait mutex...",pthread_self());
      if(pthread_mutex_lock(mutex)<0)
         return (void*)0;
      shared_var++;
      printf_r("thread[%ul] var=%d\r\n",pthread_self(),shared_var);

      //usleep(2000000);
      pthread_cond_signal(&cond);

      if(pthread_mutex_unlock(mutex)<0)
         return (void*)0;
      //
      usleep(100000);
      
   }
}

void test_pthread_cond1(void){
   int i=0;
   pthread_t my_pthread1;
   pthread_attr_t thread_attr;

   //
   pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

   //
   shared_var=0;
   //
   pthread_mutex_init(&mutex,(pthread_mutexattr_t*)0);
   pthread_cond_init(&cond,(pthread_condattr_t*)0);
   
   //
   thread_attr.stacksize = 2048;
   thread_attr.stackaddr = NULL;
   thread_attr.priority  = 100;
   thread_attr.timeslice = 1;

   //if(pthread_mutex_lock(&mutex)<0)
   //return;
   

   pthread_create(&my_pthread1, &thread_attr,my_pthread_cond1_routine,&mutex);
   printf("thread created 1 [%ul]\r\n",my_pthread1);

   //usleep(5000000);

   //
   //
   for(;;){
      if(shared_var==25){
         pthread_mutex_destroy(&mutex);
         exit(0);
      }
      //
      pthread_mutex_lock(&mutex);

      if(pthread_cond_wait(&cond,&mutex)<0)
         return;
      printf("main[%ul] var=%d\r\n",pthread_self(),shared_var);
      //
      pthread_mutex_unlock(&mutex);
      //
      //usleep(1000000);
      //
   }

}


/*--------------------------------------------
| Name:        timer_test
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/

void timer_handler(int sig){
   printf("timer sighandler(%d) %d\r\n",getpid(),alarm_cpt++);
   if(alarm_cpt==2)
      exit(0);
}



void timer_test(void){
   struct sigaction sa={0};
   timer_t timerid;
   struct itimerspec itimer;

   sa.sa_handler=timer_handler;
   sigaction(SIGALRM,&sa,NULL);

   timer_create(CLOCK_REALTIME,(struct sigevent*)0,&timerid);

   itimer.it_value.tv_sec=1;
   itimer.it_value.tv_nsec=1000000000;//1s
   itimer.it_interval.tv_sec=2;//1s
   itimer.it_interval.tv_nsec=1000000000;//1s

   timer_settime(&timerid,CLOCK_REALTIME, &itimer,(struct itimerspec *)0);

   for(;;){
      pause();
   }

}

void timer_test_rt(void){
   struct sigaction sa={0};
   timer_t timerid;
   struct sigevent sigevent={0};
   struct siginfo  info={0};
   struct itimerspec itimer;

   sigset_t set;
   
   sigevent.sigev_signo=SIGRTMIN;
   timer_create(CLOCK_REALTIME,&sigevent,&timerid);

   sa.sa_flags=SA_SIGINFO;
   sa.sa_handler=timer_handler;
   sigaction( sigevent.sigev_signo,&sa,NULL);

   sigemptyset(&set);
   sigaddset(&set,sigevent.sigev_signo);
   sigprocmask(SIG_SETMASK,&set,(sigset_t*)0);

   itimer.it_value.tv_sec=1;
   itimer.it_value.tv_nsec=1000000000;//1s
   itimer.it_interval.tv_sec=2;//1s
   itimer.it_interval.tv_nsec=1000000000;//1s
   timer_settime(&timerid,CLOCK_REALTIME, &itimer,(struct itimerspec *)0);
   //
   usleep(30000000);
   //
   for(;;){
      sigwaitinfo(&set,&info);
      printf("si_code:%d si_signo=%d\r\n",info.si_code,info.si_signo);
   }

}

/*--------------------------------------------
| Name:        test_mqueue
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void* pthread_mq_reader_routine(void* arg){

   int fd_r;
   char buf[12];
   int prio=0;

   fd_r=mq_open("/var/mq0",O_RDONLY);

   //usleep(4000000);
   for(;;){
      int cb;

      //cb=read(fd_r,buf,sizeof(buf));
      cb=mq_receive(fd_r,buf,sizeof(buf),&prio);
      printf_r("mqr: %s [%d]\r\n",buf,prio);
      usleep(2000000);
   }
   
   return (void*)0;
}

void test_mqueue(void){
   int fd_w;
   char buf[12];
   unsigned char i=0;

   int prio=1;

   struct mq_attr attr;
   pthread_t      pthread_mq_reader;
   pthread_attr_t thread_attr;


   attr.mq_maxmsg=10;
   attr.mq_msgsize=sizeof(buf);

   fd_w=mq_open("/var/mq0",O_CREAT|O_WRONLY,0,&attr);

   //
   thread_attr.stacksize = 2048;
   thread_attr.stackaddr = NULL;
   thread_attr.priority  = 100;
   thread_attr.timeslice = 1;
   
   pthread_create(&pthread_mq_reader, &thread_attr,pthread_mq_reader_routine,(void*)0);
   printf("thread reader created [%ul]\r\n",pthread_mq_reader);


   for(;;){
      int d;
      i++;
      d=i;
      sprintf(buf,"coucou %3d",d);
      //write(fd_w,buf,strlen(buf)+1);
      mq_send(fd_w,buf,strlen(buf)+1,prio);
      printf_r("mqw: %s [%d]\r\n",buf,prio);
      //usleep(2000000);
   }

}

/*--------------------------------------------
| Name:        test_sem
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static int i_sem_test;
void* pthread_sem_routine(void* arg){
   sem_t* p_sem = (sem_t*)(arg);

   for(;;){
      sem_wait(p_sem);
      i_sem_test++;
      printf_r("thread: i=%d\r\n",i_sem_test);
      usleep(1000000);
      sem_post(p_sem);
      //
      usleep(1000);//for context switch
   }

}

void test_sem(void){
   pthread_t my_pthread;
   pthread_attr_t thread_attr;
   //
   sem_t sem;
   //
   sem_init(&sem,0,1);
   //
   thread_attr.stacksize = 2048;
   thread_attr.stackaddr = NULL;
   thread_attr.priority  = 100;
   thread_attr.timeslice = 1;
   //
   i_sem_test=0;
   //
   pthread_create(&my_pthread, &thread_attr,pthread_sem_routine,&sem);
   printf("main: thread  %ul\r\n",my_pthread);

   for(;;){
      sem_wait(&sem);
      i_sem_test++;
      printf_r("main: i=%d\r\n",i_sem_test);
      usleep(1000000);
      sem_post(&sem);
      //
      usleep(1000);//for context switch
   }

}

/*--------------------------------------------
| Name:        test_stdio
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void test_stdio(void){
   char  tokenstring[] = "15 12 14...";
   char  s[81];
   char  c;
   int   i;
   float fp;

   double df=1.5678e-6;
   float f =(float)1.5678e-6;

   printf("test stdio:%d\r\n",2);

   printf("%e\r\n",df);
   printf("%e\r\n",f);

   /* Input various data from tokenstring: */
   sscanf( tokenstring, "%s", s );
   sscanf( tokenstring, "%c", &c );
   sscanf( tokenstring, "%d", &i );
   sscanf( tokenstring, "%f", &fp );

   /* Output the data read */
   printf( "string    = %s\r\n", s );
   printf( "character = %c\r\n", c );
   printf( "integer:  = %d\r\n", i );
   printf( "float:    = %f\r\n", fp );
}

/*--------------------------------------------
| Name:        tst_sdcard
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/

/*
unsigned short mdbfs_profiler_counter=0;
float mdbfs_profiler_time=0;
float mdbfs_profiler_time_max=0;
float mdbfs_profiler_time_min=0;
volatile unsigned short _start_counter;
volatile unsigned short _stop_counter;

#define __mdbfs_profiler_start(){\
     mdbfs_profiler_counter=(*AT91C_TC1_CV);\
   }

      

#define __mdbfs_profiler_stop(){\
    unsigned short __counter__;\
    __counter__= *AT91C_TC1_CV;\
   if(__counter__ > mdbfs_profiler_counter )\
      mdbfs_profiler_counter=(__counter__)-mdbfs_profiler_counter;\
   else\
      mdbfs_profiler_counter=(0xffff-mdbfs_profiler_counter)+__counter__;\
   mdbfs_profiler_time= ( (float)(mdbfs_profiler_counter) * (float)PROFILER_PERIOD );\
   if(!mdbfs_profiler_time_max && !mdbfs_profiler_time_max){\
         mdbfs_profiler_time_max=mdbfs_profiler_time;\
         mdbfs_profiler_time_min=mdbfs_profiler_time;\
   }\
   if(mdbfs_profiler_time>mdbfs_profiler_time_max)mdbfs_profiler_time_max=mdbfs_profiler_time;\
   if(mdbfs_profiler_time<mdbfs_profiler_time_min)mdbfs_profiler_time_min=mdbfs_profiler_time;\
}

void tst_fs_sdcard(void){
   int fd;
   int cb;
   char buf[512];

   //
   __mdbfs_profiler_start();
   fd=open("/mnt/f1",O_CREAT|O_RDWR,0);
   __mdbfs_profiler_stop();
  
   //
   lseek(fd,0,SEEK_SET);

   //
   __mdbfs_profiler_start();
   if((cb=write(fd,buf,sizeof(buf)))<sizeof(buf))
      return ;
   __mdbfs_profiler_stop();
   
   //
   lseek(fd,0,SEEK_SET);
   
   //
   __mdbfs_profiler_start();
   if((cb=read(fd,buf,sizeof(buf)))<sizeof(buf))
      return;
   __mdbfs_profiler_stop();
   
   //
   close(fd);
}
*/


/*--------------------------------------------
| Name:        test_benchmark_write_fich
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
/*
uchar8_t g_write_buffer [512];   // 120 voies*4 octet d'entete => 480 octets
void test_benchmark_write_fich(char *nom){
   // clock_gettime variable
   struct   timespec tp_deb,tp_fin;
   uint32_t time_memo_deb,time_memo_fin;
      off_t pos;
   int fd,cpt;      
   char path[32];
   // Enable peripheral clock for selected PIOA   / PA28
   *AT91C_PMC_PCER     = 1 << AT91C_ID_PIOA; // GPIO clock enable  
   (*AT91C_PIOA_PER)   = 1 << 28;            // PA28
   (*AT91C_PIOA_OER)   = 1 << 28;            // Ouput
   
   sprintf(path,"/mnt/%s",nom);
   //  fd=open("/mnt/32k.txt",O_WRONLY|O_RDONLY,0);
  fd=open(path,O_WRONLY|O_RDONLY,0);
  
  if (fd != -1)
  {   
    // pos = lseek(fd,0,SEEK_END);    
    // lseek(fd,10000,SEEK_CUR); 
    

    for(cpt=0;cpt < 1000;cpt++)
    {
     lseek(fd,0,SEEK_SET); 

//    clock_gettime(CLOCK_MONOTONIC, &tp_deb);  // initial time for top 500ms 
//    time_memo_deb           = tp_deb.tv_sec*1000L + tp_deb.tv_nsec/1000000L;  
     
    ((*AT91C_PIOA_SODR) = (1<<28));           // PA28_1    
    write(fd,g_write_buffer,sizeof(g_write_buffer));        
    ((*AT91C_PIOA_CODR) = (1<<28));     // PA28_0

//    clock_gettime(CLOCK_MONOTONIC, &tp_fin);  // initial time for top 500ms 
//    time_memo_fin           = tp_fin.tv_sec*1000L + tp_fin.tv_nsec/1000000L;  
    
    // usleep ( 100); // 100 us
    }
    
    //  printf("time = %d",time_memo_fin-time_memo_deb);   
  }
   
  close (fd);
}
*/

/*-------------------------------------------
| Name:sh_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/

char const buf_w[]="test2 w\r\n";
char const buf_r[]="test2 r\r\n";

//int main(int argc,char* argv[]){

int test2_main(int argc,char* argv[]){
   char buf[50]={0};
   int cb=0;
   //int fd;
   //char* p;
   //unsigned int count;
   unsigned int i=0;

   cb=sizeof(process_t);

   //MEASR_PRM_VOLT_PERIOD_ZERO prm={0};
   /*char mode=0;
   int len;
   int i=0;
   pid_t pid;
   int status;*/
   
   /*uint16_t coeff_pince;

   #if defined (__IAR_SYSTEMS_ICC)
   extern uint16_t _get_sp (void);
   fprintf (stderr,"test2 sp=%X\r\n",_get_sp ());
   #endif*/

   //write(1,"echo ctrl-x to exit\r\n",strlen("echo ctrl-x to exit\r\n"));

   //test_waitpid();
   //basic_test();
   //socket_test();
   //socket_client_tcp();
   //file_test();
   // test_eth();
   //write_test();
   //alarm_test();
   //timer_test();
   //timer_test_rt();
   //test_pthread1();
   //test_pthread_mutex1();
   //test_pthread_cond1();
   //lock_test();
   //test_mqueue();
   test_stdio();
   //test_sem();

   //test_rs_asyc3_cal();
   //test_modbus();
   //tst_fs_sdcard();
   //test_pthread1();

   //lock_test();
   //test_benchmark_write_fich(argv[1]);
   return 0;
   

   /*while(1){
      unsigned char c = (unsigned char)getc(stdin);
      printf("%02x\n\r",(unsigned int)c);
   }*/

   /*printf("v.intgr1=%f;\r\n",40.0);
   printf("v.intgr2=%f;\r\n",10.0);
   printf("i.intgr1=%f;\r\n",0.15);
   printf("i.intgr2=%f;\r\n",0.22);*/
/*
   if(argv[1] && argv[1][0]=='r'){
      fd=open("/var/rvolt",O_RDONLY,0);
      mode='r';
      p=(char*)buf_r;
      len = strlen(p);
   }else if(argv[1] && argv[1][0]=='w'){
      fd=open("/var/rvolt",O_WRONLY,0);
      mode='w';
      p=(char*)buf_w;
      len = strlen(p);
   }
   if(fd<0)
      return -1;

   for(;;){
      //TRACEDEBUG(PLB_bug_test2_in);
      //msr_adj_readSt (_ADJ_CLAMP_T,&coeff_pince,1);   // init StAdjust with recent value
      //TRACEDEBUG(PLB_bug_test2_out);
      if(mode=='r'){
         write(1,"r%",2);
         lseek(fd,0,SEEK_SET);
         write(1,"r0",2);
         read(fd,&prm,sizeof(prm));
         write(1,"r1",2);
         if(!(++i%10)){
            i=0;
            write(1,"r2",2);
            //if((long)(_last_heap_object)!=12750)
            //   write(1,"!!",2);
            write(1,"wtp0",4);
            if(pid>0)
               waitpid(pid,&status,0);
            write(1,"wtp1",4);
            if(!(pid=vfork())){
               write(1,"r3",2);
               execl("od","od","-hx","/var/rvolt",(char*)0);
               write(1,"r4",2);
               exit(2);
            }
         }
         if(pid<1)
            write(1,"$$",2);
         write(1,"r5",2);
         waitpid(0,&status,WNOHANG);
         write(1,"r6",2);
      }else if(mode=='w'){
         lseek(fd,0,SEEK_SET);
         write(fd,&prm,sizeof(prm));
      }
      write(1,p,len);
      usleep(400000);//0.4 sec
      
   }*/

   //fread_test();

   //timer test
/*
   TABSR.1=0; //TA1 stop flag
   TA1MR=0x80; // 1000 0000
   UDF|=0x02; //TA1 up count
   TA1 =32000;
   TABSR.1=1; //TA1 start flag
   
   //count = TA1;
   usleep(100000);//100 msec
   TABSR.1=0; //TA1 stop flag
   count = TA1;

   printf("count=%u\r\n",count);
   */

   return 0;
}


/*===========================================
End of Sourcetest2.c
=============================================*/
