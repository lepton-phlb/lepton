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

/*===========================================
Includes
=============================================*/

#define  assert(__cond__)

#include "lib/libc/ctype/ctype.h"

#include "kernel/core/types.h"
#include "kernel/core/devio.h"
#include "kernel/core/errno.h"

#include "lib/libc/stdint.h"
#include "lib/libc/stdio/stdio.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

/*===========================================
Global Declaration
=============================================*/
#define va_strt      va_start


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:scanf
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __scanf(const char * fmt, ...){
   va_list ptr;
   int rv;
   va_strt(ptr, fmt);
   rv = __vfscanf(stdin,(char *)fmt,ptr);
   va_end(ptr);
   return rv;
}

/*-------------------------------------------
| Name:sscanf
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __sscanf(const char * sp, const char * fmt, ...)
{
   FILE string[1] =
   {
      {0, (char*)(unsigned) -1, 0, 0, (char*) (unsigned) -1, -1,
       _IOFBF | __MODE_READ}
   };

   //thread safe (stdio string mutex)
   va_list ptr;
   int rv;
   va_strt(ptr, fmt);
   string->bufpos = (char*)sp;
   rv = __vfscanf(string,(char *)fmt,ptr);
   va_end(ptr);

   return rv;
}

/*-------------------------------------------
| Name:fscanf
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __fscanf(FILE * fp, const char * fmt, ...)
{
   va_list ptr;
   int rv;
   va_strt(ptr, fmt);
   rv = __vfscanf(fp,(char *)fmt,ptr);
   va_end(ptr);
   return rv;
}


/*-------------------------------------------
| Name:vscanf
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __vscanf(const char *fmt,va_list ap){
   return __vfscanf(stdin,(char *)fmt,ap);
}

/*-------------------------------------------
| Name:vsscanf
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __vsscanf(char * sp,const char *fmt, va_list ap)
{
   int rv;
   FILE string[1] =
   {
      {0, (char*)(unsigned) -1, 0, 0, (char*) (unsigned) -1, -1,
       _IOFBF | __MODE_READ}
   };

   string->bufpos = sp;
   rv = __vfscanf(string,(char *)fmt,ap);
   return rv;
}


/* #define	skip()	do{c=getc(fp); if (c<1) goto done;}while(isspace(c))*/

#define skip()  while(isspace(c)) { if ((c=getc(fp))<1) goto done; }

#if FLOATS
/* fp scan actions */
   #define F_NADA  0    /* just change state */
   #define F_SIGN  1    /* set sign */
   #define F_ESIGN 2    /* set exponent's sign */
   #define F_INT   3    /* adjust integer part */
   #define F_FRAC  4    /* adjust fraction part */
   #define F_EXP   5    /* adjust exponent part */
   #define F_QUIT  6

   #define NSTATE  8
   #define FS_INIT         0    /* initial state */
   #define FS_SIGNED       1    /* saw sign */
   #define FS_DIGS         2    /* saw digits, no . */
   #define FS_DOT          3    /* saw ., no digits */
   #define FS_DD           4    /* saw digits and . */
   #define FS_E            5    /* saw 'e' */
   #define FS_ESIGN        6    /* saw exp's sign */
   #define FS_EDIGS        7    /* saw exp's digits */

   #define FC_DIG          0
   #define FC_DOT          1
   #define FC_E            2
   #define FC_SIGN         3

/* given transition,state do what action? */
int fp_do[][NSTATE] = {
   {F_INT,F_INT,F_INT,
    F_FRAC,F_FRAC,
    F_EXP,F_EXP,F_EXP},         /* see digit */
   {F_NADA,F_NADA,F_NADA,
    F_QUIT,F_QUIT,F_QUIT,F_QUIT,F_QUIT},        /* see '.' */
   {F_QUIT,F_QUIT,
    F_NADA,F_QUIT,F_NADA,
    F_QUIT,F_QUIT,F_QUIT},      /* see e/E */
   {F_SIGN,F_QUIT,F_QUIT,F_QUIT,F_QUIT,
    F_ESIGN,F_QUIT,F_QUIT},             /* see sign */
};
/* given transition,state what is new state? */
int fp_ns[][NSTATE] = {
   {FS_DIGS,FS_DIGS,FS_DIGS,
    FS_DD,FS_DD,
    FS_EDIGS,FS_EDIGS,FS_EDIGS},        /* see digit */
   {FS_DOT,FS_DOT,FS_DD,}, /* see '.' */
   {0,0,
    FS_E,0,FS_E,}, /* see e/E */
   {FS_SIGNED,0,0,0,0,
    FS_ESIGN,0,0},      /* see sign */
};
/* which states are valid terminators? */
int fp_sval[NSTATE] = {
   0,0,1,0,1,0,0,1
};
#endif

