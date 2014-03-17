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
#ifndef _UFSX_H
#define _UFSX_H


/*============================================
| Includes
==============================================*/
#define USE_UFSX

#include "kernel/fs/ufs/ufscore.h"

/*============================================
| Declaration
==============================================*/

//specific ufs function
int _ufsx_mountdir(desc_t desc,inodenb_t original_root_node,inodenb_t target_root_node);
int _ufsx_readdir(desc_t desc,dirent_t* dirent);
int _ufsx_telldir(desc_t desc);
int _ufsx_seekdir(desc_t desc,int loc);
inodenb_t _ufsx_lookupdir(desc_t desc,char* filename);
inodenb_t _ufsx_create(desc_t desc,char* filename, int attr);
int _ufsx_remove(desc_t desc_ancst,desc_t desc);
int _ufsx_rename(desc_t desc,const char*  old_name, char* new_name);
int _ufsx_statfs(mntdev_t* pmntdev,struct statvfs *statvfs);

extern fsop_t ufsx_op;



#endif
