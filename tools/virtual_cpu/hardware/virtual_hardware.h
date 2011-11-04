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

#ifndef VIRTUAL_HARDWARE_H_
#define VIRTUAL_HARDWARE_H_

#include <sys/types.h>
//define dummy functions for all hardware
typedef int (*hdwr_load)(void * data);
typedef int (*hdwr_open)(void * data);
typedef int (*hdwr_close)(void * data);
typedef int (*hdwr_read)(void * data);
typedef int (*hdwr_write)(void * data);
typedef int (*hdwr_seek)(void * data);
typedef int (*hdwr_ioctl)(void * data);

typedef struct {
   char *name;
   int fd;

   hdwr_load load;
   hdwr_load open;
   hdwr_load close;
   hdwr_load read;
   hdwr_load write;
   hdwr_load seek;
   hdwr_load ioctl;
}hdwr_info_t;

typedef hdwr_info_t *      phdwr_info_t;

//
#define __fdev_not_fd      -1

#endif /* VIRTUAL_HARDWARE_H_ */
