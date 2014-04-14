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


/**
 * \addtogroup lepton_kernel
 * @{
 *
 */

/**
 * \defgroup IPC les IPC
 * @{
 *     Le communication inter-processus permettent  deux processus de communiquer par l'intermdiaire de mcanismes fournis par le systme d'exploitation.
 *     Ces mcanismes sont les suivants : les tubes et les tubes nomms. Les tubes nomms sont juste une extension des tubes.\n
 *     \n
 *     Les tubes (pipe.c et pipe.h) :
 *     Les tubes sont des FIFO, donc unidirectionnelles.
 *     Pour ouvrir un tube il faut utilis la fonction pipe(). Cette fonction retourne deux descripteurs de fichiers.
 *     Le premier descripteurs est le celui de lecture, le second celui d'criture.
 *     Pour effectuer les oprations de lecture et d'criture, il suffit d'utiliser les fonctions read() et write() standards.\n
 *     \n
 *
 */


/*===========================================
Includes
=============================================*/
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/syscall.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"
#include "kernel/core/pipe.h"
#include "kernel/fs/vfs/vfs.h"




/*===========================================
Global Declaration
=============================================*/

opipe_t opipe_lst[__MAX_PIPE];

int _sys_pipe_load(void);
int _sys_pipe_open(desc_t desc,int o_flag);
int _sys_pipe_close(desc_t desc);
int _sys_pipe_isset_read(desc_t desc);
int _sys_pipe_isset_write(desc_t desc);
int _sys_pipe_read(desc_t desc,char* buffer,int nbyte );
int _sys_pipe_write(desc_t desc,const char* buffer,int nbyte );
int _sys_pipe_seek(desc_t desc,int offset,int origin);


const char _sys_pipe_name[]="fifo";

dev_map_t dev_pipe_map={
   _sys_pipe_name,
   S_IFCHR,
   _sys_pipe_load,
   _sys_pipe_open,
   _sys_pipe_close,
   _sys_pipe_isset_read,
   _sys_pipe_isset_write,
   _sys_pipe_read,
   _sys_pipe_write,
   _sys_pipe_seek
};


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:_sys_pipe_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
/*! \fn int _sys_pipe_open(desc_t desc,int o_flag)
    \brief chargement du drivers suyte
    \return -1 si erreur sinon 0.
*/
int _sys_pipe_load(void){
   pipe_desc_t pipe_desc;

   for(pipe_desc=0; pipe_desc<__MAX_PIPE; pipe_desc++) {
      opipe_lst[pipe_desc].pipe_desc = -1;
      opipe_lst[pipe_desc].desc_r = -1;
      opipe_lst[pipe_desc].desc_w = -1;
   }

   return 0;
}

/*-------------------------------------------
| Name:_sys_pipe_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
/*! \fn int _sys_pipe_open(desc_t desc,int o_flag)
    \brief ouverture d'un tube de communication entre deux processus.

    \param desc descripteur de fichier
    \param o_flag option d'ouverture.

    \note Cette fonction permet de rcuprer un tube associ  deux descripteurs de fichier (un descripteur en lecture et un autre en criture).
          L'attribut des descripteurs de fichier de type pipe est S_IFCHR|S_IFIFO.
          En effet les tubes sont vus comme des priphriques de type S_IFCHR.
          Les descripteurs de fichier pour les tubes ont une structure particulire.
          Il possdent le champs pipe_desc dans la structure ext (vfs/vfstypes.h).
          pipe_desc est le descripteur de tube qui permet de retrouver la structure de donnes qui lui est associe (kernel/pipe.c
          et kernel/pipe.h) le nombre de tube disponible est fix par la constante __MAX_PIPE et la taille du buffer associ au tube __PIPE_SIZE.

          Lors de la cration d'un tube c'est le descripteur de lecture qui est allou en premier et qui permet d'obtenir le descripteur
          de tube qui permet de renseigner le champ pipe_desc.
          Le descripteur suivant est forcment le descripteur d'criture et utilise le mme descripteur de tube pipe_desc prcdemment allou.
          Pour les tubes nomms, c'est la fonction mknod qui est utilise avec le paramtre S_IFIFO.
          Dans ce cas, la cration du tube est identique  l'utilisation de la fonction pipe mais le descripteur de tube pipe_desc est insr
          dans l'inoeud du fichier associ au tube.

    \return -1 si erreur sinon 0.
*/
int _sys_pipe_open(desc_t desc,int o_flag){

   pipe_desc_t pipe_desc = -1;

   if(ofile_lst[desc].attr&S_IFIFO)
      pipe_desc= ofile_lst[desc].ext.pipe_desc;

   //to remove: just preserve compatiblity with last version
   if(pipe_desc>=0) return -1;

   //
   if(o_flag & O_RDONLY) {
      if(pipe_desc<0) {
         int i=0;
         //
         for(i=0; i<__MAX_PIPE; i++) {
            if(opipe_lst[i].desc_r<0 && opipe_lst[i].pipe_desc==-1) {
               pipe_desc=i;
               break;
            }
         }
      }

      if(pipe_desc<0)
         return -1;

      opipe_lst[pipe_desc].desc_r=desc;

   }else if(o_flag & O_WRONLY) {
      if(pipe_desc<0) {
         int i=0;
         //
         for(i=0; i<__MAX_PIPE; i++) {
            if(opipe_lst[i].desc_w<0 && opipe_lst[i].pipe_desc==-1) {
               pipe_desc=i;
               break;
            }
         }
      }

      if(pipe_desc<0)
         return -1;

      opipe_lst[pipe_desc].desc_w=desc;
   }
   //
   opipe_lst[pipe_desc].size=0;
   ofile_lst[desc].ext.pipe_desc = pipe_desc;
   //
   if(opipe_lst[pipe_desc].desc_r>=0 && opipe_lst[pipe_desc].desc_w>=0)
      opipe_lst[pipe_desc].pipe_desc=pipe_desc;

   return 0;
}

