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

#include "kernel/core/errno.h"
#include "kernel/core/kernel.h"
#include "kernel/core/dirent.h"
#include "kernel/core/system.h"
#include "kernel/core/systime.h"
#include "kernel/core/stat.h"

#include "kernel/fs/vfs/vfstypes.h"
#include "kernel/fs/vfs/vfsdev.h"
#include "rootfscore.h"
#include "rootfs.h"

#include <string.h>
#include <stdlib.h>

/*===========================================
Global Declaration
=============================================*/
static const char _rtfsstr[]=".";
static const char __rtfsstr[]="..";


fsop_t rootfs_op={
   _rtfs_loadfs,
   _rtfs_checkfs,
   _rtfs_makefs,
   _rtfs_readfs,
   _rtfs_writefs,
   _rtfs_statfs,
   _rtfs_mountdir,
   _rtfs_readdir,
   _rtfs_telldir,
   _rtfs_seekdir,
   _rtfs_lookupdir,
   _rtfs_mknod,
   _rtfs_create,
   _rtfs_open,    //open
   _rtfs_close,   //close
   _rtfs_read,    //read
   _rtfs_write,   //write
   _rtfs_seek,    //seek
   _rtfs_truncate,
   _rtfs_remove,
   _rtfs_rename
};

/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:_rtfs_fpos
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _rtfs_fpos(int offset,int* blk,int* pos)
{
   *blk  = (offset) / (RTFS_BLOCK_SIZE);
   *pos  = (offset) % (RTFS_BLOCK_SIZE);
}

