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

/*!
   \option
      -w filter command line
      -f filter command file

   \brief
      display binary files with specific format
      and localisation.

   \use
      wraper -w [#2/fr.2%f#4] fbin
      display float 4 bytes size at 2 byte from begining of "fbin" binary file.

      wraper -f f1 fbin
      display all data from fbin binary file with  filter format file f1.

      sample:
         vln=['/var/rvolt'@10]\r\n
         [$v0=%f#4]
         [$v1=%f#4]
         [$v2=%f#4]
         [`$v0 1000 / $v0=`]
         vln  =[$v0]\r\n
         vlpe =[$v1]\r\n
         vnpe =[$v2]\r\n
         [`10 $l0=`]
         msg=[/fr.$l0]\r\n
*/

/*===========================================
Includes
=============================================*/
#include <stdlib.h>
#ifdef WRAPR_STDLN
   #include <io.h>
   #include <stdio.h>
   #include <fcntl.h>
   #include <string.h>
#else
   #include "kernel/core/signal.h"
   #include "kernel/core/libstd.h"
   #include "kernel/core/devio.h"
   #include "kernel/core/fcntl.h"
   #include "kernel/core/wait.h"

   #include "lib/libc/stdio/stdio.h"
   #include "lib/libc/misc/ftoa.h"
#endif

/*===========================================
Global Declaration
=============================================*/

#ifdef WRAPR_STDLN
   #define off_t long
   #define ftoa2(buf,f,u)
   #define ftoa(buf,f)
   #define PATH_MAX
char const wrapr_lex_file[]=".wrpr";
#else
char const wrapr_lex_file[]="/var/.wrpr";
#endif

//command line
#define  OPT_MSK_W 0x01 //-w filter command line.
#define  OPT_MSK_F 0x02 //-f filter command file.
#define  OPT_MSK_L 0x04 //-l fr,us,sp,en,it... set language.


//wrapr filter grammar
#define GET_SWITCH_ON             0 // [
#define GET_SWITCH_QUOT           1 // '
#define GET_SWITCH_RQUOT          2 // `
#define GET_OFFSET                3 // #
#define GET_POS                   4 // @
#define GET_LANG                  5 // /lang
#define GET_MSGNO                 6 // .number
#define GET_FORMAT                7 // %
#define GET_SIZE                  8 // #
#define GET_SWITCH_OFF            9 // ]
#define GET_SPECIAL_CODE_HEXA    10 // \xhhh
#define GET_SPECIAL_CODE_OCTAL   11 // \ooo
#define GET_VAR                  12 // $var name
#define GET_VAL                  13 // = value
#define PRINT_VAL                14 // [$val]
#define GET_FLAG                 15 // %(*+-)
#define GET_WIDTH                16 // %xxx
#define GET_PRECIS               17 // %.yyy
#define GET_TYPE                 18 // f,c,d,l...

typedef struct {
   int fd;
   int fd_lexem;
   unsigned int option;
   int line;
   int pos;
   char* lang;
}wrapr_t;

//npr
typedef union {
   long d;
   float f;
}v_t;

typedef struct {
   char type; //d:integer f:float c:string.
   v_t v;
}rpn_stack_t;

#define RPN_STACK_SIZE 5

/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:add_lexem
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int add_lexem(int fd, char* name,char type,char size,void* value){
   char i;
   lseek(fd,0,SEEK_END); //at end of file
   if(write(fd,name,strlen(name)+1)<0) // +1 for '\0'
      return -1;
   if(write(fd,&type,1)<0)
      return -1;
   if(write(fd,&size,1)<0)
      return -1;
   if(!value) {
      for(i=0; i<size; i++) {
         char c=0;
         if(write(fd,&c,1)<0)
            return -1;

      }
   }else{
      write(fd,value,size);
   }
   return 0;
}

