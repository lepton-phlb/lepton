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
#include "kernel/stat.h"
#include "kernel/statvfs.h"
#include "kernel/dirent.h"
#include "kernel/devio.h"
#include "kernel/ioctl.h"
#include "kernel/time.h"
#include "kernel/wait.h"
#include "kernel/fcntl.h"
#include "kernel/select.h"

#include "termios/termios.h"
#include "stdio/stdio.h"

#include "pthread/pthread.h"

#include "dev/dev_mem/dev_mem.h"

#include "libc/misc/crc.h"

/*============================================
| Global Declaration 
==============================================*/

#define  OPT_MSK_I 0x01L //-i define input device
#define  OPT_MSK_O 0x02L //-o define output device
#define  OPT_MSK_C 0x04L //-c only copy from copy mode
#define  OPT_MSK_S 0x08L //-s secure in from copy mode

const char* __f_memd =".memd";

//
#pragma pack(push, 1)
typedef uint16_t addr_t;
typedef uint16_t sz_t;

typedef struct{
   sz_t     sz;
   addr_t   addr;
}mem_header_t;

typedef uchar8_t mem_data_t;

#pragma pack(pop)

typedef struct{
   int fd;// /dev/mem
}mem_init_t;


#define MAX_DATA_SZ 256+2+32
#define SYNC_C 'c'
#define SYNC_S 's'

#define MAX_SYNC 50

