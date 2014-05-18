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
#ifndef _PIPE_H
#define _PIPE_H


/*===========================================
Includes
=============================================*/
#include "kernel/core/kernelconf.h"
#include "kernel/fs/vfs/vfsdev.h"

/*===========================================
Declaration
=============================================*/


#define __MAX_PIPE  __KERNEL_MAX_PIPE

#define __PIPE_SIZE __KERNEL_PIPE_SIZE  //see size in kernel/kernelconf.h

typedef int pipe_desc_t;

extern dev_map_t dev_pipe_map;

typedef struct {
   pipe_desc_t pipe_desc;
   desc_t desc_r;
   desc_t desc_w;
   int size;
   char buf[__PIPE_SIZE];
}opipe_t;

extern opipe_t opipe_lst[__MAX_PIPE];

#endif
