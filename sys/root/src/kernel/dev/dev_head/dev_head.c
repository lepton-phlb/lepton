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
Includes
=============================================*/
#include <string.h>
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/system.h"
#include "kernel/core/stat.h"
#include "kernel/core/fcntl.h"

#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/fs/vfs/vfstypes.h"

/*===========================================
Global Declaration
=============================================*/
const char dev_head_name[]="head\0";

int dev_head_load(void);
int dev_head_open(desc_t desc, int o_flag);
int dev_head_close(desc_t desc);
int dev_head_isset_read(desc_t desc);
int dev_head_isset_write(desc_t desc);
int dev_head_read(desc_t desc, char* buf,int size);
int dev_head_write(desc_t desc, const char* buf,int size);
int dev_head_seek(desc_t desc,int offset,int origin);
int dev_head_ioctl(desc_t desc,int request,va_list ap);

dev_map_t dev_head_map={
   dev_head_name,
   S_IFCHR,
   dev_head_load,
   dev_head_open,
   dev_head_close,
   dev_head_isset_read,
   dev_head_isset_write,
   dev_head_read,
   dev_head_write,
   dev_head_seek,
   dev_head_ioctl,
};


/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:dev_head_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_head_load(void){
   return 0;
}

/*-------------------------------------------
| Name:dev_head_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_head_open(desc_t desc, int o_flag){

   return 0;
}

/*-------------------------------------------
| Name:dev_head_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_head_close(desc_t desc){

   return 0;
}

/*-------------------------------------------
| Name:dev_head_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_head_isset_read(desc_t desc){
   desc_t desc_link=ofile_lst[desc].desc_nxt[0];
   if(desc_link<0)
      return -1;
   if(!ofile_lst[desc_link].pfsop->fdev.fdev_isset_read)
      return -1;
   return ofile_lst[desc_link].pfsop->fdev.fdev_isset_read(desc_link);
}

/*-------------------------------------------
| Name:dev_head_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_head_isset_write(desc_t desc){
   desc_t desc_link=ofile_lst[desc].desc_nxt[1];
   if(desc_link<0)
      return -1;
   if(!ofile_lst[desc_link].pfsop->fdev.fdev_isset_write)
      return -1;
   return ofile_lst[desc_link].pfsop->fdev.fdev_isset_write(desc_link);
}

/*-------------------------------------------
| Name:dev_head_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_head_read(desc_t desc, char* buf,int size){
   desc_t desc_link=ofile_lst[desc].desc_nxt[0];
   if(desc_link<0)
      return -1;
   if(!ofile_lst[desc_link].pfsop->fdev.fdev_read)
      return -1;
   return ofile_lst[desc_link].pfsop->fdev.fdev_read(desc_link,buf,size);
}

/*-------------------------------------------
| Name:dev_head_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_head_write(desc_t desc, const char* buf,int size){
   desc_t desc_link=ofile_lst[desc].desc_nxt[1];
   if(desc_link<0)
      return -1;
   if(!ofile_lst[desc_link].pfsop->fdev.fdev_write)
      return -1;
   return ofile_lst[desc_link].pfsop->fdev.fdev_write(desc_link,buf,size);
}

/*-------------------------------------------
| Name:dev_head_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_head_seek(desc_t desc,int offset,int origin){
   int r;
   //
   if(ofile_lst[desc].oflag & O_RDONLY) {
      desc_t desc_link=ofile_lst[desc].desc_nxt[0];
      if(desc_link<0)
         return -1;
      if(!ofile_lst[desc_link].pfsop->fdev.fdev_seek)
         return -1;
      r=ofile_lst[desc_link].pfsop->fdev.fdev_seek(desc_link,offset,origin);
   }

   if(ofile_lst[desc].desc_nxt[0]==ofile_lst[desc].desc_nxt[1])
      return r;

   if(ofile_lst[desc].oflag & O_WRONLY) {
      desc_t desc_link=ofile_lst[desc].desc_nxt[1];
      if(desc_link<0)
         return -1;
      if(!ofile_lst[desc_link].pfsop->fdev.fdev_seek)
         return -1;
      r=ofile_lst[desc_link].pfsop->fdev.fdev_seek(desc_link,offset,origin);
   }
   return r;
}

/*--------------------------------------------
| Name:        dev_head_ioctl
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_head_ioctl(desc_t desc,int request,va_list ap){
   int r;
   //
   if(ofile_lst[desc].oflag & O_RDONLY) {
      desc_t desc_link=ofile_lst[desc].desc_nxt[0];
      if(desc_link<0)
         return 0;  //no yet avalaible, it's not an error
      if(!ofile_lst[desc_link].pfsop->fdev.fdev_ioctl)
         return -1;
      r=ofile_lst[desc_link].pfsop->fdev.fdev_ioctl(desc_link,request,ap);
   }

   if(ofile_lst[desc].desc_nxt[0]==ofile_lst[desc].desc_nxt[1])
      return r;

   if(ofile_lst[desc].oflag & O_WRONLY) {
      desc_t desc_link=ofile_lst[desc].desc_nxt[1];
      if(desc_link<0)
         return 0;  //no yet avalaible, it's not an error
      if(!ofile_lst[desc_link].pfsop->fdev.fdev_ioctl)
         return -1;
      r=ofile_lst[desc_link].pfsop->fdev.fdev_ioctl(desc_link,request,ap);
   }
   return r;
}


/*============================================
| End of Source  : dev_head.c
==============================================*/
