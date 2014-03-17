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

//based on uC-libc-140298\uC-libc\stdio2

/*
 * This file based on printf.c from 'Dlibs' on the atari ST  (RdeBath)
 *
 *
 *    Dale Schumacher                         399 Beacon Ave.
 *    (alias: Dalnefre')                      St. Paul, MN  55104
 *    dal@syntel.UUCP                         United States of America
 *  "It's not reality that's important, but how you perceive things."
 */

/* Altered to use stdarg, made the core function vfprintf.
 * Hooked into the stdio package using 'inside information'
 * Altered sizeof() assumptions, now assumes all integers except chars
 * will be either
 *  sizeof(xxx) == sizeof(long) or sizeof(xxx) == sizeof(short)
 *
 * -RDB
 */

/*===========================================
Includes
=============================================*/
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "kernel/core/types.h"
#include "kernel/core/libstd.h"
#include "kernel/core/devio.h"
#include "kernel/core/errno.h"

#include "lib/libc/stdio/stdio.h"
#include "lib/libc/misc/ltostr.h"



/*===========================================
Global Declaration
=============================================*/
#define va_strt      va_start
#define FLOATS

/*===========================================
Implementation
=============================================*/

/*--------------------------------------------
| Name:        __printf_r
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
#define PRINTF_R_BUF_SZ 64
int __printf_r(const char * fmt, ...){
   char buf[PRINTF_R_BUF_SZ];
   int cb=0;
   FILE string[1] =
   {
      {0, 0, (char*)(unsigned) -1, 0, (char*) (unsigned) -1, -1,
       _IOFBF | __MODE_WRITE}
   };

   va_list ptr;
   int rv;

   va_strt(ptr, fmt);
   string->bufpos = buf;
   string->bufend = buf+sizeof(buf);

#if USE_FULL_STDIO_PRINTF
   rv = __vfnprintf(string, PRINTF_R_BUF_SZ, fmt, ptr);
#else
   rv = __vfprintf(string,fmt,ptr);
#endif

   va_end(ptr);
   *(string->bufpos) = 0;


   while(cb<rv) {
      int r=-1;
      if((r=write(1,buf+cb,rv-cb))<0)
         return r;
      cb+=r;
   }

   return cb;
}

/*--------------------------------------------
| Name:        __asprintf
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int __asprintf(char **app, const char *fmt, ...)
{
#if USE_FULL_STDIO_PRINTF
   va_list ptr;
   int rv, i;
   char *p;                                             /* unitialized warning is ok here */
   /*
    * First  iteration - find out size of buffer required and allocate it.
    * Second iteration - actually produce output.
    */
   rv = 0;
   for (i=0; i<2; i++) {
      va_strt(ptr, fmt);
      rv = __vsnprintf(p, rv, fmt, ptr);
      va_end(ptr);

      if (i==0) {                                       /* first time through so */
         p = malloc(++rv);                   /* allocate the buffer */
         *app = p;
         if (!p) {
            return -1;
         }
      }
   }
   return rv;
#else
   return -1;
#endif
}

