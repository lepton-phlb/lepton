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
#include "kernel/core/system.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"
#include "kernel/core/ioctl.h"

#include "kernel/fs/vfs/vfsdev.h"

#include "dev_mem.h"


/*============================================
| Global Declaration 
==============================================*/
const char dev_mem_name[]="mem\0";

int dev_mem_load(void);
int dev_mem_open(desc_t desc, int o_flag);
int dev_mem_close(desc_t desc);
int dev_mem_isset_read(desc_t desc);
int dev_mem_isset_write(desc_t desc);
int dev_mem_read(desc_t desc, char* buf,int size);
int dev_mem_write(desc_t desc, const char* buf,int size);
int dev_mem_seek(desc_t desc,int offset,int origin);
int dev_mem_ioctl(desc_t desc,int request,va_list ap);

dev_map_t dev_mem_map={
   dev_mem_name,
   S_IFBLK,
   dev_mem_load,
   dev_mem_open,
   dev_mem_close,
   dev_mem_isset_read,
   dev_mem_isset_write,
   dev_mem_read,
   dev_mem_write,
   dev_mem_seek,
   dev_mem_ioctl //ioctl
};

//
typedef unsigned long vector_listener_t;
typedef struct {
      int addr;
      int len;
      void* pdata;

      //liste des descripteurs desc_t qui sont abonnés a cette zone de données
      vector_listener_t vector_listener;

      //liste des indications "0 lu / 1 non lu" pour les descripteurs desc_t qui sont abonnés a cette zone de données 
      vector_listener_t vector_isset_read;
}addr_t;

static addr_t addr_list[MAX_MEM_ADDR];

//
static kernel_pthread_mutex_t _dev_mem_mutex;

#define __dev_mem_lock()      kernel_pthread_mutex_lock(&_dev_mem_mutex)
#define __dev_mem_unlock()    kernel_pthread_mutex_unlock(&_dev_mem_mutex)


