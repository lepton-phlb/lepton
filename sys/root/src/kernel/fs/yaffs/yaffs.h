/*--------------------------------------------
| Copyright(C) 2005 CHAUVIN-ARNOUX
|---------------------------------------------
| Project:         
| Project Manager: 
| Source:          yaffs.h
| Path:        C:\tauon\sys\root\src\kernel\fs\yaffs
| Authors:     
| Plateform:   
| Created:     
| Revision/Date: $Revision: 1.1 $  $Date: 2010-05-31 13:44:38 $ 
| Description: 
|---------------------------------------------
| Historic:    
|---------------------------------------------
| Authors     | Date     | Comments  
| $Log: not supported by cvs2svn $
|---------------------------------------------*/


/*============================================
| Compiler Directive   
==============================================*/
#ifndef _YAFFS_H
#define _YAFFS_H


/*============================================
| Includes 
==============================================*/


/*============================================
| Declaration  
==============================================*/


//specific ufs function
int _yaffs_loadfs(void);
int _yaffs_mountdir(desc_t desc,inodenb_t original_root_node,inodenb_t target_root_node);
int _yaffs_readdir(desc_t desc,dirent_t* dirent);
int _yaffs_telldir(desc_t desc);
int _yaffs_seekdir(desc_t desc,int loc); 
inodenb_t _yaffs_lookupdir(desc_t desc,char* filename);
int _yaffs_mknod(desc_t desc,inodenb_t inodenb,dev_t dev);
inodenb_t _yaffs_create(desc_t desc,char* filename, int attr);
int _yaffs_open(desc_t desc);
int _yaffs_close(desc_t desc);
int _yaffs_read(desc_t desc,char* buffer,int size);
int _yaffs_write(desc_t desc,char* buffer,int size);
int _yaffs_seek(desc_t desc, int offset, int origin);
int _yaffs_truncate(desc_t desc, off_t length);
int _yaffs_remove(desc_t desc_ancst,desc_t desc);
int _yaffs_rename(desc_t desc,const char*  old_name, char* new_name);

int _yaffs(desc_t dev_desc);

extern fsop_t yaffs_op;

#endif