/*-------------------------------------------
| Name:printf
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __printf(const char * fmt, ...){
   va_list ptr;
   int rv;
   //__thr_safe_lock(stdout);
   va_strt(ptr, fmt);

#if USE_FULL_STDIO_PRINTF
   rv = __vfnprintf(stdout, -1, fmt, ptr);
#else
   rv = __vfprintf(stdout,fmt,ptr);
#endif
   va_end(ptr);
   //__thr_safe_unlock(stdout);
   return rv;
}

/*--------------------------------------------
| Name:        __fnprintf
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int __fnprintf(FILE * fp, size_t size, const char *fmt, ...)
{

#if USE_FULL_STDIO_PRINTF
   va_list ptr;
   int rv;

   va_strt(ptr, fmt);
   rv = __vfnprintf(fp, size, fmt, ptr);
   va_end(ptr);
   return rv;
#else
   return -1;
#endif

}

/*--------------------------------------------
| Name:        __snprintf
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int __snprintf(char *sp, size_t size, const char *fmt, ...)
{
#if USE_FULL_STDIO_PRINTF
   va_list ptr;
   int rv;

   va_strt(ptr, fmt);
   rv = __vsnprintf(sp, size, fmt, ptr);
   va_end(ptr);
   return rv;
#else
   return -1;
#endif
}

/*-------------------------------------------
| Name:sprintf
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __sprintf(char * sp, const char * fmt, ...){
   FILE string[1] =
   {
      {0, 0, (char*)(unsigned) -1, 0, (char*) (unsigned) -1, -1,
       _IOFBF | __MODE_WRITE}
   };

   va_list ptr;
   int rv;

#if USE_FULL_STDIO_PRINTF
   va_strt(ptr, fmt);
   rv = __vsnprintf(sp, -1, fmt, ptr);
   va_end(ptr);
   return rv;
#else
   va_strt(ptr, fmt);
   string->bufpos = sp;
   rv = __vfprintf(string,fmt,ptr);
   va_end(ptr);
   *(string->bufpos) = 0;
   return rv;
#endif
}

/*-------------------------------------------
| Name:__fprintf
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __fprintf(FILE * fp, const char * fmt, ...){
   va_list ptr;
   int rv;

#if USE_FULL_STDIO_PRINTF
   va_strt(ptr, fmt);
   rv = __vfnprintf(fp, -1, fmt, ptr);
   va_end(ptr);
   return rv;
#else
   va_strt(ptr, fmt);
   rv = __vfprintf(fp,fmt,ptr);
   va_end(ptr);
   return rv;
#endif

}

/*-------------------------------------------
| Name:vprintf
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __vprintf(const char *fmt,va_list ap){
#if USE_FULL_STDIO_PRINTF
   return __vfnprintf(stdout,-1,fmt,ap);
#else
   return __vfprintf(stdout,fmt,ap);
#endif
}

/*-------------------------------------------
| Name:vsprintf
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __vsprintf(char * sp,const char *fmt,va_list ap)
{
#if USE_FULL_STDIO_PRINTF
   return vsnprintf(sp, -1, fmt, ap);
#else
   FILE string[1] =
   {
      {0, 0, (char*)(unsigned) -1, 0, (char*) (unsigned) -1, -1,
       _IOFBF | __MODE_WRITE}
   };
   int rv;

   string->bufpos = sp;
   rv = __vfprintf(string,fmt,ap);
   *(string->bufpos) = 0;
   return rv;
#endif
}

/*--------------------------------------------
| Name:        __vsnprintf
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int __vsnprintf(char * sp, size_t size,const char *  fmt, va_list ap)
{
   int rv;

#if USE_FULL_STDIO_PRINTF
   FILE f;
   /*
    * As we're only using the putc macro in vfnprintf, we don't need to
    * initialize all FILE f's fields.
    */
   f.bufwrite = (char *) ((unsigned) -1);
   f.bufpos = sp;
   f.mode = _IOFBF;

   rv = __vfnprintf(&f, size, fmt, ap);
   if (size) {                                          /* If this is going to a buffer, */
      *(f.bufpos) = 0;                          /* don't forget to nul-terminate. */
   }
   return rv;
#else
   FILE string[1] =
   {
      {0, 0, (char*)(unsigned) -1, 0, (char*) (unsigned) -1, -1,
       _IOFBF | __MODE_WRITE}
   };

   string->bufpos = sp;
   string->bufend = sp+size;
   rv = __vfprintf(string,fmt,ap);
   *(string->bufpos) = 0;
   return rv;
#endif

}

/*--------------------------------------------
| Name:        USE_FULL_STDIO_PRINTF
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
#if USE_FULL_STDIO_PRINTF

enum {
   FLAG_PLUS = 0,
   FLAG_MINUS_LJUSTIFY,
   FLAG_HASH,
   FLAG_0_PAD,
   FLAG_SPACE,
};

   #define WANT_DOUBLE 1

   #if (__KERNEL_CPU_ARCH_SUPPORT_FORMAT>32)
      #define WANT_LONG_LONG 1
      #define WANT_LONG_LONG_ERROR 1
   #else
      #define WANT_LONG_LONG 0
      #define WANT_LONG_LONG_ERROR 0
   #endif

/* layout                   01234  */
static const char spec[] = "+-#0 ";

   #if WANT_LONG_LONG || WANT_LONG_LONG_ERROR
static const char qual[] = "hlLq";
   #else
static const char qual[] = "hl";
   #endif

   #if !WANT_LONG_LONG && WANT_LONG_LONG_ERROR
static const char ll_err[] = "<LONG-LONG>";
   #endif

   #if !WANT_DOUBLE && WANT_DOUBLE_ERROR
static const char dbl_err[] = "<DOUBLE>";
   #endif

   #if WANT_DOUBLE || WANT_DOUBLE_ERROR
