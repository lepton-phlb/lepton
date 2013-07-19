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
| Includes    
==============================================*/
#include <stdlib.h>
#include <string.h>

#include "kernel/core/errno.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/systime.h"
#include "kernel/core/stat.h"
#include "kernel/fs/vfs/vfstypes.h"


#include "kernel/fs/yaffs/core/yaffs_guts.h"
#include "kernel/fs/yaffs/core/yportenv.h"
#include "kernel/fs/yaffs/core/yaffs_trace.h"

#include "kernel/fs/yaffs/yaffscore.h"
#include "kernel/fs/yaffs/yaffs.h"


/*===========================================
Global Declaration
=============================================*/

//
fsop_t yaffs_op={
   _yaffs_loadfs,
   _yaffs_checkfs,
   _yaffs_makefs,
   _yaffs_readfs,
   _yaffs_writefs,
   _yaffs_statfs,
   _yaffs_mountdir,
   _yaffs_readdir,
   _yaffs_telldir,
   _yaffs_seekdir,
   _yaffs_lookupdir,
   _yaffs_mknod,
   _yaffs_create,
   _yaffs_open,    //open
   _yaffs_close,   //close
   _yaffs_read,    //read
   _yaffs_write,   //write
   _yaffs_seek,    //seek
   _yaffs_truncate,
   _yaffs_remove,
   _yaffs_rename
};

extern int yaffs_UnlinkObject(yaffs_Object *obj);//from yaffs_guts.c

/*===========================================
Implementation
=============================================*/


