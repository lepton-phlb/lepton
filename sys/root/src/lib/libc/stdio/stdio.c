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

//based on uC-libc-140298\uC-libc\stdio2

/* Copyright (C) 1996 Robert de Bath <rdebath@cix.compulink.co.uk>
 * This file is part of the Linux-8086 C library and is distributed
 * under the GNU Library General Public License.
 */

/* This is an implementation of the C standard IO package.
 */

/*===========================================
Includes
=============================================*/
#include "kernel/core/types.h"
#include "kernel/core/libstd.h"
#include "kernel/core/devio.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/errno.h"

#include "lib/libc/stdio/stdio.h"

#include <string.h>
/*===========================================
Global Declaration
=============================================*/

extern FILE *__IO_list;         /* For fflush at exit */

#define Inline_init __io_init_vars()

FILE *__IO_list = 0;            /* For fflush at exit */

#if !defined(__KERNEL_LOAD_LIB)
//not thread safe
static char bufin[BUFSIZ];
static char bufout[BUFSIZ];
static char buferr[BUFSIZ];

   #if defined(__GNUC__)
FILE stdin[1] __attribute__ ((visibility("hidden"))) =
   #else
FILE stdin[1] =
   #endif
{
   {bufin, bufin, bufin, bufin, bufin + sizeof(bufin),
    0, _IOFBF | __MODE_READ | __MODE_IOTRAN}
};

   #if defined(__GNUC__)
FILE stdout[1] __attribute__ ((visibility("hidden"))) =
   #else
FILE stdout[1] =
   #endif
{
   {bufout, bufout, bufout, bufout, bufout + sizeof(bufout),
    1,
   #if defined (__KERNEL_NET_IPSTACK)
    /*_IOLBF*/ _IONBF
   #else
    _IONBF
   #endif
    | __MODE_WRITE | __MODE_IOTRAN}        //_IONBF instead _IOFBF (force fflush buffer) in printf.
};

   #if defined(__GNUC__)
FILE stderr[1] __attribute__ ((visibility("hidden"))) =
   #else
FILE stderr[1] =
   #endif
{
   {buferr, buferr, buferr, buferr, buferr + sizeof(buferr),
    2,
   #if defined (__KERNEL_NET_IPSTACK)
    _IOLBF           /*_IONBF*/
   #else
    _IONBF
   #endif
    | __MODE_WRITE | __MODE_IOTRAN}        //_IONBF instead _IOFBF (force fflush buffer) in printf.
};

#endif

#define __buferr (stderr->unbuf)        /* Stderr is unbuffered */

/* Call the stdio initiliser; it's main job it to call atexit */

/* Note: This def of READING is ok since 1st ungetc puts in buf. */
#define READING(fp) (fp->bufstart < fp->bufread)
#define WRITING(fp) (fp->bufwrite > fp->bufstart)

#define READABLE(fp) (fp->bufread != 0)
#define WRITEABLE(fp) (fp->bufwrite != 0)
#define EOF_OR_ERROR(fp) (fp->mode & (__MODE_EOF | __MODE_ERR))

/*===========================================
Implementation
=============================================*/


/*-------------------------------------------
| Name:__stdio_close_all
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void __stdio_close_all(pid_t pid){
   FILE *fp;
   __fflush(stdout);
   __fflush(stderr);
   for (fp = __IO_list; fp; fp = fp->next)
   {
      __fflush(fp);
      close(fp->fd);
      /* Note we're not de-allocating the memory */
      /* There doesn't seem to be much point :-) */
      fp->fd = -1;
   }
}

/*-------------------------------------------
| Name:__stdio_init
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void __stdio_init(){
#if !defined(__KERNEL_LOAD_LIB)
   pthread_mutexattr_t mutex_attr=0;

   kernel_pthread_mutex_init(&stdin->mutex,&mutex_attr);
   kernel_pthread_mutex_init(&stdout->mutex,&mutex_attr);
   kernel_pthread_mutex_init(&stderr->mutex,&mutex_attr);
#endif
}

/*-------------------------------------------
| Name:__io_init_vars
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void __io_init_vars(){
   static int first_time = 1;
   if( !first_time ) return; first_time = 1;
   if (__isatty(1))
      stdout->mode |= _IOLBF;
   //atexit(__stdio_close_all);//to do: restore atexit
}

/*-------------------------------------------
| Name:fputc
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __fputc(int ch, FILE *fp){
   register int v;

   __thr_safe_lock(fp);

   Inline_init;

   v = fp->mode;
   /* If last op was a read ... */
   if ((v & __MODE_READING) && __fflush(fp)) {
      __thr_safe_unlock(fp);
      return EOF;
   }

   /* Can't write or there's been an EOF or error then return EOF */
   if ((v & (__MODE_WRITE | __MODE_EOF | __MODE_ERR)) != __MODE_WRITE) {
      __thr_safe_unlock(fp);
      return EOF;
   }

   /* In MSDOS translation mode */
