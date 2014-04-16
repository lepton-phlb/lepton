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
| Includes
==============================================*/
#include <stdlib.h>
#include <string.h>

#include "kernel/core/errno.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"
#include "kernel/core/system.h"
#include "kernel/core/systime.h"
#include "kernel/core/stat.h"
#include "kernel/core/kernel_object.h"
#include "kernel/fs/vfs/vfstypes.h"

#include "kofs.h"

/*============================================
| Global Declaration
==============================================*/

static const char _kofsstr[]=".";
static const char __kofsstr[]="..";

//
fsop_t kofs_op={
   _kofs_loadfs,
   _kofs_checkfs,
   _kofs_makefs,
   _kofs_readfs,
   _kofs_writefs,
   _kofs_statfs,
   _kofs_mountdir,
   _kofs_readdir,
   _kofs_telldir,
   _kofs_seekdir,
   _kofs_lookupdir,
   _kofs_mknod,
   _kofs_create,
   _kofs_open,    //open
   _kofs_close,   //close
   _kofs_read,    //read
   _kofs_write,   //write
   _kofs_seek,    //seek
   _kofs_truncate,
   _kofs_remove,
   _kofs_rename
};

typedef uint16_t kofs_ino_t;
typedef uint16_t kofs_attr_t;
typedef uint16_t kofs_size_t;

typedef struct kofs_node_st {
   kofs_ino_t ino;
   char name[KOFS_MAX_FILENAME];

   kofs_attr_t attr;
   kofs_size_t size;

   time_t cmtime;     ///creation/modifed time.

   void* p_o; //objects

   struct kofs_node_st *next;
}kofs_node_t;

typedef struct kofs_entry_st {
   kofs_ino_t ino;
   char name[KOFS_MAX_FILENAME];
}kofs_entry_t;

#define KOFS_INO_ROOT         0
#define KOFS_INO_PROC         1
#define KOFS_INO_PROC_LST     512
#define KOFS_INO_PROC_THREAD  1024
#define KOFS_INO_PROC_OBJECTS 2048
#define KOFS_INO_PROC_OBJECTS_OBJECT (2048+1024)

#define KOFS_INO_SEM          2
#define KOFS_INO_MQUEUE       3

#define KOFS_INO_ROOT_MAX                 (KOFS_INO_MQUEUE+1)
#define KOFS_INO_PROC_LST_MAX             (KOFS_INO_PROC_LST+(PROCESS_MAX+1))
#define KOFS_INO_PROC_THREAD_MAX          (KOFS_INO_PROC_THREAD+1024)
#define KOFS_INO_PROC_OBJECTS_MAX         (KOFS_INO_PROC_OBJECTS+1024)
#define KOFS_INO_PROC_OBJECTS_OBJECT_MAX  (KOFS_INO_PROC_OBJECTS_OBJECT+1024)


struct kofs_node_st kofs_root_node[]={
   {KOFS_INO_PROC,   "proc",      S_IFDIR, 0*sizeof(kofs_entry_t), (time_t)0, (void*)0,
    (struct kofs_node_st *)&kofs_root_node[KOFS_INO_PROC+1]},
   {KOFS_INO_SEM,    "sem",       S_IFDIR, 0*sizeof(kofs_entry_t), (time_t)0, (void*)0,
    (struct kofs_node_st *)&kofs_root_node[KOFS_INO_SEM+1]},
   {KOFS_INO_MQUEUE, "mqueue",    S_IFDIR, 0*sizeof(kofs_entry_t), (time_t)0, (void*)0, NULL}

};

const int kofs_root_node_sz=sizeof(kofs_root_node);
const int kofs_root_node_entry_max=sizeof(kofs_root_node)/sizeof(kofs_node_t);


/*============================================
| Implementation
==============================================*/

