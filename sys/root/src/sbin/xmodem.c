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
      -r receive mode
      -s send mode
      -a at position in file
      -k 1024 bytes (1K) mode.

   \brief
      binary file transfer with xmodem protocol.

   \use
      xmodem -r  /etc/f1 from host to target
      xmodem -s  /etc/f1 from target to host



*/


/* this code needs standard functions memcpy() and memset()
   and input/output functions _inbyte() and _outbyte().

   the prototypes of the input/output functions are:
     int _inbyte(unsigned short timeout); // msec timeout
     void _outbyte(int c);

    compilation with gcc:
    gcc /mnt/disk_x/sources/sbin/xmodem.c -D __UNIX__ -o xmodem.exe

*/
/*===========================================
Includes
=============================================*/
#ifdef __UNIX__
   #include <errno.h>
   #include <stdlib.h>
   #include <io.h>
   #include <termios.h>
   #include <fcntl.h>
   #include <sys/unistd.h>
   #include <stdio.h>
#else
   #include <stdlib.h>
   #include <string.h>

   #include "kernel/core/errno.h"
   #include "kernel/core/libstd.h"
   #include "kernel/core/devio.h"

   #include "lib/libc/termios/termios.h"

   #include "kernel/core/stat.h"
   #include "kernel/core/statvfs.h"

   #include "lib/libc/stdio/stdio.h"
#endif



/*===========================================
Global Declaration
=============================================*/


#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define CTRLZ 0x1A

#define DLY_1S 1000
#define MAXRETRANS 25

#if defined(CPU_M16C62)
#elif defined(CPU_ARM7) || defined(CPU_ARM9) || defined(CPU_WIN32) || defined(__UNIX__)
   #define MODE_1K_SUPPORTED
#endif

#if defined (MODE_1K_SUPPORTED)
   #define MODE_128     0
   #define MODE_1K      1
   #define XMDM_SZ   1024
   #define XMDM_BUF_SZ XMDM_SZ+3+2 /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
#else
   #define MODE_128     0
   #define MODE_1K      0
   #define XMDM_SZ    128
   #define XMDM_BUF_SZ XMDM_SZ+3+2  /* 128 for XModem 128 + 3 head chars + 2 crc + nul */
#endif



static const unsigned short crc16tab[256]= {
   0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
   0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
   0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
   0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
   0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
   0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
   0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
   0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
   0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
   0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
   0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
   0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
   0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
   0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
   0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
   0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
   0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
   0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
   0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
   0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
   0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
   0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
   0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
   0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
   0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
   0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
   0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
   0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
   0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
   0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
   0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
   0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:_inbyte
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _inbyte(int fd_ttys,unsigned short timeout){
   char c;
   if(read(fd_ttys,&c,1)<=0)
      return -1;

   return (c&0x00FF);
}

/*-------------------------------------------
| Name:_outbyte
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _outbyte(int fd_ttys,int c){
   write(fd_ttys,(char*)&c,1);
}

/*-------------------------------------------
| Name:crc16_ccitt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
unsigned short crc16_ccitt(const void *buf, int len)
{
   register int counter;
   register unsigned short crc = 0;
   char* p = (char*)buf;
   for( counter = 0; counter < len; counter++) {
      crc = (crc<<8) ^ crc16tab[((crc>>8) ^ *p)&0x00FF];
      p++;
   }
   return crc;
}

/*-------------------------------------------
| Name:check
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int check(int crc, const unsigned char *buf, int sz)
{
   if (crc) {
      unsigned short crc = crc16_ccitt(buf, sz);
      unsigned short tcrc = (buf[sz]<<8)+buf[sz+1];
      if (crc == tcrc)
         return 1;
   }
   else {
      int i;
      unsigned char cks = 0;
      for (i = 0; i < sz; ++i) {
         cks += buf[i];
      }
      if (cks == buf[sz])
         return 1;
   }

   return 0;
}

/*-------------------------------------------
| Name:flushinput
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static void flushinput(int fd_ttys)
{
   while (_inbyte(fd_ttys,((DLY_1S)*3)>>1) >= 0)
      ;
}

/*-------------------------------------------
| Name:xmodem_receive
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xmodem_receive(int fd,int fd_ttys_in,int fd_ttys_out, int verbose)
{
   unsigned char xbuff[XMDM_BUF_SZ+5];      /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
   unsigned char *p;
   int bufsz, crc = 0;
   unsigned char trychar = 'C';
   unsigned char packetno = 1;
   int i = 0;
   int c = 0;
   int len = 0;
   int retry, retrans = MAXRETRANS;
   int r=0;
   int w=0;
   int cb=0;

   for(;; ) {
      for( retry = 0; retry < 16; ++retry) {
         if (trychar)
            _outbyte(fd_ttys_out,trychar);
         if ((c = _inbyte(fd_ttys_in,(DLY_1S)<<1)) >= 0) {
            switch (c) {
            case SOH:
               bufsz = 128;
               goto start_recv;
            case STX:
               if(XMDM_SZ<1024) {
                  //mode 1k not supported for lepton process stack limitation for this cpu
                  flushinput(fd_ttys_in);
                  _outbyte(fd_ttys_out,ACK);
                  return -1;                               /* canceled by remote */
               }
               bufsz = 1024;
               goto start_recv;
            case EOT:
               flushinput(fd_ttys_in);
               _outbyte(fd_ttys_out,ACK);
               return len;                          /* normal end */
            case CAN:
               if ((c = _inbyte(fd_ttys_in,DLY_1S)) == CAN) {
                  flushinput(fd_ttys_in);
                  _outbyte(fd_ttys_out,ACK);
                  return -1;                               /* canceled by remote */
               }
               break;
            default:
               break;
            }
         }
      }
      if (trychar == 'C') { trychar = NAK; continue; }
      flushinput(fd_ttys_in);
      _outbyte(fd_ttys_out,CAN);
      _outbyte(fd_ttys_out,CAN);
      _outbyte(fd_ttys_out,CAN);
      return -2;           /* sync error */