/* layout                     012345678901234567   */
static const char u_spec[] = "%nbopxXudicsfgGeEaA";
   #else
/* layout                     0123456789012   */
static const char u_spec[] = "%nbopxXudics0";
   #endif

/* WARNING: u_spec and u_radix need to stay in agreement!!! */
/* u_radix[i] <-> u_spec[i+2] for unsigned entries only */
static const char u_radix[] = "\x02\x08\x10\x10\x10\x0a";

extern int __dtostr(FILE * fp, size_t size, long double x,
                    char flag[], int width, int preci, char mode);

extern char *__ultostr(char *buf, unsigned long uval, int base, int uppercase);
extern char *__ltostr(char *buf, long val, int base, int uppercase);

   #if WANT_LONG_LONG
extern char *__ulltostr(char *buf, unsigned long long uval, int base, int uppercase);
extern char *__lltostr(char *buf, long long val, int base, int uppercase);
   #endif

/*--------------------------------------------
| Name:        vfnprintf
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int __vfnprintf(FILE * op, size_t max_size, const char *fmt, va_list ap)
{
   int i, cnt, lval;
   char *p;
   const char *fmt0;
   int buffer_mode;
   int preci, width;
   #define upcase i
   int radix, dpoint /*, upcase*/;
   #if WANT_LONG_LONG
   char tmp[BUFSIZ*2];
   #else
   char tmp[BUFSIZ];
   #endif
   char flag[sizeof(spec)];

   cnt = 0;
   if((int)max_size<0)
      max_size=BUFSIZ;

   /* This speeds things up a bit for line unbuffered */
   buffer_mode = (op->mode & __MODE_BUF);
   op->mode &= (~__MODE_BUF);

   while (*fmt) {
      if (*fmt == '%') {
         fmt0 = fmt;                                    /* save our position in case of bad format */
         ++fmt;
         width = -1;                                    /* min field width */
         preci = -5;                                    /* max string width or mininum digits */
         radix = 10;                                    /* number base */
         dpoint = 0;                                    /* found decimal point */
   #if INT_MAX != LONG_MAX
         lval = 0;                                      /* sizeof(int) != sizeof(long) */
   #else
         lval = 1;                                      /* sizeof(int) == sizeof(long) */
   #endif

         tmp[1] = 0;                                    /* set things up for %c -- better done here */

         /* init flags */
         for (p =(char *) spec; *p; p++) {
            flag[p-spec] = '\0';
         }
         flag[FLAG_0_PAD] = ' ';

         /* process optional flags */
         for (p = (char *)spec; *p; ) {
            if (*fmt == *p) {
               flag[p-spec] = *fmt++;
               p = (char *)spec;                          /* restart scan */
            } else {
               p++;
            }
         }

         if (!flag[FLAG_PLUS]) {
            flag[FLAG_PLUS] = flag[FLAG_SPACE];
         }

         /* process optional width and precision */
         do {
            if (*fmt == '.') {
               ++fmt;
               dpoint = 1;
            }
            if (*fmt == '*') {                          /* parameter width value */
               ++fmt;
               i = va_arg(ap, int);
            } else {
               for ( i = 0; (*fmt >= '0') && (*fmt <= '9'); ++fmt ) {
                  i = (i * 10) + (*fmt - '0');
               }
            }

            if (dpoint) {
               preci = i;
               if (i<0) {
                  preci = -5;
               }
            } else {
               width = i;
               if (i<0) {
                  width = -i;
                  flag[FLAG_MINUS_LJUSTIFY] = 1;
               }
            }
         } while ((*fmt == '.') && !dpoint );

         /* process optional qualifier */
         for (p = (char *) qual; *p; p++) {
            if (*p == *fmt) {
               lval = p - qual;
               ++fmt;
   #if WANT_LONG_LONG || WANT_LONG_LONG_ERROR
               if ((*p == 'l') && (*fmt == *p)) {
                  ++lval;
                  ++fmt;
               }
   #endif /* WANT_LONG_LONG || WANT_LONG_LONG_ERROR */
            }
         }

   #if WANT_GNU_ERRNO
         if (*fmt == 'm') {
            flag[FLAG_PLUS] = '\0';
            flag[FLAG_0_PAD] = ' ';
            p = strerror(errno);
            goto print;
         }
   #endif

         /* process format specifier */
         for (p = (char *) u_spec; *p; p++) {
            if (*fmt != *p) continue;
            if (p-u_spec < 1) {                         /* print a % */
               goto charout;
            }
            if (p-u_spec < 2) {                         /* store output count in int ptr */
               *(va_arg(ap, int *)) = cnt;
               goto nextfmt;
            }
            if (p-u_spec < 8) {                     /* unsigned conversion */
               radix = u_radix[p-u_spec-2];
               upcase = ((int)'x') - *p;
               if (*p == 'p') {
                  lval = (sizeof(char *) == sizeof(long));
                  upcase = 0;
               }
   #if WANT_LONG_LONG || WANT_LONG_LONG_ERROR
               if (lval >= 2) {
      #if WANT_LONG_LONG
                  p = __ulltostr(tmp + sizeof(tmp) - 1,
                                 va_arg(ap, unsigned long long),
                                 radix, upcase);
      #else
                  (void) va_arg(ap, unsigned long long);                                /* cary on */
                  p = (char *) ll_err;
      #endif /* WANT_LONG_LONG */
               } else {
   #endif /* WANT_LONG_LONG || WANT_LONG_LONG_ERROR */
   #if UINT_MAX != ULONG_MAX
               /* sizeof(unsigned int) != sizeof(unsigned long) */
               p = __ultostr(tmp + sizeof(tmp) - 1, (unsigned long)
                             ((lval)
                              ? va_arg(ap, unsigned long)
                              : va_arg(ap, unsigned int)),
                             radix, upcase);
   #else
               /* sizeof(unsigned int) == sizeof(unsigned long) */
               p = __ultostr(tmp + sizeof(tmp) - 1,/*32,*/ (unsigned long)
                             va_arg(ap, unsigned long),
                             radix, upcase);
   #endif
   #if WANT_LONG_LONG || WANT_LONG_LONG_ERROR
            }
   #endif /* WANT_LONG_LONG || WANT_LONG_LONG_ERROR */
               flag[FLAG_PLUS] = '\0';                          /* meaningless for unsigned */
               if (flag[FLAG_HASH] && (*p != '0')) {                          /* non-zero */
                  if (radix == 8) {
                     *--p = '0';                                        /* add leadding zero */
                  } else if (radix != 10) {                               /* either 2 or 16 */
                     flag[FLAG_PLUS] = '0';
                     *--p = 'b';
                     if (radix == 16) {
                        *p = 'x';
                        if (*fmt == 'X') {
                           *p = 'X';
                        }
                     }
                  }
               }
            } else if (p-u_spec < 10) {                     /* signed conversion */
   #if WANT_LONG_LONG || WANT_LONG_LONG_ERROR
               if (lval >= 2) {
      #if WANT_LONG_LONG
                  p = __lltostr(tmp + sizeof(tmp) - 1,
                                va_arg(ap, long long), 10, 0);
      #else
                  (void) va_arg(ap, long long);                               /* carry on */
                  p = (char *) ll_err;
      #endif /* WANT_LONG_LONG */
               } else {
   #endif /* WANT_LONG_LONG || WANT_LONG_LONG_ERROR */
   #if INT_MAX != LONG_MAX
               /* sizeof(int) != sizeof(long) */
               p = __ltostr(tmp + sizeof(tmp) - 1, (long)
                            ((lval)
                             ? va_arg(ap, long)
                             : va_arg(ap, int)), 10, 0);
   #else
               /* sizeof(int) == sizeof(long) */
               p = __ltostr(tmp + sizeof(tmp) - 1,/*32,*/ (long)
                            va_arg(ap, long), 10, 0);
   #endif
   #if WANT_LONG_LONG || WANT_LONG_LONG_ERROR
            }
   #endif /* WANT_LONG_LONG || WANT_LONG_LONG_ERROR */
            } else if (p-u_spec < 12) {                         /* character or string */
               flag[FLAG_PLUS] = '\0';
               flag[FLAG_0_PAD] = ' ';
               if (*p == 'c') {                                 /* character */
                  p = tmp;
                  *p = va_arg(ap, int);
               } else {                                 /* string */
                  p = va_arg(ap, char *);
                  if (!p) {
                     p = "(null)";
                  }
               }
   #if WANT_DOUBLE || WANT_DOUBLE_ERROR
            } else if (p-u_spec < 27) {                                 /* floating point */
   #endif /* WANT_DOUBLE || WANT_DOUBLE_ERROR */
   #if WANT_DOUBLE
               if (preci < 0) {
                  preci = 6;
               }
               cnt += __dtostr(op,
                               ((int)max_size > cnt ? (int)max_size - cnt : 0),
                               (long double) ((lval > 1)
                                              ? va_arg(ap, long double)
                                              : va_arg(ap, double)),
                               flag, width,  preci, *fmt);
               goto nextfmt;
   #elif WANT_DOUBLE_ERROR
               (void) ((lval > 1) ? va_arg(ap, long double)
                       : va_arg(ap, double));                                  /* carry on */
               p = (char *) dbl_err;
   #endif /* WANT_DOUBLE */
            }

   #if WANT_GNU_ERRNO
print:
   #endif
            {                                                   /* this used to be printfield */
               int len;

               /* cheaper than strlen call */
               for ( len = 0; p[len]; len++ ) { }

               if ((*p == '-')
   #if WANT_GNU_ERRNO
                   && (*fmt != 'm')
   #endif
                   && (*fmt != 's')) {
                  flag[FLAG_PLUS] = *p++;
                  --len;
               }
               if (flag[FLAG_PLUS]) {
                  ++len;
                  ++preci;
                  if (flag[FLAG_PLUS] == '0') {                               /* base 16 */
                     ++preci;                                    /* account for x or X */
                  }
               }

               if (preci >= 0) {
                  if ((*fmt == 's')
   #if WANT_GNU_ERRNO
                      || (*fmt == 'm')
   #endif
                      ) {
                     if (len > preci) {
                        len = preci;
                     } else {
                        preci = len;
                     }
                  }
                  preci -= len;
                  if (preci < 0) {
                     preci = 0;
                  }
                  width -= preci;
               }

               width -= len;
               if (width < 0) {
                  width = 0;
               }

               if (preci < 0) {
                  preci = 0;
                  if (!flag[FLAG_MINUS_LJUSTIFY]
                      /* && flag[FLAG_PLUS] */
                      && (flag[FLAG_0_PAD] == '0')) {
                     preci = width;
                     width = 0;
                  }
               }

               while (width + len + preci) {
                  unsigned char ch;
                  /* right padding || left padding */
                  if ((!len && !preci)
                      || (width && !flag[FLAG_MINUS_LJUSTIFY])) {
                     ch = ' ';
                     --width;
                  } else if (flag[FLAG_PLUS]) {
                     ch = flag[FLAG_PLUS];                                    /* sign */
                     if (flag[FLAG_PLUS]=='0') {                                        /* base 16 case */
                        flag[FLAG_PLUS] = *p++;                                         /* get the x|X */
                     } else {
                        flag[FLAG_PLUS] = '\0';
                     }
                     --len;
                  } else if (preci) {
                     ch = '0';
                     --preci;
                  } else {
                     ch = *p++;                                    /* main field */
                     --len;
                  }

                  if (++cnt < (int)max_size) {
                     putc(ch, op);
                  }
                  if ((ch == '\n') && (buffer_mode == _IOLBF)) {
                     fflush(op);
                  }
               }
            }
            goto nextfmt;
         }

         fmt = fmt0;                    /* this was an illegal format */
      }

charout:
      if (++cnt < (int)max_size) {
         putc(*fmt, op);                /* normal char out */
      }
      if ((*fmt == '\n') && (buffer_mode == _IOLBF)) {
         fflush(op);
      }

nextfmt:
      ++fmt;
   }

   op->mode |= buffer_mode;

   if( buffer_mode == _IONBF )
      __fflush(op);
   if (buffer_mode == _IOLBF) {
      op->bufwrite = op->bufpos;
   }

   if (ferror(op)) {
      cnt = -1;
   }
   return (cnt);
}

