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

//based on uC-libc-140298\uC-libc\stdio2

#ifndef _L_STDIO_H
#define _L_STDIO_H

#include <stdarg.h>

//#include <features.h>
#include <kernel/core/kernelconf.h>
#include <kernel/core/types.h>
#include <kernel/core/kernel_pthread.h>
#include <lib/libc/libc.h>

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

#define _IOFBF		0x00	/* full buffering */
#define _IOLBF		0x01	/* line buffering */
#define _IONBF		0x02	/* no buffering */
#define __MODE_BUF	0x03	/* Modal buffering dependent on isatty */

#define __MODE_FREEBUF	0x04	/* Buffer allocated with malloc, can free */
#define __MODE_FREEFIL	0x08	/* FILE allocated with malloc, can free */

#define __MODE_READ	0x10	/* Opened in read only */
#define __MODE_WRITE	0x20	/* Opened in write only */
#define __MODE_RDWR	0x30	/* Opened in read/write */

#define __MODE_READING	0x40	/* Buffer has pending read data */
#define __MODE_WRITING	0x80	/* Buffer has pending write data */

#define __MODE_EOF	0x100	/* EOF status */
#define __MODE_ERR	0x200	/* Error status */
#define __MODE_UNGOT	0x400	/* Buffer has been polluted by ungetc */

#ifdef __MSDOS__
#define __MODE_IOTRAN	0x1000	/* MSDOS nl <-> cr,nl translation */
#else
#define __MODE_IOTRAN	0
#endif

/* when you add or change fields here, be sure to change the initialization
 * in stdio_init and fopen */
struct __stdio_file {
  unsigned char *bufpos;   /* the next byte to write to or read from */
  unsigned char *bufread;  /* the end of data returned by last read() */
  unsigned char *bufwrite; /* highest address writable by macro */
  unsigned char *bufstart; /* the start of the buffer */
  unsigned char *bufend;   /* the end of the buffer; ie the byte after the last
                              malloc()ed byte */

  int fd; /* the file descriptor associated with the stream */
  int mode;

  char unbuf[8];	   /* The buffer for 'unbuffered' streams */
  struct __stdio_file * next;
  kernel_pthread_mutex_t mutex;
};

#define EOF	(-1)
#ifndef NULL
#define NULL	(0)
#endif

typedef struct __stdio_file FILE;


//#define __thr_safe_lock(fp) if(fp && (fp->fd>=0) && (fp->fd<=2))   pthread_mutex_lock(&fp->mutex)
//#define __thr_safe_unlock(fp) if(fp && (fp->fd>=0) && (fp->fd<=2)) pthread_mutex_unlock(&fp->mutex)



//not thread safe
//low ram

   #ifdef __AS386_16__
      #define BUFSIZ	(256)
   #elif defined(CPU_WIN32) || defined(CPU_GNU32)
      #define BUFSIZ	(256)
   #elif defined(CPU_ARM9)
      #define BUFSIZ (256) //if you used shttpd and ftpd
   #elif defined(CPU_ARM7) || defined(CPU_M16C62)
      #define BUFSIZ (64)
   #elif defined(CPU_CORTEXM)
      #define BUFSIZ (32)
   #endif

#if !defined(__KERNEL_LOAD_LIB)
   extern FILE stdin[1];
   extern FILE stdout[1];
   extern FILE stderr[1];

#elif defined(__KERNEL_LOAD_LIB)
   typedef struct libc_stdio_data_st{
      FILE _stdin;
      FILE _stdout;
      FILE _stderr;
      unsigned char _buf_in[BUFSIZ];
      unsigned char _buf_out[BUFSIZ];
      unsigned char _buf_err[BUFSIZ];
   }libc_stdio_data_t;

   #define stdin  ((FILE*)(&((libc_stdio_data_t*)(__lib_data(libc)))->_stdin))
   #define stdout ((FILE*)(&((libc_stdio_data_t*)(__lib_data(libc)))->_stdout))
   #define stderr ((FILE*)(&((libc_stdio_data_t*)(__lib_data(libc)))->_stderr))
#endif
   //
   #define __thr_safe_lock(fp)   //if((fp->fd>=0) && (fp->fd<=2)) kernel_pthread_mutex_lock(&fp->mutex)
   #define __thr_safe_unlock(fp) //if((fp->fd>=0) && (fp->fd<=2)) kernel_pthread_mutex_unlock(&fp->mutex)

