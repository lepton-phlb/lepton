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
#ifndef _LIBSTD_H
#define _LIBSTD_H


/*===========================================
Includes
=============================================*/
#include "kernel/core/process.h"

/*===========================================
Declaration
=============================================*/

#ifdef __cplusplus
extern "C" {
#endif
pid_t    execl(const char* path, const char* arg,...);
pid_t    execv(const char* path, const char* argv[]);
pid_t    execve(const char* path, const char* argv[], const char* envp);
pid_t    vfork(void);
int      pause(void);
int      dup(int fildes);
int      dup2(int fildes, int fildes2);
int      pipe(int fildes[2]);
int      ftruncate(int fildes, off_t length);
int      truncate(const char *path, off_t length);
int      rmdir(const char *path);
int      chdir(const char *path);
//
pid_t _system_setpgid(pid_t pid, pid_t id_grp);
pid_t _system_getpgrp(void);
pid_t _system_getpgrp2(pid_t pid);
int _system_system(const char * command);
//
unsigned int alarm(unsigned int seconds);
//
void _system_exit(int status);
int _system_atexit(void (*func)(void));
void* _system_malloc(size_t size);
void* _system_calloc(size_t nelem, size_t elsize);
void* _system_realloc(void *p, size_t size);
void  _system_free(void* p);
char* _system_getcwd(char *buf, size_t size);
//
int _system_usleep(useconds_t useconds);
//
void* _system_alloca(size_t size);

#ifdef __cplusplus
}
#endif

//surround _sys_getpid and _sys_getppid in process.h
#define getpid()  _sys_getpid()

#define getppid() _sys_getppid()


//pid_t _system_setpgid(pid_t pid, pid_t id_grp);
#define setpgid _system_setpgid

//pid_t _system_getpgrp(void);
#define getpgrp _system_getpgrp

//pid_t _system_getpgrp2(pid_t pid);
#define getpgrp2 _system_getpgrp2

//int _system_system(const char * command);
#define system _system_system

#define exit _system_exit

//int _system_atexit(void (*func)(void));
#define atexit _system_atexit

//void* _system_malloc(size_t size);
#define malloc _system_malloc

//void* _system_calloc(size_t nelem, size_t elsize);
#define calloc _system_calloc

//void* _system_realloc(void *p, size_t size);
#define realloc _system_realloc

//void  _system_free(void* p);
#define free _system_free

//char* _system_getcwd(char *buf, size_t size);
#define getcwd _system_getcwd

//
#define usleep _system_usleep

#ifdef alloca
#undef alloca
#endif

#define alloca _system_alloca

#define __isatty(fd) 0 //to do: implement isatty()

#ifdef errno
#undef errno
#endif
#define errno __sys_errno

#endif
