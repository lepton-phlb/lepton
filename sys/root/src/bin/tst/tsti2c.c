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


/*!
   \option
      -i i2c chip id
      -a address or command in chip
      -l data length
      -r test repetition 
      
   \brief

      
   \use
*/

/*============================================
| Includes    
==============================================*/
#include <stdlib.h>
#include "kernel/signal.h"
#include "kernel/libstd.h"
#include "kernel/devio.h"
#include "kernel/fcntl.h"
#include "kernel/wait.h"
#include "kernel/time.h"
#include "kernel/wait.h"
#include "kernel/fcntl.h"
#include "kernel/select.h"

#include "stdio/stdio.h"
#include "misc/prsopt.h"

/*============================================
| Global Declaration 
==============================================*/
#define CMD_MAX 64
#define I2C_BUFFER_MAX 128

static const char prompt[]="$>";


/*============================================
| Implementation 
==============================================*/
/*-------------------------------------------
| Name:cmd_line
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int cmd_line(char *buf, int len)
{
	int i = 0;
	signed char ch;

	while(1){

      if(read(0,&ch,1)<=0)
         ch='\0';

		//ch = getchar();

      //to do: up down key filter.
      if(i==CMD_MAX){
         buf[i-1]=0;
         return -1;
      }

		switch(ch){
      //
      case -1:
         buf[i] = 0;
         return i;

      case 127://DEL
		case   8://^H
			if(i>0){
            ch=8;//^H
            write(1,&ch,1);
            ch=' ';
            write(1,&ch,1);
            ch=8;//^H
            write(1,&ch,1);
				i --;
			}
			break;

      case '\0':
         buf[i] = 0;
         return i;
		

      case '\x18'://ctrl-x:exit
         write(1,"\r\n",2);
         exit(0);
      break;

      case '\n':
      break;

      case '\r':
	
			buf[i] = 0;
         write(1,"\r\n",2);
			return i;

		default:
			buf[i] = ch;
			putchar(ch);
			i++;
		}
	}
	
	return i;
}

/*--------------------------------------------
| Name:        hex2dec
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int hex2dec(char* v){
   int l;
   int i;
   int r=0;
   unsigned int d;
   unsigned int m;

   if((l=strlen(v))>4)
      return 0;

   for(i=0;i<l;i++){
      if(v[i]>='0' && v[i]<='9')
         d=v[i]-'0';
      else if(v[i]>='A' && v[i]<='F')
         d=v[i]-'A'+10;
      else if(v[i]>='a' && v[i]<='f')
         d=v[i]-'a'+10;

      if((l-(i+1))){
         m= (0x10<<(l-(i+2)));
         r+=(d*m);
      }else
         r+=d;
   }
      
      
   return r;
}

/*-------------------------------------------
| Name:tstlcd_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int tsti2c_main(int argc,char* argv[]){
   int fd;
   char buf[CMD_MAX+1];
   struct prsopt_t _prs_opt;
   struct opt_t* opt;

   int rw=0;
   unsigned int _opt=0;

   //i2c id
   unsigned char i2c_id;
   //i2c address size
   uint16_t  i2c_address_sz;
   //i2c address
   uint16_t  i2c_address;
   //i2c data length
   uint16_t i2c_data_length;
   //i2c data buffer
   uint8_t i2c_buffer[I2C_BUFFER_MAX];

   fd=open("/dev/i2c0",O_RDWR,0);
   if(fd<0){
      printf("error: cannot open /dev/i2c0\r\n");
      return 0;
   }

   //
   for(;;){

      write(1,prompt,sizeof(prompt)-1);
      if(cmd_line(buf,CMD_MAX)<=0)
         return 0;
    
      rw =0 ;
      opt= prsopt(&_prs_opt,buf,"isald|");
      while(opt){
         switch(opt->opt){
            //
            case 'i':
               i2c_id = hex2dec(opt->v);
               i2c_buffer[0] = (unsigned char) (i2c_id>>1);
            break;

            //
            case 'a':{
               int l= strlen(opt->v);
               if(l>4){
                  printf("error: invalid address\r\n");
                  break;
               }
               
               if(l<=2){
                  i2c_address_sz =1;
                  i2c_address = hex2dec(opt->v);
                  i2c_buffer[1] = (unsigned char) i2c_address_sz;
                  i2c_buffer[2] = (unsigned char) i2c_address;
               }else{
                  i2c_address_sz =2;
                  i2c_address = hex2dec(opt->v);
                  i2c_buffer[1] = (unsigned char) i2c_address_sz;
                  i2c_buffer[2] = (unsigned char) (i2c_address >> 8) ;
                  i2c_buffer[3] = (unsigned char) i2c_address ;
               }
            }
            break;

            case 'l':{
               i2c_data_length = atoi(opt->v);
            }

         }
         //next option
         opt= prsopt(&_prs_opt,0,"isald|");
      }

      //execute command
      if(!rw){
         //read i2c
         if(i2c_address_sz=1)
            printf("read 0x%x @ 0x%x len=%d...",i2c_id,i2c_buffer[2],i2c_data_length);
         else
            printf("read 0x%x @ 0x%x%x len=%d...",i2c_id,i2c_buffer[2],i2c_buffer[3],i2c_data_length);
         if(read(fd,i2c_buffer,i2c_data_length+i2c_address_sz+2)<0){
            printf("error!\r\n");
         }else{
            int i;
            for(i=0;i<i2c_data_length;i++)
               printf(" 0x%x",i2c_buffer[i2c_address_sz+2+i]);
            printf("\r\n");
         }
      }

   }

   return 0;
}
/*============================================
| End of Source  : tsti2c.c
==============================================*/