/*-------------------------------------------
| Name:get_lexem
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int get_lexem(int fd, char* name,char* type,char* size,void* value){
   char c;
   char i=0;
   signed char st=0;

   //
   lseek(fd,0,SEEK_SET); //at begining of file

   //
   while(read(fd,&c,1)>0) {
      if(st<0 && c=='\0') { // go to next lexeme
         char _type;
         char _size;
         if(read(fd,&_type,1)<=0)
            return -1;
         if(read(fd,&_size,1)<=0)
            return -1;
         if(lseek(fd,_size,SEEK_CUR)<0)
            return -1;
         st=0;
         i=0;
      }else if(!st && c!=name[i++]) { //lexeme not match
         st=-1;
         i=0;
      }else if(!st && c=='\0') { //lexeme is match
         if(!type || read(fd,type,1)<=0)
            return -1;
         if(!size || read(fd,size,1)<=0)
            return -1;
         if(!value || read(fd,value,*size)<0)
            return -1;

         return 0;
      }
   }

   return -1;
}

/*-------------------------------------------
| Name:set_lexem
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int set_lexem(int fd, char* name,char type,void* value){
   char c;
   char i=0;
   signed char st=0;

   //
   lseek(fd,0,SEEK_SET); //at begining of file

   //
   while(read(fd,&c,1)>0) {
      if(st<0 && c=='\0') { // go to next lexeme
         char _type;
         char _size;
         if(read(fd,&_type,1)<=0)
            return -1;
         if(read(fd,&_size,1)<=0)
            return -1;
         if(lseek(fd,_size,SEEK_CUR)<0)
            return -1;
         st=0;
         i=0;
      }else if(!st && c!=name[i++]) { //lexeme not match
         st=-1;
         i=0;
      }else if(!st && c=='\0') { //lexeme is match
         char _type, _size;
         if(read(fd,&_type,1)<=0)
            return -1;
         if(type!=_type) {
            lseek(fd,-1,SEEK_CUR); // rewrite type value
            if(write(fd,&type,1)<=0)
               return -1;
         }
         if(read(fd,&_size,1)<=0)
            return -1;
         if(write(fd,value,_size)<0)
            return -1;

         return 0;
      }
   }

   return -1;
}

/*-------------------------------------------
| Name:rpn
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int rpn(wrapr_t* wrapr,const char* filter,char* var){
   char type='d';
   char st=0;
   char j=0;

   //npr
   rpn_stack_t rpn_stack[RPN_STACK_SIZE]={0};
   int rpn_stack_index=1;
   int i = wrapr->pos;

   rpn_stack[0].type = 'd';
   rpn_stack[0].v.d = 0;


   while(filter[i]!='`') {

      if(   (filter[i]>='0' && filter[i]<='9')
            || (filter[i]>='A' && filter[i]<='Z')
            || (filter[i]>='a' && filter[i]<='z')
            || (filter[i]=='_')
            || (filter[i]=='.')
            || (filter[i]=='$')) {  //get value

         if(filter[i]=='.')
            type='f';

         var[j++]=filter[i];
         var[j]='\0';

      }else {

         if(    filter[i]!=' '
                && filter[i]!='+'
                && filter[i]!='-'
                && filter[i]!='*'
                && filter[i]!='/'
                && filter[i]!='=') {
            wrapr->pos = i;
            return -1;
         }

         if(!j && filter[i]==' ') {
            i++;
            continue;
         }

         //
         j=0;

         //
         if(var[0]=='$') {
            char buf[4]={0};
            char* p=buf;
            int _type=0;
            int _sz=0;
            if(get_lexem(wrapr->fd_lexem,(char*)&var[1],(char*)&_type,(char*)&_sz,buf)<0) {
               _type = 0;
               if(add_lexem(wrapr->fd_lexem,(char*)&var[1],(char)_type,sizeof(v_t),buf)<0) {
                  wrapr->pos = i;
                  return -1;
               }
            }

            rpn_stack[rpn_stack_index].type=_type;
            if(_type=='f')
               rpn_stack[rpn_stack_index].v.f=*(float*)((unsigned long)(p));
            else if(_type=='d')
               rpn_stack[rpn_stack_index].v.d=*(long*)((unsigned long)(p));
            rpn_stack_index++;
         }else if(var[0]!='\0') {
            rpn_stack[rpn_stack_index].type=type;
            if(type=='f')
               rpn_stack[rpn_stack_index].v.f=(float)atof(var);
            else if(type=='d')
               rpn_stack[rpn_stack_index].v.d=atol(var);
            rpn_stack_index++;
         }
         var[0]='\0';
         type='d';
         if(filter[i]==' ') {
            i++;
            continue;
         }

         //calcul
         if(   rpn_stack[rpn_stack_index-2].type=='f'
               || rpn_stack[rpn_stack_index-1].type=='f') {
            float a;
            float b;
            if(rpn_stack[rpn_stack_index-2].type=='d')
               a = (float)rpn_stack[rpn_stack_index-2].v.d;
            else
               a = (float)rpn_stack[rpn_stack_index-2].v.f;

            if(rpn_stack[rpn_stack_index-1].type=='d')
               b = (float)rpn_stack[rpn_stack_index-1].v.d;
            else
               b = (float)rpn_stack[rpn_stack_index-1].v.f;

            rpn_stack[rpn_stack_index-2].type='f';

            switch(filter[i]) {
            case '+':
               rpn_stack[rpn_stack_index-2].v.f = a+b;
               //printf("result=%f\r\n",rpn_stack[rpn_stack_index-2].v.f);
               break;
            case '-':
               rpn_stack[rpn_stack_index-2].v.f = a-b;
               //printf("result=%f\r\n",rpn_stack[rpn_stack_index-2].v.f);
               break;
            case '*':
               rpn_stack[rpn_stack_index-2].v.f = a*b;
               //printf("result=%f\r\n",rpn_stack[rpn_stack_index-2].v.f);
               break;
            case '/':
               rpn_stack[rpn_stack_index-2].v.f = a/b;
               //printf("result=%f\r\n",rpn_stack[rpn_stack_index-2].v.f);
               break;
            case '=': {
               int _type=0;
               int _sz=0;
               char buf[4]={0};
               if(get_lexem(wrapr->fd_lexem,(char*)&var[1],(char*)&_type,(char*)&_sz,buf)<0) {
                  wrapr->pos = i;
                  return -1;
               }   //var not found

               /* if(_type!=0 && _type!=rpn_stack[rpn_stack_index-2].type)
                   return -1; //invalid type*/

               _type=rpn_stack[rpn_stack_index-2].type;

               if(set_lexem(wrapr->fd_lexem,(char*)&var[1],(char)_type,(char*)&a)<0) {
                  wrapr->pos = i;
                  return -1;
               }
            }
            break;

            default:
               break;
            }
         }else{
            int a = rpn_stack[rpn_stack_index-2].v.d;
            int b = rpn_stack[rpn_stack_index-1].v.d;

            switch(filter[i]) {
            case '+':
               rpn_stack[rpn_stack_index-2].v.d = a+b;
               //printf("result=%d\r\n",rpn_stack[rpn_stack_index-2].v.d);
               break;
            case '-':
               rpn_stack[rpn_stack_index-2].v.d = a-b;
               //printf("result=%d\r\n",rpn_stack[rpn_stack_index-2].v.d);
               break;
            case '*':
               rpn_stack[rpn_stack_index-2].v.d = a*b;
               //printf("result=%d\r\n",rpn_stack[rpn_stack_index-2].v.d);
               break;
            case '/':
               rpn_stack[rpn_stack_index-2].v.d = a/b;
               //printf("result=%d\r\n",rpn_stack[rpn_stack_index-2].v.d);
               break;
            case '=': {
               int _type=0;
               int _sz=0;
               char buf[4]={0};
               if(get_lexem(wrapr->fd_lexem,(char*)&var[1],(char*)&_type,(char*)&_sz,buf)<0) {
                  wrapr->pos = i;
                  return -1;
               }   //var not found

               /*if(_type!=0 && _type!=rpn_stack[rpn_stack_index-2].type)
                  return -1; //invalid type*/

               _type=rpn_stack[rpn_stack_index-2].type;

               if(set_lexem(wrapr->fd_lexem,(char*)&var[1],(char)_type,(char*)&a)<0) {
                  wrapr->pos = i;
                  return -1;
               }
            }
            break;

            default:
               break;
            }

         }
         if(rpn_stack_index>0)
            rpn_stack_index--;
         else // this not be valid.
            rpn_stack_index=0;

      }

      i++;
   }

   wrapr->pos = ++i;
   return 0;
}


