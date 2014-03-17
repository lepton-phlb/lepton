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
#include <string.h>

/*===========================================
Global Declaration
=============================================*/
const char* const __ascii_arr_e[]={
   "00","01","02","03","04","05","06","07","08","09",
   "10","11","12","13","14","15","16","17","18","19",
   "20","21","22","23","24","25","26","27","28","29",
   "30","31","32","33","34","35","36","37","38","39"
};

char const __ascii_arr_unit[]={
   'p','p','p', //pico   -12
   'n','n','n', //nano   -9
   'u','u','u', //nano   -6
   'm','m','m', //milli  -3
   ' ',' ',' ',
   'K','K','K', //kilo   +3
   'M','M','M', //mega   +6
   'G','G','G', //giga   +9
   'P','P','P' //peta   +12
};


/*===========================================
Implementation
=============================================*/


char* ftoa(char* buf,float f){
   //ANSI/IEEE Standard 754-1985
   unsigned long l = *((long*)&f);
   unsigned long lmt=0;
   float pow10 = 1;
   float r;
   char dp=0;
   signed char e=0;
   char es = 0;
   signed char s;
   signed char d;
   signed char i=0;


   if( (s=((l&0x80000000) ? -1 : +1))<0 )
      buf[i++]='-';
   else
      buf[i++]='+';

   e= (signed char)((l&0x7F800000)>>23)-127;

   if(e>=0) {
      es = '+';
      pow10=(float)1e10;
   }else{
      es = '-';
      buf[i++]='0';
      pow10=(float)1e-1;
   }

   //e= abs( (((l&0x7F800000)>>23)-127) );

   e=0;
   //
   l =(l&(~0x80000000)); //inline fabs //f =fabs(f);
   memcpy(&f,&l,4);
   lmt = 1<<(10);
   while(  lmt ) {
      r=(f/pow10);

      if(!((int)pow10) && !dp) {
         dp=1;
         buf[i++]='.';
         e=0;
      }
      d = (char)r;
      if(d<0)
         break;

      if(i!=1 || d!=0 || !l) {
         lmt=lmt>>1;
         buf[i++]=d+48;
         f = f - d*pow10;
         if(!d)
            e++;
      }

      pow10=pow10/(float)1e1;

   }

   if(e>=12)
      e=e-12;
   else
      e=0;

   buf[i++]='e';
   buf[i++]=es;
   buf[i++]=__ascii_arr_e[e][0];
   buf[i++]=__ascii_arr_e[e][1];
   buf[i]='\0';

   return buf;
}

