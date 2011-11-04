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
#include "kernel/signal.h"
#include "kernel/libstd.h"
#include "kernel/devio.h"
#include "kernel/fcntl.h"
#include "kernel/wait.h"
#include "kernel/stat.h"
#include "termios/termios.h"

#include "stdio/stdio.h"

#include "kernel/net/socket.h"

#include "pthread/pthread.h"



/*============================================
| Global Declaration 
==============================================*/
#define TSTIP_BUF_SZ 1024
#define NONE    0
#define ODD     1
#define EVEN    2

#define NbAbon    1 //16    // Nombre d'abonn�s Modbus Esclaves � interroger

typedef struct TB_Abonne_st
{unsigned char nab;
 unsigned int fct;
 unsigned int adres;
 unsigned int nbmot;
 unsigned char tprep;
 int TbData[125];
};


/* Variables globales */
/**********************/

/*============================================
| Prototypes 
==============================================*/
static int setparity(struct termios *termios_p, int par);
static int setdatasize(struct termios *termios_p, int nbbit);
static int setstopbit(struct termios *termios_p, unsigned char nbbit);


/*============================================
| Implementation 
==============================================*/

/*--------------------------------------------
| Name:        tstip_thread_ls
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void* tstip_thread_ls(void* p)
{
 int *fd=(int*)p;
 int i;
 char bufcar[20];
 char buf[256];
 int fdin=fd[0];
 int fdout=fd[1];
 int nbcar=fd[2];
 struct TB_Abonne_st TB_Abonne[NbAbon]={0};
 
 for(i=0;i<nbcar;i++)
   bufcar[i] = i + '0';
   bufcar[i-1] = ' ';
   bufcar[i-2] = ' ';

   for(;;){
    //usleep(50000);
    read(fdin,buf,sizeof(buf));
    i = write(fdout,bufcar,nbcar);
  }


 return (void*)0;
}



/*-------------------------------------------
| Name:setparity
| Description:Set the parity stored in *TERMIOS_P
| Parameters: par
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int setparity(struct termios *termios_p, int par)
{
 termios_p->c_cflag &= ~PARODD;
 switch(par)
    {case NONE : termios_p->c_cflag &= ~PARENB;
                 break;
     case ODD  : termios_p->c_cflag |= PARODD;
     case EVEN : termios_p->c_cflag |= PARENB;
                 break;
    }
 return 0;
}


/*-------------------------------------------
| Name:setdatasize
| Description:Set the data size stored in *TERMIOS_P
| Parameters: nbbit
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int setdatasize(struct termios *termios_p, int nbbit)
{termios_p->c_cflag &= ~CSIZE;
 termios_p->c_cflag |= nbbit;
 return 0;
}


/*-------------------------------------------
| Name:setstopbit
| Description:Set the number of stop bit stored in *TERMIOS_P
| Parameters: nbbit
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int setstopbit(struct termios *termios_p, unsigned char nbbit)
{if(nbbit == 2)
    termios_p->c_cflag |= CSTOPB;   // 2 stop bit
 else
    termios_p->c_cflag &= ~CSTOPB;  // 1 stop bit
 return 0;
}



/*--------------------------------------------
| Name:        tstip_main
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/

int tstip_dbg_flg=0;

int tstip_main(int argc, char* argv[])
{/* Variable Socket Ethernet */
 int sock=0,s=0;
 int i, len=0;
 int oflag=0;
 struct sockaddr_in addr={0},rem={0};

 char buf1[TSTIP_BUF_SZ];
 char buf2[TSTIP_BUF_SZ*2];
 int sz= sizeof(buf1);  

 /* Variable thread LS */
 pthread_t pthread_ls;
 pthread_attr_t thread_attr;
 int fd[5];

 /* Variable Liaison Serie */
 int fdin,fdout;
 struct termios _termconf ={0};
 struct termios _termios  ={0};

 
   
 for(i=0;i<sizeof(buf1);i++)
   {buf1[i]=i;
    buf2[i]=sz-i;
   }

 usleep(1000000);
 
 /* Initialisation Liaison Serie */
 fdin  = open("/dev/rtu0",O_RDONLY,0);      // Lecture sur la liaison serie 0  (1er descripteur ouvert => 0)
 fdout = open("/dev/rtu0",O_WRONLY,0);      // Ecriture sur la liaison serie 0 (2nd descripteur ouvert => 1)
 tcgetattr(fdin,&_termconf);                // Permet d'obtenir les param�tres courants de la liaison ttys1
 cfsetispeed(&_termconf, B38400);
 cfsetospeed(&_termconf, B38400);
 setparity(&_termconf, NONE);
 setdatasize(&_termconf, CS8);
 setstopbit(&_termconf, 1);
 //_termios.c_cc[VTIME]=20;                 //read timeout 20*0.1s=2 seconds  (rend la main au bout d'un time out de 2 s si aucun carac recu)
 tcsetattr(fdin,TCSANOW,&_termconf);        // r��criture des param�tres courants de la liaison ttys1
 fd[0] = fdin;                              // Descripteur de Lecture sur la liaison serie 
 fd[1] = fdout;                             // Descripteur d'Ecriture sur la liaison serie   
 fd[2] = 10;                                // Nombre de caract�res � emettre 
   
 /* Initialisation du thread */
 thread_attr.stacksize = 2048;
 thread_attr.stackaddr = NULL;
 thread_attr.priority  = 100;
 thread_attr.timeslice = 1;

 pthread_create(&pthread_ls, &thread_attr, tstip_thread_ls,&fd);

 /* Initialisation de la Socket */
 sock = socket(AF_INET,SOCK_STREAM,0);	
 addr.sin_family = AF_INET;
 addr.sin_port = htons(2003);
 addr.sin_addr.s_addr = INADDR_ANY;

 bind(sock,(struct sockaddr *)&addr,sizeof(addr));
		   
 listen(sock,5);
 len = sizeof(rem);

 while(1)
  {unsigned char b=0;
   int cb=0;

   s = accept(sock,(struct sockaddr*)&rem,&len);

   for(;;)
        {tstip_dbg_flg=1;
         cb = recv(s,buf2,5,0);
         if(cb<0)
            break;
         tstip_dbg_flg=2;
         //usleep(100000);
         buf1[0]='#';
         buf1[1]=(++i);
         buf1[2]='#';
         cb = send(s,buf1,TSTIP_BUF_SZ,0);
         if(cb<0)
            break;
         tstip_dbg_flg=3;
         //usleep(1000000);
        }

      close(s);
   }  

  return 0;
}

/*============================================
| End of Source  : tstip.c
==============================================*/