/*-------------------------------------------
| Name:wrapr_parser
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int wrapr_parser(wrapr_t* wrapr, const char * filter){
   int i = 0;
   int j = 0;
   char var_buf[16]={0};
   char* var=var_buf;

   char format[8]={0};
   char size[8]={0};

   char buf[PATH_MAX]={0};
   char bin_buf[8];
   char* p = bin_buf;

   char * lang    = format;
   char * msg_no  = size;
   char * offset  = format;
   char * fname   = buf;

   int prev_st = GET_SWITCH_ON;
   int st = GET_SWITCH_ON;
   int unspec = 0;
   char type;


   if(!filter)
      return -1;

   while(filter[i]!='\0') {

      wrapr->pos=i;

      //
      if(!unspec && st==GET_SWITCH_ON && filter[i]=='~') {
         unspec = 1;
      }else if(unspec && st==GET_SWITCH_ON) {
         unspec=0;
         write(1,&filter[i],1);
      }else if(!unspec && st==GET_SWITCH_ON && filter[i]=='\\') {

         switch(filter[++i]) {
            char c;

         case 'a':   //\a Bell (alert)
            c ='\a';
            write(1,&c,1);
            break;

         case 'b':   //\b Backspace
            c ='\b';
            write(1,&c,1);
            break;

         case 'f':   //\f Formfeed
            c ='\f';
            write(1,&c,1);
            break;

         case 'n':   //\n New line
            c ='\n';
            write(1,&c,1);
            break;

         case 'r':   //\r Carriage return
            c ='\r';
            write(1,&c,1);
            break;

         case 't':   //\t Horizontal tab
            c ='\t';
            write(1,&c,1);
            break;

         case 'v':   //\v Vertical tab
            c ='\v';
            write(1,&c,1);
            break;

         case '\'':   //\' Single quotation mark
            c ='\'';
            write(1,&c,1);
            break;

         case '"':   //\"  Double quotation mark
            c ='"';
            write(1,&c,1);
            break;

         case '\\':   //\\ Backslash
            c ='\\';
            write(1,&c,1);
            break;

         case '?':   //\? Literal question mark
            c ='\?';
            write(1,&c,1);
            break;

         case '\0':
            return 0; //exit

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

      }else if(!unspec && st==GET_SPECIAL_CODE_HEXA) {

         if( (filter[i]>='0' && filter[i]<='9') && (j<2)) {
            buf[0]=(buf[0]<<4)+filter[i]-'0';
            j++;
         }else if( ((filter[i]>='a' && filter[i]<='f')) && (j<2)) {
            buf[0]=(buf[0]<<4)+filter[i]-'a'+10;
            j++;
         }else if( ((filter[i]>='A' && filter[i]<='F')) && (j<2)) {
            buf[0]=(buf[0]<<4)+filter[i]-'A'+10;
            j++;
         }else{
            write(1,&buf[0],1);
            st=GET_SWITCH_ON;
            i--;
         }

         //end of cmd line
         if(filter[i+1]=='\0') {
            write(1,&buf[0],1);
            st=GET_SWITCH_ON;
         }

      }else if(!unspec && st==GET_SPECIAL_CODE_OCTAL) {

         if( (filter[i]>='0' && filter[i]<='9') && (j<2)) {
            buf[0]=(buf[0]<<3)+filter[i]-'0';
            j++;
         }else{
            write(1,&buf[0],1);
            st=GET_SWITCH_ON;
            i--;
         }

         //end of cmd line
         if(filter[i+1]=='\0') {
            write(1,&buf[0],1);
            st=GET_SWITCH_ON;
         }

      }else if(!unspec && st== GET_SWITCH_ON && filter[i]=='[') {
         st = GET_FORMAT;
      }else if(!unspec
               && st!= GET_SWITCH_ON
               && st!= GET_SWITCH_RQUOT
               && st!= GET_VAR
               && filter[i]=='$') {
         prev_st = st;
         st = GET_VAR;
         j=0;
         var[j]='\0';
      }else if(!unspec && st== GET_FORMAT
               && (filter[i]=='%'
                   ||filter[i]=='/'
                   ||filter[i]=='#'
                   ||filter[i]=='@'
                   ||filter[i]=='\''
                   ||filter[i]=='`')) {

         if(filter[i]=='%') {
            st = GET_SIZE;
            j=0;
            format[j++]='%';
            format[j]='\0';
         }else if(filter[i]=='/') {
            st = GET_LANG;
            j=0;
            lang[j]='\0';
         }else if(filter[i]=='#') {
            st = GET_OFFSET;
            j=0;
            lang[j]='\0';
         }else if(filter[i]=='@') {
            st = GET_POS;
            j=0;
            lang[j]='\0';
         }else if(filter[i]=='\'') {
            st = GET_SWITCH_QUOT;
            j=0;
            fname[j]='\0';
         }else if(filter[i]=='`') {
            st = GET_SWITCH_RQUOT;
            j=0;
         }
      }else if(!unspec && st== GET_WIDTH) {
      }else if(!unspec && st== GET_PRECIS) {
      }else if(!unspec && st== GET_SIZE
               && (filter[i]=='#' || filter[i]==']')) {
         type = filter[i-1];
         if(filter[i]==']' && var[0]) {
            st = PRINT_VAL;
            i--;
         }else{
            st = GET_SWITCH_OFF;
         }
         j=0;
         size[j]='\0';
      }else if(!unspec && st== GET_SWITCH_RQUOT) {
         wrapr->pos = i;
         if(rpn(wrapr,filter,var_buf)<0)
            return -1;
         i=wrapr->pos;
         //next state
         if(filter[i]==']') {
            st = GET_SWITCH_ON;
         }else{
            st = GET_FORMAT;
            i--;
         }
         j=0;
      }else if(!unspec && st== GET_VAR) {
         if(   (filter[i]>='0' && filter[i]<='9')
               || (filter[i]>='A' && filter[i]<='Z')
               || (filter[i]>='a' && filter[i]<='z')
               || (filter[i]=='_') ) { //get value
            var[j++]=filter[i];
            var[j]='\0';
         } else {

            if(filter[i]==']' && prev_st==GET_FORMAT) {
               st = PRINT_VAL;
               i--;
            }else if(filter[i]=='=' && prev_st==GET_FORMAT) {
               st = prev_st;
               prev_st = GET_VAL;
            }else{
               st = prev_st;
               prev_st = GET_VAR;
               i--;
            }
            j=0;
         }
      }else if(!unspec && st== GET_SWITCH_QUOT) {
         if(filter[i]=='\'') {
            if(strlen(fname)) {
               if((wrapr->fd=open(fname,O_RDONLY,0))<0)
                  return -1;
            }
            //next state
            j=0;
            if(filter[++i]==']') {
               st = GET_SWITCH_ON;
            }else{
               st = GET_FORMAT;
               i--;
            }
         }else{ //get value
            fname[j++] = filter[i];
            fname[j] = '\0';
         }
      }else if(!unspec && (st== GET_OFFSET || st== GET_POS)) {
         //interpret command ans value
         if( filter[i]<'0'
             || filter[i]>'9'
             && (filter[i] !='+'
                 || filter[i] !='-')) {
            int ofs = atoi(offset);
            //
            if(st== GET_OFFSET ) {
               if(lseek(wrapr->fd,(off_t)ofs,SEEK_CUR)<0)
                  return -1;
            }else if(st== GET_POS ) {
               if(lseek(wrapr->fd,(off_t)ofs,SEEK_SET)<0)
                  return -1;
            }

            //next state
            if(filter[i]==']') {
               st = GET_SWITCH_ON;
            }else{
               st = GET_FORMAT;
               i--;
            }
            j=0;
         }else{ //get value
            offset[j++] = filter[i];
            offset[j] = '\0';
         }

      }else if(!unspec && st== GET_LANG) {
         if(filter[i]!='.') {
            lang[j++] = filter[i];
            lang[j]='\0';
         }else{
            st = GET_MSGNO;
            j=0;
            msg_no[j]='\0';
         }

      }else if(!unspec && st== GET_MSGNO) {
         if( filter[i]<'0' || filter[i]>'9') {
            int msg_nb;
            int nb = 0;
            char* msg=0;
            FILE  *stream;

            //get msg number
            if((prev_st == GET_VAR) && strlen(var)) {
               char _type=0;
               char _sz=0;
               prev_st = GET_MSGNO;
               //reset temporary buffer
               memset(bin_buf,0,sizeof(bin_buf));
               if(get_lexem(wrapr->fd_lexem,var,&_type,&_sz,bin_buf)<0)
                  return -1;  //var not exist
               if(_type=='f' || _type=='s')
                  return -1;  //invalid type

               msg_nb = *(int*)((unsigned long)(p));
            }else{
               msg_nb = atoi(msg_no);
            }

            if(msg_nb<=0)
               return -1;

            //get lang
            strcpy(buf,"/usr/share/locale/");

            if(wrapr->lang)
               strcat(buf,wrapr->lang);
            else
               strcat(buf,lang);

            strcat(buf,"/msr.m");
            if( !(stream = fopen( buf, "r" )))
               return -1;
            //get msg
            while(fscanf( stream, "%d:%s",&nb,buf)>=2) {
               if(nb!=msg_nb) continue;

               msg =buf;
               break;
            }

            //output
            if(msg) {
               int _cb=0;
               unsigned char _l = strlen(msg);
               while((_cb+=write(1,msg+_cb,_l-_cb))<_l) ;
            }
            //next state
            if(filter[i]==']') {
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
      }else if(!unspec
               && (st== GET_SWITCH_OFF ||  st== PRINT_VAL)
               && (filter[i]==']')) {
         int sz = atoi(size);
         int r=0;

         //reset temporary buffer
         memset(bin_buf,0,sizeof(bin_buf));

         //fill bin buffer
         if(st==PRINT_VAL && strlen(var)) {
            if(get_lexem(wrapr->fd_lexem,var,(char*)&type,(char*)&sz,bin_buf)<0)
               return -1;  //var not declare
            if(!format[0]) {
               format[0]='%';
               format[1]=type;
               format[2]='\0';
            }
         }else{
            //read   offset+=sz;
            if((r=read(wrapr->fd,bin_buf,sz))<0)
               return -1;
         }

         st = GET_SWITCH_ON;

         //
         if(prev_st== GET_VAL && strlen(var)) {
            int _type=0;
            int _sz=0;
            if(!get_lexem(wrapr->fd_lexem,var,(char*)&_type,(char*)&_sz,(char*)0)) {
               if(type!=_type || sz!=_sz) //already exist and redefinition
                  return -1;
               set_lexem(wrapr->fd_lexem,var,(char)type,bin_buf);
            }else{
               add_lexem(wrapr->fd_lexem,var,(char)type,(char)sz,bin_buf);
            }

         }else{
            switch(type) {
            case 'c':    //int single-byte character; when used with wprintf functions, specifies a wide character.
            case 'C':    //int wide character; when used with wprintf functions, specifies a single-byte character.
            case 'd':    //int Signed decimal integer.
            case 'i':    //int Signed decimal integer.
            case 'o':    //int Unsigned octal integer.
            case 'u':    //int Unsigned decimal integer.
            case 'x':    //int Unsigned hexadecimal integer, using abcdef.
            case 'X':    //int Unsigned hexadecimal integer, using ABCDEF.
            case 'l':
            {
               long l = 0;
               switch(sz) {
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
               if(type=='l')
                  sprintf(buf,"%ld",l);
               else
                  sprintf(buf,format,l);


            } break;

            case 'f':    //float
            {
               float f;
               switch(sz) {
               case 4:
               case 8:
                  f = *(float*)((unsigned long)(p));
                  break;

               default:
                  return -1;
               }
               //
               sprintf(buf,format,f);
            }
            break;

            case 'e':    //double
            case 'E':    //double
            case 'F':    //double formatted
            case 'g':    //double
            case 'G':    //double
            {
               double f;

               switch(sz) {
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
               if(type=='F') {
                  char u;
                  char len;
                  ftoa2(buf,(float)f,&u);
                  len=strlen(buf);
                  buf[len++]=' ';
                  buf[len++]=u;
                  buf[len++]='\0';
               }else{
                  ftoa(buf,(float)f);
               }

            } break;

            case 'n':    //Pointer
            case 'p':    //Pointer
            case 's':    //String
            case 'S':    //String
            {
               sprintf(buf,format,p);
            } break;
            }

            //output
            {
               int _cb=0;
               unsigned char _l = strlen(buf);
               while((_cb+=write(1,buf+_cb,_l-_cb))<_l) ;
            }
            //write(1,buf,strlen(buf));
         }

      }else if(st == GET_SWITCH_ON) {
         if(filter[i]!='\r'
            && filter[i]!='\n')
            write(1,&filter[i],1);
      }else if(st!= GET_SWITCH_ON) {
         switch(st) {
         case GET_SIZE:   //get format string
            format[j++]=filter[i];
            format[j]='\0';
            break;
         case GET_SWITCH_OFF:   //get size
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
typedef struct {
   float f;
   short d;
   char s1[15];
   //char  s2[5];
   short c;
   char s3[5];
   //char  s4[5];
}m_tst_t;

m_tst_t const m_tst = {(float)0.547,25,"str1",/*"str2",*/ '2',"str3"}; //,"str4"};

