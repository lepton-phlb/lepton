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
#ifndef _FCNTL_H
#define _FCNTL_H


/*===========================================
Includes
=============================================*/
#include "kernel/core/types.h"

/*===========================================
Declaration
=============================================*/

#define FCNTL_ARG_MAX   2

//Values for cmd used by fcntl() (the following values are unique): 
#define F_DUPFD      0x0001//Duplicate file descriptor. 
#define F_GETFD      0x0002//Get file descriptor flags. 
#define F_SETFD      0x0004//Set file descriptor flags. 
#define F_GETFL      0x0008//Get file status flags and file access modes. 
#define F_SETFL      0x0010//Set file status flags. 
#define F_GETLK      0x0020//Get record locking information. 
#define F_SETLK      0x0040//Set record locking information. 
#define F_SETLKW     0x0080//Set record locking information; wait if blocked. 


//File descriptor flags used for fcntl(): see system.h oflag
#define FD_CLOEXEC  0x8000  //Close the file descriptor upon execution of an exec family function. 


//Values for l_type used for record locking with fcntl() (the following values are unique): 
#define F_RDLCK  0x01 //Shared or read lock. 
#define F_UNLCK  0x02 //Unlock. 
#define F_WRLCK  0x03 //Exclusive or write lock. 

//The values used for l_whence, SEEK_SET, SEEK_CUR and SEEK_END are defined as described in <unistd.h>. 

//The following four sets of values for oflag used by open() are bitwise distinct: 
//see kernel/system.h
#define O_EXCL       0x0040//Exclusive use flag. 
#define O_NOCTTY     0x0080//Do not assign controlling terminal. 
#define O_TRUNC      0x0100//Truncate flag. 


//Mask for use with file access modes: 
#define O_ACCMODE

//
struct flock{
    short l_type; //F_RDLCK,F_WRLCK,F_UNLCK
    short l_whence;//SEEK_SET,SEEK_CUR, SEEK_END
    off_t l_start;
    off_t l_len; //0 <=> file
    pid_t l_pid;
};

//
#ifdef __cplusplus
extern "C" {
#endif
int fcntl(unsigned int fd, unsigned int cmd,...);
#ifdef __cplusplus
}
#endif

#endif