//c++
#ifdef __cplusplus
   extern "C" {
#endif

int __printf_r(const char * fmt, ...);
int __printf(const char * fmt, ...);
int __sprintf(char * sp, const char * fmt, ...);
int __fprintf(FILE * fp, const char * fmt, ...);
int __vprintf(const char *fmt,va_list ap);
int __vsprintf(char * sp,const char *fmt,va_list ap);
int __vsnprintf(char * buf, size_t size,const char *  format, va_list arg);
int __vfprintf(FILE *op,register const char *fmt,register va_list ap);
int __vfnprintf(FILE * op, size_t max_size, const char *fmt, va_list ap);

int __asprintf(char **app, const char *fmt, ...);
int __fnprintf(FILE * fp, size_t size, const char *fmt, ...);
int __snprintf(char *sp, size_t size, const char *fmt, ...);

int __scanf(const char * fmt, ...);
int __sscanf(const char * sp, const char * fmt, ...);
int __fscanf(FILE * fp, const char * fmt, ...);
int __vscanf(const char *fmt,va_list ap);
int __vsscanf(char * sp,const char *fmt, va_list ap);
//int __vfscanf(register FILE *fp,register char *fmt,va_list ap);
int __vfscanf(FILE *fp,const char *fmt,va_list ap);

void     __stdio_init();
void     __stdio_close_all(pid_t pid);
void     __io_init_vars();

int      __fputc(int ch, FILE *fp);
int      __fgetc(FILE *fp);
int      __fflush(FILE *fp);
char*    __gets(char *str);
int      __puts(char *str);
char*    __fgets(char *s,size_t count,FILE *f);
int      __fputs(char *str,FILE *fp);
int      __fread(char *buf,int size,int nelm,FILE *fp);
int      __fwrite(char *buf,int size,int nelm,FILE *fp);
void     __rewind(FILE * fp);
int      __fseek(FILE *fp,long offset,int ref);
long     __ftell(FILE * fp);
FILE*    __fopen(const char *fname,int fd,FILE *fp,char *mode);
int      __fclose(FILE *fp);
void     __setbuffer(FILE * fp,char * buf,int size);
int      __setvbuf(FILE * fp,char * buf,int mode,size_t size);
int      __ungetc(int c,FILE *fp);

#ifdef __cplusplus
   }
#endif

#define printf_r     __printf_r
#define printf       __printf
#define sprintf      __sprintf
#define fprintf      __fprintf
#define vprintf      __vprintf
#define vsprintf     __vsprintf
#define vsnprintf    __vsnprintf

#if USE_FULL_STDIO_PRINTF
   #define vfprintf(__op__,__fmt__,__ap__) __vfnprintf(__op__,(-1),__fmt__,__ap__)
   #define vfnprintf(__op__,__max_size__,__fmt__,__ap__) __vfnprintf(__op__,__max_size__,__fmt__,__ap__)
#else
   #define vfprintf(__op__,__fmt__,__ap__) __vfprintf(__op__,__fmt__,__ap__)
   #define vfnprintf(__op__,__max_size__,__fmt__,__ap__) (-1)
#endif


#define asprintf     __asprintf
#define fnprintf     __fnprintf
#define snprintf     __snprintf


#define scanf        __scanf
#define sscanf       __sscanf
#define fscanf       __fscanf
#define vscanf       __vscanf
#define vsscanf      __vsscanf
#define vfsscanf     __vfsscanf

#define stdio_close_all __stdio_close_all
#define io_init_vars    __io_init_vars
#define fputc           __fputc
#define fgetc           __fgetc
#define fflush          __fflush
#define gets            __gets
#define puts            __puts
#define fgets           __fgets
#define fputs           __fputs
#define fread           __fread
#define fwrite          __fwrite
#define rewind          __rewind
#define fseek           __fseek
#define ftell           __ftell
#define fclose          __fclose
#define setbuffer       __setbuffer
#define setvbuf         __setvbuf
#define ungetc          __ungetc


#define putc(c, stream)	\
    (((stream)->bufpos >= (stream)->bufwrite) ? __fputc((c), (stream))	\
                          : (unsigned char) (*(stream)->bufpos++ = (c))	)

#define getc(stream)	\
  (((stream)->bufpos >= (stream)->bufread) ? __fgetc(stream):		\
    (*(stream)->bufpos++))

#define putchar(c)   putc((c), stdout)
#define getchar()    getc(stdin)

#define ferror(fp)	(((fp)->mode&__MODE_ERR) != 0)
#define feof(fp)   	(((fp)->mode&__MODE_EOF) != 0)
#define clearerr(fp)	((fp)->mode &= ~(__MODE_EOF|__MODE_ERR),0)
#define fileno(fp)	((fp)->fd)



/* declare functions; not like it makes much difference without ANSI */
/* RDB: The return values _are_ important, especially if we ever use
        8086 'large' model
 */

/* These two call malloc */
#define setlinebuf(__fp)  __setvbuf((__fp), (char*)0, _IOLBF, 0)

/* These don't */
#define setbuf(__fp, __buf) __setbuffer((__fp), (__buf), BUFSIZ)

#define fopen(__file, __mode)         __fopen((__file), -1, (FILE*)0, (__mode))
#define freopen(__file, __mode, __fp) __fopen((__file), -1, (__fp), (__mode))
#define fdopen(__file, __mode)  __fopen((char*)0, (__file), (FILE*)0, (__mode))


#define stdio_pending(fp) ((fp)->bufread>(fp)->bufpos)

#endif /* __STDIO_H */