start_recv:
      if (trychar == 'C') crc = 1;
      trychar = 0;
      p = xbuff;
      *p++ = c;

      /*
                for (i = 0;  i < (bufsz+(crc?1:0)+3); ++i) {
                        if ((c = _inbyte(fd_ttys_in,DLY_1S)) < 0) goto reject;
                        *p++ = c;
                }
      */

      cb=0;
      r=0;
      while( (cb+=r)<(bufsz+(crc ? 1 : 0)+3) ) {
         if((r=read(fd_ttys_in,p+cb,(bufsz+(crc ? 1 : 0)+3)-cb))<0)
            goto reject;
      }

      if (xbuff[1] == (unsigned char)(~xbuff[2]) &&
          (xbuff[1] == packetno || xbuff[1] == (unsigned char)packetno-1) &&
          check(crc, &xbuff[3], bufsz)) {
         if (xbuff[1] == packetno)       {
            w=0;
            cb=0;
            /*
                                register int count = destsz - len;
                                if (count > bufsz) count = bufsz;
                                if (count > 0) {
                                        memcpy (&dest[len], &xbuff[3], count);
                                        len += count;
                                }
            */
            while((cb+=w)<bufsz) {
               w=write(fd,&xbuff[3+cb],bufsz-cb);
            }
            //
            ++packetno;
            len+=bufsz;
            //
            retrans = MAXRETRANS+1;
            if(verbose)
               fprintf(stderr,"rcv pckt[%9d],[%9d]Bytes\r",packetno,len);
         }
         if (--retrans <= 0) {
            flushinput(fd_ttys_in);
            _outbyte(fd_ttys_out,CAN);
            _outbyte(fd_ttys_out,CAN);
            _outbyte(fd_ttys_out,CAN);
            return -3;                     /* too many retry error */
         }
         _outbyte(fd_ttys_out,ACK);
         continue;
      }
reject:
      flushinput(fd_ttys_in);
      _outbyte(fd_ttys_out,NAK);
   }
}

/*-------------------------------------------
| Name:xmodem_transmit
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xmodem_transmit(int fd_ttys_in,int fd_ttys_out, char mode, int fd, int srcsz,int verbose)
{
   unsigned char xbuff[XMDM_BUF_SZ+5];      /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
   int bufsz, crc = -1;
   unsigned char packetno = 1;
   int i, c, len = 0;
   int retry;

   for(;; ) {
      for( retry = 0; retry < 16; ++retry) {
         if ((c = _inbyte(fd_ttys_in,(DLY_1S)<<1)) >= 0) {
            switch (c) {
            case 'C':
               crc = 1;
               goto start_trans;
            case NAK:
               crc = 0;
               goto start_trans;
            case CAN:
               if ((c = _inbyte(fd_ttys_in,DLY_1S)) == CAN) {
                  _outbyte(fd_ttys_out,ACK);
                  flushinput(fd_ttys_in);
                  return -1;                               /* canceled by remote */
               }
               break;
            default:
               break;
            }
         }
      }
      _outbyte(fd_ttys_out,CAN);
      _outbyte(fd_ttys_out,CAN);
      _outbyte(fd_ttys_out,CAN);
      flushinput(fd_ttys_in);
      return -2;           /* no sync */

      for(;; ) {
         int cb=0;
start_trans:

         if(mode==MODE_1K && XMDM_SZ==1024) {
            xbuff[0] = STX;
            bufsz = 1024;
         }else{
            xbuff[0] = SOH;
            bufsz = 128;
         }

         xbuff[1] = packetno;
         xbuff[2] = ~packetno;
         memset (&xbuff[3], CTRLZ, bufsz);
         c=0;
         cb=0;
         while((c+=cb)<bufsz) {
            if( (cb = read(fd,&xbuff[3+c],bufsz))<=0 ) break;
         }
         //
         if (/*c >= 0*/ c>0) {
            if (crc) {
               unsigned short ccrc = crc16_ccitt(&xbuff[3], bufsz);
               xbuff[bufsz+3] = (ccrc>>8) & 0xFF;
               xbuff[bufsz+4] = ccrc & 0xFF;
            }
            else {
               unsigned char ccks = 0;
               for (i = 3; i < bufsz+3; ++i) {
                  ccks += xbuff[i];
               }
               xbuff[bufsz+3] = ccks;
            }
            for (retry = 0; retry < MAXRETRANS; ++retry) {
               for (i = 0; i < bufsz+4+(crc ? 1 : 0); ++i) {
                  _outbyte(fd_ttys_out,xbuff[i]);
               }
               if ((c = _inbyte(fd_ttys_in,DLY_1S)) >= 0 ) {
                  switch (c) {
                  case ACK:
                     ++packetno;
                     len += bufsz;
                     if(verbose)
                        fprintf(stderr,"snd pckt [%9d],[%9d]Bytes\r",packetno,len);
                     goto start_trans;
                  case CAN:
                     if ((c = _inbyte(fd_ttys_in,DLY_1S)) == CAN) {
                        _outbyte(fd_ttys_out,ACK);
                        flushinput(fd_ttys_in);
                        return -1;                                         /* canceled by remote */
                     }
                     break;
                  case NAK:
                  default:
                     break;
                  }
               }
            }
            _outbyte(fd_ttys_out,CAN);
            _outbyte(fd_ttys_out,CAN);
            _outbyte(fd_ttys_out,CAN);
            flushinput(fd_ttys_in);
            return -4;                     /* xmit error */
         }
         else {
            for (retry = 0; retry < 10; ++retry) {
               _outbyte(fd_ttys_out,EOT);
               if ((c = _inbyte(fd_ttys_in,(DLY_1S)<<1)) == ACK) break;
            }
            flushinput(fd_ttys_in);
            return (c == ACK) ? len : -5;
         }
      }
   }
}