/*-------------------------------------------
| Name:vfscanf
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _old_vfscanf(register FILE *fp,register char *fmt,va_list ap){
   register long n;
   register int c, width, lval, cnt = 0;
   int store, neg, base, wide1, endnull, rngflag, c2;
   register unsigned char *p;
   unsigned char delim[BUFSIZ], digits[17], *q;
#if FLOATS
   long frac, expo;
   int eneg, fraclen, fstate, trans;
   double fx, fp_scan();
#endif

   __thr_safe_lock(fp);

   if (!*fmt) {
      __thr_safe_unlock(fp);
      return (0);
   }

   c = getc(fp);
   while (c > 0)
   {
      store = 0;
      if (*fmt == '%')
      {
         n = 0;
         width = -1;
         wide1 = 1;
         base = 10;
         lval = (sizeof(long) == sizeof(int));
         store = 1;
         endnull = 1;
         neg = -1;

         strcpy(delim, "\011\012\013\014\015 ");
         strcpy(digits, "0123456789ABCDEF");

         if (fmt[1] == '*')
         {
            endnull = store = 0;
            ++fmt;
         }

         while (isdigit(*++fmt)) /* width digit(s) */
         {
            if (width == -1)
               width = 0;
            wide1 = width = (width * 10) + (*fmt - '0');
         }
         --fmt;