#if __MODE_IOTRAN
   if (ch == '\n' && (v & __MODE_IOTRAN) && fputc('\r', fp) == EOF) {
      __thr_safe_unlock(fp);
      return EOF;
   }
#endif

   /* Buffer is full */
   if (fp->bufpos >= fp->bufend && __fflush(fp)) {
      __thr_safe_unlock(fp);
      return EOF;
   }

   /* Right! Do it! */
   *(fp->bufpos++) = ch;
   fp->mode |= __MODE_WRITING;

   /* Unbuffered or Line buffered and end of line */
   if (((ch == '\n' && (v & _IOLBF)) || (v & _IONBF))
       && __fflush(fp)) {
      __thr_safe_unlock(fp);
      return EOF;
   }

   /* Can the macro handle this by itself ? */
   if (v & (__MODE_IOTRAN | _IOLBF | _IONBF))
      fp->bufwrite = fp->bufstart;      /* Nope */
   else
      fp->bufwrite = fp->bufend;        /* Yup */

   __thr_safe_unlock(fp);

   /* Correct return val */
   return (unsigned char) ch;
}

/*-------------------------------------------
| Name:fgetc
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __fgetc(FILE *fp){
   int ch;

   __thr_safe_lock(fp);

   if (fp->mode & __MODE_WRITING)
      __fflush(fp);
#if __MODE_IOTRAN
try_again:
#endif
   /* Can't read or there's been an EOF or error then return EOF */
   if ((fp->mode & (__MODE_READ | __MODE_EOF | __MODE_ERR)) != __MODE_READ) {
      __thr_safe_unlock(fp);
      return EOF;
   }

   /* Nothing in the buffer - fill it up */
   if (fp->bufpos >= fp->bufread)
   {
      fp->bufpos = fp->bufread = fp->bufstart;
      ch = __fread(fp->bufpos, 1, fp->bufend - fp->bufstart, fp);
      if (ch == 0) {
         __thr_safe_unlock(fp);
         return EOF;
      }

      fp->bufread += ch;
      fp->mode |= __MODE_READING;
      fp->mode &= ~__MODE_UNGOT;
   }
   ch = *(fp->bufpos++);

#if __MODE_IOTRAN
   /* In MSDOS translation mode; WARN: Doesn't work with UNIX macro */
   if (ch == '\r' && (fp->mode & __MODE_IOTRAN))
      goto try_again;
#endif

   __thr_safe_unlock(fp);
   return ch;
}

/*-------------------------------------------
| Name:fflush
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __fflush(FILE *fp){
   int len, cc, rv=0;
   char * bstart;
   //int errno;

   __thr_safe_lock(fp);

   if (fp == NULL)              /* On NULL flush the lot. */
   {
      if (__fflush(stdin)) {
         __thr_safe_unlock(fp);
         return EOF;
      }
      if (__fflush(stdout)) {
         __thr_safe_unlock(fp);
         return EOF;
      }
      if (__fflush(stderr)) {
         __thr_safe_unlock(fp);
         return EOF;
      }

      for (fp = __IO_list; fp; fp = fp->next)
         if (__fflush(fp)) {
            __thr_safe_unlock(fp);
            return EOF;
         }

      __thr_safe_unlock(fp);
      return 0;
   }

   /* If there's output data pending */
   if (fp->mode & __MODE_WRITING)
   {
      len = fp->bufpos - fp->bufstart;

      if (len)
      {
         bstart = fp->bufstart;
         /*
          * The loop is so we don't get upset by signals or partial writes.
          */
         do
         {
            cc = write(fp->fd, bstart, len);
            if( cc > 0 )
            {
               bstart+=cc; len-=cc;
            }
         }
         while ( cc>0 || (cc != -1 /*&& errno == EINTR*/)); //to do: (cc == -1 && errno == EINTR) test.
         /*
          * If we get here with len!=0 there was an error, exactly what to
          * do about it is another matter ...
          *
          * I'll just clear the buffer.
          */
         if (len)
         {
            fp->mode |= __MODE_ERR;
            rv = EOF;
         }
      }
   }
   /* If there's data in the buffer sychronise the file positions */
   else if (fp->mode & __MODE_READING)
   {
      /* Humm, I think this means sync the file like fpurge() ... */
      /* Anyway the user isn't supposed to call this function when reading */

      len = fp->bufread - fp->bufpos;   /* Bytes buffered but unread */
      /* If it's a file, make it good */
      if (len > 0 && (lseek(fp->fd, (off_t)-len, 1) < 0))
      {
         /* Hummm - Not certain here, I don't think this is reported */
         /*
          * fp->mode |= __MODE_ERR; return EOF;
          */
      }
   }

   /* All done, no problem */
   fp->mode &= (~(__MODE_READING|__MODE_WRITING|__MODE_EOF|__MODE_UNGOT));
   fp->bufread = fp->bufwrite = fp->bufpos = fp->bufstart;
   __thr_safe_unlock(fp);
   return rv;
}

