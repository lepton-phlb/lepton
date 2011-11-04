/*
The contents of this file are subject to the Mozilla Public License Version 1.1 
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
specific language governing rights and limitations under the License.

The Original Code is ______________________________________.

The Initial Developer of the Original Code is ________________________.
Portions created by ______________________ are Copyright (C) ______ _______________________.
All Rights Reserved.

Contributor(s): ______________________________________.

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
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/process.h"
#include "kernel/core/stat.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/ioctl.h"
#include "kernel/core/cpu.h"

#include "lib/libc/termios/termios.h"
#include "kernel/fs/vfs/vfsdev.h"

#include "kernel/core/ioctl_hd.h"
#include "kernel/dev/arch/all/flash/flash.h"

#ifdef WIN32
#define FTL_LOG
#endif

/*============================================
| Global Declaration 
==============================================*/

const char dev_ftl_name[]="ftl\0";

int dev_ftl_load(void);
int dev_ftl_open(desc_t desc, int o_flag);
int dev_ftl_close(desc_t desc);
int dev_ftl_isset_read(desc_t desc);
int dev_ftl_isset_write(desc_t desc);
int dev_ftl_read(desc_t desc, char* buf,int size);
int dev_ftl_write(desc_t desc, const char* buf,int size);
int dev_ftl_seek(desc_t desc,int offset,int origin);
int dev_ftl_ioctl(desc_t desc,int request,va_list ap);

dev_map_t dev_ftl_map={
   dev_ftl_name,
   S_IFBLK,
   dev_ftl_load,
   dev_ftl_open,
   dev_ftl_close,
   dev_ftl_isset_read,
   dev_ftl_isset_write,
   dev_ftl_read,
   dev_ftl_write,
   dev_ftl_seek,
   dev_ftl_ioctl //ioctl
};

#define  FTL_INVALID_ADDR  0xffffffff
typedef  uint32_t addr_t;

typedef  uint32_t cluster_no_t;
typedef  uint16_t cluster_sz_t;

typedef  uchar8_t sector_data_t;
typedef  uint32_t sector_addr_t;
typedef  uint16_t sector_no_t;
typedef  uint32_t sector_sz_t;

#define TAG_SECTOR_FREE         1
#define TAG_SECTOR_USED         0

#define TAG_SECTOR_ERASED       1
#define TAG_SECTOR_TOBEERASED   0

#define TAG_SECTOR_MOVEDONE     1
#define TAG_SECTOR_MOVERUNNING  0


typedef  uint32_t stat_t;
typedef struct {
   stat_t free:1;   //1: free, 0:used
   stat_t erase:1;  //1: erased,  0:to be erased
   stat_t move:1;   //1: move ok, 0:move running

}bits_stat_t;


//
typedef struct physector_tag_st{
   uchar8_t         sector_tag;
   bits_stat_t      sector_stat;
   cluster_no_t     cluster_no;
}cluster_tag_t;

//
typedef struct cluster_t{
   sector_no_t      phy_sector_no;
   sector_addr_t    phy_sector_addr;
   sector_addr_t    phy_cluster_addr;
   sector_sz_t      phy_sector_sz;
}cluster_t;


typedef struct ftl_st{
   sector_no_t   sector_no_max;
   cluster_no_t  cluster_no_max;
   cluster_sz_t  cluster_sz;
   cluster_t*    p_cluster_lst;
}ftl_t;

unsigned char* g_p_vector_sector_free;
uint32_t g_p_vector_sector_free_sz;
unsigned char* g_p_vector_sector_tobe_erase;
uint32_t g_p_vector_sector_tobe_erase_sz;


#define __set_vector_sector(__sector_no__)   g_p_vector_sector_free[(__sector_no__)/8]|=(0x01<<((__sector_no__)%8))
#define __unset_vector_sector(__sector_no__) g_p_vector_sector_free[(__sector_no__)/8]&=(~(0x01<<((__sector_no__)%8)))
#define __is_free_vector_sector(__sector_no__)   ((g_p_vector_sector_free[(__sector_no__)/8]&(0x01<<((__sector_no__)%8)))?0:1)

