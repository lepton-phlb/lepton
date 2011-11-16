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
#include "kernel/core/kernel.h"
#include "kernel/core/ver.h"
#include "kernel/core/sys/utsname.h"
#include "kernel/core/sys/sysctl.h"
#include "lib/libc/stdio/stdio.h"

#include <string.h>
#include <stdlib.h>

/*============================================
| Global Declaration 
==============================================*/


/*============================================
| Implementation 
==============================================*/

/*--------------------------------------------
| Name:        _ctl_kern
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _sysctl_ctl_kern(int *name, int namelen, void *oldp, int *oldlenp, void *newp,int newlen){

   if(!name || namelen<0){
      return -1;
   }

   switch(name[0]){
      //
      case KERN_UNAME:
         if(!oldp){
            *oldlenp=sizeof(utsname_t);
            return 0;
         }else{
            utsname_t* p_utsname=(utsname_t*)0;
            if(*oldlenp<sizeof(utsname_t))
               return -1;

            p_utsname=(utsname_t*)oldp;

            memset(p_utsname,0,sizeof(utsname_t));
            //sprintf(p_utsname->machine,"a0350,a0502,cm100,a0428",)
            sprintf(p_utsname->sysname,"lepton-%s-%d",__KERNEL_CPU_NAME,__KERNEL_CPU_ARCH);
            sprintf(p_utsname->release,"%s",__lepton_version__);
            sprintf(p_utsname->version,"%s-%s",__kernel_date__,__kernel_time__);
            
         }
      return 0;

      //
      default:
      return -1;
      
   }

   return -1;
}
/*--------------------------------------------
| Name:        _sys_sysctl
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _sys_sysctl(int *name, int namelen, void *oldp, int *oldlenp, void *newp,int newlen){
   if(!name || namelen<0){
      return -1;
   }
   
   switch(name[0]){
      case CTL_KERN:
         return _sysctl_ctl_kern((++name),(--namelen),oldp,oldlenp,newp,newlen);
      break;
   }

   return -1;
}


/*============================================
| End of Source  : sysctl.c
==============================================*/
