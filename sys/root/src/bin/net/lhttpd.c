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
#include "kernel/devio.h"
#include "stdio/stdio.h"
#include "kernel/net/socket.h"

//#define LWIP_COMPAT_SOCKETS 1
//#include "lwip/include/lwip/sockets.h"




/*===========================================
Global Declaration
=============================================*/
/* The HTTP server states: */
#define HTTP_NOGET        0
#define HTTP_FILE         1
#define HTTP_TEXT         2
#define HTTP_FUNC         3
#define HTTP_END          4
#define HTTP_ACKED        5


#define ISO_G        0x47
#define ISO_E        0x45
#define ISO_T        0x54
#define ISO_slash    0x2f
#define ISO_c        0x63
#define ISO_g        0x67
#define ISO_i        0x69
#define ISO_space    0x20
#define ISO_nl       0x0a
#define ISO_cr       0x0d
#define ISO_a        0x61
#define ISO_t        0x74
#define ISO_hash     0x23
#define ISO_period   0x2e


//#define _INDEX_HTML "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">\r\n<html><head><title>HELLO</title></head><body bgcolor=white><center><h3><font face=\"Helvetica\"><P>WEB SERVER ON M16C</P></h3><a href=\"copyr.html\">Copyright</a>&copy; 2002<a href=\"about.html\">about A0380 project</a></center></body></html>"
#define _INDEX_HTML "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\"><html><head><title>welco</title></head><body bgcolor=white><center><h3><font face=\"Helvetica\"><P>lepton httpd server</P></h3><a href=\"copyr.html\">Copyright</a>&copy; 2003<a href=\"about.html\">about A0383 project</a></center></body></html>"

char index_html[]=_INDEX_HTML;


/*===========================================
Implementation
=============================================*/


/*-------------------------------------------
| Name:lhttpd_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int lhttpd_main(int argc, char* argv[]){
   int fd = -1;
   int sock = -1;
   int sock_c = -1;
   struct sockaddr_in addr;
   int addrlen;

   char sip_addr[18]={0};

   char * file;
   char buffer[256];
   int cb;
   int len;

   if(chdir("/usr/bin/net/html")<0) {
      //printf("error /usr/bin/net/html not exist!");
      return -1;
   }

   //
   sock=socket(PF_INET,SOCK_STREAM,0);

   //
   addr.sin_port=htons(80);
   bind(sock,(struct sockaddr*)&addr,sizeof(struct sockaddr_in));

   //
   if(listen(sock,10)!=0)
      return -1;

   for(;; ) {


      //printf("wait connection\n");
      sock_c = accept(sock,(struct sockaddr*)&addr,&addrlen);
      //inet_ntoa(sip_addr,addr.sin_addr);

      //printf("receive connection\n");

      if((cb=recv(sock_c,buffer, sizeof(buffer)-1,0))>0) {
         int i=0;

         /* Check for GET. */
         if(buffer[0] != ISO_G ||
            buffer[1] != ISO_E ||
            buffer[2] != ISO_T ||
            buffer[3] != ISO_space) {
            continue;

         }

         //printf("GET\n");

         // Find the file we are looking for.
         for(i = 4; i < 40; ++i) {
            if(buffer[i] == ISO_space ||
               buffer[i] == ISO_cr ||
               buffer[i] == ISO_nl) {

               buffer[i] = 0;
               continue;
            }
         }

         file = (buffer+5);
         printf("open file %s\n",file);

         fd= open(file,O_RDONLY,0);
         if(fd<0) {
            fd=open("index.html",O_RDONLY,0);
            if(fd<0)
               continue;
         }

         //send(sock_c,"HTTP/1.0 200 OK\r\n",strlen("HTTP/1.0 200 OK\r\n"),0);

         while((len=read(fd,buffer,sizeof(buffer)-1))>0) {
            int _cb;
            cb=0;
            buffer[len]=0;
            //printf("%s",buffer);
            while(cb<len) {


               printf("send...\n");
               if( (_cb=send(sock_c,buffer+cb,len-cb,0))<0) {
                  printf("send error\n");
                  break;
               }
               cb+=_cb;
               printf("ok\n");
            }
            if(_cb<0)
               break;
         }

         close(fd);

         printf("shutdown ...\n");
         shutdown(sock_c,0);
         printf("shutdown ok!\n");
      }
   }

   return 0;
}

/*===========================================
End of Source lhttpd.c
=============================================*/


