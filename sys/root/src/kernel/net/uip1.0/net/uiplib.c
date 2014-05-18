/*
The contents of this file are subject to the Mozilla Public License Version 1.1 
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
specific language governing rights and limitations under the License.

The Original Code is ______________________________________.

The Initial Developer of the Original Code is ________________________.
Portions created by ______________________ are Copyright (C) ______ _______________________.
All Rights Reserved.

Contributor(s): ______________________________________.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license 
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable 
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under 
the MPL, indicate your decision by deleting  the provisions above and replace 
them with the notice and other provisions required by the [eCos GPL] License. 
If you do not delete the provisions above, a recipient may use your version of this file under 
either the MPL or the [eCos GPL] License."
*/


#include "uip.h"
#include "uiplib.h"


/*-----------------------------------------------------------------------------------*/
unsigned char
uiplib_ipaddrconv(char *addrstr, unsigned char *ipaddr)
{
  unsigned char tmp;
  char c;
  unsigned char i, j;

  tmp = 0;
  
  for(i = 0; i < 4; ++i) {
    j = 0;
    do {
      c = *addrstr;
      ++j;
      if(j > 4) {
	return 0;
      }
      if(c == '.' || c == 0) {
	*ipaddr = tmp;
	++ipaddr;
	tmp = 0;
      } else if(c >= '0' && c <= '9') {
	tmp = (tmp * 10) + (c - '0');
      } else {
	return 0;
      }
      ++addrstr;
    } while(c != '.' && c != 0);
  }
  return 1;
}

/*-----------------------------------------------------------------------------------*/
