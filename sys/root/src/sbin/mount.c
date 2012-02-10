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
#include <stdlib.h>
#include <string.h>

#include "kernel/core/libstd.h"
#include "kernel/core/stat.h"
#include "kernel/core/statvfs.h"
#include "kernel/core/devio.h"
#include "kernel/core/time.h"
#include "kernel/core/ioctl.h"
#include "kernel/core/stropts.h"

#include "lib/libc/stdio/stdio.h"

/*===========================================
Global Declaration
=============================================*/
static const char* fstype_list[]={"rootfs","ufs","ufsx","","msdos","vfat"};
static const int fstype_list_size=sizeof(fstype_list)/sizeof(char*);

/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:mount_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int mount_main(int argc,char* argv[]){
   //mount [-t fstype] [dev] [mountdir]

   int i;
   int error;
   int fstype = -1;
   char* token;

   if(argc<2)
      return -1;

   token = strtok(argv[1],"-");
   if(token[0]!='t') {
      int fd[2];
      //stream mount operation
      struct stat _stat;

      //
      if(!argv[1])
         return -1;
      if(!argv[2])
         return -1;
      if(!argv[3])
         return -1;

      //
      stat(argv[1],&_stat);
      if(!S_ISBLK(_stat.st_mode) && !S_ISCHR(_stat.st_mode)) {
         printf("error: %s is not device\r\n",argv[1]);
         return -1;
      }
      //
      stat(argv[2],&_stat);
      if(!S_ISBLK(_stat.st_mode) && !S_ISCHR(_stat.st_mode)) {
         printf("error: %s is not device\r\n",argv[2]);
         return -1;
      }
      //
      if((fd[0]=open(argv[1],O_RDWR,0))<0) {
         printf("error: cannot open device %s\r\n",argv[1]);
         return -1;
      }
      //
      if((fd[1]=open(argv[2],O_RDWR,0))<0) {
         printf("error: cannot open device %s\r\n",argv[2]);
         return -1;
      }
      //
      if(ioctl(fd[0],I_LINK,fd[1])<0) {
         printf("error: cannot link %s with %s\r\n",argv[1],argv[2]);
         return -1;
      }
      //
      if(fattach(fd[0],argv[3])<0) {
         printf("error: cannot attach device %s\r\n",argv[3]);
         return -1;
      }
      return 0;
   }

   //"traditional" mount operation
   for(i=0;
       i<fstype_list_size;
       i++) {

      if(!strcmp(argv[2],fstype_list[i])) {
         fstype = i;
         break;
      }
   }

   if(fstype<0)
      return -1;
   if(!argv[3])
      return -1;
   if(!argv[4])
      return -1;

   error = mount(fstype,argv[3],argv[4]);
   if(error<0)
      printf("error:mount %s on %s failed!\r\n",argv[3],argv[4]);

   return error;
}

/*===========================================
End of Sourcemount.c
=============================================*/
