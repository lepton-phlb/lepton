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
const char dev_null_name[]="null\0";

int dev_null_load(void);
int dev_null_open(desc_t desc, int o_flag);
int dev_null_close(desc_t desc);
int dev_null_isset_read(desc_t desc);
int dev_null_isset_write(desc_t desc);
int dev_null_read(desc_t desc, char* buf,int size);
int dev_null_write(desc_t desc, const char* buf,int size);
int dev_null_seek(desc_t desc,int offset,int origin);
int dev_null_ioctl(desc_t desc,int request,va_list ap);

dev_map_t dev_null_map={
   dev_null_name,
   S_IFCHR,
   dev_null_load,
   dev_null_open,
   dev_null_close,
   dev_null_isset_read,
   dev_null_isset_write,
   dev_null_read,
   dev_null_write,
   dev_null_seek,
   dev_null_ioctl,
};


/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:dev_null_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_null_load(void){
   return 0;
}

/*-------------------------------------------
| Name:dev_null_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_null_open(desc_t desc, int o_flag){

   //
   if(o_flag & O_RDONLY) {
   }

   if(o_flag & O_WRONLY) {
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_null_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_null_close(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_null_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_null_isset_read(desc_t desc){
   return -1; //to do: return 0; or return -1
}

/*-------------------------------------------
| Name:dev_null_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_null_isset_write(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_null_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_null_read(desc_t desc, char* buf,int size){
   memset(buf,0x00,size);
   return size;
}

/*-------------------------------------------
| Name:dev_null_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_null_write(desc_t desc, const char* buf,int size){
   __fire_io(ofile_lst[desc].owner_pthread_ptr_write);
   return size;
}

/*-------------------------------------------
| Name:dev_null_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_null_seek(desc_t desc,int offset,int origin){
   return 0;
}

/*--------------------------------------------
| Name:        dev_null_ioctl
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_null_ioctl(desc_t desc,int request,va_list ap){
   return 0;
}

/*===========================================
End of Source dev_null.c
=============================================*/