#else //end of USE_FULL_STDIO_PRINTF section

/*--------------------------------------------
| Name:        DEFAULT PRINTF
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/


   #ifdef FLOATS
int (*__fp_print)() = 0;
   #endif

/*-------------------------------------------
| Name:prtfld
| Description:
| Parameters:
| Return Type:
| Comments: Output the given field in the manner
|           specified by the arguments.
|           Return the number of characters output.
| See:
---------------------------------------------*/
static int __prtfld(register FILE *op,
                    register unsigned char *buf,
                    int ljustf,
                    register char sign,
                    char pad,
                    register int width,
                    int preci,
                    int buffer_mode){

   register int cnt = 0, len;
   register unsigned char ch;

   len = strlen(buf);

   if (*buf == '-')
      sign = *buf++;
   else if (sign)
      len++;

   if ((preci != -1) && (len > preci))  /* limit max data width */
      len = preci;

   if (width < len)             /* flexible field width or width overflow */
      width = len;

   /*
    * at this point: width = total field width len   = actual data width
    * (including possible sign character)
    */
   cnt = width;
   width -= len;

   while (width || len)
   {
      if (!ljustf && width)     /* left padding */
      {
         if (len && sign && (pad == '0'))
            goto showsign;
         ch = pad;
         --width;
      }else if (len) {
         if (sign)
         {
showsign: ch = sign;                    /* sign */
            sign = '\0';
         }
         else
            ch = *buf++;        /* main field */
         --len;
      }else{
         ch = pad;              /* right padding */
         --width;
      }
      putc(ch, op);
      if( ch == '\n' && buffer_mode == _IOLBF )
         __fflush(op);
   }

   return (cnt);
}