/*============================================
| Implementation 
==============================================*/
/*--------------------------------------------
| Name:        dev_mem_add_addr
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_mem_add_addr(addr_t addr){
   int i;
   for(i=0;i<MAX_MEM_ADDR;i++){
      if((addr_list[i].addr>=0) && (addr_list[i].addr!=addr.addr) )continue;
      
      if(addr_list[i].addr<0){
         addr_list[i]=addr;
         addr_list[i].vector_listener     = 0x00000000;
         addr_list[i].vector_isset_read   = 0x00000000;
         addr_list[i].len = addr .len;
      }

      
      if(addr.pdata){
         addr_list[i].len   = addr .len;
         addr_list[i].pdata = addr .pdata;
      }else{
         addr.pdata=(void*)0;
      }

      return 0;
   }
   return -1;
}

/*--------------------------------------------
| Name:        dev_mem_del_addr
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_mem_del_addr(addr_t addr){
   int i;
   for(i=0;i<MAX_MEM_ADDR;i++){
      if(addr_list[i].addr!=addr.addr)continue;
      addr_list[i].addr= -1;
      return 0;
   }
   return -1;
}

/*--------------------------------------------
| Name:        dev_mem_register_listener
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_mem_register_listener(desc_t desc, int addr){
   int i;
   vector_listener_t vector_msk=0x00000001<<desc;
   //lookup addr
   for(i=0;i<MAX_MEM_ADDR;i++){
      if(addr_list[i].addr!=addr)continue;
      addr_list[i].vector_listener|=vector_msk;
      return 0;
   }
   return -1;
}

/*--------------------------------------------
| Name:        dev_mem_unregister_listener
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_mem_unregister_listener(desc_t desc, int addr){
   int i;
   vector_listener_t vector_msk=0x00000001<<desc;
   //lookup addr
   for(i=0;i<MAX_MEM_ADDR;i++){
      if(addr_list[i].addr!=addr)continue;
      addr_list[i].vector_listener&=~(vector_msk);
      addr_list[i].vector_isset_read&=~(vector_msk);
      return 0;
   }
   return -1;
}

/*--------------------------------------------
| Name:        dev_mem_load
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_mem_load(void){
   int i;
   pthread_mutexattr_t  mutex_attr=0;

   kernel_pthread_mutex_init(&_dev_mem_mutex,&mutex_attr);

   //
   for(i=0;i<MAX_MEM_ADDR;i++){
      addr_list[i].addr=-1;
      addr_list[i].pdata=(void*)0;
      addr_list[i].vector_listener     = 0x00000000;
      addr_list[i].vector_isset_read   = 0x00000000;
   }
   
   return 0;
}

/*--------------------------------------------
| Name:        dev_mem_open
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_mem_open(desc_t desc, int o_flag){
   return 0;
}

/*--------------------------------------------
| Name:        dev_mem_close
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_mem_close(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_proc_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_mem_isset_read(desc_t desc){
   int i;
   for(i=0;i<MAX_MEM_ADDR;i++){
      if(   ((addr_list[i].vector_listener>>desc)&0x00000001)
         && (addr_list[i].vector_isset_read&(0x00000001<<desc)) )
         return 0;
   }
   return -1;
}

/*-------------------------------------------
| Name:dev_proc_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_mem_isset_write(desc_t desc){
      return 0;//the write operation is synchrone
}

/*--------------------------------------------
| Name:        dev_mem_read
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_mem_read(desc_t desc,char* buf, int size){
   int i=0; 
   int j=-1;
   int data_offset;

   int ret=-1;

   kernel_pthread_t* pthread_ptr;

   if(!(pthread_ptr = kernel_pthread_self()))
      return -1;

   for(i=0;i<=MAX_MEM_ADDR;i++){
      //nothing change for the current offset?
      if(i==MAX_MEM_ADDR){
         // not register listener make simple copy of current address
         if(j>=0){
            i=j;
            goto label_dev_mem_read_copy;
         }  

         //nothing change at current offset
         if((ofile_lst[desc].oflag&O_NONBLOCK) || !ret)
            return ret;//if !ret change detected on this descripteur but not at the current offset

         //no change detected for on any address for this desc
         __wait_io_int(pthread_ptr);//blocking call
         //restart find change
         i=0;
      }
      
      //get address index for current offset
      if(   (ofile_lst[desc].offset>=addr_list[i].addr)
         && (ofile_lst[desc].offset < (addr_list[i].addr+addr_list[i].len)) ) 
         j=i;

      //
      if(addr_list[i].vector_listener&(0x00000001<<desc))//is it register listener
         j=-1;//yes

      //change?
      if( (addr_list[i].vector_isset_read&(0x00000001<<desc)) ){
         ret=0;////something change yes
         //lseek() change at the current offset
         if(   (ofile_lst[desc].offset<addr_list[i].addr)
            || (ofile_lst[desc].offset >= (addr_list[i].addr+addr_list[i].len)) ) 
            continue;//continue to try find change at the current offset
         
         //something change at the current offset go to read the address and copy in the buf
         
      }else{
         //continue to find change
         continue;
      }
      
      //yes, now read the address and copy in the buf
      //pthread_clear_event(&ofile_lst[desc].pthread_owner);
label_dev_mem_read_copy:
      //lock
      __dev_mem_lock();
      //check real address
      if(!addr_list[i].pdata){
         __dev_mem_unlock();
         return -1;
      }

      //
      if(buf)
         addr_list[i].vector_isset_read&=~(0x00000001<<desc);

      data_offset= ofile_lst[desc].offset-addr_list[i].addr;
      if(size>(addr_list[i].len-data_offset))
         size = addr_list[i].len-data_offset;

      //WARNING TO DO
      //check new size<old size. risk of buffer overflow

      if(buf)
         memcpy(buf,(char*)addr_list[i].pdata+data_offset,size);
      else
         size=-size;

      ofile_lst[desc].offset+=size;

      //unlock
      __dev_mem_unlock();

      return size;
   }
   return ret;
}

/*--------------------------------------------
| Name:        dev_mem_write
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_mem_write(desc_t desc,const char* buf, int size){
   int i; 
   int data_offset;

   for(i=0;i<MAX_MEM_ADDR;i++){
      char shift;
      if(   (ofile_lst[desc].offset<addr_list[i].addr)
         || (ofile_lst[desc].offset >= (addr_list[i].addr+addr_list[i].len)) )continue;
      
      //to do lock
      __dev_mem_lock();

      //check real address
      if(!addr_list[i].pdata){
         __dev_mem_unlock();
         return -1;
      }

      
      data_offset= ofile_lst[desc].offset-addr_list[i].addr;
      if(size>(addr_list[i].len-data_offset))
         size = addr_list[i].len-data_offset;

      //put option filter with ioctl()
      /*
      if(!memcmp((char*)addr_list[i].pdata+data_offset,buf,size))
         return 0; //identical no modification
      */

      memcpy((char*)addr_list[i].pdata+data_offset,buf,size);

      ofile_lst[desc].offset+=size;

      for(shift=0;shift<(sizeof(vector_listener_t)*8);shift++)
         if(((addr_list[i].vector_listener>>shift)&0x00000001)){
            
            addr_list[i].vector_isset_read|=(0x00000001<<shift);
            //if(!(ofile_lst[shift].oflag&O_NONBLOCK))
            __fire_io_int(ofile_lst[shift].owner_pthread_ptr_read);
               //pthread_fire_event(&ofile_lst[shift].pthread_owner);
         }

      //to do unlock
      __dev_mem_unlock();
      return size;
   }
   return -1;
}

