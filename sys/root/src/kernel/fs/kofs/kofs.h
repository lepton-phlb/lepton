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
#ifndef _KOFS_H
#define _KOFS_H


/*============================================
| Includes
==============================================*/
#include "kernel/fs/vfs/vfstypes.h"
#include "kernel/fs/ufs/ufsdriver.h"


/*============================================
| Declaration
==============================================*/
#define KOFS_MAX_FILENAME 32

//specific kofs function
int _kofs_loadfs(void);
int _kofs_mountdir(desc_t desc,inodenb_t original_root_node,inodenb_t target_root_node);
int _kofs_readdir(desc_t desc,dirent_t* dirent);
int _kofs_telldir(desc_t desc);
int _kofs_seekdir(desc_t desc,int loc);
inodenb_t _kofs_lookupdir(desc_t desc,char* filename);
int _kofs_mknod(desc_t desc,inodenb_t inodenb,dev_t dev);
inodenb_t _kofs_create(desc_t desc,char* filename, int attr);
int _kofs_open(desc_t desc);
int _kofs_close(desc_t desc);
int _kofs_read(desc_t desc,char* buffer,int size);
int _kofs_write(desc_t desc,char* buffer,int size);
int _kofs_seek(desc_t desc, int offset, int origin);
int _kofs_truncate(desc_t desc, off_t length);
int _kofs_remove(desc_t desc_ancst,desc_t desc);
int _kofs_rename(desc_t desc,const char*  old_name, char* new_name);

int _kofs_checkfs(mntdev_t* pmntdev);
int _kofs_makefs(desc_t dev_desc,struct vfs_formatopt_t* vfs_formatopt);
int _kofs_readfs(mntdev_t* pmntdev);
int _kofs_writefs(mntdev_t* pmntdev);
int _kofs_statfs(mntdev_t* pmntdev,struct statvfs *statvfs);



extern fsop_t kofs_op;
#endif
