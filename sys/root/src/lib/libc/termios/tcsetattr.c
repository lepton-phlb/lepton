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
#include "kernel/core/errno.h"
#include "kernel/core/devio.h"
#include "lib/libc/termios/termios.h"


#if !defined(__powerpc__) && !defined(__sparc__) && !defined(__alpha__) && !defined(__hppa__)
   #if TCSANOW==0 && TCSADRAIN==1 && TCSAFLUSH==2 && TCSETSW-TCSETS==1 && TCSETSF-TCSETS==2
      #define shortcut
   #endif
#endif

int  tcsetattr ( int fd, int optional_actions, const struct termios* termios_p )
{
#ifdef shortcut

   if ( (unsigned int)optional_actions < 3u )
      return ioctl ( fd, TCSETS+optional_actions, termios_p );

   //errno = EINVAL;
   return -1;

#else

   switch ( optional_actions ) {
   case TCSANOW:
      return ioctl ( fd, TCSETS, termios_p );
   case TCSADRAIN:
      return ioctl ( fd, TCSETSW, termios_p );
   case TCSAFLUSH:
      return ioctl ( fd, TCSETSF, termios_p );
   default:
      //errno = EINVAL;
      return -1;
   }

#endif
}