/*-------------------------------------------
| Name:isinf
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int isinf(float f) {
   union {
      unsigned long l;
      float f;
   } u;
   u.f=f;
   return (u.l==0x7FF00000l ? 1 : u.l==0xFFF00000l ? -1 : 0);
}

/*-------------------------------------------
| Name:isnan
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int isnan(float f) {
   union {
      unsigned long l;
      float f;
   } u;
   u.f=f;
   return (u.l==0x7FF80000l || u.l==0x7FF00000l || u.l==0xfff80000l);
}

/*-------------------------------------------
| Name:copystring
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int copystring(char* buf,int maxlen, const char* s) {
   int i;
   for (i=0; i<3&&i<maxlen; ++i)
      buf[i]=s[i];
   if (i<maxlen) { buf[i]=0; ++i; }
   return i;
}


/*-------------------------------------------
| Name:__dtostr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __ftostr(float f,char *buf,unsigned int maxlen,unsigned int prec,unsigned int prec2) {
   /*  unsigned long long m=u.l & ((1ull<<52)-1); */
   /* step 2: exponent is base 2, compute exponent for base 10 */
   signed long e10;
   /* step 3: calculate 10^e10 */
   unsigned int i;
   float backup=f;
   float tmp;
   char *oldbuf=buf;
   #if 1
   union {
      unsigned long l;
      float f;
   } u; // = { .d=d };
   /* step 1: extract sign, mantissa and exponent */
   signed long e;
   u.f=f;
   e=((u.l>>/*52*/ 23)&((1<</*11*/ 8)-1))-/*1023*/ 127;
   #else
      #if __BYTE_ORDER == __LITTLE_ENDIAN
   signed long e=(((((unsigned long*)&f)[1])>>20)&((1<<11)-1))-1023;
      #else
   signed long e=(((*((unsigned long*)&f))>>20)&((1<<11)-1))-1023;
      #endif
   #endif


   if ((i=isinf(f))) return copystring(buf,maxlen,i>0 ? "inf" : "-inf");
   if (isnan(f)) return copystring(buf,maxlen,"nan");
   e10=1+(long)(e*0.30102999566398119802); /* log10(2) */
   /* Wir iterieren von Links bis wir bei 0 sind oder maxlen erreicht
    * ist.  Wenn maxlen erreicht ist, machen wir das nochmal in
    * scientific notation.  Wenn dann von prec noch was brig ist, geben
    * wir einen Dezimalpunkt aus und geben prec2 Nachkommastellen aus.
    * Wenn prec2 Null ist, geben wir so viel Stellen aus, wie von prec
    * noch brig ist. */
   if (f==0.0) {
      prec2=prec2==0 ? 1 : prec2+2;
      prec2=prec2>maxlen ? 8 : prec2;
      i=0;
      if (prec2 && (long)u.l<0) { buf[0]='-'; ++i; }
      for (; i<prec2; ++i) buf[i]='0';
      buf[buf[0]=='0' ? 1 : 2]='.'; buf[i]=0;
      return i;
   }

   if (f < 0.0) { f=-f; *buf='-'; --maxlen; ++buf; }

   /*
      Perform rounding. It needs to be done before we generate any
      digits as the carry could propagate through the whole number.
   */

   tmp = (float)0.5;
   for (i = 0; i < prec2; i++) { tmp *= (float)0.1; }
   f += tmp;

   if (f < 1.0) { *buf='0'; --maxlen; ++buf; }
