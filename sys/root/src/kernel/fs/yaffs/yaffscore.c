/*--------------------------------------------
| Copyright(C) 2005 CHAUVIN-ARNOUX
|---------------------------------------------
| Project:         
| Project Manager: 
| Source:          yaffscore.c
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
| Includes    
==============================================*/
#include "kernel/core/errno.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/systime.h"
#include "kernel/core/malloc.h"
#include "kernel/core/stat.h"


#include "kernel/fs/yaffs/core/yaffs_guts.h"
#include "kernel/fs/yaffs/core/yportenv.h"
#include "kernel/fs/yaffs/core/yaffs_trace.h"

#include "kernel/fs/yaffs/core/direct/yaffs_norif1.h"

#include "kernel/fs/yaffs/yaffscore.h"
#include "kernel/fs/yaffs/yaffs.h"


/*============================================
| Global Declaration 
==============================================*/
static yaffs_Device _yaffs_dev;

//used by yaffs_guts
unsigned yaffs_traceMask;
unsigned int yaffs_wr_attempts;

//used by yaffs nor sim (to be removed)
int random_seed;
int simulate_power_failure;

static struct ylist_head search_contexts;

struct yaffs_core_context_st yaffs_core_context_lst[MAX_OPEN_FILE];


/*============================================
| Implementation 
==============================================*/
/*-------------------------------------------
| Name:_yaffs_statfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _yaffs_statfs(mntdev_t* pmntdev,struct statvfs *statvfs){

   return 0;
}
/*-------------------------------------------
| Name:_yaffs_readfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _yaffs_readfs(mntdev_t* pmntdev){

   int err=0;

   pmntdev->inodetbl_size=1024*1024;//not yet dynamic.

   //see http://www.yaffs.net/yaffs-direct-user-guide
   // /M18-1 yaffs1 on M18 nor sim
   memset(&_yaffs_dev,0,sizeof(_yaffs_dev));
   _yaffs_dev.param.totalBytesPerChunk = 512;// report this parameters in yaffs_norif1.c.
   _yaffs_dev.param.nChunksPerBlock = 240;//report this parameters in yaffs_norif1.c. truncate( (block size/(chunk size+spare size+m18 size));
   _yaffs_dev.param.nReservedBlocks = 2;
   _yaffs_dev.param.startBlock = 238;//for x1062 //32;//0; // Can use block 0
   _yaffs_dev.param.endBlock = 253; //for x1062 2MBytes //64;//16/*31*/; // Last block
   _yaffs_dev.param.useNANDECC = 0; // use YAFFS's ECC
   _yaffs_dev.param.nShortOpCaches = 15; // Use caches
   _yaffs_dev.context = (void *) pmntdev;	// Used to identify the device in fstat.
   _yaffs_dev.param.writeChunkToNAND   = ynorif1_WriteChunkToNAND;
   _yaffs_dev.param.readChunkFromNAND  = ynorif1_ReadChunkFromNAND;
   _yaffs_dev.param.eraseBlockInNAND   = ynorif1_EraseBlockInNAND;
   _yaffs_dev.param.initialiseNAND     = ynorif1_InitialiseNAND;
   _yaffs_dev.param.deinitialiseNAND   = ynorif1_DeinitialiseNAND;

   pmntdev->fs_info.yaffs_info.p_yaffs_dev=&_yaffs_dev;

   //   
   if(!_yaffs_dev.isMounted){
      int result;
	   //dev->readOnly = readOnly ? 1 : 0;
	   result = yaffs_GutsInitialise(&_yaffs_dev);
      if(result == YAFFS_FAIL){
		   // todo error - mount failed (-ENOMEM);
	      err = result ? 0 : -1;
      }
   }else{
      //todo error - already mounted. -EBUSY;
      err = -1;
   }

   return err;
}

/*-------------------------------------------
| Name:_yaffs_writefs
| Description:
| Parameters:
| Return Type:
| Comments:0
| See:
---------------------------------------------*/
int _yaffs_writefs(mntdev_t* pmntdev){
   yaffs_FlushEntireDeviceCache((yaffs_Device*)pmntdev->fs_info.yaffs_info.p_yaffs_dev);
   return 0;
}

/*-------------------------------------------
| Name:_yaffs_makefs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _yaffs_makefs(desc_t dev_desc,struct vfs_formatopt_t* vfs_formatopt){
   return 0;
}

/*-------------------------------------------
| Name:_yaffs_checkfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _yaffs_checkfs(mntdev_t* pmntdev){
   return 0;
}

/*--------------------------------------------
| Name:        yaffsfs_CurrentTime
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
unsigned long yaffsfs_CurrentTime(){
   static struct __timeval tv;
   _sys_gettimeofday(&tv,0L);
   return tv.tv_sec;
}

/*--------------------------------------------
| Name:        yaffs_malloc
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void *yaffs_malloc(size_t size){
   return _sys_malloc(size);
}

/*--------------------------------------------
| Name:        yaffs_free
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void yaffs_free(void *ptr){
   _sys_free(ptr);
}

/*--------------------------------------------
| Name:        _yaffs_core_dir_rewind
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static void _yaffs_core_dir_rewind(yaffs_dir_search_context_t *dsc)
{
	if(dsc &&
	   dsc->dirObj &&
	   dsc->dirObj->variantType == YAFFS_OBJECT_TYPE_DIRECTORY){

           dsc->offset = 0;

           if( ylist_empty(&dsc->dirObj->variant.directoryVariant.children))
                dsc->nextReturn = NULL;
           else
                dsc->nextReturn = ylist_entry(dsc->dirObj->variant.directoryVariant.children.next,
                                                yaffs_Object,siblings);
        } else {
		/* Hey someone isn't playing nice! */
	}
}