/*-------------------------------------------
| Name:fgets
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
char * __fgets(char *s,size_t count,FILE *f)
{
   char *ret;
   register size_t i;
   register int ch;

   __thr_safe_lock(f);

   ret = s;
   for (i = count; i > 0; i--)
   {
      ch = getc(f);
      if (ch == EOF)
      {
         if (s == ret) {
            __thr_safe_unlock(f);
            return 0;
         }
         break;
      }
      *s++ = (char) ch;
      if (ch == '\n')
         break;
   }
   *s = 0;

   if (ferror(f)) {
      __thr_safe_unlock(f);
      return 0;
   }

   __thr_safe_unlock(f);
   return ret;
}

/*-------------------------------------------
| Name:gets
| Description:
| Parameters:
| Return Type:
| Comments:BAD function; DON'T use it!
| See:
---------------------------------------------*/
char * __gets(char *str)
{
   /* Auwlright it will work but of course _your_ program will crash */
   /* if it's given a too long line */
   register char *p = str;
   register int c;

   __thr_safe_lock(stdin);

   while (((c = getc(stdin)) != EOF) && (c != '\n'))
      *p++ = c;
   *p = '\0';

   __thr_safe_unlock(stdin);

   return  (((c == EOF) && (p == str)) ? NULL : str);   /* NULL == EOF */
}

/*-------------------------------------------
| Name:fputs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __fputs(char *str,FILE *fp){
   register int n = 0;

   __thr_safe_lock(fp);

   while (*str)
   {
      if (putc(*str++, fp) == EOF) {
         __thr_safe_unlock(fp);
         return (EOF);
      }
      ++n;
   }
   __thr_safe_unlock(fp);
   return (n);
}

/*-------------------------------------------
| Name:puts
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __puts(char *str){
   register int n;

   __thr_safe_lock(stdout);
   if (((n = __fputs(str, stdout)) == EOF)
       || (putc('\n', stdout) == EOF)) {
      __thr_safe_unlock(stdout);
      return (EOF);
   }
   __thr_safe_unlock(stdout);
   return (++n);
}

/*-------------------------------------------
| Name:fread
| Description:
| Parameters:
| Return Type:
| Comments:fread will often be used to read in
|          large chunks of data calling read()
|          directly can be a big win in this case.
|          Beware also fgetc calls this function to fill the buffer.
|          This ignores __MODE__IOTRAN;
|          probably exactly what you want. (It _is_ whatfgetc wants)
| See:
---------------------------------------------*/
int __fread(char *buf,int size,int nelm,FILE *fp){
   int len, v;
   unsigned bytes, got = 0;

   __thr_safe_lock(fp);

   Inline_init;

   v = fp->mode;

   /* Want to do this to bring the file pointer up to date */
   if (v & __MODE_WRITING)
      __fflush(fp);

   /* Can't read or there's been an EOF or error then return zero */
   if ((v & (__MODE_READ | __MODE_EOF | __MODE_ERR)) != __MODE_READ) {
      __thr_safe_unlock(fp);
      return 0;
   }

   /* This could be long, doesn't seem much point tho */
   bytes = size * nelm;

   len = fp->bufread - fp->bufpos;
   if ( (unsigned)len >= bytes)         /* Enough buffered */
   {
      memcpy(buf, fp->bufpos, (unsigned) bytes);
      fp->bufpos += bytes;
      __thr_safe_unlock(fp);
      return bytes;
   }
   else if (len > 0)            /* Some buffered */
   {
      memcpy(buf, fp->bufpos, len);
      fp->bufpos += len;
      got = len;
   }

   /* Need more; do it with a direct read */
   len = read(fp->fd, buf + got, (unsigned) (bytes - got));

   /* Possibly for now _or_ later */
   if (len < 0)
   {
      fp->mode |= __MODE_ERR;
      len = 0;
   }
   else if (len == 0)
      fp->mode |= __MODE_EOF;

   __thr_safe_unlock(fp);
   return (got + len) / size;
}