/*  printf("e=%d e10=%d prec=%d\n",e,e10,prec); */
   if (e10>0) {
      int first=1;      /* are we about to write the first digit? */
      tmp = 10.0;
      i=e10;
      while (i>10) { tmp=tmp*(float)1e10; i-=10; }
      while (i>1) { tmp=tmp*10; --i; }
      /* the number is greater than 1. Iterate through digits before the
       * decimal point until we reach the decimal point or maxlen is
       * reached (in which case we switch to scientific notation). */
      while (tmp>0.9) {
         char digit;
         float fraction=f/tmp;
         digit=(int)(fraction);         /* floor() */
         if (!first || digit) {
            first=0;
            *buf=digit+'0'; ++buf;
            if (!maxlen) {
               /* use scientific notation */
               int len=__ftostr(backup/tmp,oldbuf,maxlen,prec,prec2);
               int initial=1;
               if (len==0) return 0;
               maxlen-=len; buf+=len;
               if (maxlen>0) {
                  *buf='e';
                  ++buf;
               }
               --maxlen;
               for (len=1000; len>0; len/=10) {
                  if (e10>=len || !initial) {
                     if (maxlen>0) {
                        *buf=(e10/len)+'0';
                        ++buf;
                     }
                     --maxlen;
                     initial=0;
                     e10=e10%len;
                  }
               }
               if (maxlen>0) goto fini;
               return 0;
            }
            f-=digit*tmp;
            --maxlen;
         }
         tmp/=(float)10.0;
      }
   }
   else
   {
      tmp = (float)0.1;
   }

   if (buf==oldbuf) {
      if (!maxlen) return 0; --maxlen;
      *buf='0'; ++buf;
   }
   if (prec2 || prec>(unsigned int)(buf-oldbuf)+1) {    /* more digits wanted */
      if (!maxlen) return 0; --maxlen;
      *buf='.'; ++buf;
      prec-=buf-oldbuf-1;
      if (prec2) prec=prec2;
      if (prec>maxlen) return 0;
      while (prec>0) {
         char digit;
         float fraction=f/tmp;
         digit=(int)(fraction);         /* floor() */
         *buf=digit+'0'; ++buf;
         f-=digit*tmp;
         tmp/=(float)10.0;
         --prec;
      }
   }