#ifdef WRAPR_STDLN
int main(int argc,char* argv[]){
#else
int wrapr_main(int argc,char* argv[]){
#endif

   int i;
   unsigned int opt=0;
   int fd_wrapr;
   char * wrapr_filter=(char*)0;
   wrapr_t wrapr={0};
#ifdef WRAPR_STDLN
   int fd;
   if((fd=open("./wtst",O_CREAT|O_RDWR,0777))>=0) {
      write(fd,&m_tst,sizeof(m_tst));
      close(fd);
   }
#endif

   //get option
   for(i=1; i<argc; i++) {
      if(argv[i][0]=='-') {
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1; c<l; c++) {
            switch(argv[i][c]) {
            case 'w':
               opt |= OPT_MSK_W;
               //command line filter
               if((i+1) == argc)   //not enough parameter
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
               //filter file path
               if((i+1) == argc)   //not enough parameter
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

            case 'l':
               opt |= OPT_MSK_L;
               //set language (fr,us,sp,en,it...)
               if((i+1) == argc)   //not enough parameter
                  return -1;
               if(argv[i][c+1])
                  break;
               if(argv[i+1][0]=='-')
                  break;
               //
               i++;
               if(!argv[i])
                  return -1;
               wrapr.lang = argv[i];

               break;

            }
         }
      }else{
         if(!argv[i])
            return -1;
         if((wrapr.fd = open(argv[i],O_RDONLY,0))<0)
            return -1;
         lseek(wrapr.fd,(off_t)0,SEEK_SET);
      }
   }

   //lexem file
   if((wrapr.fd_lexem = open(wrapr_lex_file,O_CREAT|O_RDWR|O_TRUNC,0777))<0)
      return -1;
   //
   if(opt&OPT_MSK_W) {

      if(wrapr_parser(&wrapr,wrapr_filter)<0)
         return -1;

   }else if(opt&OPT_MSK_F) {

      int r = 0;
      char c = 0;
      char wrapr_filter_buf[64]={0};
      wrapr.line = 0;
      wrapr.pos  = 0;

      for(i=0; i<sizeof(wrapr_filter_buf); i++) {
         c='\0';
         r=read(fd_wrapr,&c,1);
         if(r<=0)
            c='\0';
         wrapr_filter_buf[i]=c;
         if(c=='\r' || c=='\n' || c=='\0') {
            wrapr_filter_buf[i]='\0';
            i=-1; //cause "i++" in for instruction.
            if(strlen(wrapr_filter_buf)) {
               wrapr.line++;
               if(wrapr_parser(&wrapr,wrapr_filter_buf)<0) {
                  fprintf(stderr,"error at line %d pos %d\r\n",wrapr.line,wrapr.pos+1);
                  return -1;
               }
            }
         }

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