/*-------------------------------------------
| Name:_rtfs_mountdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _rtfs_mountdir(desc_t desc,inodenb_t original_root_node,inodenb_t target_root_node){
   int r;
   rtfs_block_dir_t      dir;
   _rtfs_seek(desc,0,SEEK_SET);
   while( (r=_rtfs_read(desc,(char*)&dir,sizeof(rtfs_block_dir_t)))>0){
      if(dir.inode==original_root_node){
         int offset = sizeof(rtfs_block_dir_t);
         _rtfs_seek(desc,-1*offset,SEEK_CUR);
         dir.inode = (unsigned short)target_root_node;
         _rtfs_write(desc,(char*)&dir,sizeof(rtfs_block_dir_t));
         return 0;
      }
   }

   return -1;
}

/*-------------------------------------------
| Name:_rtfs_readdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _rtfs_readdir(desc_t desc,dirent_t* dirent){

   rtfs_block_dir_t      dir;
   int r;

   r  = _rtfs_read(desc,(char*)&dir,sizeof(rtfs_block_dir_t));
   if(r<=0)
      return -1;

   strcpy(dirent->d_name,dir.name);
   dirent->inodenb = dir.inode;

   return 0;
}

/*-------------------------------------------
| Name:_rtfs_telldir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _rtfs_telldir(desc_t desc){
   int loc;
   loc = _rtfs_seek(desc,0,SEEK_CUR)/sizeof(rtfs_block_dir_t);
   return loc;
}

/*-------------------------------------------
| Name:_rtfs_seekdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _rtfs_seekdir(desc_t desc,int loc){

   int pos = loc * sizeof(rtfs_block_dir_t);
   if(_rtfs_seek(desc,pos,SEEK_SET)<0)
      return -1;

   return loc;
}
/*-------------------------------------------
| Name:_rtfs_lookup
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
inodenb_t _rtfs_lookupdir(desc_t desc,char* filename){

   int r;

   rtfs_block_dir_t      dir;
   rtfs_inodenb_t          _inode=INVALID_INODE_NB;

   dir.inode   = (unsigned char)ofile_lst[desc].inodenb;

   if( (__rtfsinode_lst(dir.inode).attr&S_IFDIR) != S_IFDIR)
      return INVALID_INODE_NB; 

   //
   _rtfs_open(desc);
   while( (r=_rtfs_read(desc,(char*)&dir,sizeof(rtfs_block_dir_t))) )
   {
      //Match token and one entry in catalog
      if(strcmp(dir.name,filename))
         continue;

      _inode=dir.inode;
      break;
   }
   _rtfs_close(desc);

   return _inode;
}

/*-------------------------------------------
| Name:_rtfs_mknod
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _rtfs_mknod(desc_t desc,inodenb_t inodenb,dev_t dev){

   __rtfsinode_lst(inodenb).blk[0]=dev;

   return 0;
}

/*-------------------------------------------
| Name:_rtfs_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
inodenb_t _rtfs_create(desc_t desc,char* filename, int attr){

   rtfs_inodenb_t        _dir_inode;
   rtfs_inodenb_t        _inode;
   rtfs_block_dir_t    dir;

   static struct __timeval tv;//common

   if(strlen(filename)>RTFS_MAX_FILENAME){
      __kernel_set_errno(ENAMETOOLONG); 
      return -1; //file name is too long
   }

   _dir_inode = (rtfs_inodenb_t)ofile_lst[desc].inodenb;
   
   _inode=(rtfs_inodenb_t)_rtfs_allocnode();
   if(_inode==INVALID_RTFSNODE) 
      return -1;

   //set creation date
   _sys_gettimeofday(&tv,0L);
   __rtfsinode_lst(_inode).cmtime = tv.tv_sec;
   //
   __rtfsinode_lst(_inode).attr=attr;
   __rtfsinode_lst(_inode).size=0;

   //Add entry;
   _rtfs_open(desc);
   dir.inode=(unsigned char)_inode;
   strcpy(dir.name,filename);
   _rtfs_seek(desc,0,SEEK_END);
   _rtfs_write(desc,(char*)&dir,sizeof(rtfs_block_dir_t));
   _rtfs_close(desc);

   if((attr&S_IFDIR)!=S_IFDIR)
      return _inode;

   //Add in new directory "." an ".."
   ofile_lst[desc].inodenb=_inode;
   _rtfs_open(desc);
   _rtfs_seek(desc,0,SEEK_END);

   //"."
   dir.inode=(unsigned char)_inode;
   strcpy(dir.name,_rtfsstr);
   _rtfs_write(desc,(char*)&dir,sizeof(rtfs_block_dir_t));

   //".."
   dir.inode=(unsigned char)_dir_inode;
   strcpy(dir.name,__rtfsstr);
   _rtfs_write(desc,(char*)&dir,sizeof(rtfs_block_dir_t));

   _rtfs_close(desc);

   return _inode;
}


/*-------------------------------------------
| Name:_rtfs_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _rtfs_open(desc_t desc){

   rtfs_inodenb_t  _inode   = (rtfs_inodenb_t)ofile_lst[desc].inodenb;

   ofile_lst[desc].offset  =0;
   ofile_lst[desc].attr    = __rtfsinode_lst(_inode).attr;
   ofile_lst[desc].size    = __rtfsinode_lst(_inode).size;
   ofile_lst[desc].cmtime  = __rtfsinode_lst(_inode).cmtime;

   if((ofile_lst[desc].attr & (S_IFCHR | S_IFBLK )) ){
      ofile_lst[desc].ext.dev = __rtfsinode_lst(_inode).blk[0];
   }else if(ofile_lst[desc].attr & S_IFIFO){
      ofile_lst[desc].ext.pipe_desc = __rtfsinode_lst(_inode).blk[0] ;
   }

   return 0;
}

/*-------------------------------------------
| Name:_rtfs_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _rtfs_close(desc_t desc){


   rtfs_inodenb_t  _inode   = (rtfs_inodenb_t)ofile_lst[desc].inodenb;

   __rtfsinode_lst(_inode).size=ofile_lst[desc].size;

   if(IS_FSTATUS_MODIFIED(ofile_lst[desc].status)){
      static struct __timeval tv;//common
      //set modification date
      _sys_gettimeofday(&tv,0L);
      __rtfsinode_lst(_inode).cmtime = tv.tv_sec;
   }

   //
   if((ofile_lst[desc].attr & (S_IFCHR | S_IFBLK )) ){
      __rtfsinode_lst(_inode).blk[0] = ofile_lst[desc].ext.dev;
   }else if(ofile_lst[desc].attr & S_IFIFO){
      __rtfsinode_lst(_inode).blk[0] = ofile_lst[desc].ext.pipe_desc;
   }

   //
   ofile_lst[desc].offset  =  0;
   ofile_lst[desc].attr    =  S_IFNULL;
   ofile_lst[desc].size    =  0;

   return 0;
}

/*-------------------------------------------
| Name:_rtfs_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _rtfs_read(desc_t desc,char* buffer,int size){

   int r=0;
   rtfs_inodenb_t inode   = (rtfs_inodenb_t)ofile_lst[desc].inodenb;
   
   rtfs_blocknb_t blknb;

   int  blk;
   int  pos;
   int  cpsize=0;
   int  filsz = ofile_lst[desc].size;
   
   while( r<size && ofile_lst[desc].offset<filsz){

      _rtfs_fpos(ofile_lst[desc].offset,&blk,&pos);
      blknb=__rtfsinode_lst(inode).blk[blk];

      cpsize=RTFS_BLOCK_SIZE-pos;

      if( cpsize > (size-r))
         cpsize=size-r;
      if((ofile_lst[desc].offset+cpsize)>=filsz)
         cpsize=filsz-ofile_lst[desc].offset;
      
      memcpy(buffer+r,&rtfsblk_lst[blknb].byte[pos],cpsize);

      r+=cpsize;
      ofile_lst[desc].offset+=cpsize;
   }
   
   return r;
}

/*-------------------------------------------
| Name:_rtfs_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _rtfs_write(desc_t desc,char* buffer,int size){

   int w = 0;
   
   rtfs_inodenb_t inode   = (rtfs_inodenb_t)ofile_lst[desc].inodenb;

   rtfs_blocknb_t blknb;

   int  _blk;
   int _pos;

   int  blk;
   int pos;
   
   int  cpsize=size;

   /*if(!ofile_lst[desc].offset)
      ofile_lst[desc].offset--;*/

   _rtfs_fpos(ofile_lst[desc].offset-1,&blk,&pos);
   
   while( w<size ){

      _blk=blk;
      _pos=pos;

      _rtfs_fpos(ofile_lst[desc].offset++,&blk,&pos);

      if(blk>=MAX_RTFS_BLOCK)
         return -1;

      if( (_blk!=blk && ofile_lst[desc].offset>ofile_lst[desc].size) 
         || ofile_lst[desc].size==0){

         if((blknb=_rtfs_allocblk())==INVALID_RTFSBLOCK)//no block available
            break;

         __rtfsinode_lst(inode).blk[blk]=blknb;
      
      }else
         blknb=__rtfsinode_lst(inode).blk[blk];

      
      rtfsblk_lst[blknb].byte[pos]=buffer[w];

      w++;
      
      if( ofile_lst[desc].offset>(ofile_lst[desc].size))
         ofile_lst[desc].size = ofile_lst[desc].offset;
      
   }

   //set modified status
   ofile_lst[desc].status|=MSK_FSTATUS_MODIFIED;

   return w;
}