fmtnxt:
         ++fmt;
         switch (tolower(*fmt)) /* tolower() is a MACRO! */
         {
         case '*':
            endnull = store = 0;
            goto fmtnxt;

         case 'l':              /* long data */
            lval = 1;
            goto fmtnxt;
         case 'h':              /* short data */
            lval = 0;
            goto fmtnxt;

         case 'i':              /* any-base numeric */
            base = 0;
            goto numfmt;

         case 'b':              /* unsigned binary */
            base = 2;
            goto numfmt;

         case 'o':              /* unsigned octal */
            base = 8;
            goto numfmt;

         case 'x':              /* unsigned hexadecimal */
            base = 16;
            goto numfmt;

         case 'd':              /* SIGNED decimal */
            neg = 0;
         /* FALL-THRU */

         case 'u':              /* unsigned decimal */
numfmt: skip();

            if (isupper(*fmt))
               lval = 1;

            if (!base)
            {
               base = 10;
               neg = 0;
               if (c == '%')
               {
                  base = 2;
                  goto skip1;
               }
               else if (c == '0')
               {
                  c = getc(fp);
                  if (c < 1)
                     goto savnum;
                  if ((c != 'x')
                      && (c != 'X'))
                  {
                     base = 8;
                     digits[8] = '\0';
                     goto zeroin;
                  }
                  base = 16;
                  goto skip1;
               }
            }

            if ((neg == 0) && (base == 10)
                && ((neg = (c == '-')) || (c == '+')))
            {
skip1:
               c = getc(fp);
               if (c < 1)
                  goto done;
            }

            digits[base] = '\0';
            p = ((unsigned char *)
                 strchr(digits, toupper(c)));

            if ((!c || !p) && width)
               goto done;

            while (p && width-- && c)
            {
               n = (n * base) + (p - digits);
               c = getc(fp);
zeroin:
               p = ((unsigned char *)
                    strchr(digits, toupper(c)));
            }
savnum:
            if (store)
            {
               if (neg == 1)
                  n = -n;
               if (lval)
                  *va_arg(ap, long*) = n;
               else
                  *va_arg(ap, short*) = (short)n;  //patch (short)
               ++cnt;
            }
            break;

#if FLOATS
         case 'e':              /* float */
         case 'f':
         case 'g':
            skip();

            if (isupper(*fmt))
               lval = 1;

            fstate = FS_INIT;
            neg = 0;
            eneg = 0;
            n = 0;
            frac = 0;
            expo = 0;
            fraclen = 0;

            while (c && width--)
            {
               if (c >= '0' && c <= '9')
                  trans = FC_DIG;
               else if (c == '.')
                  trans = FC_DOT;
               else if (c == '+' || c == '-')
                  trans = FC_SIGN;
               else if (tolower(c) == 'e')
                  trans = FC_E;
               else
                  goto fdone;

               switch (fp_do[trans][fstate])
               {
               case F_SIGN:
                  neg = (c == '-');
                  break;
               case F_ESIGN:
                  eneg = (c == '-');
                  break;
               case F_INT:
                  n = 10 * n + (c - '0');
                  break;
               case F_FRAC:
                  frac = 10 * frac + (c - '0');
                  fraclen++;
                  break;
               case F_EXP:
                  expo = 10 * expo + (c - '0');
                  break;
               case F_QUIT:
                  goto fdone;
               }
               fstate = fp_ns[trans][fstate];
               c = getc(fp);
            }

fdone:
            if (!fp_sval[fstate])
               goto done;
            if (store)
            {
               fx = fp_scan(neg, eneg, n, frac, expo, fraclen);
               if (lval)
                  *va_arg(ap, double *) = fx;
               else
                  *va_arg(ap, float *) = fx;
               ++cnt;
            }
            break;
#endif

         case 'c':              /* character data */
            width = wide1;
            lval = endnull = 0;
            delim[0] = '\0';
            goto strproc;

         case '[':              /* string w/ delimiter set */

            /* get delimiters */
            p = delim;

            if (*++fmt == '^')
            {
               fmt++;
               lval = 0;
            }
            else
               lval = 1;

            rngflag = 2;
            if ((*fmt == ']') || (*fmt == '-'))
            {
               *p++ = *fmt++;
               rngflag = 0;
            }

            while (*fmt != ']')
            {
               if (*fmt == '\0')
                  goto done;
               switch (rngflag)
               {
               case 1:
                  c2 = *(p - 2);
                  if (c2 <= *fmt)
                  {
                     p -= 2;
                     while (c2 < *fmt)
                        *p++ = c2++;
                     rngflag = 2;
                     break;
                  }
               /* fall thru intentional */

               case 0:
                  rngflag = (*fmt == '-');
                  break;

               case 2:
                  rngflag = 0;
               }

               *p++ = *fmt++;
            }

            *p = '\0';
            goto strproc;

         case 's':              /* string data */
            lval = 0;
            skip();
strproc:
            /* process string */
            p = va_arg(ap, unsigned char *);

            /* if the 1st char fails, match fails */
            if (width)
            {
               q = ((unsigned char *)
                    strchr(delim, c));
               if ((c < 1) || lval == (q==0))
               {
                  if (endnull)
                     *p = '\0';
                  goto done;
               }
            }

            for (;; )            /* FOREVER */
            {
               if (store)
                  *p++ = c;
               if (((c = getc(fp)) < 1) ||
                   (--width == 0))
                  break;

               q = ((unsigned char *)
                    strchr(delim, c));
               if (lval == (q==0))
                  break;
            }

            if (store)
            {
               if (endnull)
                  *p = '\0';
               ++cnt;
            }
            break;

         case '\0':             /* early EOS */
            --fmt;
         /* FALL THRU */

         default:
            goto cmatch;
         }
      }
      else if (isspace(*fmt))   /* skip whitespace */
      {
         skip();
      }
      else
      {                         /* normal match char */
cmatch:
         if (c != *fmt)
            break;
         c = getc(fp);
      }

      if (!*++fmt)
         break;
   }

done:                           /* end of scan */
   if ((c == EOF) && (cnt == 0)) {
      __thr_safe_unlock(fp);
      return (EOF);
   }

   if( c != EOF )
      __ungetc(c, fp);

   __thr_safe_unlock(fp);
   return (cnt);
}




#define __UCLIBC_HAS_FLOATS__

static int valid_digit(char c, char base)
{
   if (base == 16) {
      return isxdigit(c);
   } else {
      return (isdigit(c) && (c < '0' + base));
   }
}

#ifdef __UCLIBC_HAS_LONG_LONG__
extern unsigned long long
_strto_ll(const char *str, char **endptr, int base, int uflag);
#endif

extern unsigned long
_strto_l(const char *str, char **endptr, int base, int uflag);

struct scan_cookie {
   FILE *fp;
   int nread;
   int width;
   int width_flag;
   int ungot_char;
   int ungot_flag;
};