/*--------------------------------------------
| Name:        _kofs_itoa
| Description:
| Parameters:  none
| Return Type: none
| Comments: from http://www.jb.man.ac.uk/~slowe/cpp/itoa.html
| See:
----------------------------------------------*/
char* _kofs_ultoa(unsigned long value, char* result, int base) {

   char* ptr = result, *ptr1 = result, tmp_char;
   unsigned long tmp_value;

   // check that the base if valid
   if (base < 2 || base > 36) {
      *result = '\0';
      return result;
   }

   do {
      tmp_value = value;
      value /= base;
      *ptr++ =
         "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 +
                                                                                    (tmp_value -
                                                                                     value * base)];
   } while ( value );

   // Apply negative sign
   if (tmp_value < 0) *ptr++ = '-';
   *ptr-- = '\0';
   while(ptr1 < ptr) {
      tmp_char = *ptr;
      *ptr--= *ptr1;
      *ptr1++ = tmp_char;
   }
   return result;
}

/*-------------------------------------------
| Name:_kofs_statfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kofs_statfs(mntdev_t* pmntdev,struct statvfs *statvfs){

   memset(statvfs,0,sizeof(struct statvfs));

   statvfs->f_bsize  =0; //RTFS_BLOCK_SIZE;
   statvfs->f_frsize =0; //RTFS_BLOCK_SIZE;

   statvfs->f_blocks = 0; //rtfs_blkalloc_size<<3;//*8
   statvfs->f_namemax = KOFS_MAX_FILENAME;

   statvfs->f_bfree = 0;

   return 0;
}
/*-------------------------------------------
| Name:_kofs_makefs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kofs_makefs(desc_t dev_desc,struct vfs_formatopt_t* vfs_formatopt){

   return 0;
}

/*-------------------------------------------
| Name:_kofs_readfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kofs_readfs(mntdev_t* pmntdev){

   pmntdev->inodetbl_size =4096; //RTFS_NODETBL_SIZE;
   //_kofs_offset = pmntdev->inodenb_offset;

   return 0;
}

/*-------------------------------------------
| Name:_kofs_writefs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kofs_writefs(mntdev_t* pmntdev){
   return 0;
}

/*-------------------------------------------
| Name:_kofs_checkfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kofs_checkfs(mntdev_t* pmntdev){
   return 0;
}


/*-------------------------------------------
| Name:_kofs_mountdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kofs_mountdir(desc_t desc,inodenb_t original_root_node,inodenb_t target_root_node){
   return -1;
}

/*-------------------------------------------
| Name:_kofs_readdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kofs_readdir(desc_t desc,dirent_t* dirent){

   inodenb_t _ino_logic= ofile_lst[desc].inodenb;
   kofs_ino_t _ino_phys= 0;

   _ino_phys = (kofs_ino_t)__cvt2physnode(desc,_ino_logic);

   if(_ino_phys==KOFS_INO_ROOT) {
      uchar8_t* p=(uchar8_t*)&kofs_root_node[0];
      kofs_node_t* p_kofs_node;

      if(ofile_lst[desc].offset>=kofs_root_node_sz)
         return -1;
      //
      p+=ofile_lst[desc].offset;
      p_kofs_node=(kofs_node_t*)p;
      //
      ofile_lst[desc].offset+= sizeof(kofs_node_t);
      //
      dirent->inodenb = __cvt2logicnode(desc,p_kofs_node->ino);
      strcpy(dirent->d_name,p_kofs_node->name);

   }else if(_ino_phys==KOFS_INO_PROC) {
      int pid=ofile_lst[desc].offset/sizeof(process_t)+1;

      if(pid>=PROCESS_MAX)
         return -1;
      //
      while(pid<=PROCESS_MAX) {
         //
         if(process_lst[pid]) {
            char* str;
            //
            _ino_phys=(pid+KOFS_INO_PROC_LST);
            //
            dirent->inodenb = __cvt2logicnode(desc,_ino_phys);
            strcpy(dirent->d_name,
                   ( (str=
                         strrchr(process_lst[pid]->argv[0],
                                 '/')) ? (str+1) : process_lst[pid]->argv[0] ) );
            //
            ofile_lst[desc].offset+=sizeof(process_t);
            pid=ofile_lst[desc].offset/sizeof(process_t)+1;
            return 0;
         }
         //
         ofile_lst[desc].offset+=sizeof(process_t);
         pid=ofile_lst[desc].offset/sizeof(process_t)+1;
      }

      return -1;
   }else if(_ino_phys>KOFS_INO_PROC_LST && _ino_phys<KOFS_INO_PROC_LST_MAX) {
      int pid = (_ino_phys - KOFS_INO_PROC_LST);
      int iteration = ofile_lst[desc].offset;

      //
      if(process_lst[pid]) {
         char buf[KOFS_MAX_FILENAME];
         kernel_pthread_t* pthread_ptr=process_lst[pid]->pthread_ptr;
         //
         while(pthread_ptr) {
            if(!iteration--)
               break;
            pthread_ptr=pthread_ptr->next;
         }

         //
         if(!pthread_ptr) {
            if(!iteration) {
               _ino_phys=(pid+KOFS_INO_PROC_OBJECTS);
               //
               dirent->inodenb = __cvt2logicnode(desc,_ino_phys);
               strcpy(dirent->d_name,"objects");
               //
               ofile_lst[desc].offset++;
               //
               return 0;
            }

            return -1;
         }
         //
         _ino_phys=(pthread_ptr->id+KOFS_INO_PROC_THREAD);
         //
         dirent->inodenb = __cvt2logicnode(desc,_ino_phys);
         strcpy(dirent->d_name,"pthread.");
         //ltoa(pthread_ptr->id,dirent->d_name,16);
         strcat(dirent->d_name,_kofs_ultoa(pthread_ptr->id,buf,16));
         //
         ofile_lst[desc].offset++;

      }

   }else if(_ino_phys>KOFS_INO_PROC_OBJECTS && _ino_phys<KOFS_INO_PROC_OBJECTS_MAX) {

      int pid = (_ino_phys - KOFS_INO_PROC_OBJECTS);
      int iteration = ofile_lst[desc].offset;

      //
      if(process_lst[pid]) {
         char buf[KOFS_MAX_FILENAME];
         kernel_object_t* kernel_object_ptr=process_lst[pid]->kernel_object_head;
         unsigned long kernel_object_addr;
         //
         while(kernel_object_ptr) {
            if(!iteration--)
               break;
            kernel_object_ptr=kernel_object_ptr->next;
         }

         //
         if(!kernel_object_ptr) {
            return -1;
         }
         //
         _ino_phys=(kernel_object_ptr->id+KOFS_INO_PROC_OBJECTS_OBJECT);
         //
         dirent->inodenb = __cvt2logicnode(desc,_ino_phys);
         //
         switch(kernel_object_ptr->type) {
         //
         case KERNEL_OBJECT_PTRHEAD_MUTEX:
            strcpy(dirent->d_name,"mutex.0x");
            break;
         //
         case KERNEL_OBJECT_SEM:
            strcpy(dirent->d_name,"sem.anonymous.0x");
            break;
         //
         case KERNEL_OBJECT_TIMER:
            strcpy(dirent->d_name,"timer.0x");
            break;
         //
         case KERNEL_OBJECT_PIPE:
            strcpy(dirent->d_name,"pipe.0x");
            break;

         default:
            strcpy(dirent->d_name,"unknow.0x");
            break;
         }

         kernel_object_addr=(unsigned long)kernel_object_ptr;

         //ltoa(pthread_ptr->id,dirent->d_name,16);
         strcat(dirent->d_name,_kofs_ultoa(kernel_object_addr,buf,16));
         //
         ofile_lst[desc].offset++;

      }

   }


   return 0;
}

/*-------------------------------------------
| Name:_kofs_telldir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kofs_telldir(desc_t desc){
   int loc=0;
   inodenb_t _ino_logic= ofile_lst[desc].inodenb;
   kofs_ino_t _ino_phys= 0;

   _ino_phys = (kofs_ino_t)__cvt2physnode(desc,_ino_logic);

   if(_ino_phys==KOFS_INO_ROOT) {
      loc = ofile_lst[desc].offset / sizeof(kofs_node_t);
   }

   return loc;
}

/*-------------------------------------------
| Name:_kofs_seekdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kofs_seekdir(desc_t desc,int loc){
   inodenb_t _ino_logic=ofile_lst[desc].inodenb;
   kofs_ino_t _ino_phys= 0;

   _ino_phys = (kofs_ino_t)__cvt2physnode(desc,_ino_logic);

   if(_ino_phys==KOFS_INO_ROOT) {
      int pos = loc * sizeof(kofs_node_t);
      ofile_lst[desc].offset+=pos;
   }

   return loc;
}

/*-------------------------------------------
| Name:_kofs_lookupdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
inodenb_t _kofs_lookupdir(desc_t desc,char* filename){
   inodenb_t _ino_logic= ofile_lst[desc].inodenb;
   kofs_ino_t _ino_phys= 0;

   _ino_phys = (kofs_ino_t)__cvt2physnode(desc,_ino_logic);

   if(   filename[0]=='.'
         && filename[1]=='\0') {

      return ofile_lst[desc].inodenb;

   }else if(   filename[0]=='.'
               && filename[1]=='.'
               && filename[2]=='\0') {

      return ofile_lst[desc].dir_inodenb;

   }else if(_ino_phys==KOFS_INO_ROOT) {
      uchar8_t* p=(uchar8_t*)&kofs_root_node[0];
      kofs_node_t* p_kofs_node;
      int offset=0;
      while(offset<kofs_root_node_sz) {
         //
         p_kofs_node=(kofs_node_t*)p;
         //
         if(!strcmp(filename,p_kofs_node->name)) {
            _ino_logic = __cvt2logicnode(desc,p_kofs_node->ino);
            return _ino_logic;
         }
         //
         offset+= sizeof(kofs_node_t);
         p+=offset;
      }

   }else if(_ino_phys==KOFS_INO_PROC) {
      int pid=ofile_lst[desc].offset/sizeof(process_t)+1;
      //
      while(pid<=PROCESS_MAX) {
         //
         if(process_lst[pid]) {
            char* str;
            _ino_phys=(pid+KOFS_INO_PROC_LST);
            //
            if(!strcmp(filename,
                       ( (str=
                             strrchr(process_lst[pid]->argv[0],
                                     '/')) ? (str+1) : process_lst[pid]->argv[0] ) ) ) {
               //if(!strcmp(filename,process_lst[pid]->argv[0])){
               _ino_logic = __cvt2logicnode(desc,_ino_phys);
               return _ino_logic;
            }
         }
         //
         ofile_lst[desc].offset+=sizeof(process_t);
         pid=ofile_lst[desc].offset/sizeof(process_t)+1;
      }
   }else if(_ino_phys>KOFS_INO_PROC_LST && _ino_phys<KOFS_INO_PROC_LST_MAX) {
      int pid = (_ino_phys - KOFS_INO_PROC_LST);
      //
      if(process_lst[pid]) {
         char buf[KOFS_MAX_FILENAME];
         char _buf[KOFS_MAX_FILENAME];
         kernel_pthread_t* pthread_ptr=process_lst[pid]->pthread_ptr;
         //
         while(pthread_ptr) {
            //
            _ino_phys=(pthread_ptr->id+KOFS_INO_PROC_THREAD);
            //
            _ino_logic = __cvt2logicnode(desc,_ino_phys);
            //
            strcpy(buf,"pthread.");
            strcat(buf,_kofs_ultoa(pthread_ptr->id,_buf,16));
            //
            if(!strcmp(buf,filename))
               return _ino_logic;
            //
            pthread_ptr=pthread_ptr->next;
         }

         //
         if(!pthread_ptr) {
            _ino_phys=(pid+KOFS_INO_PROC_OBJECTS);
            //
            _ino_logic = __cvt2logicnode(desc,_ino_phys);
            strcpy(buf,"objects");
            //
            if(!strcmp(buf,filename))
               return _ino_logic;
         }
         //
      }
   }else if(_ino_phys>KOFS_INO_PROC_OBJECTS && _ino_phys<KOFS_INO_PROC_OBJECTS_MAX) {

      int pid = (_ino_phys - KOFS_INO_PROC_OBJECTS);
      //
      if(process_lst[pid]) {
         char buf[KOFS_MAX_FILENAME];
         char _buf[KOFS_MAX_FILENAME];

         kernel_object_t* kernel_object_ptr=process_lst[pid]->kernel_object_head;
         unsigned long kernel_object_addr;
         //
         while(kernel_object_ptr) {

            //
            _ino_phys=(kernel_object_ptr->id+KOFS_INO_PROC_OBJECTS_OBJECT);
            //
            _ino_logic = __cvt2logicnode(desc,_ino_phys);
            //
            switch(kernel_object_ptr->type) {
            //
            case KERNEL_OBJECT_PTRHEAD_MUTEX:
               strcpy(buf,"mutex.0x");
               break;
            //
            case KERNEL_OBJECT_SEM:
               strcpy(buf,"sem.anonymous.0x");
               break;
            //
            case KERNEL_OBJECT_TIMER:
               strcpy(buf,"timer.0x");
               break;
            //
            case KERNEL_OBJECT_PIPE:
               strcpy(buf,"pipe.0x");
               break;

            default:
               strcpy(buf,"unknow.0x");
               break;
            }

            kernel_object_addr=(unsigned long)kernel_object_ptr;
            //ltoa(pthread_ptr->id,dirent->d_name,16);
            strcat(buf,_kofs_ultoa(kernel_object_addr,_buf,16));
            //
            if(!strcmp(buf,filename))
               return _ino_logic;
            //
            kernel_object_ptr=kernel_object_ptr->next;
         }
      }
   }



   return INVALID_INODE_NB;
}

/*-------------------------------------------
| Name:_kofs_mknod
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kofs_mknod(desc_t desc,inodenb_t inodenb,dev_t dev){
   return 0;
}

/*-------------------------------------------
| Name:_kofs_create
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
inodenb_t _kofs_create(desc_t desc,char* filename, int attr){

   return -1; /*_inode*/;
}

