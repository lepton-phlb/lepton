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

#define TRACE_C
#include "lld.h"
#include "trace.h"

extern DWORD os_GetCurrentTicks();


#define STORE_TIMESTAMP         trace[cycle].timestamp = os_GetCurrentTicks()


int polling_transitions = 0;
POLLING_TRANSITION transitions[MAX_NUM_OF_TRANSITIONS];
CYCLE trace[MAX_TRACE_CYCLES];
int cycle = -1;
int trace_wrapped = FALSE;



void StoreTraceCycle(OPERATION op, FLASHDATA * base, ADDRESS offset, ADDRESS addr, FLASHDATA data)
{
   cycle++;
   if (cycle >= MAX_TRACE_CYCLES)
   {
      cycle = 0;
      trace_wrapped = TRUE;
   }
   trace[cycle].op        = op;
   trace[cycle].base      = base;
   trace[cycle].offset    = offset;
   trace[cycle].addr      = addr;
   trace[cycle].data      = data;

}



void FlashWrite(FLASHDATA * addr, ADDRESS offset, FLASHDATA data)
{
   FLASH_OFFSET((addr),(offset)) = (data);
   StoreTraceCycle(OP_WRITE, addr, offset, (ADDRESS)((volatile FLASHDATA*)(addr) + (offset)), data);

#ifdef PRINT_TIMESTAMP
   STORE_TIMESTAMP;
#endif
}

FLASHDATA FlashRead(FLASHDATA * addr, ADDRESS offset)
{
   FLASHDATA data;
   data = FLASH_OFFSET((addr),(offset));
   StoreTraceCycle(OP_READ, addr, offset, (ADDRESS)((volatile FLASHDATA*)(addr) + (offset)), data);

#ifdef PRINT_TIMESTAMP
   STORE_TIMESTAMP;
#endif

   return(data);
}