#ifdef __UCLIBC_HAS_LONG_LONG__
static const char qual[] = "hl" /* "jtz" */ "Lq";
/* char = -2, short = -1, int = 0, long = 1, long long = 2 */
static const char qsz[] = { -1, 1,           2, 2 };
#else
static const char qual[] = "hl" /* "jtz" */;
static const char qsz[] = { -1, 1,         };
#endif

#ifdef __UCLIBC_HAS_FLOATS__
static int __strtold(long double *ld, struct scan_cookie *sc);
/*01234567890123456 */
static const char spec[]  = "%n[csoupxXidfeEgG";
#else
static const char spec[]  = "%n[csoupxXid";
#endif
/* radix[i] <-> spec[i+5]     o   u   p   x   X  i   d */
static const char radix[] = { 8, 10, 16, 16, 16, 0, 10 };

static void init_scan_cookie(struct scan_cookie *sc, FILE *fp)
{
   sc->fp = fp;
   sc->nread = 0;
   sc->width_flag = 0;
   sc->ungot_flag = 0;
}

static int scan_getc_nw(struct scan_cookie *sc)
{
   if (sc->ungot_flag == 0) {
      sc->ungot_char = getc(sc->fp);
   } else {
      sc->ungot_flag = 0;
   }
   if (sc->ungot_char > 0) {
      ++sc->nread;
   }
   sc->width_flag = 0;
   return sc->ungot_char;
}

static int scan_getc(struct scan_cookie *sc)
{
   if (sc->ungot_flag == 0) {
      sc->ungot_char = getc(sc->fp);
   }
   sc->width_flag = 1;
   if (--sc->width < 0) {
      sc->ungot_flag = 1;
      return 0;
   }
   sc->ungot_flag = 0;
   if (sc->ungot_char > 0) {
      ++sc->nread;
   }
   return sc->ungot_char;
}

static void scan_ungetc(struct scan_cookie *sc)
{
   if (sc->ungot_flag != 0) {
      assert(sc->width < 0);
      return;
   }
   if (sc->width_flag) {
      ++sc->width;
   }
   sc->ungot_flag = 1;
   if (sc->ungot_char > 0) {            /* not EOF or EOS */
      --sc->nread;
   }
}

static void kill_scan_cookie(struct scan_cookie *sc)
{
   if (sc->ungot_flag) {
      ungetc(sc->ungot_char,sc->fp);
   }
}

