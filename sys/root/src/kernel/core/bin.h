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
#ifndef _BIN_H
#define _BIN_H

/*===========================================
Includes
=============================================*/
#include "kernel/core/types.h"
#include "kernel/core/process.h"

/*===========================================
Declaration
=============================================*/
#define EXEC_SIGNT   0x1b //'esc'

//
typedef struct {
   char signature;
   pthread_priority_t   priority;
   pthread_stacksize_t  stacksize;
   pthread_timeslice_t  timeslice;
   int16_t              index;
}exec_file_t;

typedef struct{
   const char * name;
   _main_routine_t      start_routine;
   pthread_priority_t   priority;
   pthread_stacksize_t  stacksize;
   pthread_timeslice_t  timeslice;
}bin_t;

extern const bin_t* bin_lst;
extern const int bin_lst_size;


int _mkbin(const char* ref,const char* name);

#endif
