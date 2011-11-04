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

#ifndef VIRTUAL_CPU_GUI_H_
#define VIRTUAL_CPU_GUI_H_

#include <gtk/gtk.h>
#include <glib.h>

#include "hardware/virtual_hardware.h"

#ifndef  UI_FILE
#define  UI_FILE  "ui/x1062/x1062_gui.xml"
#endif

//structure for gtk read event
typedef struct gint(*read_gio_watch)(GIOChannel *channel, gchar *buf, guint count, guint *bytes_read);

typedef struct virtual_gui_event_st {
   void *         watch;
   read_gio_watch func;
}virtual_gui_event_t;


//structure for application
typedef struct {
   void *               shm_base_addr;//shared memory ptr

   phdwr_info_t *       hdwr_lst; //available device

   virtual_gui_event_t* gui_event;

   GdkRgbCmap* cmap;//contain palette

   int synth2app;
   int app2synth;
}virtual_cpu_t;

#endif /* VIRTUAL_CPU_GUI_H_ */
