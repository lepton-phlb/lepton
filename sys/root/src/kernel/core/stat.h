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
#ifndef _STAT_H
#define _STAT_H


/*===========================================
Includes
=============================================*/


/*===========================================
Declaration
=============================================*/
struct stat {
   dev_t st_dev;              //ID of device containing file
   ino_t st_ino;              //file serial number
   mode_t st_mode;            //mode of file (see below)
   nlink_t st_nlink;          //number of links to the file (not used)
   uid_t st_uid;              //user ID of file (not used)
   gid_t st_gid;              //group ID of file (not used)
   dev_t st_rdev;             //device ID (if file is character or block special)
   off_t st_size;             //file size in bytes (if file is a regular file)
   time_t st_atime;           //time of last access
   time_t st_mtime;           //time of last data modification
   time_t st_ctime;           //time of last status change
   //blksize_t st_blksize;    //a filesystem-specific preferred I/O block size for
   //this object.  In some filesystem types, this may
   //vary from file to file
   //blkcnt_t  st_blocks;     //number of blocks allocated for this object
};

//The following macros will test whether a file is of the specified type.
//The value m supplied to the macros is the value of st_mode from a stat structure.
//The macro evaluates to a non-zero value if the test is true, 0 if the test is false.

//Test for a block special file.
#define S_ISBLK(m) (m&S_IFBLK)
//Test for a character special file.
#define S_ISCHR(m) (m&S_IFCHR)
//Test for a directory.
#define S_ISDIR(m) (m&S_IFDIR)
//Test for a pipe or FIFO special file.
#define S_ISFIFO(m) (m&S_IFIFO)
//Test for a regular file.
#define S_ISREG(m) (m&S_IFREG)
//Test for a symbolic link.
#define S_ISLNK(m) (m&S_IFLNK)


/* POSIX masks for st_mode. */
#define S_IRWXU   00700         /* owner:  rwx------ */
#define S_IRUSR   00400         /* owner:  r-------- */
#define S_IWUSR   00200         /* owner:  -w------- */
#define S_IXUSR   00100         /* owner:  --x------ */

#define S_IRWXG   00070         /* group:  ---rwx--- */
#define S_IRGRP   00040         /* group:  ---r----- */
#define S_IWGRP   00020         /* group:  ----w---- */
#define S_IXGRP   00010         /* group:  -----x--- */

#define S_IRWXO   00007         /* others: ------rwx */
#define S_IROTH   00004         /* others: ------r-- */
#define S_IWOTH   00002         /* others: -------w- */
#define S_IXOTH   00001         /* others: --------x */



//The implementation may implement message queues, semaphores, or shared memory objects as distinct file types.
//The following macros test whether a file is of the specified type.
//The value of the buf argument supplied to the macros is a pointer to a stat structure.
//The macro evaluates to a non-zero value if the specified object is implemented as a distinct file type
//and the specified file type is contained in the stat structure referenced by buf.
//Otherwise, the macro evaluates to zero.

//Test for a message queue
#define S_TYPEISMQ(buf)
//Test for a semaphore
#define S_TYPEISSEM(buf)
//Test for a shared memory object
#define S_TYPEISSHM(buf)

//The following are declared as functions and may also be defined as macros.
//Function prototypes must be provided for use with an ISO C compiler.

//from unistd.h
/* Values for the second argument to access.
   These may be OR'd together.  */
#define  R_OK  4     /* Test for read permission.  */
#define  W_OK  2     /* Test for write permission.  */
#define  X_OK  1     /* Test for execute permission.  */
#define  F_OK  0     /* Test for existence.  */


#ifndef __cplusplus
int    chmod(const char *, mode_t);
int    fchmod(int, mode_t);
int    fstat(int, struct stat *);
int    lstat(const char *, struct stat *);
int    mkdir(const char *, mode_t);
int    mkfifo(const char *, mode_t);
int    mknod(const char *, mode_t, dev_t);
int    stat(const char *, struct stat *);
mode_t umask(mode_t);
int access (const char *, int);
int chmod(const char *path, mode_t mode);

#else
extern "C" {
int    chmod(const char *, mode_t);
int    fchmod(int, mode_t);
int    fstat(int, struct stat *);
int    lstat(const char *, struct stat *);
int    mkdir(const char *, mode_t);
int    mkfifo(const char *, mode_t);
int    mknod(const char *, mode_t, dev_t);
int    stat(const char *, struct stat *);
mode_t umask(mode_t);
int access (const char *, int);
int chmod(const char *path, mode_t mode);
}

#endif

#endif
