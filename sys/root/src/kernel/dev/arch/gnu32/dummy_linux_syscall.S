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

// The numbers come from <asm/unistd.h>
#define SYS_read                3
#define SYS_write               4
#define SYS_open                5
#define SYS_close               6
#define SYS_time                13
#define SYS_lseek               19
#define SYS_fsync               118
#define SYS_sigreturn           119
#define SYS_fdatasync           148
#define SYS_rt_sigreturn		173

#define NAME(X) _sys_##X 
#define END(X)  1: ; .type NAME(X),@function ; .size NAME(X),1b - NAME(X)

#define SYSCALL0(X) \
	.globl NAME(X) \
	NAME(x): \
    push %ebx; \
    lea     SYS_##x, %eax; \
    int $0x80; \
    pop %ebx; \
    ret; \
    END(x)


#define SYSCALL1(x)                             \
        .globl NAME(x) ;                        \
NAME(x):                                        \
        push %ebx;                              \
        mov 8(%esp), %ebx;                      \
        lea     SYS_##x, %eax;                  \
        int $0x80;                              \
        pop %ebx;                               \
        ret;                                    \
    END(x)

#define SYSCALL2(x)                             \
        .globl NAME(x) ;                        \
NAME(x):                                        \
        push %ebx;                              \
        mov 8(%esp), %ebx;                      \
        mov 12(%esp), %ecx;                     \
        lea     SYS_##x, %eax;                  \
        int $0x80;                              \
        pop %ebx;                               \
        ret;                                    \
    END(x)

#define SYSCALL3(x)                             \
        .globl NAME(x) ;                        \
NAME(x):                                        \
                                                \
        push %ebx;                              \
        mov 8(%esp), %ebx;                      \
        mov 12(%esp), %ecx;                     \
        mov 16(%esp), %edx;                     \
        lea     SYS_##x, %eax;                  \
        int $0x80;                              \
        pop %ebx;                               \
        ret;                                    \
        END(x)


#define SYSCALL4(x)                             \
        .globl NAME(x) ;                        \
NAME(x):                                        \
                                                \
        push %ebx;                              \
        push %esi;                              \
        mov 12(%esp), %ebx;                     \
        mov 16(%esp), %ecx;                     \
        mov 20(%esp), %edx;                     \
        mov 24(%esp), %esi;                     \
        lea     SYS_##x, %eax;                  \
        int $0x80;                              \
        pop %esi;                               \
        pop %ebx;                               \
        ret;                                    \
        END(x)

#define SYSCALL5(x)                             \
        .globl NAME(x) ;                        \
NAME(x):                                        \
        push %ebx;                              \
        push %esi;                              \
        push %edi;                              \
        mov 16(%esp), %ebx;                     \
        mov 20(%esp), %ecx;                     \
        mov 24(%esp), %edx;                     \
        mov 28(%esp), %esi;                     \
        mov 32(%esp), %edi;                     \
        lea     SYS_##x, %eax;                  \
        int $0x80;                              \
        pop %edi;                               \
        pop %esi;                               \
        pop %ebx;                               \
        ret;                                    \
        END(x)


//==========================================================================
// Initial asm stuff for all functions.
        .text
        .align  2


SYSCALL3(read)
SYSCALL3(write)
SYSCALL3(open)
SYSCALL1(close)
SYSCALL1(time)
SYSCALL3(lseek)
SYSCALL1(fsync)
SYSCALL1(fdatasync)

// ----------------------------------------------------------------------------
// Special support for returning from a signal handler. In theory no special
// action is needed, but with some versions of the kernel on some
// architectures that is not good enough. Instead returning has to happen
// via another system call.         

        .align 16
        .global _sys_restore_rt
_sys_restore_rt:
        movl    $SYS_rt_sigreturn, %eax
        int     $0x80
1:              
        .global _sys_restore
_sys_restore:
        popl    %eax
        movl    $SYS_sigreturn, %eax
        int     $0x80
1:              

/*
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
| Revision 1.1  2009/03/30 11:16:10  jjp
| First import of tauon
|
|---------------------------------------------*/
	
