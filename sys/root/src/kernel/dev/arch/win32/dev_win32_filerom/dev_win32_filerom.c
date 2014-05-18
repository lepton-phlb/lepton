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
//specific win32 include
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

//lepton include
#include "kernel/core/system.h"
#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/fs/vfs/vfstypes.h"



/*===========================================
Global Declaration
=============================================*/
const char dev_win32_filerom_name[]="hdb\0";

int dev_win32_filerom_load(void);
int dev_win32_filerom_open(desc_t desc, int o_flag);
int dev_win32_filerom_close(desc_t desc);
int dev_win32_filerom_isset_read(desc_t desc);
int dev_win32_filerom_isset_write(desc_t desc);
int dev_win32_filerom_read(desc_t desc, char* buf,int size);
int dev_win32_filerom_write(desc_t desc, const char* buf,int size);
int dev_win32_filerom_seek(desc_t desc,int offset,int origin);

dev_map_t dev_win32_filerom_map={
   dev_win32_filerom_name,
   S_IFBLK,
   dev_win32_filerom_load,
   dev_win32_filerom_open,
   dev_win32_filerom_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_win32_filerom_read,
   dev_win32_filerom_write,
   dev_win32_filerom_seek,
   __fdev_not_implemented //ioctl
};


//
#define FILEROM_MEMORYSIZE 32*1024 //11*1024 //14Ko//(32KB)
static char memory[FILEROM_MEMORYSIZE]={0};

static int fh=-1;

static int instance_counter=0;


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:dev_win32_filerom_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_filerom_load(void){
   return 0;
}

/*-------------------------------------------
| Name:dev_win32_filerom_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_filerom_open(desc_t desc, int o_flag){


   if(fh==-1) {

      if( (fh =
              _open( ".\\fsrom.o",_O_RDWR|_O_CREAT|_O_EXCL|_O_BINARY,_S_IREAD|_S_IWRITE)) == -1 ) {
         DWORD dwError=GetLastError();

         if(dwError!=ERROR_FILE_EXISTS)
            return -1;

         if( (fh = _open( ".\\fsrom.o",_O_RDWR |_O_BINARY,_S_IREAD|_S_IWRITE )) == -1 )
            return -1;

         _lseek( fh, 0, SEEK_SET );

      }else{
         int w=0;

         close(fh);
         if( (fh =
                 _open( ".\\fsrom.o",_O_RDWR|_O_TRUNC|_O_EXCL|_O_BINARY,_S_IREAD|_S_IWRITE)) == -1 )
            return -1;


         if(( w = _write(fh,memory,sizeof( memory ))) == -1 )
            printf( "fsrom creation failed" );
         else
            printf( "fsrom creation size %u bytes ok.\n", w );

         _lseek(fh,0,SEEK_SET );
      }
   }

   //
   if(o_flag & O_RDONLY) {
   }

   if(o_flag & O_WRONLY) {
   }

   instance_counter++;

   return 0;
}

/*-------------------------------------------
| Name:dev_win32_filerom_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_filerom_close(desc_t desc){

   if(fh==-1)
      return -1;

   instance_counter--;

   if(instance_counter<0) {
      instance_counter=0;
      _close(fh);
      fh = -1;
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_win32_filerom_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_filerom_isset_read(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_win32_filerom_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_filerom_isset_write(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_win32_filerom_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_filerom_read(desc_t desc, char* buf,int size){
   int r;
   if(ofile_lst[desc].offset>=FILEROM_MEMORYSIZE)
      return -1;
   _lseek( fh, ofile_lst[desc].offset, SEEK_SET);
   r= _read( fh,buf,size);
   ofile_lst[desc].offset = _lseek( fh, 0, SEEK_CUR);
   //to remove : test
   //printf("<- read offset=%d\n",ofile_lst[desc].offset);
   return r;
}

/*-------------------------------------------
| Name:dev_win32_filerom_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_filerom_write(desc_t desc, const char* buf,int size){
   int w;
   if(ofile_lst[desc].offset>=FILEROM_MEMORYSIZE)
      return -1;
   _lseek( fh, ofile_lst[desc].offset, SEEK_SET);
   w = _write( fh,buf,size);
   //_commit(fh);
   ofile_lst[desc].offset = _lseek( fh, 0, SEEK_CUR);
   //to remove : test
   //printf("-> write offset=%d\n",ofile_lst[desc].offset);
   return w;
}

/*-------------------------------------------
| Name:dev_win32_filerom_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_filerom_seek(desc_t desc,int offset,int origin){
   if(ofile_lst[desc].offset>=FILEROM_MEMORYSIZE)
      return -1;
   ofile_lst[desc].offset = _lseek( fh, offset, origin);

   return ofile_lst[desc].offset;
}


/*===========================================
End of Sourcedrv_filerom.c
=============================================*/
