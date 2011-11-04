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
#include <stdlib.h>
#include "kernel/signal.h"
#include "kernel/libstd.h"
#include "kernel/devio.h"
#include "kernel/fcntl.h"
#include "kernel/wait.h"

#include "stdio/stdio.h"

#include "misc/ftoa.h"

/*===========================================
Global Declaration
=============================================*/
//command line
#define  OPT_MSK_W 0x01 //-w filter command line
#define  OPT_MSK_F 0x02 //-f filter command file


//wrapr filter grammar
#define GET_SWITCH_ON            0 //[
#define GET_OFFSET               1 // #
#define GET_LANG                 2 // /lang
#define GET_MSGNO                3 // .number
#define GET_FORMAT               4 // %
#define GET_SIZE                 5 // #
#define GET_SWITCH_OFF           6 // [
#define GET_SPECIAL_CODE_HEXA    7//\xhhh
#define GET_SPECIAL_CODE_OCTAL   8//\ooo


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:wrapr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int wrapr(int fd, const char * filter){
   int i = 0;
   int j = 0;
   char  format[8]={0};
   char  size[8]={0};
   
   char * lang    = format;
   char * msg_no  = size;
   char * offset  = format;

   char  buf[PATH_MAX]={0};
   char  bin_buf[8];
   char* p = bin_buf;

   int st = GET_SWITCH_ON;
   int unspec = 0;
   char type;
   
   
   if(!filter) 
      return -1;

   while(filter[i]!='\0'){
          
      if(!unspec && st==GET_SWITCH_ON && filter[i]=='~'){
               unspec = 1;
      }else if(unspec && st==GET_SWITCH_ON){
         unspec=0;
         write(STDOUT_FILENO,&filter[i],1);
      }else if(!unspec && st==GET_SWITCH_ON && filter[i]=='\\'){

         switch(filter[++i]){
            char c;

            case 'a'://\a Bell (alert) 
               c ='\a';
               write(STDOUT_FILENO,&c,1);
            break;

            case 'b'://\b Backspace 
               c ='\b';
               write(STDOUT_FILENO,&c,1);
            break;

            case 'f'://\f Formfeed 
               c ='\f';
               write(STDOUT_FILENO,&c,1);
            break;

            case 'n'://\n New line 
               c ='\n';
               write(STDOUT_FILENO,&c,1);
            break;

            case 'r'://\r Carriage return 
               c ='\r';
               write(STDOUT_FILENO,&c,1);
            break;

            case 't'://\t Horizontal tab 
               c ='\t';
               write(STDOUT_FILENO,&c,1);
            break;

            case 'v'://\v Vertical tab 
               c ='\v';
               write(STDOUT_FILENO,&c,1);
            break;

            case '\''://\' Single quotation mark 
               c ='\'';
               write(STDOUT_FILENO,&c,1);
            break;

            case '"'://\"  Double quotation mark 
               c ='"';
               write(STDOUT_FILENO,&c,1);
            break;

            case '\\'://\\ Backslash 
               c ='\\';
               write(STDOUT_FILENO,&c,1);
            break;

            case '?'://\? Literal question mark 
               c ='\?';
               write(STDOUT_FILENO,&c,1);
            break;

            case '\0':
            return 0;//exit

            case 'x':
               st=GET_SPECIAL_CODE_HEXA;
               j=0;
            break;

            default:
               st=GET_SPECIAL_CODE_OCTAL;
               j=0;
               i--;
            break;
         
         }

         //
         buf[0]=0;
        
      }else if(!unspec && st==GET_SPECIAL_CODE_HEXA){

         if( (filter[i]>='0' && filter[i]<='9') && (j<2)){ 
            buf[0]=(buf[0]<<4)+filter[i]-'0';
            j++;
         }else if( ((filter[i]>='a' && filter[i]<='f')) && (j<2)){ 
            buf[0]=(buf[0]<<4)+filter[i]-'a'+10;
            j++;
         }else if( ((filter[i]>='A' && filter[i]<='F')) && (j<2)){ 
            buf[0]=(buf[0]<<4)+filter[i]-'A'+10;
            j++;
         }else{
            write(STDOUT_FILENO,&buf[0],1);
            st=GET_SWITCH_ON;
            i--;
         }

         //end of cmd line
         if(filter[i+1]=='\0'){
            write(STDOUT_FILENO,&buf[0],1);
            st=GET_SWITCH_ON;
         }
         
      }else if(!unspec && st==GET_SPECIAL_CODE_OCTAL){

         if( (filter[i]>='0' && filter[i]<='9') && (j<2)){ 
            buf[0]=(buf[0]<<3)+filter[i]-'0';
            j++;
         }else{
            write(STDOUT_FILENO,&buf[0],1);
            st=GET_SWITCH_ON;
            i--;
         }

         //end of cmd line
         if(filter[i+1]=='\0'){
            write(STDOUT_FILENO,&buf[0],1);
            st=GET_SWITCH_ON;
         }
         
      }else if(!unspec && st== GET_SWITCH_ON && filter[i]=='['){
         st = GET_FORMAT;
      }else if(!unspec && st== GET_FORMAT 
               && (filter[i]=='%'||filter[i]=='/'||filter[i]=='#')){

         if(filter[i]=='%'){
            st = GET_SIZE;
            j=0;
            format[j++]='%';
            format[j]='\0';
         }else if(filter[i]=='/'){
            st = GET_LANG;
            j=0;
            lang[j]='\0';
         }else if(filter[i]=='#'){
            st = GET_OFFSET;
            j=0;
            lang[j]='\0';
         }

      
      }else if(!unspec && st== GET_OFFSET){
         if( filter[i]<'0' 
            || filter[i]>'9' 
            && (filter[i] !='+'
            || filter[i] !='-')) {
            int ofs = atoi(offset);
            //
            lseek(fd,(off_t)ofs,SEEK_CUR);
            //next state
            if(filter[i]==']'){
               st = GET_SWITCH_ON;
            }else{
               st = GET_FORMAT;
               i--;
            }
            j=0;
         }else{
            offset[j++] = filter[i];
            offset[j] = '\0';
         }
         
      }else if(!unspec && st== GET_LANG){
         if(filter[i]!='.'){
            lang[j++] = filter[i];
            lang[j]='\0';
         }else{
            st = GET_MSGNO;
            j=0;
            msg_no[j]='\0';
         }
         
      }else if(!unspec && st== GET_MSGNO){
         if( filter[i]<'0' || filter[i]>'9') {
            int msg_nb;
            int nb = 0;
            char* msg=0;
            FILE  *stream;
            
            //get msg number
            msg_nb = atoi(msg_no);

            if(msg_nb<=0)
               return -1;
            
            //get lang
            strcpy(buf,"/usr/share/locale/");
            strcat(buf,lang);
            strcat(buf,"/msr.m");
            if( !(stream = fopen( buf, "r" )))
               return -1;
            //get msg
            while(fscanf( stream, "%d:%s",&nb,buf)==2){
               if(nb!=msg_nb) continue;

               msg =buf;
               break;
            }

            //output
            if(msg)
               write(STDOUT_FILENO,msg,strlen(msg));
            //next state            
            if(filter[i]==']'){
               st = GET_SWITCH_ON;
            }else{
               st = GET_FORMAT;
               i--;
            }
            j=0;
            fclose(stream);
         }else {
            msg_no[j++] = filter[i];
         }
         msg_no[j]='\0';
      }else if(!unspec && st== GET_SIZE && filter[i]=='#'){
         type = filter[i-1];
         st = GET_SWITCH_OFF;
         j=0;
         size[j]='\0';
      }else if(!unspec && st== GET_SWITCH_OFF && filter[i]==']'){
         int sz = atoi(size);
         int r;
         st = GET_SWITCH_ON;

         //reset temporary buffer
         memset(bin_buf,0,sizeof(bin_buf));
         //read   offset+=sz;
         if((r=read(fd,bin_buf,sz))<0)
            return -1;

         //
         switch(type){
            case 'c': //int single-byte character; when used with wprintf functions, specifies a wide character. 
            case 'C': //int wide character; when used with wprintf functions, specifies a single-byte character. 
            case 'd': //int Signed decimal integer. 
            case 'i': //int Signed decimal integer. 
            case 'o': //int Unsigned octal integer. 
            case 'u': //int Unsigned decimal integer. 
            case 'x': //int Unsigned hexadecimal integer, using abcdef. 
            case 'X': //int Unsigned hexadecimal integer, using ABCDEF.
            {
               long l = 0;
               switch(sz){
                  case 1:
                     l = *(char*)((unsigned long)(p));
                  break;

                  case 2:
                     l = *(short*)((unsigned long)(p));
                  break;

                  case 4:
                     l = *(long*)((unsigned long)(p));
                  break;

                  default:
                  return -1;
               }
               sprintf(buf,format,l);

            }break;

            case 'e': //double 
            case 'E': //double 
            case 'f': //double 
            case 'g': //double 
            case 'G': //double 
            {
               double f;

               switch(sz){
                  case 4:
                     f = *(float*)((unsigned long)(p));
                  break;

                  case 8:
                     f = *(double*)((unsigned long)(p));
                  break;

                  default:
                  return -1;
               }
               // %f not yet supported on lepton's stdio
               //sprintf(buf,format,f);
               ftoa(buf,(float)f);

            }break;

            case 'n': //Pointer 
            case 'p': //Pointer 
            case 's': //String  
            case 'S': //String 
            {
               sprintf(buf,format,p);
            }break;
         }

         write(STDOUT_FILENO,buf,strlen(buf));
               
      }else if(st == GET_SWITCH_ON){
         write(STDOUT_FILENO,&filter[i],1);
      }else if(st!= GET_SWITCH_ON){
         switch(st){
            case GET_SIZE://get format string
               format[j++]=filter[i];
               format[j]='\0';
            break;
            case GET_SWITCH_OFF://get size
               size[j++]=filter[i];
               size[j]='\0';
            break;
         }
      }

      i++;
   }

   //
   if(st!=GET_SWITCH_ON)
      return -1;

   return 0;
}

