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
int rttmr_create(tmr_t* tmr,rttmr_attr_t* rttmr_attr)
{
#if defined(USE_ECOS)
   {
      cyg_handle_t counter_hdl;
      cyg_handle_t sys_clk;

      if(!tmr || !rttmr_attr)
         return -1;

      sys_clk = cyg_real_time_clock();
      cyg_clock_to_counter( sys_clk, &counter_hdl );
      cyg_alarm_create( counter_hdl,
                        rttmr_attr->func,
                        rttmr_attr->data,
                        &tmr->alarm_hdl,
                        &tmr->alarm_obj);
      //need a time when the first alarm occurs
      cyg_alarm_initialize(tmr->alarm_hdl, __get_timer_ticks(), rttmr_attr->tm_msec);
      cyg_alarm_disable(tmr->alarm_hdl);
   }
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
int rttmr_start(tmr_t* tmr)
{
   if(!tmr)
      return -1;

#if defined(USE_ECOS)
   cyg_alarm_enable(tmr->alarm_hdl);
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
int rttmr_stop(tmr_t* tmr)
{
   if(!tmr)
      return -1;

#if defined(USE_ECOS)
   cyg_alarm_disable(tmr->alarm_hdl);
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
int rttmr_restart(tmr_t* tmr)
{
   if(!tmr)
      return -1;

#if defined(USE_ECOS)
   cyg_alarm_enable(tmr->alarm_hdl);
#endif
   return 0;
}

/*-------------------------------------------
| Name:rttmr_reload
| Description: load a new delay in timer
| Parameters:
| Return Type:
| Comments: a reload stops the timer, rttmr_restart is needed to run again
| See:
---------------------------------------------*/
int rttmr_reload(tmr_t* tmr, time_t delay)
{
   if (!tmr)
      return -1;

#if defined(USE_ECOS)
   //stop timer
   cyg_alarm_disable(tmr->alarm_hdl);
   //change delay
   cyg_alarm_initialize(tmr->alarm_hdl, __get_timer_ticks(), delay);
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
int rttmr_delete(tmr_t* tmr)
{
   if(!tmr)
      return -1;

#if defined(USE_ECOS)
   cyg_alarm_delete(tmr->alarm_hdl);
#endif
   return 0;
}

/*===========================================
| End of Source : rttimer.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
| Revision 1.1  2009/03/30 11:18:54  jjp
| First import of tauon
|
| Revision 1.1.1.1  2009/02/05 15:29:49  jjp
| First import in CVS
|
|---------------------------------------------
=============================================*/
