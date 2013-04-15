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
#include <windows.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <conio.h>
#include <winsock.h>
#include <string.h>

#include "kernel/core/ucore/embOSW32_100/segger_intr.h"


/*===========================================
Global Declaration
=============================================*/

#define SUPPORTED_VERSION_MINOR         1
#define SUPPORTED_VERSION_MAJOR         1

#define STOPSERVER_TIMEOUT          10

#define ERR_NO_SOCKET           -1
#define ERR_BIND_FAILED         -2
#define ERR_LISTEN_FAILED       -3
#define ERR_ACCEPT_FAILED       -4
#define ERR_CONNECT_FAILED      -5
#define ERR_NO_HOSTNAME         -6


#define TCP_PORT_SERVER    2000
#define TCP_PORT_CLIENT    2000

HANDLE hthr;
DWORD thr_id;
HANDLE hsock_event;

volatile static int thr_stop_flg = 0;
volatile static int socket_rcv_intr_enbl = 0;
volatile static int socket_snd_intr_enbl = 0;

volatile static unsigned char rcv_data;

SOCKET clt_sock=0;

/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:ip_start
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int ip_start(WSADATA * pwsaData){
   WORD wVersionRequested;
   wVersionRequested = MAKEWORD(SUPPORTED_VERSION_MINOR,SUPPORTED_VERSION_MAJOR);
   return WSAStartup(0x0101, pwsaData);
}

/*-------------------------------------------
| Name:ip_stop
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void ip_stop(void){
   WSACleanup();
}

/*-------------------------------------------
| Name:tcp_srv
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
DWORD tcp_srv(LPVOID lpv){
   SOCKET sock;
   int nRc;
   struct sockaddr_in addr;

   SOCKADDR_IN acc_sin;
   int sin_len=sizeof(SOCKADDR_IN);

   int cb=-1;

   char buffer[100];
   printf("socket thread started!\n");

   sock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
   if(sock==INVALID_SOCKET)
   {
      return(ERR_NO_SOCKET);
   }

   ZeroMemory(&addr,sizeof(addr));
   addr.sin_family=PF_INET;
   addr.sin_port=htons(TCP_PORT_SERVER);
   addr.sin_addr.s_addr=INADDR_ANY;
   //addr.sin_addr.s_addr=inet_addr("14.2.211.157");
   //addr.sin_addr.s_addr=inet_addr("128.1.1.1");


   nRc=bind(sock,(struct sockaddr FAR *)&addr,sizeof(struct sockaddr));
   if(nRc!=0) {
      int NET_ERROR=WSAGetLastError();
      closesocket(sock);
      return(ERR_BIND_FAILED);
   }

   nRc=listen(sock,SOMAXCONN);
   if(nRc!=0) {
      closesocket(sock);
      return(ERR_LISTEN_FAILED);
   }

   cb=-1;

   for(;; ) {

      printf("socket wait connection...\n");

      //
      clt_sock = accept( sock,(struct sockaddr FAR *) &acc_sin,(int FAR *) &sin_len);

      printf("socket receive connection\n");
      while(cb) {
         int i;

         if((cb=recv(clt_sock,buffer,sizeof(buffer),0))<=0) break;

         for(i=0; i<cb; i++) {
            rcv_data=buffer[i];
            if(socket_rcv_intr_enbl) {
               emuFireInterrupt(125);
               //Synchro
               WaitForSingleObject(hsock_event,10);
            }
         }


         //send(clt_sock,buffer,1,0);
      }

   }

   shutdown(sock,2);
   closesocket(sock);

   return 0;
}

/*-------------------------------------------
| Name:read_socket_0
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
unsigned char read_socket_0(void) {
   unsigned char data = rcv_data;

   //Synchro
   SetEvent(hsock_event);
   return data;
}

/*-------------------------------------------
| Name:write_socket_0
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
unsigned char write_socket_0(unsigned char data) {

   if(clt_sock>0)
      send(clt_sock,&data,1,0);

   if(socket_snd_intr_enbl)
      emuFireInterrupt(126);

   return 0;
}

/*-------------------------------------------
| Name:set_socket_rcv_intr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void set_socket_0_rcv_intr(int enable){
   socket_rcv_intr_enbl = enable;
}

/*-------------------------------------------
| Name:set_socket_snd_intr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void set_socket_0_snd_intr(int enable){
   socket_snd_intr_enbl = enable;
}


/*-------------------------------------------
| Name:start_socketsrv_0
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int start_socketsrv_0(int argc, char* argv[]){
   WSADATA wsaData;

   clt_sock=0;

   if (ip_start(&wsaData)) {
      printf("ip_start failed!");
      return -1;
   }



   hsock_event=CreateEvent(NULL,FALSE,FALSE,NULL);

   hthr = CreateThread( (LPSECURITY_ATTRIBUTES) NULL,
                        0,
                        (LPTHREAD_START_ROUTINE) tcp_srv,
                        NULL,
                        0, &thr_id);
   if(!hthr)
      return -1;


   return 0;
}

/*-------------------------------------------
| Name:close_socketsrv_0
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int close_socketsrv_0(void){
   shutdown(clt_sock,2);
   closesocket(clt_sock);
   return 0;
}

/*-------------------------------------------
| Name:stop_socketsrv_0
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int stop_socketsrv_0(void){
   thr_stop_flg=1;

   shutdown(clt_sock,2);
   closesocket(clt_sock);
   Sleep(1000);

   ip_stop();

   clt_sock=0;

   return 0;
}


/*===========================================
End of Source dev_socketsrv_0.c
=============================================*/