/*-------------------------------------------
| Name:_pipe_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_pipe_close(desc_t desc){

   pipe_desc_t pipe_desc = ofile_lst[desc].ext.pipe_desc;
   desc_t desc_r=opipe_lst[pipe_desc].desc_r;
   desc_t desc_w=opipe_lst[pipe_desc].desc_w;

   //
   if(pipe_desc<0)
      return -1;
   //

   if((ofile_lst[desc].oflag & O_RDONLY)
      && !ofile_lst[desc].nb_reader) {

      pid_t pid;
      unsigned char i;
      unsigned char d;

      //send SIGPIPE to all writer process
      __atomic_in();
      //
      if(desc_w>=0) {
         for(pid=0; pid<=PROCESS_MAX; pid++) {
            if(!process_lst[pid]) continue;
            for(i=0; i<__FDSET_LONGS; i++) {
               for(d=0; d<__fds_size; d++) {
                  int fd;
                  if( !((process_lst[pid]->fds_bits[i]>>d)&0x01) ) continue;
                  fd=((i<<__shl_fds_bits)+d); //i*8+d

                  //printf("pipe_close pid=%d nbreader=%d desc_w=%d fd=%d\n",pid,ofile_lst[desc].nb_reader,desc_w,fd);
                  //send SIGPIPE to pid
                  if(desc_w==process_lst[pid]->desc_tbl[fd]) {
                     kill_t kill_dt;
                     kill_dt.pid = pid;
                     kill_dt.sig = SIGPIPE;
                     kill_dt.atomic = 1; //__clrirq(), __setirq() not used.
                     //send SIGPIPE to pid process
                     _syscall_kill(process_lst[pid]->pthread_ptr,pid,&kill_dt);
                     //_sys_kill(pid,SIGPIPE,1);
                  }
               }
            }
         } //end for
      } //end if
        //
      opipe_lst[pipe_desc].desc_r=-1;
      //
      __atomic_out();
      //

   }else if((ofile_lst[desc].oflag & O_WRONLY)
            && !ofile_lst[desc].nb_writer) {
      //
      //opipe_lst[pipe_desc].size=0;
      //
      opipe_lst[pipe_desc].desc_w=-1;

      //signal to reader process: no writer.
      if(desc_r>=0)
         __fire_io(ofile_lst[desc_r].owner_pthread_ptr_read);
   }
   //
   if(opipe_lst[pipe_desc].desc_w<0 && opipe_lst[pipe_desc].desc_r<0) {
      //
      ofile_lst[desc].ext.pipe_desc=-1;
      opipe_lst[pipe_desc].pipe_desc=-1;
   }

   return 0;
}

/*-------------------------------------------
| Name:_sys_pipe_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_pipe_isset_read(desc_t desc){
   pipe_desc_t pipe_desc = ofile_lst[desc].ext.pipe_desc;
   desc_t desc_w         = opipe_lst[pipe_desc].desc_w;
   if(desc_w<0)
      return 0;
   if(opipe_lst[pipe_desc].size!=0 || ofile_lst[desc_w].nb_writer<=0)
      return 0;
   else
      return -1;
}

/*-------------------------------------------
| Name:_sys_pipe_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_pipe_isset_write(desc_t desc){
   pipe_desc_t pipe_desc = ofile_lst[desc].ext.pipe_desc;
   desc_t desc_r         = opipe_lst[pipe_desc].desc_r;
   if(desc_r<0)
      return 0;
   if(opipe_lst[pipe_desc].size<__PIPE_SIZE || ofile_lst[desc_r].nb_reader<=0)
      return 0;
   else
      return -1;
}

/*-------------------------------------------
| Name:_pipe_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_pipe_read(desc_t desc,char* buffer,int nbyte ){

   int i=0;
   pipe_desc_t pipe_desc;
   desc_t desc_w;

   //prevent any operation on descriptor
   __syscall_lock();
   //
   pipe_desc = ofile_lst[desc].ext.pipe_desc;
   desc_w    = opipe_lst[pipe_desc].desc_w;
   //
   if(desc_w<0 && !opipe_lst[pipe_desc].size) {
      __syscall_unlock();
      return -1;
   }
   //
   if(!opipe_lst[pipe_desc].size) {
      //to remove:lion's trap
      if(desc_w>=0)
         opipe_lst[pipe_desc].size=0;
      __syscall_unlock();
      return 0;
   }
   //
   if(nbyte>__PIPE_SIZE)
      nbyte = __PIPE_SIZE;

   for(i=0; i<nbyte; i++) {
      buffer[i]=opipe_lst[pipe_desc].buf[ofile_lst[desc].offset];
      if(++ofile_lst[desc].offset==__PIPE_SIZE)
         ofile_lst[desc].offset=0;
      //
      //printf("pzr:%d\r\n",opipe_lst[pipe_desc].size-1);
      //
      if(!(--opipe_lst[pipe_desc].size)) {
         i++;
         break;
      }
   }
   //signal to writer process: buffer is empty and ready for write.
   if(desc_w>=0)
      __fire_io(ofile_lst[desc_w].owner_pthread_ptr_write);
   //
   __syscall_unlock();
   //
   return i;
}

/*-------------------------------------------
| Name:_pipe_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_pipe_write(desc_t desc,const char* buffer,int nbyte ){

   int i;
   pipe_desc_t pipe_desc;
   desc_t desc_r;
   int size;

   //prevent any operation on descriptor
   __syscall_lock();
   //
   pipe_desc = ofile_lst[desc].ext.pipe_desc;
   desc_r = opipe_lst[pipe_desc].desc_r;
   //
   if(desc_r<0) {
      __syscall_unlock();
      return -1;
   }
   //size available in pipe
   if(ofile_lst[desc].offset>=ofile_lst[desc_r].offset) {
      size=(__PIPE_SIZE-ofile_lst[desc].offset)+ofile_lst[desc_r].offset-1;
   }else if(ofile_lst[desc].offset<ofile_lst[desc_r].offset) {
      size=ofile_lst[desc_r].offset-ofile_lst[desc].offset-1;
   }
   //
   if(nbyte>size)
      nbyte=size;
   //
   for(i=0; i<nbyte; i++) {
      opipe_lst[pipe_desc].buf[ofile_lst[desc].offset]=buffer[i];
      //
      if(++ofile_lst[desc].offset==__PIPE_SIZE)
         ofile_lst[desc].offset=0;
   }
   //
   opipe_lst[pipe_desc].size+=nbyte;
   //
   //to remove: lion's trap
   if(!opipe_lst[pipe_desc].size) {
      __syscall_unlock();
      return 0;
   }
   //
   //printf("pzw:%d\r\n",opipe_lst[pipe_desc].size);
   //signal write until space available in this pipe else wait reader free space.
   if(opipe_lst[pipe_desc].size<__PIPE_SIZE)
      __fire_io(ofile_lst[desc].owner_pthread_ptr_write);
   //signal to reader process: buffer is not empty and ready for read.
   if(desc_r>=0)
      __fire_io(ofile_lst[desc_r].owner_pthread_ptr_read);
   //
   __syscall_unlock();
   //
   return i;
}

/*-------------------------------------------
| Name:_sys_pipe_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_pipe_seek(desc_t desc,int offset,int origin){
   return 0;
}

/** @} */
/** @} */

/*===========================================
End of Sourcepipe.c
=============================================*/