#define __set_vector_tobe_erase_sector(__sector_no__)   g_p_vector_sector_tobe_erase[(__sector_no__)/8]|=(0x01<<((__sector_no__)%8))
#define __unset_vector_tobe_erase_sector(__sector_no__) g_p_vector_sector_tobe_erase[(__sector_no__)/8]&=(~(0x01<<((__sector_no__)%8)))
#define __is_vector_tobe_erase_sector(__sector_no__)   ((g_p_vector_sector_tobe_erase[(__sector_no__)/8]&(0x01<<((__sector_no__)%8)))?1:0)


int _ftl_move_sector(desc_t desc, cluster_no_t cluster_no,addr_t current_phy_addr,int size);

//to do: must be set automaticly with acces datat size for each device
#define DEV_SECTOR_SZ_MAX  64*1024
#define DEV_CLUSTER_SZ_MAX 8*1024

/*============================================
| Implementation 
==============================================*/


/*--------------------------------------------
| Name:        _ftl_ioctl_flash
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _ftl_ioctl_flash(desc_t desc,int request,...){
   va_list ap;
   int r=-1;

   va_start(ap, request);
   r= ofile_lst[desc].pfsop->fdev.fdev_ioctl(desc,request,ap);
   va_end(ap);

   return r;
}

/*--------------------------------------------
| Name:        _ftl_check_flash
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static int _ftl_check_flash(desc_t desc, desc_t desc_link){
   char buf[2]={0};
   if(ofile_lst[desc_link].pfsop->fdev.fdev_read(desc_link,buf,sizeof(buf))<0)
      return -1;
   if((buf[0]&0x0f)!=0x05)
      return -1;

   return 0;
}

/*--------------------------------------------
| Name:        _ftl_scan_flash
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static int _ftl_scan_flash(desc_t desc, desc_t desc_link){
   int i=0;
   int r=0;

   cluster_tag_t cluster_tag;

   addr_t sector_addr=0;
   addr_t sector_offs=0;
   unsigned long dev_sz;
   int alloc_cluster_lst_sz;


   sector_no_t sector_no =0;
   sector_sz_t sector_sz =0;

   cluster_no_t cluster_no =0;

   flash_sector_t* p_sector_map;
   ftl_t* p_ftl;

   if(!ofile_lst[desc_link].p)
      return -1;
      
   p_sector_map=(flash_sector_t*)((dev_flash_t*)ofile_lst[desc_link].p)->p_flash_type->p_sector_map;
   if(!p_sector_map)
      return -1;

   if((r=_ftl_ioctl_flash(desc_link,HDGETSZ,&dev_sz))<0)
      return -1;
   
   if( !(ofile_lst[desc].p = malloc(sizeof(ftl_t))) )
      return -1;

   //
   p_ftl = (ftl_t*)ofile_lst[desc].p;
   //
   p_ftl->cluster_sz = DEV_CLUSTER_SZ_MAX;
   p_ftl->cluster_no_max   = (dev_sz/p_ftl->cluster_sz);
   //
   alloc_cluster_lst_sz=(dev_sz/p_ftl->cluster_sz)*sizeof(cluster_t);
   p_ftl->p_cluster_lst    = (cluster_t*)malloc(alloc_cluster_lst_sz);
   if(!p_ftl->p_cluster_lst)
      return -1;
   //
   for(cluster_no=0;cluster_no<p_ftl->cluster_no_max;cluster_no++){
      p_ftl->p_cluster_lst[cluster_no].phy_cluster_addr = FTL_INVALID_ADDR;
      p_ftl->p_cluster_lst[cluster_no].phy_sector_addr  = FTL_INVALID_ADDR;
      p_ftl->p_cluster_lst[cluster_no].phy_sector_no    = -1;
      p_ftl->p_cluster_lst[cluster_no].phy_sector_sz    = 0;
   }
   
   //
   while(sector_addr<dev_sz){
      if(!sector_sz || !((sector_offs)%sector_sz)){
         if((sector_addr>0) && (sector_addr >= p_sector_map[i+1].addr))
            i++;
         if(p_sector_map[i].addr==0xFFFFFFFF)
            break;
         sector_sz = p_sector_map[i].size;
         sector_no++;
         sector_addr+=sector_sz;
         sector_offs=sector_sz;
         
      }
   }

   p_ftl->sector_no_max  = sector_no;

   //status vector
   g_p_vector_sector_free_sz= (sector_no/8)+1;
   g_p_vector_sector_free  = (unsigned char*)malloc(g_p_vector_sector_free_sz);
   if(!g_p_vector_sector_free)
      return -1;
   memset(g_p_vector_sector_free,0,g_p_vector_sector_free_sz);
   //
   g_p_vector_sector_tobe_erase_sz= (sector_no/8)+1;
   g_p_vector_sector_tobe_erase = (unsigned char*)malloc(g_p_vector_sector_tobe_erase_sz);
   if(!g_p_vector_sector_tobe_erase)
      return -1;
   memset(g_p_vector_sector_tobe_erase,0,g_p_vector_sector_tobe_erase_sz);

   //
   cluster_no  = 0;
   sector_addr = 0;
   sector_offs = 0;
   sector_no   = 0;
   sector_sz   = 0;

   i=0;

   //read data at physical addr
   while(sector_addr<dev_sz){
      if(!sector_sz || !((sector_offs)%sector_sz)){

         cluster_no_t sector_cluster_no_max =0;
         cluster_no_t sector_cluster_no =0;

         if( (sector_addr >= p_sector_map[i+1].addr))
            i++;
         if(p_sector_map[i].addr==0xFFFFFFFF)
            break;

         sector_sz = p_sector_map[i].size;

         sector_cluster_no_max = sector_sz/p_ftl->cluster_sz;

         //cluster table
         for(sector_cluster_no=0;sector_cluster_no<(sector_cluster_no_max);sector_cluster_no++){

            //set position
            if((r= ofile_lst[desc_link].pfsop->fdev.fdev_seek(desc_link,sector_addr+(sector_cluster_no*p_ftl->cluster_sz),SEEK_SET))<0)
               return -1;

            //read cluster tag
            if((r= ofile_lst[desc_link].pfsop->fdev.fdev_read(desc_link,(char*)&cluster_tag,sizeof(cluster_tag_t)))!=sizeof(cluster_tag_t))
               return -1;

            if((cluster_tag.sector_tag&0x0f)==0x05){
               cluster_no_t _cluster_no = cluster_tag.cluster_no;

               if(sector_cluster_no==0){
                  if(sector_cluster_no==0 && cluster_tag.sector_stat.erase==TAG_SECTOR_TOBEERASED){
                      _ftl_ioctl_flash(desc_link,HDCLRSCTR,sector_addr);
                      break;//go to next sector
                  }

                  if(sector_cluster_no==0 && cluster_tag.sector_stat.free==TAG_SECTOR_USED){
                     __set_vector_sector(sector_no);//set vector used
                     #ifdef FTL_LOG
                     printf("sector[%4d] used\r\n", sector_no);
                     #endif
                  }else{
                     #ifdef FTL_LOG
                     printf("sector[%4d] not used\r\n", sector_no);
                     #endif
                  }

                  //to do: si 2 secteurs pointent sur le même numéro de cluster
                  //verifier le tag .move et prendre systématiquement le secteur avec le tag TAG_SECTOR_MOVERUNNING.
                  if(p_ftl->p_cluster_lst[_cluster_no].phy_cluster_addr !=FTL_INVALID_ADDR){

                     //to do check constitnce of sector (all cluster muste be tagged with 0xf5)
                     //compare data in sector src and dest.
                     //error:already assgined.
                     cluster_tag_t prev_cluster_tag;
                     //read tag
                     if((r= ofile_lst[desc_link].pfsop->fdev.fdev_seek(desc_link,p_ftl->p_cluster_lst[_cluster_no].phy_sector_addr,SEEK_SET))<0)
                        return -1;
                     //read cluster tag
                     if((r= ofile_lst[desc_link].pfsop->fdev.fdev_read(desc_link,(char*)&prev_cluster_tag,sizeof(cluster_tag_t)))!=sizeof(cluster_tag_t))
                        return -1;

                     if(prev_cluster_tag.sector_stat.move==TAG_SECTOR_MOVERUNNING){
                        _ftl_move_sector(desc, _cluster_no,0,0);
                     }else if(cluster_tag.sector_stat.move==TAG_SECTOR_MOVERUNNING){
                        p_ftl->p_cluster_lst[_cluster_no].phy_cluster_addr = sector_addr+sector_cluster_no*p_ftl->cluster_sz;
                        p_ftl->p_cluster_lst[_cluster_no].phy_sector_addr  = sector_addr;
                        p_ftl->p_cluster_lst[_cluster_no].phy_sector_no    = sector_no;
                        p_ftl->p_cluster_lst[_cluster_no].phy_sector_sz    = sector_sz;
                        _ftl_move_sector(desc, _cluster_no,0,0);
                     }
                  }//already assigned
               }/*
               else{
                  //to do: 8/11/2007: bug must be fix: bug description: erase all sector after relink ftl on device
                  //error: non consistant sector;  erase this sector
                  _ftl_ioctl_flash(desc_link,HDCLRSCTR,sector_addr);
                  break;//go to next sector
               }*/
               
               p_ftl->p_cluster_lst[_cluster_no].phy_cluster_addr = sector_addr+sector_cluster_no*p_ftl->cluster_sz;
               p_ftl->p_cluster_lst[_cluster_no].phy_sector_addr  = sector_addr;
               p_ftl->p_cluster_lst[_cluster_no].phy_sector_no    = sector_no;
               p_ftl->p_cluster_lst[_cluster_no].phy_sector_sz    = sector_sz;
            }
         }
         
         sector_no++;
         sector_addr+=sector_sz;
         sector_offs=sector_sz;
         
      }
   }
  
   /*
   for(cluster_no=0;cluster_no<p_ftl->cluster_no_max;cluster_no++){
      printf("s[%4d]c[%4d]@=0x%08x %8x\r\n",
         p_ftl->p_cluster_lst[cluster_no].phy_sector_no,
         cluster_no,
         p_ftl->p_cluster_lst[cluster_no].phy_sector_addr,
         p_ftl->p_cluster_lst[cluster_no].phy_sector_sz);
   }
   */
      
   return 0;
}

