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

#ifndef TRACE_H
#define TRACE_H

typedef DWORD TIMESTAMP;

typedef enum
{
   OP_READ = 1,
   OP_WRITE
}OPERATION;

typedef struct
{
   OPERATION op;
   FLASHDATA *   base;
   ADDRESS offset;
   ADDRESS addr;
   FLASHDATA data;
#ifdef PRINT_TIMESTAMP
   TIMESTAMP timestamp;
#endif
}CYCLE;

typedef struct
{
   POLLING_TYPE polling_type;      /* tells us the type of operation - erase, prog */
   FLASHDATA polling_data[5];      /* shows us whats the last five polls looked like */
   FLASHDATA expected_data;        /* the value being prog or erased */
   DEVSTATUS return_code;          /* tells us what conclusion the sw had */
}POLLING_TRANSITION;

#define MAX_NUM_OF_TRANSITIONS 100
#define MAX_TRACE_CYCLES 512

#ifndef TRACE_C
extern CYCLE trace[MAX_TRACE_CYCLES];
extern int cycle;
extern int trace_wrapped;
#endif

#endif