/*--------------------------------------------
| Name:        _yaffs_core_dir_advance
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static void _yaffs_core_dir_advance(yaffs_dir_search_context_t *dsc)
{
   if(dsc &&
	   dsc->dirObj &&
           dsc->dirObj->variantType == YAFFS_OBJECT_TYPE_DIRECTORY){

           if( dsc->nextReturn == NULL ||
               ylist_empty(&dsc->dirObj->variant.directoryVariant.children))
                dsc->nextReturn = NULL;
           else {
                   struct ylist_head *next = dsc->nextReturn->siblings.next;

                   if( next == &dsc->dirObj->variant.directoryVariant.children)
                        dsc->nextReturn = NULL; /* end of list */
                   else
                        dsc->nextReturn = ylist_entry(next,yaffs_Object,siblings);
           }
        } else {
                /* Hey someone isn't playing nice! */
   }
}

/*--------------------------------------------
| Name:        _yaffs_core_remove_object_callback
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static void _yaffs_core_remove_object_callback(yaffs_Object *obj){
   struct ylist_head *i;
   yaffs_dir_search_context_t *dsc;

   /* if search contexts not initilised then skip */
   if(!search_contexts.next)
          return;

   /* Iterate through the directory search contexts.
   * If any are the one being removed, then advance the dsc to
   * the next one to prevent a hanging ptr.
   */
   ylist_for_each(i, &search_contexts) {
      if (i) {
         dsc = ylist_entry(i, yaffs_dir_search_context_t,others);
         if(dsc->nextReturn == obj)
            _yaffs_core_dir_advance(dsc);
      }
   }

}

/*--------------------------------------------
| Name:        _yaffs_core_opendir
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _yaffs_core_opendir(desc_t desc)
{
 	yaffs_Object *obj = NULL;
	yaffs_dir_search_context_t *dsc = NULL;

   obj= yaffs_core_context_lst[desc].obj;

	if(obj && obj->variantType == YAFFS_OBJECT_TYPE_DIRECTORY){

		dsc = &yaffs_core_context_lst[desc].dir;
		//
		memset(dsc,0,sizeof(yaffs_dir_search_context_t));
      //
      dsc->magic = YAFFS_MAGIC;
      dsc->dirObj = obj;
      //
      yaffs_GetObjectName(obj,dsc->name,NAME_MAX);
      //yaffs_strncpy(dsc->name,dirname,NAME_MAX);
      //
      YINIT_LIST_HEAD(&dsc->others);

      if(!search_contexts.next)
              YINIT_LIST_HEAD(&search_contexts);

      ylist_add(&dsc->others,&search_contexts);       
      _yaffs_core_dir_rewind(dsc);

   }else{
      return -1;
   }
	
	return 0;
}

/*--------------------------------------------
| Name:        _yaffs_core_readdir
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
struct yaffs_dirent_st *_yaffs_core_readdir(desc_t desc){
	yaffs_dir_search_context_t *dsc = &yaffs_core_context_lst[desc].dir;
	struct yaffs_dirent_st *retVal = NULL;

	if(dsc && dsc->magic == YAFFS_MAGIC){
		//yaffsfs_SetError(0);
		if(dsc->nextReturn){
			dsc->de.d_ino = yaffs_GetEquivalentObject(dsc->nextReturn)->objectId;
			dsc->de.d_dont_use = (unsigned)dsc->nextReturn;
			dsc->de.d_off = dsc->offset++;
			yaffs_GetObjectName(dsc->nextReturn,dsc->de.d_name,NAME_MAX);
			if(yaffs_strnlen(dsc->de.d_name,NAME_MAX+1) == 0)
			{
				// this should not happen!
				yaffs_strcpy(dsc->de.d_name,_Y("zz"));
			}
			dsc->de.d_reclen = sizeof(struct yaffs_dirent_st);
			retVal = &dsc->de;
			_yaffs_core_dir_advance(dsc);
		} else
			retVal = NULL;
	} 
   /*else
		yaffsfs_SetError(-EBADF);*/

	return retVal;
}

/*--------------------------------------------
| Name:        _yaffs_core_rewinddir
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void _yaffs_core_rewinddir(desc_t desc){
	yaffs_dir_search_context_t *dsc = &yaffs_core_context_lst[desc].dir;
	_yaffs_core_dir_rewind(dsc);
}

/*--------------------------------------------
| Name:        _yaffs_core_closedir
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _yaffs_core_closedir(desc_t desc){
	yaffs_dir_search_context_t *dsc = &yaffs_core_context_lst[desc].dir;
   dsc->magic = 0;
   ylist_del(&dsc->others); /* unhook from list */      
   return 0;
}

/*============================================
| End of Source  : yaffscore.c
==============================================*/
