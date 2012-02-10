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
#include <stdlib.h>
#include "kernel/core/signal.h"
#include "kernel/core/libstd.h"
#include "kernel/core/devio.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/wait.h"

#include "lib/libc/stdio/stdio.h"

/*
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
*/


/*===========================================
Global Declaration
=============================================*/
typedef struct {
   char bflag;
   char cflag;
   char dflag;
   char oflag;
   char xflag;
   char hflag;
   char vflag;
   int linenr;
   int width;
   int state;
   int ever;
   int prevwds[8];
   long off;
   char buf[128];
   char buffer[BUFSIZ];
   int next;
   int bytespresent;
}od_state_t;

#define bflag              od_state->bflag
#define cflag              od_state->cflag
#define dflag              od_state->dflag
#define oflag              od_state->oflag
#define xflag              od_state->xflag
#define hflag              od_state->hflag
#define vflag              od_state->vflag
#define linenr             od_state->linenr
#define width              od_state->width
#define state              od_state->state
#define ever               od_state->ever
#define prevwds            od_state->prevwds
#define off                od_state->off
#define buf                od_state->buf
#define buffer             od_state->buffer
#define next               od_state->next
#define bytespresent       od_state->bytespresent

/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:usage
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static void usage()
{
   fprintf(stderr, "Usage: od [-bcdhovx] [file] [ [+] offset [.] [b] ]\r\n");
}

/*-------------------------------------------
| Name:hexit
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static char hexit(int k){
   if (k <= 9)
      return('0' + k);
   else
      return('A' + k - 10);
}

/*-------------------------------------------
| Name:addrout
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void addrout(od_state_t* od_state, long l){
   int i;

   if (hflag == 0) {
      for (i = 0; i < 7; i++)
         printf("%c", (int) ((l >> (18 - 3 * i)) & 07) + '0');
   } else {
      for (i = 0; i < 7; i++)
         printf("%c", hexit((int) ((l >> (24 - 4 * i)) & 0x0F)));
   }
}

/*-------------------------------------------
| Name:outnum
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void outnum(int num, int radix)
{
/* Output a number with all leading 0s present.  Octal is 6 places,
 * decimal is 5 places, hex is 4 places.
 */
   int d, i;
   unsigned val;
   char s[8];

   val = (unsigned) num;
   if (radix == 8)
      d = 6;
   else if (radix == 10)
      d = 5;
   else if (radix == 16)
      d = 4;
   else if (radix == 7) {
      d = 3;
      radix = 8;
   }
   for (i = 0; i < d; i++) {
      s[i] = val % radix;
      val -= s[i];
      val = val / radix;
   }
   for (i = d - 1; i >= 0; i--) {
      if (s[i] > 9)
         printf("%c", 'a' + s[i] - 10);
      else
         printf("%c", s[i] + '0');
   }
}

/*-------------------------------------------
| Name:outword*
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void outword(od_state_t* od_state, int val, int radix){
/* Output 'val' in 'radix' in a field of total size 'width'. */

   int i;

   if (radix == 16) i = width - 4;
   if (radix == 10) i = width - 5;
   if (radix == 8) i = width - 6;
   if (i == 1)
      printf(" ");
   else if (i == 2)
      printf("  ");
   else if (i == 3)
      printf("   ");
   else if (i == 4)
      printf("    ");
   outnum(val, radix);
}

