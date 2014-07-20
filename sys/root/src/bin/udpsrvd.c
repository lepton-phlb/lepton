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
#include "kernel/core/etypes.h"
#include "kernel/core/kernel.h"
#include "kernel/core/signal.h"
#include "kernel/core/libstd.h"
#include "kernel/core/devio.h"
#include "kernel/core/wait.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/time.h"
#include "kernel/core/select.h"
#include "kernel/core/errno.h"

#include "lib/libc/stdio/stdio.h"
#include "lib/libc/termios/termios.h"

#include "kernel/core/net/socket.h"

#include "lib/pthread/pthread.h"
#include "lib/librt/mqueue.h"
#include "lib/librt/semaphore.h"

#include "kernel/core/ioctl_hd.h"
#include "kernel/dev/arch/all/ppp/dev_ppp_uip/dev_ppp_uip.h"



/*============================================
| Global Declaration 
==============================================*/
#define BUFLEN 1024
#define NPACK 10
#define MAX_PORT 4
#define PORT 2100

#define  XML_REQUEST \
   "\
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

static char str[MAX_LEN];     /* string to send */

typedef struct arg_test_st{
  int port_no[MAX_PORT];
  int port_nb;
  unsigned long counter;
}arg_test_t;

/*============================================
| Implementation 
==============================================*/

static void socket_client_tcp(void)
{
   int sock;                  /* socket descriptor */

   struct sockaddr_in mc_addr; /* socket address structure */
   int send_len;                 /* length of string to send */
   char mc_addr_str[32];           /*  IP address */
   unsigned short mc_port;    /*  port */
   int res;

   strcpy (mc_addr_str, "193.252.22.86"); //smtp.orange.fr
   mc_port     = 25; //80;

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
   /* clear send buffer */
   memset(str, 0, sizeof(str));

   read(sock,str,sizeof(str));
   printf("rcv : %s\n",str);

   #if 0
   //strcpy (str, XML_REQUEST);
   //send_len = strlen(str);

   /* send string to multicast address */
   res = sendto(sock, str, send_len, 0,
                (struct sockaddr *) &mc_addr,
                sizeof(mc_addr));

   if (res != send_len) {
      printf("sendto() sent incorrect number of bytes");
      exit(1);
   }
   #endif

   /* clear send buffer */
   memset(str, 0, sizeof(str));
}

/*--------------------------------------------
| Name:        udp_server_daemon
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static int udp_server_daemon(arg_test_t* p_arg_test){
   struct sockaddr_in6 si_me, si_other;
   int i;
   int s[MAX_PORT];
   int slen=sizeof(si_other);
   fd_set fdset_read;
   char buf[BUFLEN];
   int cb=0;
   iface_ppp_stat_t ppp_stat={0};
   int fd_ppp;
   
   int sock_errno=ECONNABORTED;
   //
   for(;;){
      
      
      //stack ready
      //network (ppp) upped? 
      fd_ppp = open("/dev/net/ppp",O_RDONLY,0);
      //
      while( ioctl(fd_ppp,PPPSTAT,&ppp_stat)>=0 && ppp_stat.is_up!=PPP_UP ){
         usleep(500000);
      }
      //stack ready and network (ppp) upped, (re-)open sockets
      for(i=0;i<p_arg_test->port_nb;i++){
         //create socket                        
         if ((s[i]=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
            return -1;
         
         memset((char *) &si_me, 0, sizeof(si_me));
         si_me.sin6_family = AF_INET;
         si_me.sin6_port = htons(p_arg_test->port_no[i]);
         si_me.sin6_addr = in6addr_any;
         si_me.sin6_scope_id = 0; //if_nametoindex("ppp");
         
         if (bind(s[i], (struct sockaddr*)&si_me, sizeof(si_me))==-1)
            return -1;
      }
      
      //stack ready, network (ppp) upped and sockets opened
      for (;;){
         int r;
         struct timeval	tv={0};
         //
         FD_ZERO(&fdset_read);
         //
         for(i=0;i<p_arg_test->port_nb;i++){
            FD_SET(s[i],&fdset_read);
         }
         //
         tv.tv_sec	= 1;
		   tv.tv_usec	= 0;
         //
         r=select(s[p_arg_test->port_nb-1]+1,&fdset_read,(fd_set*)0,(fd_set*)0,&tv);
         //
         if(r==0){
            /*
            if(!stop_start_uip_core){
              dev_core_ioctl(desc_ppp,PPPSHTDWN);
            }
            */
            continue;
         }
         //
         if(r<0)
           break;
         //
         cb = 0;
         //
         for(i=0;i<p_arg_test->port_nb;i++){
            if(!FD_ISSET(s[i],&fdset_read) )
              continue;
            //
            if ((cb=recvfrom(s[i], buf, BUFLEN, MSG_PEEK, (struct sockaddr*)&si_other, (socklen_t*)&slen))<0){
                 break;
            }
               
            if ((cb=recvfrom(s[i], buf, cb, 0, (struct sockaddr*)&si_other, (socklen_t*)&slen))<0){
                 break;
            }
            //
            p_arg_test->counter++;
               //
            if((cb=sendto(s[i],buf,cb,0,(struct sockaddr*)&si_other, slen))<0){
                 break;
            }
         }     
         //
         if((sock_errno=(errno))!=0)
           break;
      }
      //
      for(i=0;i<p_arg_test->port_nb;i++){
        close(s[i]);
      }
      //
      close(fd_ppp);
   }
   //
   return 0;
}

/*--------------------------------------------
| Name:        udpsrvd_main
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int udpsrvd_main(int argc,char* argv[]){
   arg_test_t arg_test={0};

   arg_test.port_no[0]=PORT;
   arg_test.port_no[1]=PORT+1;
   arg_test.port_nb = 2;

   //udp_server_daemon(&arg_test);

   socket_client_tcp();


   return 0;
}

/*============================================
| End of Source  : udpsrvd.c
==============================================*/