/*-------------------------------------------
| Name:main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int wrapr_main(int argc,char* argv[]){

   int i;
   unsigned int opt=0;
   int fd_bin;
   int fd_wrapr;
   char * wrapr_filter=(char*)0;
   
   //get option
   for(i=1;i<argc;i++){
      if(argv[i][0]=='-'){
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1;c<l;c++){
            switch(argv[i][c]){
               case 'w':
                  opt |= OPT_MSK_W;
                  //get rotary switch position
                  if((i+1) == argc)//not enough parameter
                     return -1;
                  if(argv[i][c+1])
                     break;
                  if(argv[i+1][0]=='-')
                     break;
                  //
                  i++;
                  if(argv[i])
                     wrapr_filter = argv[i];
                 
               break;

               case 'f':
                  opt |= OPT_MSK_F;
                  //get stdin device
                  if((i+1) == argc)//not enough parameter
                     return -1;
                  if(argv[i][c+1])
                     break;
                  if(argv[i+1][0]=='-')
                     break;
                  //
                  i++;
                  if(!argv[i])
                     return -1;
                  if((fd_wrapr = open(argv[i],O_RDONLY,0))<0)
                     return -1;
                  lseek(fd_wrapr,(off_t)0,SEEK_SET);
                  
               break;
               
            }
         }
      }else{
         if(!argv[i])
            return -1;
         if((fd_bin = open(argv[i],O_RDONLY,0))<0)
            return -1;
         lseek(fd_bin,(off_t)0,SEEK_SET);
      }
   }

   //
   if(opt&OPT_MSK_W){

      if(wrapr(fd_bin,wrapr_filter)<0)
         return -1;

   }else if(opt&OPT_MSK_F){

      int   r = 0;
      char  c = 0;
      static char wrapr_filter_buf[64]={0};

      for(i=0;i<sizeof(wrapr_filter_buf);i++){
         c='\0';
         r=read(fd_wrapr,&c,1);
         
         wrapr_filter_buf[i]=c;
         if((c=='\n' || c=='\0') && (r>0))
            if(wrapr(fd_bin,wrapr_filter_buf)<0)
               return -1;

         if(r<=0)
            return 0;
      }
   }

   //
   

   return 0;
}

/*===========================================
End of Source wrapr.c
=============================================*/