/*--------------------------------------------
| Name:        _ftl_get_free_sector
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _ftl_get_free_sector(desc_t desc, cluster_no_t cluster_no){
   sector_no_t current_sector_no = -1;
   sector_no_t new_sector_no     = -1;
   sector_sz_t new_sector_sz     = 0;
   addr_t      new_sector_addr   = FTL_INVALID_ADDR;

   sector_no_t sector_no =0;
   addr_t sector_addr=0;
   ftl_t* p_ftl = (ftl_t*)ofile_lst[desc].p;
   cluster_sz_t  cluster_sz = p_ftl->cluster_sz;

   desc_t desc_link;

   flash_sector_t* p_sector_map;

   int i =0;

   //
   desc_link = ofile_lst[desc].desc_nxt[0];
   if(desc_link<0)
      return -1;

   //
   p_sector_map = (flash_sector_t*)((dev_flash_t*)ofile_lst[desc_link].p)->p_flash_type->p_sector_map;

   //
   new_sector_no = p_ftl->p_cluster_lst[cluster_no].phy_sector_no;
   current_sector_no = new_sector_no;
   //to do : fix limit when new_sector_no<0)
   //
   while((++new_sector_no)!=current_sector_no){
      if(new_sector_no >= (p_ftl->sector_no_max/2) )
         new_sector_no = 0;

      if(__is_free_vector_sector(new_sector_no)){
         cluster_no_t sector_cluster_no_max =0;
         cluster_no_t sector_cluster_no =0;
         cluster_no_t sector_cluster_no_limit=0;
         cluster_tag_t cluster_tag;
         int r=-1;
         //
         while(p_sector_map[i].addr!=0xFFFFFFFF){
            sector_sz_t sector_sz=p_sector_map[i].size;
            sector_no_t sector_no_limit_high = (sector_no_t)((p_sector_map[i+1].addr-p_sector_map[i].addr)/sector_sz)+sector_no;
      
            //
            if(new_sector_addr==FTL_INVALID_ADDR && new_sector_no<sector_no_limit_high ){
               new_sector_addr = (new_sector_no-sector_no)*sector_sz+sector_addr;
               new_sector_sz= sector_sz;
               break;
            }

            //
            i++;
            sector_no+=(sector_no_t)((p_sector_map[i].addr-p_sector_map[i-1].addr)/sector_sz);
            sector_addr+=p_sector_map[i].addr;
      
         }
        
         //
         sector_cluster_no_max = (DEV_SECTOR_SZ_MAX)/p_ftl->cluster_sz;
         cluster_no=(cluster_no*p_ftl->cluster_sz)/(DEV_SECTOR_SZ_MAX);
         sector_cluster_no = ((cluster_no*p_ftl->cluster_sz)%(DEV_SECTOR_SZ_MAX))/p_ftl->cluster_sz;
         sector_cluster_no_limit = sector_cluster_no;
         
         //
         do{
            if((sector_cluster_no*new_sector_sz)<(DEV_SECTOR_SZ_MAX)){
               //this sector is erased (security)
               if((r= ofile_lst[desc_link].pfsop->fdev.fdev_seek(desc_link,new_sector_addr+(sector_cluster_no*p_ftl->cluster_sz),SEEK_SET))<0)
                  return -1;
               if((r= ofile_lst[desc_link].pfsop->fdev.fdev_read(desc_link,(char*)&cluster_tag,sizeof(cluster_tag_t)))!=sizeof(cluster_tag_t))
                  return -1;

               if(cluster_tag.sector_stat.erase == TAG_SECTOR_TOBEERASED) //erase this sector
                  _ftl_ioctl_flash(desc_link,HDCLRSCTR,new_sector_addr+((new_sector_no+sector_cluster_no)*p_ftl->cluster_sz));
               __set_vector_sector(new_sector_no+sector_cluster_no);
            }

            cluster_tag.sector_tag=0xf5;
            cluster_tag.cluster_no = cluster_no+sector_cluster_no;
            
            cluster_tag.sector_stat.free = TAG_SECTOR_USED;
            
            p_ftl->p_cluster_lst[cluster_no+sector_cluster_no].phy_cluster_addr = new_sector_addr+sector_cluster_no*p_ftl->cluster_sz;
            p_ftl->p_cluster_lst[cluster_no+sector_cluster_no].phy_sector_addr  = new_sector_addr;
            p_ftl->p_cluster_lst[cluster_no+sector_cluster_no].phy_sector_no    = new_sector_no+(sector_no_t)sector_cluster_no;
            p_ftl->p_cluster_lst[cluster_no+sector_cluster_no].phy_sector_sz    = new_sector_sz;
           
            //set position
            if((r= ofile_lst[desc_link].pfsop->fdev.fdev_seek(desc_link,new_sector_addr+(sector_cluster_no*p_ftl->cluster_sz),SEEK_SET))<0)
               return -1;
            if((r= ofile_lst[desc_link].pfsop->fdev.fdev_write(desc_link,(char*)&cluster_tag,sizeof(cluster_tag_t)))!=sizeof(cluster_tag_t))
               return -1;

            #ifdef FTL_LOG
            printf("new sector:[%04d] cluster[%03d] addr=0x%08x\r\n",new_sector_no,cluster_no+sector_cluster_no,p_ftl->p_cluster_lst[cluster_no+sector_cluster_no].phy_cluster_addr);
            #endif
         }while( (sector_cluster_no = ((++sector_cluster_no)%sector_cluster_no_max))!=sector_cluster_no_limit );
            
         //
         return 0;
      }
   }
   return -1;
}

/*--------------------------------------------
| Name:        _ftl_move_sector
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _ftl_move_sector(desc_t desc, cluster_no_t cluster_no,addr_t current_phy_addr,int size){

   desc_t desc_link;

   sector_no_t src_sector_no = -1;
   sector_sz_t src_sector_sz  = 0;
   addr_t src_sector_addr  = FTL_INVALID_ADDR;
   addr_t src_cluster_addr = FTL_INVALID_ADDR;
   
   sector_no_t dest_sector_no =-1;
   sector_sz_t dest_sector_sz = 0;
   addr_t dest_sector_addr = FTL_INVALID_ADDR;
   
   int nb_cluster_tags;

   ftl_t* p_ftl = (ftl_t*)ofile_lst[desc].p;
   cluster_sz_t  cluster_sz = p_ftl->cluster_sz;

   //to do: buffer must be allocated in ftl for each link
   static char _buf[/*256*/1024]={0};

   int cb=0;
   int i =0;
   int sz=sizeof(_buf);
   
   //
   desc_link = ofile_lst[desc].desc_nxt[0];
   if(desc_link<0)
      return -1;

   //
   src_sector_addr  =   p_ftl->p_cluster_lst[cluster_no].phy_sector_addr;
   src_sector_no    =   p_ftl->p_cluster_lst[cluster_no].phy_sector_no;
   src_sector_sz    =   p_ftl->p_cluster_lst[cluster_no].phy_sector_sz;
   src_cluster_addr =   p_ftl->p_cluster_lst[cluster_no].phy_cluster_addr;

   //
   if(_ftl_get_free_sector(desc,cluster_no)<0)
      return -1;

   //
   dest_sector_addr =   p_ftl->p_cluster_lst[cluster_no].phy_sector_addr;
   dest_sector_no   =   p_ftl->p_cluster_lst[cluster_no].phy_sector_no;
   dest_sector_sz   =   p_ftl->p_cluster_lst[cluster_no].phy_sector_sz;


   //tag move running old src sector
   cb=0;
   while(cb<(DEV_SECTOR_SZ_MAX)){
      int r;
      cluster_tag_t cluster_tag;
      //
      if((r= ofile_lst[desc_link].pfsop->fdev.fdev_seek(desc_link,src_sector_addr+cb,SEEK_SET))<0)
         return -1;
      if((r= ofile_lst[desc_link].pfsop->fdev.fdev_read(desc_link,(char*)&cluster_tag,sizeof(cluster_tag_t)))!=sizeof(cluster_tag_t))
         return -1;
      //
      cluster_tag.sector_stat.move = TAG_SECTOR_MOVERUNNING; //sector move running

      if((r= ofile_lst[desc_link].pfsop->fdev.fdev_seek(desc_link,src_sector_addr+cb,SEEK_SET))<0)
         return -1;
      if((r= ofile_lst[desc_link].pfsop->fdev.fdev_write(desc_link,(char*)&cluster_tag,sizeof(cluster_tag_t)))!=sizeof(cluster_tag_t))
         return -1;
      //
      //_ftl_ioctl_flash(desc_link,HDCLRSCTR,src_sector_addr+cb);
      __unset_vector_sector(src_sector_no);
      src_sector_no++;
      cb+=src_sector_sz;
   }

   //warning integer operation N+: not make factorisation
   //nb_cluster_tags= ((current_phy_addr+size)/p_ftl->cluster_sz)-(current_phy_addr/p_ftl->cluster_sz);//
  
   nb_cluster_tags = (size/(p_ftl->cluster_sz-sizeof(cluster_tag_t)))+1;
   if((size+nb_cluster_tags*p_ftl->cluster_sz)>(DEV_SECTOR_SZ_MAX) )
      size=(DEV_SECTOR_SZ_MAX);//singular case not need copy?
   //copy
   cb = 0;
   //
   while(cb<(DEV_SECTOR_SZ_MAX)){
      int r,w;
      //begin cluster
      if(!((src_sector_addr+cb)%p_ftl->cluster_sz))
         cb+=sizeof(cluster_tag_t);//jump over cluster tag

      //
      if(size){//align on current_phy_addr
         if((current_phy_addr - (src_sector_addr+cb))<(p_ftl->cluster_sz-sizeof(cluster_tag_t)))
            if((sz = (current_phy_addr - (src_sector_addr+cb)))>sizeof(_buf))
               if(sz>0)
                  sz = sizeof(_buf);


         if(!sz){
            cb+=(size+(nb_cluster_tags-1)*sizeof(cluster_tag_t));
            size=0;
            continue;
         }
      }else{//align on cluster_sz
         if( (sz=(p_ftl->cluster_sz-(cb%p_ftl->cluster_sz)))> sizeof(_buf))
            sz = sizeof(_buf);
      }
      //
      if((ofile_lst[desc_link].pfsop->fdev.fdev_seek(desc_link,src_sector_addr+cb,SEEK_SET))<0)
         return -1;
      if((r=ofile_lst[desc_link].pfsop->fdev.fdev_read(desc_link,_buf,sz))<sz)
         return -1;

      //write dest
      if((ofile_lst[desc_link].pfsop->fdev.fdev_seek(desc_link,dest_sector_addr+cb,SEEK_SET))<0)
         return -1;
      if((w=ofile_lst[desc_link].pfsop->fdev.fdev_write(desc_link,_buf,r))<r)
         return -1;

      
      cb+=w;
   }

   // 
   cb=0;
   while(cb<(DEV_SECTOR_SZ_MAX)){
      int r;
      if(!cb){
         cluster_tag_t cluster_tag;
         //tag use new dest sector
         if((r= ofile_lst[desc_link].pfsop->fdev.fdev_seek(desc_link,dest_sector_addr+cb,SEEK_SET))<0)
            return -1;
         if((r= ofile_lst[desc_link].pfsop->fdev.fdev_read(desc_link,(char*)&cluster_tag,sizeof(cluster_tag_t)))!=sizeof(cluster_tag_t))
            return -1;
         //
         cluster_tag.sector_stat.free = TAG_SECTOR_USED; //sector to be erased

         if((r= ofile_lst[desc_link].pfsop->fdev.fdev_seek(desc_link,dest_sector_addr+cb,SEEK_SET))<0)
            return -1;
         if((r= ofile_lst[desc_link].pfsop->fdev.fdev_write(desc_link,(char*)&cluster_tag,sizeof(cluster_tag_t)))!=sizeof(cluster_tag_t))
            return -1;
         //tag to be erase old src sector
         if((r= ofile_lst[desc_link].pfsop->fdev.fdev_seek(desc_link,src_sector_addr+cb,SEEK_SET))<0)
            return -1;
         if((r= ofile_lst[desc_link].pfsop->fdev.fdev_read(desc_link,(char*)&cluster_tag,sizeof(cluster_tag_t)))!=sizeof(cluster_tag_t))
            return -1;
         //
         cluster_tag.sector_stat.erase = TAG_SECTOR_TOBEERASED; //sector to be erased

         if((r= ofile_lst[desc_link].pfsop->fdev.fdev_seek(desc_link,src_sector_addr+cb,SEEK_SET))<0)
            return -1;
         if((r= ofile_lst[desc_link].pfsop->fdev.fdev_write(desc_link,(char*)&cluster_tag,sizeof(cluster_tag_t)))!=sizeof(cluster_tag_t))
            return -1;

      }

      //to remove: only for test without garbage collector
      _ftl_ioctl_flash(desc_link,HDCLRSCTR,src_sector_addr+cb);

      //
      __unset_vector_sector(src_sector_no);
      src_sector_no++;
      cb+=src_sector_sz;
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_ftl_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_ftl_load(void){
      return 0;
}

/*-------------------------------------------
| Name:dev_ftl_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_ftl_open(desc_t desc, int o_flag){

   //
   if(o_flag & O_RDONLY){

   }

   if(o_flag & O_WRONLY){
   }

   ofile_lst[desc].offset=0;

   return 0;
}

/*-------------------------------------------
| Name:dev_ftl_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_ftl_close(desc_t desc){

   if(!ofile_lst[desc].nb_reader && !ofile_lst[desc].nb_writer){
      ofile_lst[desc].p=(void*)0;

   }

   return 0;
}

/*-------------------------------------------
| Name:dev_ftl_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_ftl_isset_read(desc_t desc){
  return -1;
}

/*-------------------------------------------
| Name:dev_ftl_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_ftl_isset_write(desc_t desc){
      return -1;
}
/*-------------------------------------------
| Name:dev_ftl_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_ftl_read(desc_t desc, char* buf,int size){
   addr_t cluster_offs=0;
   cluster_no_t cluster_no =0;
   cluster_no_t prev_cluster_no =0;

   ftl_t* p_ftl = (ftl_t*)ofile_lst[desc].p;
   cluster_sz_t  cluster_sz = p_ftl->cluster_sz;

   unsigned long phy_addr = 0;
   int cb=0;
   int sz =size;

   desc_t desc_link = ofile_lst[desc].desc_nxt[0];

   if(desc_link<0)
      return -1;

   while(cb<size){
      int r;
      cluster_no     =  ofile_lst[desc].offset/(cluster_sz-sizeof(cluster_tag_t));
      cluster_offs   =  (ofile_lst[desc].offset%(cluster_sz-sizeof(cluster_tag_t)));
      phy_addr       =  p_ftl->p_cluster_lst[cluster_no].phy_cluster_addr+sizeof(cluster_tag_t)+cluster_offs;

      if( (cluster_offs + sz) > (cluster_sz-sizeof(cluster_tag_t)) )
         sz = (cluster_sz-sizeof(cluster_tag_t)) - cluster_offs;

      if(p_ftl->p_cluster_lst[cluster_no].phy_cluster_addr!=FTL_INVALID_ADDR){
         if((ofile_lst[desc_link].pfsop->fdev.fdev_seek(desc_link,phy_addr,SEEK_SET))<0)
            return -1;

         r=ofile_lst[desc_link].pfsop->fdev.fdev_read(desc_link,buf+cb,sz);
      }else{
         memset(buf+cb,0xff,sz);
         r=sz;
      }

      if(r>0)
         ofile_lst[desc].offset+=r;
      else
         break;

      cb+=r;

      sz = size-cb;
   }

   return cb;
}

/*-------------------------------------------
| Name:dev_ftl_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_ftl_write(desc_t desc, const char* buf,int size){
   addr_t cluster_offs=0;
   cluster_no_t cluster_no =0;
   ftl_t* p_ftl = (ftl_t*)ofile_lst[desc].p;
   cluster_sz_t  cluster_sz = p_ftl->cluster_sz;

   unsigned long phy_addr = 0;
   int cb=0;
   int sz =size;

   //
   char _buf[64]={0};
   int _offset = ofile_lst[desc].offset;

   //
   desc_t desc_link = ofile_lst[desc].desc_nxt[0];
   if(desc_link<0)
      return -1;
   
   while(cb<size){
      int r;
      int w;
      int sz_buf=sizeof(_buf);
      cluster_no     =  _offset/(cluster_sz-sizeof(cluster_tag_t));
      cluster_offs   =  (_offset%(cluster_sz-sizeof(cluster_tag_t)));
      phy_addr       =  p_ftl->p_cluster_lst[cluster_no].phy_cluster_addr+sizeof(cluster_tag_t)+cluster_offs;

      
      if(sz>sizeof(_buf)){
         if(!(sz/sizeof(_buf)))
            sz_buf=sz%sizeof(_buf);
         else
            sz_buf=sizeof(_buf);
      }else{
         sz_buf = sz;
      }

      if( (cluster_offs + sz_buf) > (cluster_sz-sizeof(cluster_tag_t)) )
         sz_buf = (cluster_sz-sizeof(cluster_tag_t)) - cluster_offs;

      if(p_ftl->p_cluster_lst[cluster_no].phy_cluster_addr!=FTL_INVALID_ADDR){

         //
         if((ofile_lst[desc_link].pfsop->fdev.fdev_seek(desc_link,phy_addr,SEEK_SET))<0)
            return -1;
         r=ofile_lst[desc_link].pfsop->fdev.fdev_read(desc_link,_buf,sz_buf);
         //
         if(r<0)
            break;
         //
         while((--r)>=0)
            if((_buf[r]&buf[cb+r])!=buf[cb+r])
               break;

         if(r>=0){
            if(_ftl_move_sector(desc,cluster_no,phy_addr,(size-cb))<0)
               return -1;
            continue;
         }

      }else{
         //alloc and tag new sector
         _ftl_get_free_sector(desc, cluster_no);
         continue;
      }

      if((ofile_lst[desc_link].pfsop->fdev.fdev_seek(desc_link,phy_addr,SEEK_SET))<0)
         return -1;
      w=ofile_lst[desc_link].pfsop->fdev.fdev_write(desc_link,buf+cb,sz_buf);

      #ifdef FTL_LOG
      printf("sz=%d phy_addr=0x%08x c[%6d] o[%6d] error=%d\r\n",sz_buf,phy_addr,cluster_no,cluster_offs,w);
      #endif

      if(w>0)
         _offset+=w;
      else
         break;

      //
      cb+=w;
      sz = size-cb;
   }

   ofile_lst[desc].offset=_offset;

   
   return cb;
}

/*-------------------------------------------
| Name:dev_ftl_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_ftl_seek(desc_t desc,int offset,int origin){
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
}

/*-------------------------------------------
| Name:dev_ftl_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_ftl_ioctl(desc_t desc,int request,va_list ap){
   switch(request){

      case I_LINK:{
         //warning must be open in O_RDONLY mode or O_RDWR
         desc_t desc_link = ofile_lst[desc].desc_nxt[0];
         if(desc_link<0)
            return -1;
         
         if(_ftl_scan_flash(desc,desc_link)<0)
            return -1;
      }
      break;

      case I_UNLINK:{
      }
      break;

      //
      default:
         return -1;


   }
   return 0;
}

/*============================================
| End of Source  : dev_ftl.c
==============================================*/
