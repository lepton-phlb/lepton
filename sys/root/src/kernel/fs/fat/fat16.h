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
#ifndef _FAT_16_H
#define _FAT_16_H
/*============================================
| Includes
==============================================*/
#include "kernel/fs/vfs/vfstypes.h"

/*============================================
| Declaration
==============================================*/

int _fat_loadfs(void);
int _fat_checkfs(mntdev_t* pmntdev);
int _fat_makefs(desc_t dev_desc,struct vfs_formatopt_t* vfs_formatopt);
int _fat_readfs(mntdev_t* pmntdev);
int _fat_writefs(mntdev_t* pmntdev);
int _fat_mountdir(desc_t desc,inodenb_t original_root_node,inodenb_t target_root_node);
int _fat_telldir(desc_t desc);
int _fat_mknod(desc_t desc,inodenb_t inodenb,dev_t dev);
int _fat_seek(desc_t desc, int offset, int origin);
int _fat_close(desc_t desc);
int _fat_read(desc_t desc,char* buffer,int size);
int _fat_write(desc_t desc,char* buffer,int size);
int _fat_truncate(desc_t desc, off_t length);
int _fat_seekdir(desc_t desc,int loc);
//
extern fsop_t fat_msdos_op;
extern fsop_t fat_vfat_op;
#endif //_FAT_16_H