/*-------------------------------------------
| Name:fwrite
| Description:
| Parameters:
| Return Type:
| Comments: Like fread, fwrite will often be used
|           to write out large chunks of data;
|           calling write() directly can be a big
|           win in this case.
|           But first we check to see if there's
|           space in the buffer.
|           Again this ignores __MODE__IOTRAN.
| See:
---------------------------------------------*/
int __fwrite(char *buf,int size,int nelm,FILE *fp){
   register int v;
   int len;
   unsigned bytes, put;
   //int errno;

   __thr_safe_lock(fp);

   v = fp->mode;
   /* If last op was a read ... */
   if ((v & __MODE_READING) && __fflush(fp)) {
      __thr_safe_unlock(fp);
      return 0;
   }

   /* Can't write or there's been an EOF or error then return 0 */
   if ((v & (__MODE_WRITE | __MODE_EOF | __MODE_ERR)) != __MODE_WRITE) {
      __thr_safe_unlock(fp);
      return 0;
   }

   /* This could be long, doesn't seem much point tho */
   bytes = size * nelm;

   len = fp->bufend - fp->bufpos;

   /* Flush the buffer if not enough room */
   if (bytes > (unsigned)len)
      if (__fflush(fp)) {
         __thr_safe_unlock(fp);
         return 0;
      }

   len = fp->bufend - fp->bufpos;
   if (bytes <= (unsigned)len)          /* It'll fit in the buffer ? */
   {
      fp->mode |= __MODE_WRITING;
      memcpy(fp->bufpos, buf, bytes);
      fp->bufpos += bytes;

      /* If we're not fully buffered */
      if (v & (_IOLBF | _IONBF))
         __fflush(fp);

      __thr_safe_unlock(fp);
      return nelm;
   }
   else /* Too big for the buffer */
   {
      put = bytes;
      do
      {
         len = write(fp->fd, buf, bytes);
         if( len > 0 ) {
            buf+=len; bytes-=len;
         }
      } while (len > 0 || (len != -1 /*&& errno == EINTR*/)); //to do: (len==-1 && errno == EINTR) test.

      if (len < 0)
         fp->mode |= __MODE_ERR;

      put -= bytes;
   }

   __thr_safe_unlock(fp);
   return put / size;
}

/*-------------------------------------------
| Name:rewind
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void __rewind(FILE * fp){
   __thr_safe_lock(fp);
   __fseek(fp, (long)0, 0);
   clearerr(fp);
   __thr_safe_unlock(fp);
}

/*-------------------------------------------
| Name:fseek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __fseek(FILE *fp,long offset,int ref)
{
   __thr_safe_lock(fp);
#if 1
   /* if __MODE_READING and no ungetc ever done can just move pointer */
   /* This needs testing! */

   if ( (fp->mode &(__MODE_READING | __MODE_UNGOT)) == __MODE_READING &&
        ( ref == SEEK_SET || ref == SEEK_CUR )) {

      long fpos = lseek(fp->fd, 0L, SEEK_CUR);
      if( fpos == -1 ) {
         __thr_safe_unlock(fp);
         return EOF;
      }

      if( ref == SEEK_CUR ) {
         ref = SEEK_SET;
         offset = fpos + offset + fp->bufpos - fp->bufread;
      }
      if( ref == SEEK_SET ) {
         if ( offset < fpos && offset >= fpos + fp->bufstart - fp->bufread ) {
            fp->bufpos = offset - fpos + fp->bufread;
            __thr_safe_unlock(fp);
            return 0;
         }
      }
   }
