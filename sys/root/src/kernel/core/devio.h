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
Compiler Directive
=============================================*/
#ifndef _DEVIO_H
#define _DEVIO_H


/*===========================================
Includes
=============================================*/
#include "kernel/core/types.h"
#include "kernel/core/system.h"


/*===========================================
Declaration
=============================================*/

extern const int16_t STDIN_FILENO;
extern const int16_t STDOUT_FILENO;
extern const int16_t STDERR_FILENO;

#ifdef __cplusplus
extern "C" {
#endif
int   mount(int fstype,const char* dev_path, const char* mount_path);
int   umount(const char* mount_path);
void  sync(void);
int   creat(const char *path,mode_t mode);
int   open(const char *path, int oflag,mode_t mode);
int   close(int fildes);
int   remove(const char *path);
int   rename(const char *old_name, const char *new_name);


off_t    lseek(int fildes, off_t offset, int whence);
ssize_t  read(int fildes, void *buf, size_t nbyte);
ssize_t  write(int fildes, const void *buf, size_t nbyte);


int ioctl(int fildes, int request, ... );

#ifdef __cplusplus
}
#endif

//posix compatiblity: symbolic link not yet supported on lepton.
//in future development syscall unlink must be implemented.
#define unlink(__path__) remove(__path__)


#endif


