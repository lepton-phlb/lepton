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
#include <stdlib.h>
#include <string.h>

#include "kernel/core/libstd.h"
#include "kernel/core/stat.h"
#include "kernel/core/statvfs.h"
#include "kernel/core/devio.h"
#include "kernel/core/time.h"
#include "kernel/core/ioctl_hd.h"

#include "kernel/fs/vfs/vfstypes.h"

/*===========================================
Global Declaration
=============================================*/
static const char* fstype_list[]={
#if __KERNEL_VFS_SUPPORT_ROOTFS==1 
   "rootfs",
#endif
#if __KERNEL_VFS_SUPPORT_UFS==1 
   "ufs",
#endif
#if __KERNEL_VFS_SUPPORT_UFSX==1 
   "ufsx",
#endif
#if __KERNEL_VFS_SUPPORT_KOFS==1
   "kofs",
#endif
#if __KERNEL_VFS_SUPPORT_MSDOS==1
   "msdos",
#endif
#if __KERNEL_VFS_SUPPORT_VFAT==1
   "vfat",
#endif
#if __KERNEL_VFS_SUPPORT_EFFS==1
   "effs"
#endif
};

static const int fstype_list_size=sizeof(fstype_list)/sizeof(char*);

#define OPT_MSK_T 0x01   //-t fs type
#define OPT_MSK_B 0x02   //-b block number
#define OPT_MSK_N 0x04   //-n node number
#define OPT_MSK_D 0x08   //-d dev path
#define OPT_MSK_S 0x10   //-s block size

#define OPT_MSK_ALL (OPT_MSK_T|OPT_MSK_B|OPT_MSK_N|OPT_MSK_D)

//ugly code to preserve compatibility with previous version of lepton
#define DFLT_DEV_SZ ((long)(32L*1024L)) //32KBytes

/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:mkfs_main
| Description:
| Parameters:
| Return Type:
| Comments:[-t fstype] [-b max block] [-n max node] [-d dev]
| See:
---------------------------------------------*/
int mkfs_main(int argc,char* argv[]){

   int i;
   int fd;
   unsigned int opt=0;
   fstype_t fstype;
   char* dev_path;
   struct vfs_formatopt_t vfs_formatopt;

   //if(argc<5) //not enough parameter
   //return -1;

   //ugly code to preserve compatibility with previous version of lepton
   vfs_formatopt.dev_sz=DFLT_DEV_SZ;

   for(i=1; i<argc; i++) {
      if(argv[i][0]=='-') {
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1; c<l; c++) {
            switch(argv[i][c]) {

            //
            case 't': {
               opt |= OPT_MSK_T;
               if((i+1) == argc)   //not enough parameter
                  return -1;
               i++;
               if(!argv[i])
                  return -1;

               for(fstype=0;
                   fstype<fstype_list_size;
                   fstype++) {

                  if(!strcmp(argv[i],fstype_list[fstype])){
                     if(!strcmp(argv[i],"effs")){
                        opt |= (OPT_MSK_B|OPT_MSK_N|OPT_MSK_S|OPT_MSK_D);
                     }
                     break;
                  }
               }

               if(fstype==fstype_list_size)
                  return -1;
            }
            break;

            //
            case 'b': {
               opt |= OPT_MSK_B;
               if((i+1) == argc)   //not enough parameter
                  return -1;
               i++;
               if(!argv[i])
                  return -1;

               vfs_formatopt.max_blk = atoi(argv[i]);
            }
            break;

            //
            case 'n': {
               opt |= OPT_MSK_N;
               if((i+1) == argc)   //not enough parameter
                  return -1;
               i++;
               if(!argv[i])
                  return -1;

               vfs_formatopt.max_node = atoi(argv[i]);
            }
            break;

            //
            case 'd': {
               opt |= OPT_MSK_D;
               if((i+1) == argc)   //not enough parameter
                  return -1;
               i++;
               if(!argv[i])
                  return -1;

               dev_path = argv[i];
            }
            break;

            //
            case 's': {
               opt |= OPT_MSK_S;
               if((i+1) == argc)   //not enough parameter
                  return -1;
               i++;
               if(!argv[i])
                  return -1;

               vfs_formatopt.blk_sz = atoi(argv[i]);
            }
            break;

            } //switch
         } //for
      }else{

         if(!argv[i])
            return -1;
      }

   } //for



   if( (opt&OPT_MSK_ALL)!=OPT_MSK_ALL )
      return -1;  //missing parameter

   if((fd = open(dev_path,O_RDONLY,0))<0)
      return -1;

   if(ioctl(fd,HDGETSZ,&vfs_formatopt.dev_sz)<0)
      vfs_formatopt.dev_sz=DFLT_DEV_SZ;

   close(fd);


   mkfs(fstype,dev_path,&vfs_formatopt);

   return 0;
}


/*===========================================
End of Source mkfs.c
=============================================*/