#endif
   /* Use fflush to sync the pointers */
   if (__fflush(fp) == EOF) {
      __thr_safe_unlock(fp);
      return EOF;
   }
   if (lseek(fp->fd, (off_t)offset, ref) < 0) {
      __thr_safe_unlock(fp);
      return EOF;
   }

   __thr_safe_unlock(fp);
   return 0;
}

/*-------------------------------------------
| Name:ftell
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
long __ftell(FILE * fp){
   int pos;

   __thr_safe_lock(fp);

   if (__fflush(fp) == EOF) {
      __thr_safe_unlock(fp);
      return EOF;
   }

   pos=lseek(fp->fd, 0L, SEEK_CUR);

   __thr_safe_unlock(fp);

   return pos;
}

/*-------------------------------------------
| Name:__fopen
| Description:
| Parameters:
| Return Type:
| Comments:This Fopen is all three of fopen,
|          fdopen and freopen. The macros in
|          stdio.h show the other names.
| See:
---------------------------------------------*/
FILE * __fopen(const char *fname,int fd,FILE *fp,char *mode)
{
   int open_mode = 0;
#if __MODE_IOTRAN
   int do_iosense = 1;
#endif
   int fopen_mode = 0;
   FILE *nfp = 0;

   /* If we've got an fp close the old one (freopen) */
   if (fp)
   {
      /* Careful, don't de-allocate it */
      fopen_mode |= (fp->mode & (__MODE_BUF | __MODE_FREEFIL | __MODE_FREEBUF));
      fp->mode &= ~(__MODE_FREEFIL | __MODE_FREEBUF);
      __fclose(fp);
   }

   /* decode the new open mode */
   while (*mode)
      switch (*mode++)
      {
      case 'r':
         fopen_mode |= __MODE_READ;
         break;
      case 'w':
         fopen_mode |= __MODE_WRITE;
         open_mode = (O_CREAT | O_TRUNC);
         break;
      case 'a':
         fopen_mode |= __MODE_WRITE;
         open_mode = (O_CREAT | O_APPEND);
         break;
      case '+':
         fopen_mode |= __MODE_RDWR;
         break;
#if __MODE_IOTRAN
      case 'b':         /* Binary */
         fopen_mode &= ~__MODE_IOTRAN;
         do_iosense=0;
         break;
      case 't':         /* Text */
         fopen_mode |= __MODE_IOTRAN;
         do_iosense=0;
         break;
#endif
      }

   /* Add in the read/write options to mode for open() */
   switch (fopen_mode & (__MODE_READ | __MODE_WRITE))
   {
   case 0:
      return 0;
   case __MODE_READ:
      open_mode |= O_RDONLY;
      break;
   case __MODE_WRITE:
      open_mode |= O_WRONLY;
      break;
   default:
      open_mode |= O_RDWR;
      break;
   }

   /* Allocate the (FILE) before we do anything irreversable */
   if (fp == 0)
   {
      nfp = malloc(sizeof(FILE));
      if (nfp == 0)
         return 0;
   }

   /* Open the file itself */
   if (fname)
      fd = open(fname, open_mode, 0666);

   if (fd < 0)                  /* Grrrr */
   {
      if (nfp)
         free(nfp);
      return 0;
   }

   /* If this isn't freopen create a (FILE) and buffer for it */
   if (fp == 0)
   {
      fp = nfp;
      fp->next = __IO_list;
      __IO_list = fp;

      fp->mode = __MODE_FREEFIL;
      if( __isatty(fd) )
      {
         fp->mode |= _IOLBF;
#if __MODE_IOTRAN
         if( do_iosense ) fopen_mode |= __MODE_IOTRAN;
#endif
      }
      else
         fp->mode |= _IOFBF;
      fp->bufstart = malloc(BUFSIZ);
      if (fp->bufstart == 0)    /* Oops, no mem */
      {                         /* Humm, full buffering with a two(!) byte
                                 * buffer. */
         fp->bufstart = fp->unbuf;
         fp->bufend = fp->unbuf + sizeof(fp->unbuf);
      }
      else
      {
         fp->bufend = fp->bufstart + BUFSIZ;
         fp->mode |= __MODE_FREEBUF;
      }
   }

   /* Ok, file's ready clear the buffer and save important bits */
   fp->bufpos = fp->bufread = fp->bufwrite = fp->bufstart;
   fp->mode |= fopen_mode;
   fp->fd = fd;

   return fp;
}