int __vfscanf(fp, format, ap)
FILE *fp;
const char *format;
va_list ap;
{
#ifdef __UCLIBC_HAS_LONG_LONG__
   #define STRTO_L_(s,e,b,u) _strto_ll(s,e,b,u)
   #define MAX_DIGITS 64
   #define UV_TYPE unsigned long long
   #define V_TYPE long long
#else
   #define STRTO_L_(s,e,b,u) _strto_l(s,e,b,u)
   #define MAX_DIGITS 32
   #define UV_TYPE unsigned long
   #define V_TYPE long
#endif
#ifdef __UCLIBC_HAS_FLOATS__
   long double ld;
#endif
   UV_TYPE uv;
   struct scan_cookie sc;
   unsigned const char *fmt;
   const char *p;
   unsigned char *b;
   void *vp;
   int cc, i, cnt;
   signed char lval;
   unsigned char store, usflag, base, invert, r0, r1;
   unsigned char buf[MAX_DIGITS+2];
   unsigned char scanset[UCHAR_MAX + 1];

   init_scan_cookie(&sc,fp);

   fmt = (unsigned const char *) format;
   cnt = 0;

   while (*fmt) {
      store = 1;
      lval = 0;
      sc.width = INT_MAX;
      if (*fmt == '%') {                        /* Conversion specification. */
         ++fmt;
         if (*fmt == '*') {                     /* Suppress assignment. */
            store = 0;
            ++fmt;
         }
         for (i = 0; isdigit(*fmt); sc.width = i) {
            i = (i * 10) + (*fmt++ - '0');                     /* Get specified width. */
         }
         for (i = 0; i < sizeof(qual); i++) {                  /* Optional qualifier. */
            if (qual[i] == *fmt) {
               ++fmt;
               lval += qsz[i];
               if ((i < 2) && (qual[i] == *fmt)) {                              /* Double h or l. */
                  ++fmt;
                  lval += qsz[i];
               }
               break;
            }
         }
         for (p = spec; *p; p++) {                      /* Process format specifier. */
            if (*fmt != *p) continue;
            if (p-spec < 1) {                     /* % - match a '%'*/
               goto matchchar;
            }
            if (p-spec < 2) {                     /* n - store number of chars read */
               *(va_arg(ap, int *)) = sc.nread;
               scan_getc_nw(&sc);
               goto nextfmt;
            }
            if (p-spec > 3) {                     /* skip white space if not c or [ */
               while (isspace(scan_getc_nw(&sc)))
               {}
               scan_ungetc(&sc);
            }
            if (p-spec < 5) {                     /* [,c,s - string conversions */
               invert = 0;
               if (*p == 'c') {
                  invert = 1;
                  if (sc.width == INT_MAX) {
                     sc.width = 1;
                  }
               }
               for (i=0; i<= UCHAR_MAX; i++) {
                  scanset[i] = ((*p == 's') ? (isspace(i) == 0) : 0);
               }
               if (*p == '[') {                          /* need to build a scanset */
                  if (*++fmt == '^') {
                     invert = 1;
                     ++fmt;
                  }
                  if (*fmt == ']') {
                     scanset[(int)']'] = 1;
                     ++fmt;
                  }
                  r0 = 0;
                  while (*fmt && *fmt !=']') {                               /* build scanset */
                     if ((*fmt == '-') && r0 && (fmt[1] != ']')) {
                        /* range */
                        ++fmt;
                        if (*fmt < r0) {
                           r1 = r0;
                           r0 = *fmt;
                        } else {
                           r1 = *fmt;
                        }
                        for (i=r0; i<= r1; i++) {
                           scanset[i] = 1;
                        }
                        r0 = 0;
                     } else {
                        r0 = *fmt;
                        scanset[r0] = 1;
                     }
                     ++fmt;
                  }
                  if (!*fmt) {                               /* format string exhausted! */
                     goto done;
                  }
               }
               /* ok -- back to common work */
               if (sc.width <= 0) {
                  goto done;
               }
               if (store) {
                  b = va_arg(ap, unsigned char *);
               } else {
                  b = buf;
               }
               cc = scan_getc(&sc);
               if (cc <= 0) {
                  scan_ungetc(&sc);
                  goto done;                               /* return EOF if cnt == 0 */
               }
               i = 0;
               while ((cc>0) && (scanset[cc] != invert)) {
                  i = 1;                               /* yes, we stored something */
                  *b = cc;
                  b += store;
                  cc = scan_getc(&sc);
               }
               if (i==0) {
                  scan_ungetc(&sc);
                  goto done;                               /* return cnt */
               }
               if (*p != 'c') {                          /* nul-terminate the stored string */
                  *b = 0;
               }
               cnt += store;
               goto nextfmt;
            }
            if (p-spec < 12) {                     /* o,u,p,x,X,i,d - (un)signed integer */
               if (*p == 'p') {
                  /* assume pointer same size as int or long. */
                  lval = (sizeof(char *) == sizeof(long));
               }
               usflag = ((p-spec) < 10);                          /* (1)0 if (un)signed */
               base = radix[(int)(p-spec) - 5];
               b = buf;
               if (sc.width <= 0) {
                  goto done;
               }
               cc = scan_getc(&sc);
               if ((cc == '+') || (cc == '-')) {                          /* Handle leading sign.*/
                  *b++ = cc;
                  cc = scan_getc(&sc);
               }
               if (cc == '0') {                          /* Possibly set base and handle prefix. */
                  if ((base == 0) || (base == 16)) {
                     cc = scan_getc(&sc);
                     if ((cc == 'x') || (cc == 'X')) {
                        /* We're committed to base 16 now. */
                        base = 16;
                        cc = scan_getc(&sc);
                     } else {                                    /* oops... back up */
                        scan_ungetc(&sc);
                        cc = '0';
                        if (base == 0) {
                           base = 8;
                        }
                     }
                  }
               }
               if (base == 0) {                          /* Default to base 10 */
                  base = 10;
               }
               /* At this point, we're ready to start reading digits. */
               if (cc == '0') {
                  *b++ = cc;                               /* Store first leading 0 */
                  do {                                  /*     but ignore others. */
                     cc = scan_getc(&sc);
                  } while (cc == '0');
               }
               while (valid_digit((char)cc,base)) {                          /* Now for nonzero digits.*/
                  if (b - buf < MAX_DIGITS) {
                     *b++ = cc;
                  }
                  cc = scan_getc(&sc);
               }
               *b = 0;                          /* null-terminate */
               if ((b == buf) || (*--b == '+') || (*b == '-')) {
                  scan_ungetc(&sc);
                  goto done;                               /* No digits! */
               }
               if (store) {
                  if (*buf == '-') {
                     usflag = 0;
                  }
                  uv = STRTO_L_(buf, NULL, base, usflag);
                  vp = va_arg(ap, void *);
                  switch (lval) {
                  case 2:                                       /* If no long long, treat as long . */
#ifdef __UCLIBC_HAS_LONG_LONG__
                     *((unsigned long long *)vp) = uv;
                     break;
#endif
                  case 1:
#if ULONG_MAX == UINT_MAX
                  case 0:                                       /* int and long int are the same */
#endif
#ifdef __UCLIBC_HAS_LONG_LONG__
                     if (usflag) {
                        if (uv > ULONG_MAX) {
                           uv = ULONG_MAX;
                        }
                     } else if (((V_TYPE)uv) > LONG_MAX) {
                        uv = LONG_MAX;
                     } else if (((V_TYPE)uv) < LONG_MIN) {
                        uv = (UV_TYPE) LONG_MIN;
                     }
#endif
                     *((unsigned long *)vp) = (unsigned long)uv;
                     break;
#if ULONG_MAX != UINT_MAX
                  case 0:                                       /* int and long int are different */
                     if (usflag) {
                        if (uv > UINT_MAX) {
                           uv = UINT_MAX;
                        }
                     } else if (((V_TYPE)uv) > INT_MAX) {
                        uv = INT_MAX;
                     } else if (((V_TYPE)uv) < INT_MIN) {
                        uv = (UV_TYPE) INT_MIN;
                     }
                     *((unsigned int *)vp) = (unsigned int)uv;
                     break;
#endif
                  case -1:
                     if (usflag) {
                        if (uv > USHRT_MAX) {
                           uv = USHRT_MAX;
                        }
                     } else if (((V_TYPE)uv) > SHRT_MAX) {
                        uv = SHRT_MAX;
                     } else if (((V_TYPE)uv) < SHRT_MIN) {
                        uv = (UV_TYPE) SHRT_MIN;
                     }
                     *((unsigned short *)vp) = (unsigned short)uv;
                     break;
                  case -2:
                     if (usflag) {
                        if (uv > UCHAR_MAX) {
                           uv = UCHAR_MAX;
                        }
                     } else if (((V_TYPE)uv) > CHAR_MAX) {
                        uv = CHAR_MAX;
                     } else if (((V_TYPE)uv) < CHAR_MIN) {
                        uv = (UV_TYPE) CHAR_MIN;
                     }
                     *((unsigned char *)vp) = (unsigned char) uv;
                     break;
                  default:
                     assert(0);
                  }
                  ++cnt;
               }
               goto nextfmt;
            }
#ifdef __UCLIBC_HAS_FLOATS__
            else {                                      /* floating point */
               if (sc.width <= 0) {
                  goto done;
               }
               if (__strtold(&ld, &sc)) {                          /* Success! */
                  if (store) {
                     vp = va_arg(ap, void *);
                     switch (lval) {
                     case 2:
                        *((long double *)vp) = ld;
                        break;
                     case 1:
                        *((double *)vp) = (double) ld;
                        break;
                     case 0:
                        *((float *)vp) = (float) ld;
                        break;
                     default:                                            /* Illegal qualifier! */
                        assert(0);
                        goto done;
                     }
                     ++cnt;
                  }
                  goto nextfmt;
               }
            }
#else
            assert(0);
#endif
            goto done;
         }
         /* Unrecognized specifier! */
         goto done;
      } if (isspace(*fmt)) {            /* Consume all whitespace. */
         while (isspace(scan_getc_nw(&sc)))
         {}
      } else {                                          /* Match the current fmt char. */
matchchar:
         if (scan_getc_nw(&sc) != *fmt) {
            goto done;
         }
         scan_getc_nw(&sc);
      }
nextfmt:
      scan_ungetc(&sc);
      ++fmt;
   }

done:                                           /* end of scan */
   kill_scan_cookie(&sc);

   if ((sc.ungot_char <= 0) && (cnt == 0) && (*fmt)) {
      return (EOF);
   }

   return (cnt);
}

