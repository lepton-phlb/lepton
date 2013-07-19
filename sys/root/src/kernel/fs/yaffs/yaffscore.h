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



/*============================================
| Compiler Directive   
==============================================*/
#ifndef _YAFFSCORE_H
#define _YAFFSCORE_H


/*============================================
| Includes 
==============================================*/



/*============================================
| Declaration  
==============================================*/

typedef long yaffs_ino_t;

typedef struct yaffs_dirent_st{
    long d_ino;                 /* inode number */
    off_t d_off;                /* offset to this dirent */
    unsigned short d_reclen;    /* length of this d_name */
    YUCHAR  d_type;	/* type of this record */
    YCHAR d_name [NAME_MAX+1];   /* file name (null-terminated) */
    unsigned d_dont_use;	/* debug pointer, not for public consumption */
}yaffs_dirent_t;

typedef struct yaffs_dir_search_context_st{
	__u32 magic;
	struct yaffs_dirent_st de;		/* directory entry being used by this dsc */
	YCHAR name[NAME_MAX+1];		/* name of directory being searched */
        yaffs_Object *dirObj;           /* ptr to directory being searched */
        yaffs_Object *nextReturn;       /* obj to be returned by next readddir */
        int offset;
        struct ylist_head others;       
}yaffs_dir_search_context_t;

typedef struct yaffs_core_context_st{
   struct yaffs_dir_search_context_st    dir;
   yaffs_Object*                         obj;
}yaffs_core_context_st;


extern struct yaffs_core_context_st yaffs_core_context_lst[MAX_OPEN_FILE];

//

int   _yaffs_statfs(mntdev_t* pmntdev,struct statvfs *statvfs);
int   _yaffs_makefs(desc_t dev_desc,struct vfs_formatopt_t* vfs_formatopt);
int   _yaffs_readfs(mntdev_t* pmntdev);
int   _yaffs_writefs(mntdev_t* pmntdev);
int   _yaffs_checkfs(mntdev_t* pmntdev);

int _yaffs_core_opendir(desc_t desc);
struct yaffs_dirent_st *_yaffs_core_readdir(desc_t desc);
void _yaffs_core_rewinddir(desc_t desc);
int _yaffs_core_closedir(desc_t desc);




#endif