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
#ifndef _RTFSROUTINE_H
#define _RTFSROUTINE_H


/*===========================================
Includes
=============================================*/
#include "kernel/core/system.h"
#include "kernel/fs/rootfs/rootfscore.h"

/*===========================================
Declaration
=============================================*/

int _rtfs_loadfs(void);
int _rtfs_mountdir(desc_t desc,inodenb_t original_root_node,inodenb_t target_root_node);
int _rtfs_readdir(desc_t desc,dirent_t* dirent);
int _rtfs_telldir(desc_t desc);
int _rtfs_seekdir(desc_t desc,int loc);
inodenb_t _rtfs_lookupdir(desc_t desc,char* filename);
int _rtfs_mknod(desc_t desc,inodenb_t inodenb,dev_t dev);
inodenb_t _rtfs_create(desc_t desc,char* filename, int attr);
int _rtfs_open(desc_t desc);
int _rtfs_close(desc_t desc);
int _rtfs_read(desc_t desc,char* buffer,int size);
int _rtfs_write(desc_t desc,char* buffer,int size);
int _rtfs_seek(desc_t desc, int offset, int origin);
int _rtfs_truncate(desc_t desc, off_t length);
int _rtfs_remove(desc_t desc_ancst,desc_t desc);
int _rtfs_rename(desc_t desc,const char*  old_name, char* new_name);

int _rtfs(void);

extern fsop_t rootfs_op;

#endif
