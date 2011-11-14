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
#include "kernel/core/signal.h"
#include "kernel/core/wait.h"
#include "kernel/core/stat.h"
#include "kernel/core/libstd.h"
#include "kernel/core/devio.h"
#include "lib/libc/stdio/stdio.h"
#include "kernel/core/net/socket.h"

/*===========================================
Global Declaration
=============================================*/


/*===========================================
Implementation
=============================================*/
static void sigchild_handler(int sig){
   static char _buf[255] = "\r\nrcv SIGCHLD\r\n";
   int status;
   pid_t pid;
   pid = waitpid((pid_t)0,&status,WNOHANG);
   if(pid>0){
      _buf[strlen("\r\nrcv SIGCHLD\r\n")]=pid+'0';
      _buf[strlen("\r\nrcv SIGCHLD\r\n")+1]='\0';
      write(1,_buf,strlen(_buf));
   }else{
      write(1,"\r\nrcv SIGCHLD pid<0\r\n",strlen("\r\nrcv SIGCHLD pid<0\r\n"));
   }
}
/*-------------------------------------------
| Name:login_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int telnetd_main(int argc, char* argv[]){
   
   int fd = -1;
   int sock = -1;
   int sock_c = -1;
   int dup_sock_c = -1;
   struct sockaddr_in addr;
   int addrlen;
   
   int r;

   //only for SIGCHLD bug test under win32
   struct sigaction sa;
   //SIGCHLD interception
   sa.sa_handler=sigchild_handler;
   sigaction(SIGCHLD,&sa,NULL);
   
   
   sock = socket(AF_INET,SOCK_STREAM,0);	
   addr.sin_family = AF_INET;
   addr.sin_port = htons(2000);
   addr.sin_addr.s_addr = INADDR_ANY;//inet_addr("14.2.212.238");//INADDR_ANY;

   bind(sock,(struct sockaddr*)&addr,sizeof(struct sockaddr_in));

   //
   r = listen(sock,2); 
   if(r!=0)
      return -1;

   for(;;){

      sock_c = accept(sock,(struct sockaddr*)&addr,&addrlen);
      // accept() can return -1 when SIGCHLD is received by this process when
      // accept() is blocked on internal lwip semaphore.
      // with ecos signal release blocking call in pthread (main and secondary).
      // in future errno must be set to EINTR.
      if(sock_c<0)
         continue;
   
      if(!vfork()){
         //printf("login from %s\r\n",addr.sin_addr.s_add);
         dup_sock_c = dup2(sock_c,4);
         close(0);
         close(1);
         close(2);
         dup(dup_sock_c);
         dup(dup_sock_c);
         dup(dup_sock_c);
         close(dup_sock_c);
         close(sock_c);//for child
         //setpgid(0,0);
         execl("/usr/sbin/lsh",NULL);
      }
      close(sock_c);//for father. dont forget sock_c from accept in father processus
      
   }
   

   return 0;

}

/*============================================
| End of Source  : telnetd.c
==============================================*/
