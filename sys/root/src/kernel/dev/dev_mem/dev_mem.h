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
| Compiler Directive   
==============================================*/
#ifndef _DEV_MEM_H
#define _DEV_MEM_H


/*============================================
| Includes 
==============================================*/


/*============================================
| Declaration  
==============================================*/

#define MAX_MEM_ADDR  31

#define MEMREG      0x0001
#define MEMUNREG    0x0002
#define MEMADD      0x0003
#define MEMFLUSH    0x0004


//begin 1 byte struct alignment 
//force compatiblity with mklepton
#pragma pack(push, 1)

//to remove: only for test
typedef struct{
   char  prm0;
   int   prm1;
   float prm2;
}struct1_t;

typedef struct{
   char  str0[10];
   char  str1[5];
   float prm2;
}struct2_t;

#pragma pack (pop)
//end 1 byte struct alignment 


#endif