/*============================================
| Implementation 
==============================================*/
/*--------------------------------------------
| Name:        memd_init
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int memd_init(int fd,int *p_addr_lst,int *addr_lst_sz,int* data_sz_max){
   FILE* stream;
   int addr=-1;
   int size=-1;
   int i=0;
   char op[3]={0};
   char c1=0;
   char c2=0;

   *data_sz_max=0;


   if( (stream  = fopen( "/usr/etc/.memd", "r" )) == NULL )
      return -1;

   rewind(stream);

   while((fscanf(stream,"@ 0x%x ; # %d ; %2s \n",&addr,&size,op)!=EOF) && i<MAX_MEM_ADDR){
      if(addr<0 || size <0)
         continue;//error

      c1= op[0];
      c2= op[1];

      if(c1=='+' || c2=='+'){
         void *p = malloc(size);
         //dynamic allocation
         ioctl(fd,MEMADD,addr,size,p);
      }else{
         ioctl(fd,MEMADD,addr,size,(void*)0);
      }

      if(c1=='r' || c2=='r'){
         if(size>*data_sz_max) *data_sz_max = size;
         ioctl(fd,MEMREG,addr);
         p_addr_lst[i]=addr;
         i++;
         *addr_lst_sz=i;
      }

      addr=-1;
      size=-1;
      op[0] =0;
      op[1] =0;
   }

   fclose(stream);
   return 0;

}

/*--------------------------------------------
| Name:        memd_copy_from_cpu
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
uint32_t memd_rec_error_crc = 0uL;
uint32_t memd_rec_error_pksz = 0uL;

int memd_copy_from_cpu(int fd[]){
   int fd0=fd[0];
   int fd1=fd[1];
   int fds;
   int cb;
   mem_header_t* p_mem_packet;
   mem_data_t*    p_data_packet;

   //mem replication data buffer allocation
   if( !(p_mem_packet = (mem_header_t*)malloc(sizeof(mem_header_t)+MAX_DATA_SZ)) )
      return -1;

   p_data_packet = ((mem_data_t*)(p_mem_packet))+sizeof(mem_header_t);

   //mem replication
   fds = open("/dev/mem",O_RDWR,0);

   //read from distant cpu
   while((cb=read(fd[0],p_mem_packet,sizeof(mem_header_t)+MAX_DATA_SZ))>0){
      crc16_t crc;
      int crc_byte_no=0;
      uchar8_t* p_crc_mem_packet=((uchar8_t*)p_mem_packet);

      if(cb<=sizeof(mem_header_t))
         continue;
      //detect crc
      if( (cb>(int)(p_mem_packet->sz+sizeof(mem_header_t)))
         && ( (cb-(p_mem_packet->sz+sizeof(mem_header_t))) == sizeof(crc16_t) ) ){
         //crc
         __init_crc16(crc);
         //
         for(crc_byte_no=0;crc_byte_no<((int)((cb-sizeof(crc16_t)) ));crc_byte_no++){
            crc=crc16(crc,p_crc_mem_packet[crc_byte_no]);
         }
         if( p_crc_mem_packet[crc_byte_no++] != ( ((uchar8_t)(crc&0xff00)) >> 8 ) ){//msb
            //error continue???
            ++memd_rec_error_crc;
            continue;
         }
         if( p_crc_mem_packet[crc_byte_no] != ( ((uchar8_t)(crc&0x00ff)) ) ){//lsb
            //error continue???
            ++memd_rec_error_crc;
            continue;
         }
      }else{
         unsigned long command=TCIFLUSH;
        //error invalid packet size
        ++memd_rec_error_pksz;
        ioctl(fd[0],TCFLSH,command);
        continue;
      }
      //
      lseek(fds,p_mem_packet->addr,SEEK_SET);
      write(fds,(char*)p_data_packet,p_mem_packet->sz);
   }

   return 0;
}

/*--------------------------------------------
| Name:        memd_copy_to_cpu
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int memd_copy_to_cpu(int fd[],int secure){

   int cb=0;

   mem_header_t* p_mem_packet;
   mem_data_t*    p_data_packet;

   int   addr_lst[MAX_MEM_ADDR];
   int   addr_lst_sz;
   int   data_sz_max;

   //mem replication initialisation
   if(memd_init(fd[2],addr_lst,&addr_lst_sz,&data_sz_max)<0)
      return -1;

   //mem replication data buffer allocation
   if( !(p_mem_packet = (mem_header_t*)malloc( sizeof(mem_header_t)+data_sz_max+sizeof(crc16_t) )) )
      return -1;
   p_data_packet = ((mem_data_t*)(p_mem_packet))+sizeof(mem_header_t);

   //wait notification
   for(;;){
      int i=0;

      for(i=0;i<addr_lst_sz;i++){
         lseek(fd[2],addr_lst[i],SEEK_SET);

         cb = read(fd[2],p_data_packet,data_sz_max);
         if(cb>0){
            p_mem_packet->addr = addr_lst[i];
            p_mem_packet->sz   = cb;

            if(secure){
               crc16_t crc;
               int crc_byte_no=0;
               uchar8_t* p_crc_mem_packet=((uchar8_t*)p_mem_packet);

               //crc
               __init_crc16(crc);
               //
               for(crc_byte_no=0;crc_byte_no<((int)(sizeof(mem_header_t)+cb));crc_byte_no++){
                  crc=crc16(crc,p_crc_mem_packet[crc_byte_no]);
               }
               p_crc_mem_packet[crc_byte_no++]  = ( ((uchar8_t)(crc&0xff00)) >> 8 );//msb
               p_crc_mem_packet[crc_byte_no]    = ( ((uchar8_t)(crc&0x00ff)) );//lsb
               cb+=sizeof(crc16_t);
            }
            //write to distant cpu
            if(write(fd[1],p_mem_packet,(sizeof(mem_header_t)+cb))<0)
               return -1;
         }
      }

   }

   return 0;
}

/*--------------------------------------------
| Name:        memd_thread_copy_from_cpu
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void* memd_thread_copy_from_cpu(void* p){
   int *fd=(int*)p;

   memd_copy_from_cpu(fd);

   return (void*)0;
}

/*--------------------------------------------
| Name:        memd_cpu_sync
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int memd_cpu_sync(void){

   char c=0;
   char sync_c=0;
   char sync_s=0;

   int cpt=MAX_SYNC;

   struct termios _termios;

   //set timeout
   tcgetattr(STDIN_FILENO,&_termios);
   _termios.c_cc[VTIME]=1;//read timeout 1*0.1s= 100 ms
   tcsetattr(STDIN_FILENO,TCSANOW,&_termios);
   //
   while(sync_c!=SYNC_C && sync_s!=SYNC_S){
      //send 'c'
      if(sync_c!=SYNC_C){
         char _c=SYNC_C;
         write(STDOUT_FILENO,&_c,1);
      }

      if(read(STDIN_FILENO,&c,1)<=0)
         continue;

      //receive 'c' and send 's' for synchro with distant cpu
      if(c==SYNC_C && !sync_s){
         sync_s=SYNC_S;
         write(STDOUT_FILENO,&sync_s,1);
      }

      //receive 's' after send 'c' for synchro with distant cpu
      if(c==SYNC_S){
         sync_c=SYNC_C;
      }

      if(!(--cpt))
         break;
   }
   //
   tcgetattr(STDIN_FILENO,&_termios);
   _termios.c_cc[VTIME]=0;//no read timeout
   tcsetattr(STDIN_FILENO,TCSANOW,&_termios);
   //
   if(cpt<=0)
      return -1;

   return 0;
}

/*--------------------------------------------
| Name:        memd_main
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int memd_main(int argc,char* argv[]){

   int i;
   int fd[3];


   pthread_t pthread_memd;
   pthread_attr_t thread_attr;

   unsigned int opt= OPT_MSK_S;

   char* __stdin=0;
   char* __stdout=0;

   //get option
   for(i=1;i<argc;i++){
      if(argv[i][0]=='-'){
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1;c<l;c++){
            switch(argv[i][c]){
               case 'i':
                  opt |= OPT_MSK_I;
                  //get stdin device
                  if((i+1) == argc)//not enough parameter
                     return -1;
                  if(argv[i][c+1])
                     break;
                  if(argv[i+1][0]=='-')
                     break;

                  i++;
                  if(argv[i])
                     __stdin = argv[i];
                  if((opt&OPT_MSK_O) && !__stdout)
                     __stdout = __stdin;

               break;

               case 'o':
                  opt |= OPT_MSK_O;
                  //get stdin device
                  if((i+1) == argc)//not enough parameter
                     return -1;
                  if(argv[i][c+1])
                     break;
                  if(argv[i+1][0]=='-')
                     break;

                  i++;
                  if(argv[i])
                     __stdout = argv[i];
                  if((opt&OPT_MSK_I) && !__stdin)
                     __stdin = __stdout;

               break;

               case 'c':// only copy from cpu mode
                  opt |= OPT_MSK_C;
               break;

               case 's':// secure mode (crc)
                  opt |= OPT_MSK_S;
               break;

            }
         }
      }else{
      }
   }

   //open stdin
   if((opt&OPT_MSK_I) && __stdin){
      //close stdin
      close(STDIN_FILENO);
      //open keyboard
      if((fd[0]=open(__stdin,O_RDONLY,0))<0){
         fprintf(stderr,"error: cannot open %s\r\n",__stdin);
         return -1;
      }
   }

   //open stdout
   if((opt&OPT_MSK_O) && __stdout){
      //close stdin
      close(STDOUT_FILENO);
      //open keyboard
      if((fd[1]=open(__stdout,O_WRONLY,0))<0){
         fprintf(stderr,"error: cannot open %s\r\n",__stdout);
         return -1;
      }
   }

   //
   if((fd[0] = open("/dev/net/slip",O_RDONLY,0))<0){
      printf("error: cannot open device /dev/slip \r\n");
      return 0;
   }

   if((fd[1] = open("/dev/net/slip",O_WRONLY,0))<0){
      printf("error: cannot open device /dev/slip \r\n");
      return 0;
   }

   //mem replication device driver
   fd[2] = open("/dev/mem",O_RDWR,0);

   //link slip protocol driver and i/o device driver
   ioctl(fd[0],I_LINK,STDIN_FILENO);
   ioctl(fd[1],I_LINK,STDOUT_FILENO);

   if(!(opt&OPT_MSK_C)){
      // copy_from_cpu and copy_to_cpu mode
      //thread com
      #if __KERNEL_CPU_ARCH == CPU_ARCH_32
         thread_attr.stacksize = 2048/*512*/;//m16c 512
      #else
         thread_attr.stacksize = 512;//m16c 512
      #endif
      thread_attr.stackaddr = NULL;
      thread_attr.priority  = 100;
      thread_attr.timeslice = 1;

      //
      pthread_create(&pthread_memd, &thread_attr,memd_thread_copy_from_cpu,&fd);

      //
      memd_copy_to_cpu(fd,(opt&OPT_MSK_S));
   }else{
      //only copy_from_cpu mode
      memd_copy_from_cpu(fd);
   }

   return 0;
}

/*============================================
| End of Source  : memd.c
==============================================*/