/*-------------------------------------------
| Name:_kofs_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kofs_open(desc_t desc){
   kofs_ino_t _ino_logic= (kofs_ino_t)ofile_lst[desc].inodenb;
   kofs_ino_t _ino_phys= 0;


   _ino_phys = (kofs_ino_t)__cvt2physnode(desc,_ino_logic);

   if(_ino_phys==KOFS_INO_ROOT) {
      ofile_lst[desc].offset  =0;
      ofile_lst[desc].attr    = S_IFDIR;
      ofile_lst[desc].size    = kofs_root_node_sz;
      ofile_lst[desc].cmtime  = 0;
   }else if(_ino_phys==KOFS_INO_PROC) {
      ofile_lst[desc].offset  =0;
      ofile_lst[desc].attr    = kofs_root_node[_ino_phys].attr;
      ofile_lst[desc].size    = PROCESS_MAX*sizeof(process_t);
      ofile_lst[desc].cmtime  = 0;
   }else if(_ino_phys==KOFS_INO_SEM) {
      ofile_lst[desc].offset  =0;
      ofile_lst[desc].attr    = kofs_root_node[_ino_phys].attr;
      ofile_lst[desc].size    = kofs_root_node[_ino_phys].size;
      ofile_lst[desc].cmtime  = 0;
   }else if(_ino_phys==KOFS_INO_MQUEUE) {
      ofile_lst[desc].offset  =0;
      ofile_lst[desc].attr    = kofs_root_node[_ino_phys].attr;
      ofile_lst[desc].size    = kofs_root_node[_ino_phys].size;
      ofile_lst[desc].cmtime  = 0;
   }else if( (_ino_phys>KOFS_INO_PROC_LST) && (_ino_phys<KOFS_INO_PROC_LST_MAX ) ) {
      int pid = (_ino_phys - KOFS_INO_PROC_LST);

      //
      ofile_lst[desc].offset  = 0;
      ofile_lst[desc].attr    = S_IFDIR;
      ofile_lst[desc].size    = 1; //1 for objects entries
      ofile_lst[desc].cmtime  = 0;

      //
      if(process_lst[pid]) {
         kernel_pthread_t* pthread_ptr=process_lst[pid]->pthread_ptr;
         while(pthread_ptr) {
            ofile_lst[desc].size++;
            pthread_ptr=pthread_ptr->next;
         }

         ofile_lst[desc].cmtime = process_lst[pid]->start_time;
      }
   }else if( (_ino_phys>KOFS_INO_PROC_THREAD) && (_ino_phys<KOFS_INO_PROC_THREAD_MAX ) ) {
      int pthread_id = (_ino_phys - KOFS_INO_PROC_THREAD);
      pid_t pid;
      kernel_pthread_t* pthread_ptr=g_pthread_lst;

      //
      ofile_lst[desc].offset  = 0;
      ofile_lst[desc].attr    = S_IFREG;
      ofile_lst[desc].size    = sizeof(kernel_pthread_t);
      ofile_lst[desc].cmtime  = 0;
      //
      while(pthread_ptr) {
         if(pthread_ptr->id==pthread_id)
            break;
         pthread_ptr=pthread_ptr->next;
      }
      //
      if(!pthread_ptr)
         return -1;

      if((pid=pthread_ptr->pid)>0)
         ofile_lst[desc].cmtime = process_lst[pid]->start_time;
   }else if( _ino_phys>KOFS_INO_PROC_OBJECTS && _ino_phys<KOFS_INO_PROC_OBJECTS_MAX) {
      pid_t pid = (_ino_phys - KOFS_INO_PROC_OBJECTS);
      kernel_object_t* kernel_object_ptr;
      //
      ofile_lst[desc].offset  = 0;
      ofile_lst[desc].attr    = S_IFDIR;
      ofile_lst[desc].size    = 0;
      ofile_lst[desc].cmtime  = 0;

      if(process_lst[pid]) {
         kernel_object_ptr=process_lst[pid]->kernel_object_head;
         //
         while(kernel_object_ptr) {
            ofile_lst[desc].size++;
            kernel_object_ptr=kernel_object_ptr->next;
         }
      }
   }else if( _ino_phys>KOFS_INO_PROC_OBJECTS_OBJECT && _ino_phys<
             KOFS_INO_PROC_OBJECTS_OBJECT_MAX) {
      kernel_object_id_t id= (_ino_phys - KOFS_INO_PROC_OBJECTS_OBJECT);
      //
      kofs_ino_t _dir_ino_phys = (kofs_ino_t)__cvt2physnode(desc,ofile_lst[desc].dir_inodenb);
      pid_t pid = (_dir_ino_phys - KOFS_INO_PROC_OBJECTS);
      //
      kernel_object_t* kernel_object_ptr=(kernel_object_t*)0;
      //
      ofile_lst[desc].offset  = 0;
      ofile_lst[desc].attr    = S_IFREG;
      ofile_lst[desc].size    = sizeof(kernel_object_t);
      ofile_lst[desc].cmtime  = 0;
      //
      if(process_lst[pid]) {
         kernel_object_ptr=process_lst[pid]->kernel_object_head;
         //
         while(kernel_object_ptr) {
            if(kernel_object_ptr->id==id)
               break;
            kernel_object_ptr=kernel_object_ptr->next;
         }
      }
      //
      if(kernel_object_ptr)
         return -1;
   }


   return 0;
}

/*-------------------------------------------
| Name:_kofs_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kofs_close(desc_t desc){
   return 0;
}


/*-------------------------------------------
| Name:_kofs_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kofs_write(desc_t desc, char* buf, int size){
   return -1;
}

/*-------------------------------------------
| Name:_kofs_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kofs_read(desc_t desc, char* buf, int size){
   return -1;
}

/*-------------------------------------------
| Name:_kofs_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kofs_seek(desc_t desc, int offset, int origin)
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
| Name:_kofs_truncate
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kofs_truncate(desc_t desc, off_t length){
   return 0;
}

/*-------------------------------------------
| Name:_kofs_remove
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kofs_remove(desc_t desc_ancst,desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:_kofs_rename
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kofs_rename(desc_t desc,const char*  old_name, char* new_name){
   return 0;
}

/*-------------------------------------------
| Name:_kofs_loadfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kofs_loadfs(void){
   return 0;
}

/*============================================
| End of Source  : kofs.c
==============================================*/
