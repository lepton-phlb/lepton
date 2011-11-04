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

#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <sys/shm.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "virtual_kb.h"
#include "virtual_hardware.h"
#include "virtual_cpu.h"
#include "virtual_cpu_gui.h"

char kb_name[] = "/dev/kb0\0";

int virtual_kb_load(void * data);
int virtual_kb_open(void * data);
int virtual_kb_close(void * data);
int virtual_kb_read(void * data);
int virtual_kb_write(void * data);
int virtual_kb_seek(void * data);
int virtual_kb_ioctl(void * data);

hdwr_info_t virtual_kb = {
   kb_name,
   __fdev_not_fd,
   virtual_kb_load,
   virtual_kb_open,
   virtual_kb_close,
   virtual_kb_read,
   virtual_kb_write,
   virtual_kb_seek,
   virtual_kb_ioctl
};

//
static virtual_kb_t * kb_0_data = NULL;
static virtual_cpu_t * vcpu = NULL;
static short kb_ok=0;

//
int virtual_kb_load(void * data) {
   vcpu = (virtual_cpu_t *)data;
   //put data pointer in write place
   kb_0_data = (virtual_kb_t *)(vcpu->shm_base_addr + KB0_OFFSET);
   DEBUG_TRACE("(F) %s load ok ..\n", virtual_kb.name);
   return 0;
}

//nothing to do
int virtual_kb_open(void * data) {
   virtual_cmd_t cmd={KB, OPS_OPEN};
   //
   kb_ok=1;
   while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   DEBUG_TRACE("(F) %s open ok..\n", virtual_kb.name);
   //
   return 0;
}

//
int virtual_kb_close(void * data) {
   virtual_cmd_t cmd={KB, OPS_CLOSE};
   //
   kb_ok=0;
   while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   DEBUG_TRACE("(F) %s open ok..\n", virtual_kb.name);
   //
   return 0;
}

//
int virtual_kb_read(void * data) {
   virtual_cmd_t cmd={KB, OPS_READ};

   //keyboard is already open
   if(kb_ok) {
      kb_0_data->data_in = *((unsigned char *) data);
      kill(getppid(), SIGIO);
      while(write(vcpu->app2synth, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
      //
      DEBUG_TRACE("(F) virtual_kb_read \tdata:%c\n",kb_0_data->data_in);
      //
      while(read(vcpu->synth2app, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   }
   return 0;
}

//
int virtual_kb_write(void * data) {
   return 0;
}

//nothing to do
int virtual_kb_seek(void * data) {
   return 0;
}

//nothing to do
int virtual_kb_ioctl(void * data) {
   return 0;
}

//
int virtual_kb_create_button(GtkBuilder *builder, GtkWidget ** button_list, int size, kb_layout_t *kb_values,
   gint (*pfn)(GtkWidget *widget, GdkEventButton * event, gpointer user_data)) {

   int i = 0;
   char button_name[BUTTON_NAME_SIZE]={0};
   for(;i<size;i++) {
      snprintf(button_name, BUTTON_NAME_SIZE,"%s%d",BUTTON_PREFIX_NAME,i+1);
      //get buttuon
      if(!(button_list[i] = GTK_WIDGET(gtk_builder_get_object(builder,button_name)))) {
         DEBUG_TRACE("(F) virtual_kb : Button number %d\n", i-1);
         return -1;
      }

      DEBUG_TRACE("(F) %s : %c\n",button_name, kb_values[i]);

      //connect button to value
      gtk_signal_connect(GTK_OBJECT (button_list[i]), "button_release_event",
               GTK_SIGNAL_FUNC(pfn), (gpointer)&(kb_values[i]));
   }
   return 0;
}