/*-------------------------------------------
| Name:ftoa2
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
char* ftoa2(char* buf,float f,char* unit){
   //ANSI/IEEE Standard 754-1985
   unsigned long l = *((long*)&f);
   unsigned long lmt=0;
   float pow10 = 1;
   float r;
   char dp=0;
   signed char e=0;
   char es = 0;
   signed char s;
   signed char d;
   signed char i=0;
   signed char u=0;
   signed char _u=0;



   if( (s=((l&0x80000000) ? -1 : +1))<0 )
      buf[i++]='-';
   else
      buf[i++]='+';

   if(!l) {
      buf[i++] = '0';
      buf[i++] = '.';
      buf[i++] = '0';
      buf[i++] = '\0';
      *unit = ' ';
      return 0;
   }

   e= (signed char)((l&0x7F800000)>>23)-127;

   if(e>=0) {
      es = '+';
      pow10=(float)1e10;
   }else{
      es = '-';
      pow10=(float)1e-1;
   }

   //
   e=0;
   //
   l =(l&(~0x80000000)); //inline fabs //f =fabs(f);
   memcpy(&f,&l,4);
   lmt = 1<<(10);
   buf[i]='\0';
   while(  lmt  ) {

      r=(f/pow10);

      ++_u;

      //
      if(buf[1] && !(_u%3) && !u)
         u=_u;

      d = (char)r;
      if(d<0)
         break;

      if(es=='-') {
         if(buf[1] || d || !l) {
            buf[i++]=d+48;
            lmt=lmt>>1;
         }

         if(!dp && (u && !(_u%3))  ) {
            dp=1;
            buf[i++]='.';
         }
      }else if(es=='+') {
         if(!dp && ( (u && !(_u%3)) || !(12-u) ) ) {
            dp=1;
            buf[i++]='.';
         }

         if(buf[1] || d || !l) {
            buf[i++]=d+48;
            lmt=lmt>>1;
         }
      }

      //
      f = f - d*pow10;
      if(!d)
         e++;


      pow10=pow10/(float)1e1;

   }

   //
   if(u>12)
      u=12;

   if(es=='+') {
      u=12-u; //10+1-u
   }else if(es=='-') {
      u=-u;
   }

   if(unit) {
      *unit = __ascii_arr_unit[12+u];
   }

   //
   buf[i]='\0';

   return buf;
}

/*-------------------------------------------
| Name:ftoa3
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
char* ftoa3(char* buf,float f,char* unit){
   //ANSI/IEEE Standard 754-1985
   unsigned long l = *((long*)&f);
   unsigned long lmt=0;
   float pow10 = 1;
   float r;
   char dp=0;
   signed char e=0;
   char es = 0;
   signed char s;
   signed char d;
   signed char i=0;
   signed char u=0;
   signed char _u=0;


   if( (s=((l&0x80000000) ? -1 : +1))<0 )
      buf[i++]='-';
   else
      buf[i++]='+';

   e= (signed char)((l&0x7F800000)>>23)-127;

   if(e>=0) {
      es = '+';
      pow10=(float)1e10;
   }else{
      es = '-';
      pow10=(float)1e0;
   }

   //
   e=0;
   //
   l =(l&(~0x80000000)); //inline fabs //f =fabs(f);
   memcpy(&f,&l,4);
   lmt = 1<<(10);
   while(  lmt  ) {
      r=(f/pow10);

      ++_u;

      d = (char)r;
      if(d<0)
         break;

      //
      if(d && !u) {
         if(es=='+')
            u=--_u;
         else if(es=='-')
            u=_u;
      }

      if(es=='-') {
         if(u || d || !l) {
            buf[i++]=d+48;
            lmt=lmt>>1;
         }

         if(u && !((10-_u)%3) && !dp) {
            dp=1;
            buf[i++]='.';
         }
      }else if(es=='+') {
         if(u && !((11-_u)%3) && !dp) {
            dp=1;
            buf[i++]='.';
         }

         if(u || d || !l) {
            buf[i++]=d+48;
            lmt=lmt>>1;
         }
      }

      //
      f = f - d*pow10;
      if(!d)
         e++;


      pow10=pow10/(float)1e1;

   }

   //
   if(u>12)
      u=12;

   if(es=='+') {
      u=11-u; //10+1-u
   }else if(es=='-') {
      u=-u+1; //-u+1
   }

   if(unit) {
      *unit = __ascii_arr_unit[u+12];
   }

   //
   buf[i]='\0';

   return buf;
}

//resolution conversion
int rcvt(char* buf,char* old_unit,char* new_unit){

   int old_r = 0;
   int new_r = 0;
   int dif_r = 0;

   //
   switch(old_unit[0]) {
   case 'p':
      old_r = -12;
      break;

   case 'n':
      old_r = -9;
      break;

   case 'u':
      old_r = -6;
      break;

   case 'm':
      old_r = -3;
      break;

   case ' ':
      old_r = 0;
      break;

   case 'K':
      old_r = 3;
      break;

   case 'M':
      old_r = 6;
      break;

   case 'G':
      old_r = 9;
      break;

   case 'P':
      old_r = 12;
      break;

   default:
      return -1;
   }

   //
   switch(new_unit[0]) {
   case 'p':
      new_r = -12;
      break;

   case 'n':
      new_r = -9;
      break;

   case 'u':
      new_r = -6;
      break;

   case 'm':
      new_r = -3;
      break;

   case ' ':
      new_r = 0;
      break;

   case 'K':
      new_r = 3;
      break;

   case 'M':
      new_r = 6;
      break;

   case 'G':
      new_r = 9;
      break;

   case 'P':
      new_r = 12;
      break;

   default:
      return -1;
   }

   dif_r = old_r - new_r;


   return 0;
}
/*===========================================
End of Source ftoa.c
=============================================*/