/*-------------------------------------------
| Name:fclose
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __fclose(FILE *fp){
   int rv = 0;
   //int errno;

   if (fp == 0)
   {
      errno = EINVAL;
      return EOF;
   }
   if (__fflush(fp))
      return EOF;

   if (close(fp->fd))
      rv = EOF;
   fp->fd = -1;

   if (fp->mode & __MODE_FREEBUF)
   {
      free(fp->bufstart);
      fp->mode &= ~__MODE_FREEBUF;
      fp->bufstart = fp->bufend = 0;
   }

   if (fp->mode & __MODE_FREEFIL)
   {
      FILE *prev = 0, *ptr;
      fp->mode = 0;

      for (ptr = __IO_list; ptr && ptr != fp; ptr = ptr->next)
         ;
      if (ptr == fp)
      {
         if (prev == 0)
            __IO_list = fp->next;
         else
            prev->next = fp->next;
      }
      free(fp);
   }
   else
      fp->mode = 0;

   return rv;
}

/*-------------------------------------------
| Name:setbuffer
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void __setbuffer(FILE * fp,char * buf,int size){
   __thr_safe_lock(fp);
   __fflush(fp);
   if( fp->mode & __MODE_FREEBUF ) free(fp->bufstart);
   fp->mode &= ~(__MODE_FREEBUF|__MODE_BUF);

   if( buf == 0 )
   {
      fp->bufstart = fp->unbuf;
      fp->bufend = fp->unbuf + sizeof(fp->unbuf);
      fp->mode |= _IONBF;
   }
   else
   {
      fp->bufstart = buf;
      fp->bufend = buf+size;
      fp->mode |= _IOFBF;
   }
   fp->bufpos = fp->bufread = fp->bufwrite = fp->bufstart;
   __thr_safe_unlock(fp);
}

/*-------------------------------------------
| Name:setvbuf
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __setvbuf(FILE * fp,char * buf,int mode,size_t size){
   __thr_safe_lock(fp);
   __fflush(fp);
   if( fp->mode & __MODE_FREEBUF ) free(fp->bufstart);
   fp->mode &= ~(__MODE_FREEBUF|__MODE_BUF);
   fp->bufstart = fp->unbuf;
   fp->bufend = fp->unbuf + sizeof(fp->unbuf);
   fp->mode |= _IONBF;

   if( mode == _IOFBF || mode == _IOLBF )
   {
      if( size <= 0  ) size = BUFSIZ;
      if( buf == 0 ) buf = malloc(size);
      if( buf == 0 ) {
         __thr_safe_unlock(fp);
         return EOF;
      }

      fp->bufstart = buf;
      fp->bufend = buf+size;
      fp->mode |= mode;
   }
   fp->bufpos = fp->bufread = fp->bufwrite = fp->bufstart;

   __thr_safe_unlock(fp);
   return 0;
}

/*-------------------------------------------
| Name:ungetc
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __ungetc(int c, FILE *fp) {
   unsigned char *p;

   __thr_safe_lock(fp);
   //__init_stdio();

   /* If can't read or there's been an error, or c == EOF, or ungot slot
    * already filled, then return EOF */
   /*
    * This can only happen if an fgetc triggered a read (that filled
    * the buffer for case 2 above) and then we ungetc 3 chars.
    */
   if (!READABLE(fp) || (fp->mode & (__MODE_UNGOT | __MODE_ERR))
       || (c == EOF) ) {
      __thr_safe_unlock(fp);
      return EOF;
   }

   if (WRITING(fp)) {         /* Commit any write-buffered chars. */
      fflush(fp);
   }

   if (fp->bufpos > fp->bufstart) { /* We have space before bufpos. */
      p = --fp->bufpos;
   } else if (fp->bufread == fp->bufpos) { /* Buffer is empty. */
      p = fp->bufread++;
   } else {
      fp->mode |= __MODE_UNGOT;
      __thr_safe_unlock(fp);
      //p = &(fp->ungot);
      return EOF;
   }
   fp->mode &= ~(__MODE_EOF); /* Clear EOF indicator. */

   if (*p != (unsigned char) c) { /* Don't store if same, because could */
      *p = (unsigned char) c; /* be sscanf from a const string!!! */
   }

   __thr_safe_unlock(fp);
   return c;
}

/*===========================================
End of Source _stdio.c
=============================================*/