/*--------------------------------------------
| Name:        dev_mem_seek
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_mem_seek(desc_t desc,int offset, int origin){

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
      break;

      default:
      return -1;
   }

   return ofile_lst[desc].offset;

   return 0;
}

/*--------------------------------------------
| Name:        dev_mem_ioctl
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_mem_ioctl(desc_t desc,int request,va_list ap){
   switch(request){
      //
      case MEMADD:{
         addr_t _addr;
         int addr = va_arg( ap, int);
         int len  = va_arg( ap, int);
         void* pdata= va_arg( ap, void*);
         int r;

        //
         _addr.pdata = pdata;
         _addr.addr  = addr;
         _addr.len   = len;

         __dev_mem_lock();
         r = dev_mem_add_addr(_addr);
         __dev_mem_unlock();
         return r;
      }
      break;

      //
      case MEMREG:{
         int addr= va_arg( ap, int);
         __dev_mem_lock();
         dev_mem_register_listener(desc,addr);
         __dev_mem_unlock();
      }
      break;

      //
      case MEMUNREG:{
        int addr= va_arg( ap, int);
        __dev_mem_lock();
        dev_mem_unregister_listener(desc,addr);
        __dev_mem_unlock();
      }
      break;

      case MEMFLUSH:{
         int i=0;
         __dev_mem_lock();
         //find event for this listener desc on all possible mem address
         for(i=0;i<MAX_MEM_ADDR;i++){
            if(   ((addr_list[i].vector_listener>>desc)&0x00000001)
               && (addr_list[i].vector_isset_read&(0x00000001<<desc)) )
                  addr_list[i].vector_isset_read&=~(0x00000001<<desc);//unset bit event
         }

        __dev_mem_unlock();
      }
      break;


      case FIONBIO:{
         int *on = va_arg( ap, int*);
         if(*on)
            ofile_lst[desc].oflag|=(O_NONBLOCK);
         else
            ofile_lst[desc].oflag&=~(O_NONBLOCK);
      }
      break;

      //
      default:
         return -1;

   }

   return 0;
}

/*============================================
| End of Source  : dev_mem.c
==============================================*/
