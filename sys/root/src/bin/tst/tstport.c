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
      no option available.
      
   \brief
      control m16c output port,serial ouput, relay (monostable,bistable).
      
   \use
      prompt:
      $>
      //[portype (p|o|r)],[portnumber (1 to ...|b|m)],[bit number (1 to ...)]"},

      -t [port type (p|o|r)] 
      p:m16c 'p'ort
      o:serial 'o'utput port
      r:'r'elay

      -n [portnumber (1 to ...|b|m)] 
      1 to ...: for 'p'ort and serial 'o'utput
      or 'r'elay type: b (bistable), m (monostable).

      -b [bit number (1 to ...)]
      or relay number.

      -v [value 0 | >=1]

      ex: -t r -n b -b 1 -v 1 --> close bistable relais 1


      type ctrl-x to exit.

*/
/*===========================================
Includes
=============================================*/
#include <stdlib.h>
#include "kernel/devio.h"
#include "kernel/libstd.h"
#include "stdio/stdio.h"
#include "misc/prsopt.h"



/*===========================================
Global Declaration
=============================================*/
#define CMD_MAX 64

static const char prompt[]="$>";

#pragma pack(push, 1)
typedef struct {
   unsigned char t;//type
   unsigned char p;//port
   unsigned char b;//bit
   unsigned char v;//value
}gpio_t;
#pragma pack(pop)

/*===========================================
Implementation
=============================================*/


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
	char ch;

	while(1){

		ch = getchar();
      //to do: up down key filter.

		switch(ch){

      case 127://DEL
		case   8://^H
			if(i>0){
				putchar(8);//^H
            putchar(' ');//^H
            putchar(8);//^H
				i --;
			}
			break;
/*
		case  8 :
			if(i>0){
				putchar(ch);
				i --;
			}
			break;
*/

      case '\x18'://ctrl-x:exit
         putchar('\r');
			putchar('\n');
         exit(0);
      break;

      case '\r':
		case '\n':
			buf[i] = 0;
         putchar('\r');
			putchar('\n');
			return i;
     
		default:
			buf[i] = ch;
			putchar(ch);
			i++;
		}
	}
	
	return 0;
}

/*-------------------------------------------
| Name:tstlcd_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int tstport_main(int argc,char* argv[]){
   char buf[CMD_MAX+1]={0};
   struct prsopt_t _prs_opt;
   struct opt_t* opt;
   gpio_t gpio;

   int fd;
      
   //
   buf[0]='\0';

   //
   if((fd=open("/dev/gpio0",O_WRONLY,0))<0){
      printf("error: cannot open gpio device\r\n");
      return -1;
   }
    
   //
   for(;;){

      //
      if(argc>1){
         int i;
         for(i=1;i<argc;i++){
            strcat(buf,argv[i]);
            strcat(buf," ");
         }
      }else{
         //interactive mode
         printf(prompt);
         cmd_line(buf,CMD_MAX);
      }
        
      //get option
      opt= prsopt(&_prs_opt,buf,"tnbv");
      while(opt){
         //printf("o=%c v=%s\r\n",opt->opt,opt->v);
         switch(opt->opt){
            case 't':
               //port type
               gpio.t = opt->v[0];//('p'|'o'|'r')
            break;

            case 'n':
               //port number or relay type 'm':monostable 'b': bistable
               if(gpio.t=='r')
                  gpio.p= opt->v[0];//'m' or 'b'
               else
                  gpio.p = atoi(opt->v);
            break;

            case 'b'://bit or relay number
               gpio.b = atoi(opt->v);
            break;

            case 'v':
               gpio.v = atoi(opt->v);
            break;
         }

         //next option
         opt= prsopt(&_prs_opt,0,"tnbv");
      }

      //run command
      if(lseek(fd,0,SEEK_SET)<0){
         printf("error: cannot access gpio device\r\n");
      }
      if(write(fd,&gpio,sizeof(gpio))<0){
         printf("error: cannot write on gpio device\r\n");
      }
      //verbose
      if(gpio.v){
         (gpio.t=='r') ?\
            printf("setport t:%c,p:%c,b:%d\r\n",\
                     gpio.t,\
                     gpio.p,\
                     gpio.b):\
            printf("setport t:%c,p:%d,b:%d\r\n",\
                     gpio.t,\
                     gpio.p,\
                     gpio.b);
         
      }else{
         (gpio.t=='r') ?\
            printf("clrport t:%c,p:%c,b:%d\r\n",\
                     gpio.t,\
                     gpio.p,\
                     gpio.b):\
            printf("clrport t:%c,p:%d,b:%d\r\n",\
                     gpio.t,\
                     gpio.p,\
                     gpio.b);
      }

      //interactive mode
      if(argc>1)
         return 0;//no
      
   }

   return 0;
}

/*===========================================
End of Sourcetstport.c
=============================================*/
