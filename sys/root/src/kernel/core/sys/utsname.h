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
#ifndef _SYS_UTSNAME_H
#define _SYS_UTSNAME_H


/*============================================
| Includes
==============================================*/


/*============================================
| Declaration
==============================================*/

#define SYS_NMLN        32

typedef struct utsname {
   char sysname[SYS_NMLN];             /* Name of this OS. */
   char nodename[SYS_NMLN];            /* Name of this network node. */
   char release[SYS_NMLN];             /* Release level. */
   char version[SYS_NMLN];             /* Version level. */
   char machine[SYS_NMLN];             /* Hardware type. */
}utsname_t;


int     _unistd_uname(struct utsname *);
#define uname(__utsname__)_unistd_uname(__utsname__)


#endif