/*-------------------------------------------
| Name:same
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int same(short *w1,int *w2){
   int i;
   i = 8;
   while (i--)
      if (*w1++ != *w2++) return(0);
   return(1);
}

/*-------------------------------------------
| Name:getwords
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int getwords(od_state_t* od_state,short **words)
{
   int count;

   if (next >= bytespresent) {
      bytespresent = read(0, buf, sizeof(buf));
      next = 0;
   }
   if (next >= bytespresent) return(0);
   *words = (short *) &buf[next];
   if (next + 16 <= bytespresent)
      count = 16;
   else
      count = bytespresent - next;

   next += count;
   return(count);
}

/*-------------------------------------------
| Name:byte
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void byte(int val,char c){
   if (c == 'b') {
      printf(" ");
      outnum(val, 7);
      return;
   }
   if (val == 0)
      printf("  \\0");
   else if (val == '\b')
      printf("  \\b");
   else if (val == '\f')
      printf("  \\f");
   else if (val == '\n')
      printf("  \\n");
   else if (val == '\r')
      printf("  \\r");
   else if (val == '\t')
      printf("  \\t");
   else if (val >= ' ' && val < 0177)
      printf("   %c", val);
   else {
      printf(" ");
      outnum(val, 7);
   }
}

/*-------------------------------------------
| Name:bdump
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void bdump(od_state_t* od_state,char bytes[16],int k,char c){
   int i;

   if (linenr++ != 1) printf("       ");
   for (i = 0; i < k; i++) byte(bytes[i] & 0377, c);
   printf("\r\n");
}

/*-------------------------------------------
| Name:wdump
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void wdump(od_state_t* od_state,short *words,int k, int radix)
{
   int i;

   if (linenr++ != 1) printf("       ");
   for (i = 0; i < (k + 1) / 2; i++) outword(od_state,words[i], radix);
   printf("\r\n");
}

/*-------------------------------------------
| Name:dumpfile
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static void dumpfile(od_state_t* od_state){

   int k;
   short *words;

   while ((k = getwords(od_state,&words))) {    /* 'k' is # bytes read */
      if (!vflag) {             /* ensure 'lazy' evaluation */
         if (k == 16 && ever == 1 && same(words, prevwds)) {
            if (state == 0) {
               printf("*\r\n");
               state = 1;
               off += 16;
               continue;
            } else if (state == 1) {
               off += 16;
               continue;
            }
         }
      }
      addrout(od_state,off);
      off += k;
      state = 0;
      ever = 1;
      linenr = 1;
      if (oflag) wdump(od_state,words, k, 8);
      if (dflag) wdump(od_state,words, k, 10);
      if (xflag) wdump(od_state,words, k, 16);
      if (cflag) bdump(od_state,(char *)words, k, (int)'c');
      if (bflag) bdump(od_state,(char *)words, k, (int)'b');
      for (k = 0; k < 8; k++) prevwds[k] = words[k];
      for (k = 0; k < 8; k++) words[k] = 0;
   }
}

/*-------------------------------------------
| Name:offset
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
long offset(od_state_t* od_state,int argc,char *argv[],int k){
   int dot, radix;
   char *p, c;
   long val;


   /* See if the offset is decimal. */
   dot = 0;
   p = argv[k];
   while (*p)
      if (*p++ == '.') dot = 1;

   /* Convert offset to binary. */
   radix = (dot ? 10 : 8);
   val = 0;
   p = argv[k];
   if (*p == '+') p++;
   while (*p != 0 && *p != '.') {
      c = *p++;
      if (c < '0' || c > '9') {
         printf("Bad character in offset: %c\r\n", c);
         exit(1);
      }
      val = radix * val + c - '0';
   }

   p = argv[k + 1];
   if (k + 1 == argc - 1 && *p == 'b') val = sizeof(buf) * val;
   return(val);
}

/*-------------------------------------------
| Name:od_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int od_main(int argc,char *argv[])
{
   int k, flags;
   char *p;
   od_state_t _od_state = {0};
   od_state_t* od_state=&_od_state;


   /* Process flags */
   //setbuf(stdout, buffer);
   flags = 0;
   p = argv[1];
   if (argc > 1 && *p == '-') {
      /* Flags present. */
      flags++;
      p++;
      while (*p) {
         switch (*p) {
         case 'b':   bflag++;        break;
         case 'c':   cflag++;        break;
         case 'd':   dflag++;        break;
         case 'h':   hflag++;        break;
         case 'o':   oflag++;        break;
         case 'v':   vflag++;        break;
         case 'x':   xflag++;        break;
         default:    usage();
         }
         p++;
      }
   } else {
      oflag = 1;
   }
   if ((bflag | cflag | dflag | oflag | xflag) == 0) oflag = 1;
   k = (flags ? 2 : 1);
   if (bflag | cflag) {
      width = 8;
   } else if (oflag) {
      width = 7;
   } else if (dflag) {
      width = 6;
   } else {
      width = 5;
   }

   /* Process file name, if any. */
   p = argv[k];
   if (k < argc && *p != '+') {
      /* Explicit file name given. */
      close(0);
      if (open(argv[k], O_RDONLY,0) != 0) {
         fprintf(stderr, "od: cannot open %s\r\n", argv[k]);
         exit(1);
      }
      k++;
   }

   /* Process offset, if any. */
   if (k < argc) {
      /* Offset present. */
      off = offset(od_state,argc, argv, k);
      off = (off / 16) * 16;
      lseek(0, (off_t)off, SEEK_SET);
   }
   dumpfile(od_state);
   addrout(od_state,off);
   printf("\r\n");
   return(0);
}



/*===========================================
End of Source od.c
=============================================*/
