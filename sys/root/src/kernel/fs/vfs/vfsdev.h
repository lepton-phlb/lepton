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

/**
 * \file
 * implementation du systme de fichiers rootfs
 * \author philippe le boulanger
 */

/*===========================================
Compiler Directive
=============================================*/
#ifndef _VFSDEV_H
#define _VFSDEV_H


/*===========================================
Includes
=============================================*/
#include "kernel/core/types.h"

#include <stdarg.h>

#define __fdev_not_implemented  (void*)0

/*===========================================
Declaration
=============================================*/


typedef void (*DEV_INTERRUPT_HANDLER)(void);
typedef DEV_INTERRUPT_HANDLER fdev_interrupt_t;


//
typedef int (*DEV_LOAD)(void);
typedef DEV_LOAD fdev_load_t;

typedef int (*DEV_OPEN)(desc_t desc, int o_flag);
typedef DEV_OPEN fdev_open_t;

typedef int (*DEV_CLOSE)(desc_t desc);
typedef DEV_CLOSE fdev_close_t;

typedef int (*DEV_ISSET_READ)(desc_t desc);
typedef DEV_ISSET_READ fdev_isset_read_t;

typedef int (*DEV_ISSET_WRITE)(desc_t desc);
typedef DEV_ISSET_READ fdev_isset_write_t;

typedef int (*DEV_READ)(desc_t desc, char* buf,int size);
typedef DEV_READ fdev_read_t;

typedef int (*DEV_WRITE)(desc_t desc, const char* buf,int size);
typedef DEV_WRITE fdev_write_t;

typedef int (*DEV_SEEK)(desc_t desc,int offset,int origin);
typedef DEV_SEEK fdev_seek_t;

typedef int (*DEV_IOCTL)(desc_t desc,int request,va_list ap);
typedef DEV_IOCTL fdev_ioctl_t;


//extended function for specific operation
typedef const void* pfdev_ext_t;

//rtc specific
typedef int (*DEV_RTC_SETTIME)(desc_t desc,char* buf,int size);
typedef DEV_RTC_SETTIME fdev_rtc_settime_t;

typedef int (*DEV_RTC_GETTIME)(desc_t desc,char* buf,int size);
typedef DEV_RTC_SETTIME fdev_rtc_gettime_t;

typedef struct {
   fdev_rtc_settime_t fdev_rtc_settime;
   fdev_rtc_gettime_t fdev_rtc_gettime;
}fdev_rtc_t;

typedef const fdev_rtc_t dev_rtc_t;

//device driver operations
typedef struct {
   const char* dev_name;
   const int dev_attr;

   fdev_load_t fdev_load;
   fdev_open_t fdev_open;
   fdev_close_t fdev_close;
   fdev_isset_read_t fdev_isset_read;
   fdev_isset_write_t fdev_isset_write;
   fdev_read_t fdev_read;
   fdev_write_t fdev_write;
   fdev_seek_t fdev_seek;
   fdev_ioctl_t fdev_ioctl;

   pfdev_ext_t pfdev_ext;
}fdev_map_t;

typedef const fdev_map_t dev_map_t;
typedef const fdev_map_t*  pdev_map_t;


//device driver list (see mklepton)
extern pdev_map_t const * pdev_lst;

extern const char max_dev;

#define __KERNEL_DEV_MAX max_dev


#endif
