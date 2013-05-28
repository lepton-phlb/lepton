/*--------------------------------------------
| Copyright(C) 2005 CHAUVIN-ARNOUX
|---------------------------------------------
| Project:         
| Project Manager: 
| Source:          yaffscore.h
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
#ifndef _YAFFSCORE_H
#define _YAFFSCORE_H


/*============================================
| Includes 
==============================================*/



/*============================================
| Declaration  
==============================================*/

typedef long yaffs_ino_t;

typedef struct yaffs_dirent_st{
    long d_ino;                 /* inode number */
    off_t d_off;                /* offset to this dirent */
    unsigned short d_reclen;    /* length of this d_name */
    YUCHAR  d_type;	/* type of this record */
    YCHAR d_name [NAME_MAX+1];   /* file name (null-terminated) */
    unsigned d_dont_use;	/* debug pointer, not for public consumption */
}yaffs_dirent_t;

typedef struct yaffs_dir_search_context_st{
	__u32 magic;
	struct yaffs_dirent_st de;		/* directory entry being used by this dsc */
	YCHAR name[NAME_MAX+1];		/* name of directory being searched */
        yaffs_Object *dirObj;           /* ptr to directory being searched */
        yaffs_Object *nextReturn;       /* obj to be returned by next readddir */
        int offset;
        struct ylist_head others;       
}yaffs_dir_search_context_t;

typedef struct yaffs_core_context_st{
   struct yaffs_dir_search_context_st    dir;
   yaffs_Object*                         obj;
}yaffs_core_context_st;


extern struct yaffs_core_context_st yaffs_core_context_lst[MAX_OPEN_FILE];

//

int   _yaffs_statfs(mntdev_t* pmntdev,struct statvfs *statvfs);
int   _yaffs_makefs(desc_t dev_desc,struct vfs_formatopt_t* vfs_formatopt);
int   _yaffs_readfs(mntdev_t* pmntdev);
int   _yaffs_writefs(mntdev_t* pmntdev);
int   _yaffs_checkfs(mntdev_t* pmntdev);

int _yaffs_core_opendir(desc_t desc);
struct yaffs_dirent_st *_yaffs_core_readdir(desc_t desc);
void _yaffs_core_rewinddir(desc_t desc);
int _yaffs_core_closedir(desc_t desc);




#endif