/*-------------------------------------------
| Name:main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
#ifdef __UNIX__
int main(int argc,char* argv[]){
#else
int xmodem_main(int argc,char* argv[]){
#endif
   int st;
   struct termios _termios;
   struct stat _stat;
   char * file=(char*)0;
   char op=0;

   int verbose=0;

   int fd=-1;
   int pos=0;
   int index;
   char mode=MODE_128; //0 (default): 128 Bytes 1:1024 Bytes (1Kbytes)

   //get options
   for(index=1; index<argc; index++) {
      if (argv[index][0] == '-') {
         switch (argv[index][1]) {
         case 'r':
         case 's':    //xmodem operation
            op=argv[index][1];
            break;
         case 'a':      //at position in file
            if(argc >= index+1) {
               pos = atoi(argv[index+1]);
               index++;
            }
            break;
         case 'k':      //1k mode
            if(argc >= index+1) {
            }

            mode = MODE_1K;
            break;

         case 'i':
            if(argc >= index+1) {
               if(!argv[index+1])
                  break;
               if((fd = open(argv[index+1],O_RDONLY,0))<0)
                  return -1;
               close(0);
               dup(fd);
               close(fd);
               index++;
            }
            break;

         case 'd':
            if(argc >= index+1) {
               if(!argv[index+1])
                  break;
               if((fd = open(argv[index+1],O_RDWR,0))<0)
                  return -1;
               close(0);
               close(1);
               dup(fd);
               dup(fd);
               close(fd);
               index++;
            }
            break;

         case 'o':
            if(argc >= index+1) {
               if(!argv[index+1])
                  break;
               if((fd = open(argv[index+1],O_WRONLY,0))<0)
                  return -1;
               close(1);
               dup(fd);
               close(fd);
               index++;
            }
            break;


         case 'v':      //verbose
            if(argc >= index+1) {
            }
            verbose =1;
            break;

         default:    //undefine argument
            return -1;
         }  //fin du switch
      }  //fin du if(argv...
      else{
         file = argv[index];
      }
   }  //fin du for
      //ici creer le fichier prm si console

   //
   if(file==(char*)0) //no file specified
      return -1;

   if(op=='r') {
      if((fd=open(file,O_WRONLY,0))<0)
         return -3;  //file open failed
   }else if(op=='s') {
      if((fd=open(file,O_CREAT|O_RDWR,777))<0)
         return -3;  //file creation failed
   }else
      return -1;  //op not defined


   fstat(fd,&_stat);
   if(S_ISDIR(_stat.st_mode))
      return -4;  //is directory, cannot be used.

   //
   tcgetattr(STDIN_FILENO,&_termios);
   _termios.c_cc[VTIME]=20; //read timeout 20*0.1s=2 seconds
   tcsetattr(STDIN_FILENO,TCSANOW,&_termios);

   //to do: disable xon/xoff

   //file position
   lseek(fd,pos,SEEK_SET);

   //
   if(op=='r') {
      st = xmodem_receive(fd,STDIN_FILENO,STDOUT_FILENO,verbose);
      close(fd);
      sync();
   }else if(op=='s') {
      st = xmodem_transmit(STDIN_FILENO,STDOUT_FILENO,mode,fd,_stat.st_size,verbose);
      close(fd);
   }



   //restore
   _termios.c_cc[VTIME]=0; //no timeout
   tcsetattr(STDIN_FILENO,TCSANOW,&_termios);
   //to do: enable xon/xoff
   return 0;
}

/*===========================================
End of Source xmodem.c
=============================================*/
