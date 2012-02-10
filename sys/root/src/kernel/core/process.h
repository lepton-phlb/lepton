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
#ifndef _PROCESS_H
#define _PROCESS_H

/**
 * \addtogroup lepton_kernel
 * @{
 *
 */

/**
 * \addtogroup process
 * @{
 *
 *
 */

/*===========================================
Includes
=============================================*/
#include "kernel/core/types.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/signal.h"


#ifdef CPU_WIN32
   #pragma pack(push, 8)
#endif


#include "kernel/core/kernel_pthread.h"
#include "kernel/core/kernel_object.h"

/*===========================================
Declaration
=============================================*/

typedef unsigned char fds_bits_t;

//descriptor table
#ifndef OPEN_MAX
   #define OPEN_MAX        15
#endif

#define __FD_SETSIZE    OPEN_MAX //nb max of descriptor for each process
#define FD_SETSIZE      OPEN_MAX //nb max of descriptor for each process
#define __NFDBITS       (8 * sizeof( fds_bits_t))
#define __FDSET_LONGS   ((__FD_SETSIZE/__NFDBITS)+1)

extern const char __fds_size;
extern const unsigned char __shl_fds_bits;

//
#define ARG_LEN_MAX 64
#define ARG_MAX   20

//
#ifndef ATEXIT_MAX
   #define ATEXIT_MAX    0 //see atexit() and kernel/kernelconf.h
#endif

//
typedef int (*_main_routine_t)(int argc,char* argv[]);
typedef _main_routine_t process_routine_t;
//typedef _start_routine_t process_routine_t;

typedef void (*_atexit_func_t)(void);
typedef _atexit_func_t atexit_func_t;


/**
 * structure de controle d'un processus
 */
typedef struct {
   pid_t pid;           /**<numero d'identification du processus courant*/
   pid_t ppid;          /**<numero d'identification du processus pre du processus courant*/

   //process group id (session leader)
   pid_t pgid;
   uid_t uid;
   uid_t gid;

   //
   time_t start_time;

   //pthread based
   kernel_pthread_t* pthread_ptr; /**<Pointeur sur la structure du pthread associ au processus courant (voir kernel/kernel_pthread.h).*/

   //
   kernel_object_t* kernel_object_head;

   //
   inodenb_t inode_curdir; /**<Numro d'inoeud du rpertoire courant du processus.*/

   //descriptor table
   desc_t desc_tbl[__FD_SETSIZE];      /**<Table des descripteurs de fichiers qui est utilise par les fonctions _get_fd() et _put_fd().*/
   fds_bits_t fds_bits[__FDSET_LONGS];  /**<Champs de bits pour l'allocation des descripteurs de fichiers (0 libre, 1 utilis). Oprations ralises par les fonctions _get_fd() et _put_fd().*/
   fds_bits_t clx_bits[__FDSET_LONGS];  /**<Champs de bits pour le close on exec.*/
   //
   char arg[ARG_LEN_MAX];  /**<chaine de caractres contenant la ligne de commande*/
   int argc;   /**<nombre d'arguments dans la ligne de commande*/
   char* argv[ARG_MAX]; /**<liste des arguments dans ligne de commande, pointe directement sur arg[]*/

   int status; /**<*/

   process_routine_t process_routine; /**<Pointeur sur la routine principale du processus process_routine().*/

   //atexit function pointer
   atexit_func_t* p_atexit_func;

   //last error
   int _errno;            /**<Dernire erreur survenu dans le processus courant.*/

}process_t;

extern pid_t pid_counter;

#define PROCESS_MAX  __KERNEL_PROCESS_MAX

/**
 * liste des processus.
 */
extern process_t* __process_lst[PROCESS_MAX];

/**
 * pointeur sur la liste des processus mais dcall d'une entr de tel faon que le pid 1 corresponde  l'indice 0 dans la liste des processus __process_lst.
 */
extern process_t** process_lst;

/**
 * liste des threads associs au processus (1 thread <-> 1 processus).
 */
extern kernel_pthread_t* process_thread_lst;


#ifdef CPU_WIN32
void display_stack(HANDLE h, char* title);
#endif

//process pthread container operation
int _sys_process_insert_pthread(process_t* process,kernel_pthread_t* p);
int _sys_process_remove_pthread(process_t* process,kernel_pthread_t* p);

//pid allocation
void  _pid(void);
int   _nextpid(pid_t* pid);

//file descriptor allocation
int _get_fd(pid_t pid,int limit);
int _put_fd(pid_t pid,int fd);
int _copy_process_fd(pid_t pid, pid_t ppid,int oflags);
int _cloexec_process_fd(pid_t pid);
int _close_process_fd(pid_t pid);

//close on exec mask operation
int _set_cloexec(pid_t pid,int fd);
int _unset_cloexec(pid_t pid,int fd);
int _is_cloexec(pid_t pid,int fd);



//process system call
int _sys_dup(pid_t pid,int fd,int limit);
int _sys_dup2(pid_t pid,int fd,int fd2);
int _sys_fcntl(pid_t pid,unsigned int fd, unsigned int cmd, unsigned int argc,void* argv[]);


pid_t _sys_krnl_exec(const char *path, const char* argv[], const char* envp, pid_t ppid,pid_t pid);
pid_t _sys_exec(const char *path, const char* argv[], const char* envp,
                kernel_pthread_t* pthread_ptr);

#ifdef __cplusplus
extern "C" {
#endif

pid_t _sys_getpid(void);
pid_t _sys_getppid(void);

#ifdef __cplusplus
}
#endif

pid_t _sys_setpgid(pid_t pid, pid_t id_grp);
pid_t _sys_getpgrp(pid_t pid);
int   _sys_adopt(pid_t pid);
void  _sys_exit(pid_t pid,int status);
pid_t _sys_waitpid(pid_t pid,pid_t child_pid,int options,int* status);

//signal system call
int _sys_sigprocmask (kernel_pthread_t* pthread_ptr, int how,sigset_t* set,sigset_t* oset);
int _sys_sigpending  (kernel_pthread_t* pthread_ptr, sigset_t* set);
int _sys_sigaction   (kernel_pthread_t* pthread_ptr, int sig,struct sigaction* act,
                      struct sigaction* oact);
//send signal system call
int _sys_kill(kernel_pthread_t* pthread_ptr,int sig,int atomic);
int _sys_kill_exit(kernel_pthread_t* pthread_ptr);

int _sys_raise(pid_t pid,int sig);


int _sys_pthread_create(kernel_pthread_t** new_kernel_pthread,
                        kernel_pthread_t* parent_pthread_ptr,
                        pthread_attr_t *attr,
                        void *(*start_routine)(void*),void *arg, pid_t pid);

int _sys_pthread_cancel(kernel_pthread_t* kernel_pthread,pid_t pid);
int _sys_pthread_cancel_all_except(pid_t pid,kernel_pthread_t* except_pthread_ptr);


#define __sys_errno __kernel_pthread_errno

#ifdef CPU_WIN32
   #pragma pack(pop)
#endif

/** @} */
/** @} */

#endif
