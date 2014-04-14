/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Philippe Le Boulanger.
Portions created by Philippe Le Boulanger are Copyright (C) 2013 <lepton.phlb@gmail.com>.
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
#include "kernel/core/kernelconf.h"
#include "kernel/core/core_rttimer.h"
#include "kernel/core/interrupt.h"

/*===========================================
Global Declaration
=============================================*/


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:rttmr_create
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int rttmr_create(tmr_t* tmr,rttmr_attr_t* rttmr_attr){
   if(!tmr || !rttmr_attr)
      return -1;
#ifdef __KERNEL_UCORE_FREERTOS
   //OS_CreateTimer(tmr,rttmr_attr->func,rttmr_attr->tm_msec);
   xTimerCreate( "timer",
                (portTickType)(rttmr_attr->tm_msec/portTICK_RATE_MS),
                pdFALSE,
                tmr,
                (tmrTIMER_CALLBACK) rttmr_attr->func );
#endif
   return 0;
}

/*-------------------------------------------
| Name:rttmr_start
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int rttmr_start(tmr_t* tmr){
   if(!tmr)
      return -1;
#ifdef __KERNEL_UCORE_FREERTOS
   while(xTimerStart(tmr, 10 )!=pdPASS);
#endif
   return 0;
}

/*-------------------------------------------
| Name:rttmr_stop
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int rttmr_stop(tmr_t* tmr){
   if(!tmr)
      return -1;
#ifdef __KERNEL_UCORE_FREERTOS
   while(xTimerStop(tmr, 10 )!=pdPASS);
#endif
   return 0;
}

/*-------------------------------------------
| Name:rttmr_restart
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int rttmr_restart(tmr_t* tmr){
   if(!tmr)
      return -1;
#ifdef __KERNEL_UCORE_FREERTOS
   //OS_RetriggerTimer(tmr);
#endif
   return 0;
}

/*-------------------------------------------
| Name:rttmr_delete
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int rttmr_delete(tmr_t* tmr){
   if(!tmr)
      return -1;
#ifdef __KERNEL_UCORE_FREERTOS
   //OS_DeleteTimer(tmr);
#endif
   return 0;
}




/*===========================================
End of Sourcerttimer.c
=============================================*/