/*-------------------------------------------
| Name:_yaffs_mountdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _yaffs_mountdir(desc_t desc,inodenb_t original_root_node,inodenb_t target_root_node){
   return -1;
}

/*-------------------------------------------
| Name:_yaffs_readdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _yaffs_readdir(desc_t desc,dirent_t* dirent){
   struct iattr attr;
   struct yaffs_dirent_st* p_yaffs_dirent;
   yaffs_Object* p_yaffs_object=yaffs_core_context_lst[desc].obj;

   //get attr
   yaffs_GetAttributes(p_yaffs_object, &attr);
   //
   ofile_lst[desc].size     = attr.ia_size;

   if(!(p_yaffs_dirent = _yaffs_core_readdir(desc)))
      return -1;

   strcpy (dirent->d_name,p_yaffs_dirent->d_name);
   dirent->inodenb = __cvt2logicnode(desc,p_yaffs_dirent->d_ino);

   return 0;
}

/*-------------------------------------------
| Name:_yaffs_telldir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _yaffs_telldir(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:_yaffs_seekdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _yaffs_seekdir(desc_t desc,int loc){
   loc=0;
   return loc;
}

/*-------------------------------------------
| Name:_yaffs_lookupdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
inodenb_t _yaffs_lookupdir(desc_t desc,char* filename){
   inodenb_t   _ino_logic= ofile_lst[desc].inodenb;
   yaffs_ino_t _ino_phys= 0;
   yaffs_Object* p_yaffs_object_dir=yaffs_core_context_lst[desc].obj;
   yaffs_Object* p_yaffs_object;

   _ino_phys = (yaffs_ino_t)__cvt2physnode(desc,_ino_logic);

   //find parent object directory
   if(!p_yaffs_object_dir){//normaly is already set by _yaffs_open().
      yaffs_Device* p_yaffs_dev=(yaffs_Device*)ofile_lst[desc].pmntdev->fs_info.yaffs_info.p_yaffs_dev;
      //
      if(!_ino_phys)//find object by id (yaffs inode number)
         p_yaffs_object_dir = p_yaffs_dev->rootDir;
      else
         p_yaffs_object_dir=yaffs_FindObjectByNumber(p_yaffs_dev,_ino_phys);
      //
      if(!p_yaffs_object_dir)
         return INVALID_INODE_NB;
      yaffs_core_context_lst[desc].obj=p_yaffs_object_dir;
   }

   //find entry in directory
   p_yaffs_object=yaffs_FindObjectByName(p_yaffs_object_dir,filename);
   if(!p_yaffs_object)
      return INVALID_INODE_NB;
   //
   p_yaffs_object = yaffs_GetEquivalentObject(p_yaffs_object);
   //
   _ino_logic = __cvt2logicnode(desc,p_yaffs_object->objectId);

   return _ino_logic;
}

/*-------------------------------------------
| Name:_rtfs_mknod
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _yaffs_mknod(desc_t desc,inodenb_t inodenb,dev_t dev){
   return 0;
}

/*-------------------------------------------
| Name:_yaffs_create
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
inodenb_t _yaffs_create(desc_t desc,char* filename, int attr){
   inodenb_t   _ino_logic= ofile_lst[desc].inodenb;
   yaffs_ino_t _ino_phys= 0;
   yaffs_Object* p_yaffs_object_dir=yaffs_core_context_lst[desc].obj;
   yaffs_Object* p_yaffs_object;

   _ino_phys = (yaffs_ino_t)__cvt2physnode(desc,_ino_logic);
   //create entry
   if((attr&S_IFDIR)==S_IFDIR){
      p_yaffs_object=yaffs_MknodDirectory(p_yaffs_object_dir,filename,0777,0,0);
   }else{
      p_yaffs_object=yaffs_MknodFile(p_yaffs_object_dir,filename,0777,0,0);
   }
   //
   if(!p_yaffs_object)
      return INVALID_INODE_NB;

   _ino_logic = __cvt2logicnode(desc,p_yaffs_object->objectId);

   return _ino_logic;
}

/*-------------------------------------------
| Name:_yaffs_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _yaffs_open(desc_t desc){   

   inodenb_t   _ino_logic= ofile_lst[desc].inodenb;
   yaffs_ino_t _ino_phys= 0;
   yaffs_Object* p_yaffs_object;
   yaffs_Device* p_yaffs_dev=(yaffs_Device*)ofile_lst[desc].pmntdev->fs_info.yaffs_info.p_yaffs_dev;

   struct iattr attr;

   _ino_phys = (yaffs_ino_t)__cvt2physnode(desc,_ino_logic);

   if(!_ino_phys)//find object by id (yaffs inode number)
      p_yaffs_object = p_yaffs_dev->rootDir;
   else
      p_yaffs_object=yaffs_FindObjectByNumber(p_yaffs_dev,_ino_phys);

      
   if(!p_yaffs_object)
      return -1;

	p_yaffs_object = yaffs_GetEquivalentObject(p_yaffs_object);

   if(!p_yaffs_object)
      return -1;

   
   //get attr
   yaffs_GetAttributes(p_yaffs_object, &attr);

   switch(p_yaffs_object->variantType){
      case YAFFS_OBJECT_TYPE_DIRECTORY:
         ofile_lst[desc].attr|=S_IFDIR;
         _yaffs_core_opendir(desc);
      break;

      case YAFFS_OBJECT_TYPE_FILE:
         ofile_lst[desc].attr|=S_IFREG;
      break;

      default:
      return -1;
   }
   //
   ofile_lst[desc].size     = attr.ia_size;
   ofile_lst[desc].offset   = 0;//-1; //Open at begin of file //bug fix.
   ofile_lst[desc].cmtime   = attr.ia_mtime;

   //set yaffs object in ofile_lst misc pointer.
   yaffs_core_context_lst[desc].obj=p_yaffs_object;

   _ino_logic = __cvt2logicnode(desc,p_yaffs_object->objectId);

   return 0;
}

/*-------------------------------------------
| Name:_yaffs_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _yaffs_close(desc_t desc){

   yaffs_Object* p_yaffs_object=yaffs_core_context_lst[desc].obj;

   if(!p_yaffs_object)
      return -1;

   yaffs_FlushFile(p_yaffs_object,1,0);

   return 0;
}


/*-------------------------------------------
| Name:_yaffs_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _yaffs_write(desc_t desc, char* buf, int size){
   int cb;
   struct iattr attr;
   yaffs_Object* p_yaffs_object=yaffs_core_context_lst[desc].obj;

   //
   if(!p_yaffs_object)
      return -1;
   //
   cb= yaffs_WriteDataToFile(p_yaffs_object,buf,ofile_lst[desc].offset,size,0);
   //get attr
   yaffs_GetAttributes(p_yaffs_object, &attr);
   //
   ofile_lst[desc].size     = attr.ia_size;
   //
   if(cb>0)
      ofile_lst[desc].offset+=cb;
   else
      return -1;
   //
   return cb;
}

/*-------------------------------------------
| Name:_yaffs_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _yaffs_read(desc_t desc, char* buf, int size){
   int cb;
   struct iattr attr;
   yaffs_Object* p_yaffs_object=yaffs_core_context_lst[desc].obj;
   //
   if(!p_yaffs_object)
      return -1;
   //get attr
   yaffs_GetAttributes(p_yaffs_object, &attr);
   //
   ofile_lst[desc].size     = attr.ia_size;
   //
   if(!ofile_lst[desc].size)
      return 0;
   //
   if(ofile_lst[desc].offset>=ofile_lst[desc].size)
      return -1;
   //
   cb= yaffs_ReadDataFromFile(p_yaffs_object,buf,ofile_lst[desc].offset,size);
   //
   if(cb>0)
      ofile_lst[desc].offset+=cb;
   else
      return -1;
   //
   return cb;
}

/*-------------------------------------------
| Name:_yaffs_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _yaffs_seek(desc_t desc, int offset, int origin)
{
   struct iattr attr;
   yaffs_Object* p_yaffs_object=yaffs_core_context_lst[desc].obj;

   //get attr
   yaffs_GetAttributes(p_yaffs_object, &attr);
   //
   ofile_lst[desc].size     = attr.ia_size;
   //
   switch(origin)
   {
      case SEEK_SET:
         //Begin of the File
         ofile_lst[desc].offset=offset;
      break;

      case SEEK_CUR:
         //Current position of the file
         ofile_lst[desc].offset=ofile_lst[desc].offset+offset;
      break;

      case SEEK_END:
         //End of the File
         ofile_lst[desc].offset = ofile_lst[desc].size+offset;
      break;

      default:
      return -1;
   }

   return ofile_lst[desc].offset;
}

/*-------------------------------------------
| Name:_yaffs_truncate
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _yaffs_truncate(desc_t desc, off_t length){
   int result;
   struct iattr attr;
   yaffs_Object* p_yaffs_object=yaffs_core_context_lst[desc].obj;
   //
   if(!p_yaffs_object)
      return -1;
   //
   result = yaffs_ResizeFile(p_yaffs_object,length);
   //get attr
   yaffs_GetAttributes(p_yaffs_object, &attr);
   //
   ofile_lst[desc].size     = attr.ia_size;
	//
   return (result) ? 0 : -1;
}

/*-------------------------------------------
| Name:_yaffs_remove
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _yaffs_remove(desc_t desc_ancst,desc_t desc){
   int result;
   yaffs_Object* p_yaffs_object=yaffs_core_context_lst[desc].obj;
   //
   if(!p_yaffs_object)
      return -1;
   //
   result = yaffs_UnlinkObject(p_yaffs_object);
	//
   return (result) ? 0 : -1;
}

/*-------------------------------------------
| Name:_yaffs_rename
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _yaffs_rename(desc_t desc,const char*  old_name, char* new_name){

   return 0;
}

/*-------------------------------------------
| Name:_yaffs_loadfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _yaffs_loadfs(void){
   return 0;
}

/*============================================
| End of Source  : yaffs.c
==============================================*/