/*-------------------------------------------
| Name:_rtfs_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _rtfs_seek(desc_t desc, int offset, int origin)
{
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
         ofile_lst[desc].offset = (ofile_lst[desc].size)+offset;
      break;

      default:
      return -1;
   }

   return ofile_lst[desc].offset;
}

/*-------------------------------------------
| Name:_rtfs_truncate
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _rtfs_truncate(desc_t desc, off_t length){

   rtfs_inodenb_t inode   = (rtfs_inodenb_t)ofile_lst[desc].inodenb;

   int  _blk;
   int  _offset;

   int  blk;
   int offset;
   int pos = (int)(__rtfsinode_lst(inode).size);

   if(!__rtfsinode_lst(inode).size)return 0;
   
   _rtfs_fpos(pos,&blk,&offset);
   
   while( --pos>=length ){

      _blk=blk;
      _offset=offset;

      _rtfs_fpos(pos,&blk,&offset);

      if(_blk!=blk || !pos)
         _rtfs_freeblk(__rtfsinode_lst(inode).blk[_blk]);
   }

   _rtfs_fpos(pos,&blk,&offset);
   if((_blk!=blk))
      _rtfs_freeblk(__rtfsinode_lst(inode).blk[_blk]);

   ofile_lst[desc].size = pos+1;
   ofile_lst[desc].offset  = pos+1;
   __rtfsinode_lst(inode).size = pos+1;

   return 0;
}

/*-------------------------------------------
| Name:_rtfs_remove
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _rtfs_remove(desc_t desc_ancst,desc_t desc){
   //remove regular file
   rtfs_block_dir_t   dir;
   int r;
   int pos;
   int offset;
   
   //
   _rtfs_open(desc_ancst);
   while( (r=_rtfs_read(desc_ancst,(char*)&dir,sizeof(rtfs_block_dir_t))) )
   {
      if(ofile_lst[desc].inodenb == dir.inode)
         break;
   }

   if(!r)
      return -1;

   offset=-1*(int)(sizeof(rtfs_block_dir_t));

   pos = _rtfs_seek(desc_ancst,offset , SEEK_CUR);

   _rtfs_seek(desc_ancst, offset, SEEK_END);

   _rtfs_read(desc_ancst,(char*)&dir,sizeof(rtfs_block_dir_t));

   _rtfs_seek(desc_ancst, pos, SEEK_SET);

   _rtfs_write(desc_ancst,(char*)&dir,sizeof(rtfs_block_dir_t));

   //free all block of desc;
   _rtfs_truncate(desc,0);
   //free inode of desc
   _rtfs_freenode(ofile_lst[desc].inodenb);

   //truncate directory desc_ancst
   _rtfs_truncate(desc_ancst,(off_t)(ofile_lst[desc_ancst].size-sizeof(rtfs_block_dir_t)));


   _rtfs_close(desc_ancst);

   return 0;
}

/*-------------------------------------------
| Name:_rtfs_rename
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _rtfs_rename(desc_t desc,const char*  old_name, char* new_name){

   //remove regular file
   rtfs_block_dir_t   dir;
   int r;
   int pos;
   int offset;
   
   //
   while( (r=_rtfs_read(desc,(char*)&dir,sizeof(rtfs_block_dir_t))) ){
      if(!strcmp(dir.name,old_name))
         break;
   }

   if(!r)
      return -1;
   offset=-1*(int)(sizeof(rtfs_block_dir_t));

   //rewind on the entry matched
   pos = _rtfs_seek(desc,offset , SEEK_CUR);

   //change the entry name
   strcpy(dir.name,new_name);

   //write the new entry name
   _rtfs_write(desc,(char*)&dir,sizeof(rtfs_block_dir_t));

   return 0;
}

/*-------------------------------------------
| Name:_rtfs_loadfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _rtfs_loadfs(void){

   rtfs_inodenb_t _inode;

   _rtfs_core();

   //make root node
   _inode = (rtfs_inodenb_t)_rtfs_allocnode();
   __rtfsinode_lst(_inode).attr  =  S_IFDIR;
   __rtfsinode_lst(_inode).size  =  0;
   
   return 0;
   
}

/*===========================================
End of Sourcertfsroutine.c
=============================================*/