/*****************************************************************************/
#ifdef __UCLIBC_HAS_FLOATS__

   #include <float.h>

   #define MAX_SIG_DIGITS 20
   #define MAX_IGNORED_DIGITS 2000
   #define MAX_ALLOWED_EXP (MAX_SIG_DIGITS + MAX_IGNORED_DIGITS + LDBL_MAX_10_EXP)

   #if (__tauon_compiler__!=__compiler_iar_arm__)
      #if LDBL_DIG > MAX_SIG_DIGITS
         #error need to adjust MAX_SIG_DIGITS
      #endif
   #endif

   #if (__tauon_compiler__!=__compiler_iar_arm__)
      #if MAX_ALLOWED_EXP > INT_MAX
         #error size assumption violated for MAX_ALLOWED_EXP
      #endif
   #endif

int __strtold(long double *ld, struct scan_cookie *sc)
{
   long double number;
   long double p10;
   int exponent_power;
   int exponent_temp;
   int negative;
   int num_digits;
   int since_decimal;
   int c;

   c = scan_getc(sc);                                   /* Decrements width. */

   negative = 0;
   switch(c) {                                  /* Handle optional sign. */
   case '-': negative = 1;              /* Fall through to get next char. */
   case '+': c = scan_getc(sc);
   }

   number = 0.;
   num_digits = -1;
   exponent_power = 0;
   since_decimal = INT_MIN;

LOOP:
   while (isdigit(c)) {                 /* Process string of digits. */
      ++since_decimal;
      if (num_digits < 0) {             /* First time through? */
         ++num_digits;                          /* We've now seen a digit. */
      }
      if (num_digits || (c != '0')) {           /* had/have nonzero */
         ++num_digits;
         if (num_digits <= MAX_SIG_DIGITS) {                /* Is digit significant? */
            number = number * 10. + (c - '0');
         }
      }
      c = scan_getc(sc);
   }

   if ((c == '.') && (since_decimal < 0)) {  /* If no previous decimal pt, */
      since_decimal = 0;                        /* save position of decimal point */
      c = scan_getc(sc);                                /* and process rest of digits */
      goto LOOP;
   }

   if (num_digits<0) {                  /* Must have at least one digit. */
      goto FAIL;
   }

   if (num_digits > MAX_SIG_DIGITS) {  /* Adjust exp for skipped digits. */
      exponent_power += num_digits - MAX_SIG_DIGITS;
   }

   if (since_decimal >= 0) {            /* Adjust exponent for decimal point. */
      exponent_power -= since_decimal;
   }

   if (negative) {                              /* Correct for sign. */
      number = -number;
      negative = 0;                             /* Reset for exponent processing below. */
   }

   /* Process an exponent string. */
   if (c == 'e' || c == 'E') {
      c = scan_getc(sc);
      switch(c) {                                       /* Handle optional sign. */
      case '-': negative = 1;                   /* Fall through to get next char. */
      case '+': c = scan_getc(sc);
      }

      num_digits = 0;
      exponent_temp = 0;
      while (isdigit(c)) {              /* Process string of digits. */
         if (exponent_temp < MAX_ALLOWED_EXP) {                /* overflow check */
            exponent_temp = exponent_temp * 10 + (c - '0');
         }
         c = scan_getc(sc);
         ++num_digits;
      }

      if (num_digits == 0) {            /* Were there no exp digits? */
         goto FAIL;
      }           /* else */
      if (negative) {
         exponent_power -= exponent_temp;
      } else {
         exponent_power += exponent_temp;
      }
   }

   if (number != 0.) {
      /* Now scale the result. */
      exponent_temp = exponent_power;
      p10 = 10.;

      if (exponent_temp < 0) {
         exponent_temp = -exponent_temp;
      }

      while (exponent_temp) {
         if (exponent_temp & 1) {
            if (exponent_power < 0) {
               number /= p10;
            } else {
               number *= p10;
            }
         }
         exponent_temp >>= 1;
         p10 *= p10;
      }
   }
   *ld = number;
   return 1;

FAIL:
   scan_ungetc(sc);
   return 0;
}
#endif /* __UCLIBC_HAS_FLOATS__ */

/*===========================================
End of Source _scanf.c
=============================================*/




