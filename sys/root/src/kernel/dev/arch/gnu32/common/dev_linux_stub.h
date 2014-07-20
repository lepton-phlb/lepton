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


/*============================================
| Compiler Directive
==============================================*/
#ifndef DEV_LINUX_STUB_H_
#define DEV_LINUX_STUB_H_
/*============================================
| Includes
==============================================*/
//voir /usr/include/bits/fcntl.h /usr/include/bits/stat.h
#define _O_RDWR   02
#define _O_CREAT  0100
#define _O_EXCL   0200
#define _O_SYNC   010000


#define _S_IREAD  0400 //S_IRUSR
#define _S_IWRITE 0200 //S_IWUSR
#define _O_TRUNC  01000

#ifndef __KERNEL_UCORE_ECOS
typedef int k_off_t;
typedef short k_ssize_t;
typedef int k_size_t;
typedef unsigned int k_time_t;

extern int _sys_open(const char *path, int oflag, ... );
extern int _sys_close(int fildes);
extern k_off_t _sys_lseek(int fildes, k_off_t offset, int whence);
extern k_ssize_t _sys_write(int fildes, const void *buf, k_size_t nbyte);
extern k_ssize_t _sys_read(int fildes, void *buf, k_size_t nbyte);
extern int _sys_fdatasync(int fildes);

extern k_time_t _sys_time(k_time_t * time);
#endif


/*============================================
| Declaration
==============================================*/


#endif /* DEV_LINUX_STUB_H_ */