fini:
   *buf=0;
   return buf-oldbuf;
}

/*-------------------------------------------
| Name:vfprintf
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __vfprintf(FILE *op,register const char *fmt,register va_list ap){
   register int i, cnt = 0, ljustf, lval;
   int preci, dpoint, width;
   char pad, sign, radix, hash;
   register char *ptmp;
   char tmp[BUFSIZ], *ltostr(), *ultostr();
   char* buf=tmp;
   int buffer_mode;

   __thr_safe_lock(op);

   /* This speeds things up a bit for unbuffered */
   buffer_mode = (op->mode&__MODE_BUF);
   op->mode &= (~__MODE_BUF);

   while (*fmt)
   {
      if (*fmt == '%')
      {
         if( buffer_mode == _IONBF )
            __fflush(op);
         ljustf = 0;            /* left justify flag */
         sign = '\0';           /* sign char & status */
         pad = ' ';             /* justification padding char */
         width = -1;            /* min field width */
         dpoint = 0;            /* found decimal point */
         preci = -1;            /* max data width */
         radix = 10;            /* number base */
         ptmp = tmp;            /* pointer to area to print */
         hash = 0;
         lval = (sizeof(int)==sizeof(long));    /* long value flaged */
fmtnxt:
         i = 0;
         for(;; )
         {
            ++fmt;
            if(*fmt < '0' || *fmt > '9' ) break;
            i = (i * 10) + (*fmt - '0');
            if (dpoint)
               preci = i;
            else if (!i && (pad == ' '))
            {
               pad = '0';
               goto fmtnxt;
            }
            else
               width = i;
         }

         switch (*fmt)
         {
         case '\0':             /* early EOS */
            --fmt;
            goto charout;

         case '-':              /* left justification */
            ljustf = 1;
            goto fmtnxt;

         case ' ':
         case '+':              /* leading sign flag */
            sign = *fmt;
            goto fmtnxt;

         case '*':              /* parameter width value */
            i = va_arg(ap, int);
            if (dpoint)
               preci = i;
            else
               width = i;
            goto fmtnxt;

         case '.':              /* secondary width field */
            dpoint = 1;
            goto fmtnxt;

         case 'l':              /* long data */
            lval = 1;
            goto fmtnxt;

         case 'h':              /* short data */
            lval = 0;
            goto fmtnxt;

         case 'd':              /* Signed decimal */
         case 'i':
   #if defined(__GNUC__)
            ptmp = ltostr(buf,32,(long) ((lval)
                                         ? va_arg(ap, long)
                                         : va_arg(ap, int)),
                          10,0);
   #else
            ptmp = ltostr(buf,32,(long) ((lval)
                                         ? va_arg(ap, long)
                                         : va_arg(ap, short)),
                          10,0);
   #endif
            goto printit;

         case 'b':              /* Unsigned binary */
            radix = 2;
            goto usproc;

         case 'o':              /* Unsigned octal */
            radix = 8;
            goto usproc;

         case 'p':              /* Pointer */
            lval = (sizeof(char*) == sizeof(long));
            pad = '0';
            width = 6;
            preci = 8;
         /* fall thru */

         case 'x':              /* Unsigned hexadecimal */
         case 'X':
            radix = 16;
         /* fall thru */

         case 'u':              /* Unsigned decimal */
usproc:
   #if defined(__GNUC__)
            ptmp = ultostr(buf,32,(unsigned long) ((lval)
                                                   ? va_arg(ap, unsigned long)
                                                   : va_arg(ap, unsigned int)),
                           radix,0);
   #else
            ptmp = ultostr(buf,32,(unsigned long) ((lval)
                                                   ? va_arg(ap, unsigned long)
                                                   : va_arg(ap, unsigned short)),
                           radix,0);
   #endif
            if( hash && radix == 8 ) { width = strlen(ptmp)+1; pad='0'; }
            goto printit;

         case '#':
            hash=1;
            goto fmtnxt;

         case 'c':              /* Character */
            ptmp[0] = va_arg(ap, int);
            ptmp[1] = '\0';
            goto nopad;

         case 's':              /* String */
            ptmp = va_arg(ap, char*);
            if(!ptmp)
               ptmp="(null)";
nopad:
            sign = '\0';
            pad = ' ';
printit:
            cnt += __prtfld(op, ptmp, ljustf,
                            sign, pad, width, preci, buffer_mode);
            break;

   #ifdef FLOATS
         case 'e':              /* float */
         case 'f':
         case 'g':
            //case 'E':
            //case 'G':
         {
            unsigned int sz;
            char*s;
            int g=(*fmt=='g');
            //(*__fp_print)(&va_arg(ap, double), *fmt, preci, ptmp);
            float f=(float)va_arg(ap,double);
            s=buf;
            if (width<=0) width=1;
            if (!dpoint) preci=6;
            //if (sign || f < +0.0) flag_in_sign=1;

            sz=__ftostr(f,s,sizeof(tmp)-1,width,preci);

            if (dpoint) {
               char *tmp;
               if ((tmp=strchr(s,'.'))) {
                  if (preci || hash) ++tmp;
                  while (preci>0 && *++tmp)
                     --preci;
                  *tmp=0;
               } else if (hash) {
                  s[sz]='.';
                  s[++sz]='\0';
               }
            }
            if (g) {
               char *tmp,*tmp1; /* boy, is _this_ ugly! */
               if ((tmp=strchr(s,'.'))) {
                  tmp1=strchr(tmp,'e');
                  while (*tmp) ++tmp;
                  if (tmp1) tmp=tmp1;
                  while (*--tmp=='0') ;
                  if (*tmp!='.') ++tmp;
                  *tmp=0;
                  if (tmp1) strcpy(tmp,tmp1);
               }
            }

            /*if ((sign) && f>=0) {
                    *(--s)=(sign)?'+':' ';
                    ++sz;
                 }*/
            ptmp = s;
            preci = -1;
            goto printit;
         }
            /* FALLTHROUGH if no floating printf available */
   #endif

         default:               /* unknown character */
            goto charout;
         }
      }
      else
      {
charout:
         putc(*fmt, op);        /* normal char out */
         ++cnt;
         if( *fmt == '\n' && buffer_mode == _IOLBF )
            __fflush(op);
      }
      ++fmt;
   }
   op->mode |= buffer_mode;
   if( buffer_mode == _IONBF )
      __fflush(op);
   if( buffer_mode == _IOLBF )
      op->bufwrite = op->bufstart;

   __thr_safe_unlock(op);
   return (cnt);
}

#endif
/*===========================================
End of Sourceprintf.c
=============================================*/


