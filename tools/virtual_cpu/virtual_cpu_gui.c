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
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include <sys/stat.h>
#include <fcntl.h>

#include <sys/shm.h>
#include <sys/mman.h>

#include <sched.h>

#include "virtual_cpu.h"
#include "virtual_cpu_gui.h"
#include "virtual_hardware_conf.h"
#include "screen/virtual_screen.h"
#include "kb/virtual_kb.h"
#include "leds/virtual_leds.h"

#include <gtk/gtk.h>
#include <glib.h>

//

//
extern phdwr_info_t hdwr_lst[];
extern int hdwr_max_dev;
static int nb_sig=0;
static virtual_cmd_t cmd;
//
#define RFDS_SIZE    hdwr_max_dev

//

GIOError read_pipe(GIOChannel *channel, gchar *buf, guint count, guint *bytes_read);
GIOError read_serial_0(GIOChannel *channel, gchar *buf, guint count, guint *bytes_read);
GIOError read_serial_1(GIOChannel *channel, gchar *buf, guint count, guint *bytes_read);
GIOError read_serial_pt(GIOChannel *channel, gchar *buf, guint count, guint *bytes_read);
GIOError read_eth_0(GIOChannel *channel, gchar *buf, guint count, guint *bytes_read);
gint read_kb(GtkWidget *widget, GdkEventButton * event, gpointer user_data);
GIOError read_rtu_0(GIOChannel *channel, gchar *buf, guint count, guint *bytes_read);
GIOError read_rtu_1(GIOChannel *channel, gchar *buf, guint count, guint *bytes_read);

//
gint on_darea_expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data);
gint Repaint(gpointer data);
gint destroy(GtkWidget *widget,gpointer gdata);

//function to call when event on fd occur
static virtual_gui_event_t gio_watch[] = {
      {NULL, (read_gio_watch)read_pipe},
      {NULL, (read_gio_watch)read_serial_0},
      {NULL, (read_gio_watch)read_serial_1},
      {NULL, (read_gio_watch)read_serial_pt},
      {NULL, (read_gio_watch)read_eth_0},
      {NULL, NULL},//kb
      {NULL, NULL},//leds
      {NULL, (read_gio_watch)read_rtu_0},//rtu0
      {NULL, (read_gio_watch)read_rtu_1},//rtu1
      {NULL, NULL},
};

//
//file descriptor for IRQ named pipe
#define PATH_SYNTH2APP     "/tmp/synth2app"
#define PATH_APP2SYNTH     "/tmp/app2synth"

//
static virtual_cpu_t V_CPU;
//
//#define MAIN_WIN_X      480//240
//#define MAIN_WIN_Y      640//420

//
void signal_handler(int sig);
void init_sighandler(void);
int load_pipe(void);
////////////////////////////////////////////////////////

//call all load hardware functions
int init_hardware(void * arg) {
   int i=0;
   int key=-1;
   virtual_cpu_t * vcpu = (virtual_cpu_t *)arg;
   //open shared memory segment for application
   if((key = shmget(VIRTUAL_SHM_KEY, VIRTUAL_SHM_SIZE, IPC_CREAT|0777)) == -1) {
      DEBUG_TRACE("(F) shmget error for apps\n");
      return -1;
   }
   //get ptr data on share memory
   if((vcpu->shm_base_addr = shmat(key, NULL,0)) == (void *)-1) {
      DEBUG_TRACE("(F) shmat error\n");
      return - 1;
   }

   //clear previous data
   memset(vcpu->shm_base_addr, 0, VIRTUAL_SHM_SIZE);
   DEBUG_TRACE("(F) virtual_cpu app load ok..\n");

   //
   V_CPU.hdwr_lst = hdwr_lst;
   for(;i<hdwr_max_dev;i++) {
      V_CPU.hdwr_lst[i]->load(arg);
   }

   return 0;
}

//read pipe and decode cmd
GIOError read_pipe(GIOChannel *channel, gchar *buf, guint count, guint *bytes_read) {
   while(read(0, (void *)&cmd, sizeof(virtual_cmd_t)) != sizeof(virtual_cmd_t));
   //decode cmd
   //decode_cmd(&cmd);
   DEBUG_TRACE("\ndecode_cmd => id :%d, func :%d\n", cmd.hdwr_id, cmd.cmd);
   switch(cmd.cmd) {
   case OPS_OPEN:
      V_CPU.hdwr_lst[cmd.hdwr_id]->open((void *)&V_CPU);
      //
      DEBUG_TRACE("\n%s -> hdwr_lst[cmd->hdwr_id]->fd : %d\n", V_CPU.hdwr_lst[cmd.hdwr_id]->name, V_CPU.hdwr_lst[cmd.hdwr_id]->fd);
      //
      if((!V_CPU.gui_event[cmd.hdwr_id].watch) && (V_CPU.hdwr_lst[cmd.hdwr_id]->fd>0)) {
         V_CPU.gui_event[cmd.hdwr_id].watch = (void *)g_io_channel_unix_new(V_CPU.hdwr_lst[cmd.hdwr_id]->fd);
         g_io_add_watch((GIOChannel *)V_CPU.gui_event[cmd.hdwr_id].watch,G_IO_IN, (GIOFunc)V_CPU.gui_event[cmd.hdwr_id].func,0);
      }
   break;

   case OPS_CLOSE:
      V_CPU.hdwr_lst[cmd.hdwr_id]->close(NULL);
   break;

   case OPS_READ:
      V_CPU.hdwr_lst[cmd.hdwr_id]->read((void *)&V_CPU);
      nb_sig++;
   break;

   case OPS_WRITE:
      V_CPU.hdwr_lst[cmd.hdwr_id]->write((void *)&V_CPU);
      nb_sig++;
   break;

   case OPS_SEEK:
      V_CPU.hdwr_lst[cmd.hdwr_id]->seek((void *)&V_CPU);
   break;

   case OPS_IOCTL:
      V_CPU.hdwr_lst[cmd.hdwr_id]->ioctl((void *)&V_CPU);
   break;

   default:
      DEBUG_TRACE("default[%d:%d]\n",cmd.hdwr_id, cmd.cmd);
   break;
   }
   //
   DEBUG_TRACE("\n\tNB_SIG(R/W):%d\n", nb_sig);

   g_io_add_watch(channel,G_IO_IN, (GIOFunc)V_CPU.gui_event[0].func,0);
   return 0;
}

//
GIOError read_serial_1(GIOChannel *channel, gchar *buf, guint count, guint *bytes_read) {
   V_CPU.hdwr_lst[SERIAL_1]->read((void *)&V_CPU);
   g_io_add_watch(channel ,G_IO_IN, (GIOFunc)V_CPU.gui_event[SERIAL_1].func,0);

   return 0;
}

//
GIOError read_serial_0(GIOChannel *channel, gchar *buf, guint count, guint *bytes_read) {
   V_CPU.hdwr_lst[SERIAL_0]->read((void *)&V_CPU);
   g_io_add_watch(channel ,G_IO_IN, (GIOFunc)V_CPU.gui_event[SERIAL_0].func,0);

   return 0;
}

//
GIOError read_serial_pt(GIOChannel *channel, gchar *buf, guint count, guint *bytes_read) {
   V_CPU.hdwr_lst[SERIAL_PT]->read((void *)&V_CPU);
   g_io_add_watch(channel ,G_IO_IN, (GIOFunc)V_CPU.gui_event[SERIAL_PT].func,0);

   return 0;
}

//
GIOError read_eth_0(GIOChannel *channel, gchar *buf, guint count, guint *bytes_read) {
   V_CPU.hdwr_lst[ETH_0]->read((void *)&V_CPU);
   g_io_add_watch(channel ,G_IO_IN, (GIOFunc)V_CPU.gui_event[ETH_0].func,0);

   return 0;
}

//
gint read_kb(GtkWidget *widget, GdkEventButton * event, gpointer user_data) {
   V_CPU.hdwr_lst[KB]->read((void *)user_data);
   return TRUE;
}

//
GIOError read_rtu_0(GIOChannel *channel, gchar *buf, guint count, guint *bytes_read) {
   V_CPU.hdwr_lst[RTU_0]->read((void *)&V_CPU);
   g_io_add_watch(channel ,G_IO_IN, (GIOFunc)V_CPU.gui_event[RTU_0].func,0);

   return 0;
}

//
GIOError read_rtu_1(GIOChannel *channel, gchar *buf, guint count, guint *bytes_read) {
   V_CPU.hdwr_lst[RTU_1]->read((void *)&V_CPU);
   g_io_add_watch(channel ,G_IO_IN, (GIOFunc)V_CPU.gui_event[RTU_1].func,0);

   return 0;
}

//
gint on_darea_expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data) {
#ifdef TRUE_COLOR
   gdk_draw_rgb_image(  widget->window,
                        widget->style->fg_gc[GTK_STATE_NORMAL],
                        0,
                        0,
                        SCREEN_XRES,
                        SCREEN_YRES,
                        GDK_RGB_DITHER_NONE,
                        (guchar *)(V_CPU.shm_base_addr + SCRN_OFFSET + SHM_SCRN_IOCTL),
                        SCREEN_XRES*(SCREEN_BPP/8));
#else
   gdk_draw_indexed_image(  widget->window,
                     widget->style->fg_gc[GTK_STATE_NORMAL],
                     0,
                     0,
                     SCREEN_XRES,
                     SCREEN_YRES,
                     GDK_RGB_DITHER_NORMAL,
                     (guchar *)(V_CPU.shm_base_addr + SCRN_OFFSET + SHM_SCRN_IOCTL),
                     SCREEN_XRES,
                     V_CPU.cmap);
#endif
   return FALSE;
}

//
gint Repaint(gpointer data) {
   GtkWidget * widget = (GtkWidget *)data;
#ifdef TRUE_COLOR
   gdk_draw_rgb_image(  widget->window,
                  widget->style->fg_gc[GTK_STATE_NORMAL],
                  0,
                  0,
                  SCREEN_XRES,
                  SCREEN_YRES,
                  GDK_RGB_DITHER_NONE,
                  (guchar *)(V_CPU.shm_base_addr + SCRN_OFFSET + SHM_SCRN_IOCTL),
                  SCREEN_XRES*(SCREEN_BPP/8));
#else
   gdk_draw_indexed_image(  widget->window,
                  widget->style->fg_gc[GTK_STATE_NORMAL],
                  0,
                  0,
                  SCREEN_XRES,
                  SCREEN_YRES,
                  GDK_RGB_DITHER_NORMAL,
                  (guchar *)(V_CPU.shm_base_addr + SCRN_OFFSET + SHM_SCRN_IOCTL),
                  SCREEN_XRES,
                  V_CPU.cmap);
#endif
   return TRUE;
}

gint destroy(GtkWidget *widget,gpointer gdata) {
   gdk_rgb_cmap_free(V_CPU.cmap);
   close(V_CPU.synth2app);
   close(V_CPU.app2synth);
   //
   DEBUG_TRACE("(F) before gtk_main_quit\n");
   gtk_main_quit();
   return(FALSE);
}

//
void signal_handler(int sig) {
   switch(sig) {
      case SIGTSTP :
         //wait next signal
         DEBUG_TRACE("(F) Child process receive SIGTSTP : STOP\n");
      break;
      case SIGCONT :
         DEBUG_TRACE("(F) Child process receive SIGCONT : RESUME\n");
      break;

      default:
      break;
   }
}

void init_sighandler(void) {
   struct sigaction sa;
   memset((void *)&sa,0,sizeof(struct sigaction));

   sa.sa_handler = signal_handler;

   sigaction(SIGTSTP, &sa, NULL);
   sigaction(SIGCONT, &sa, NULL);
}

//
int load_pipe(void) {
   DEBUG_TRACE("load pipe %s %s\n", PATH_SYNTH2APP, PATH_APP2SYNTH);
   //open named pipe
   if((V_CPU.synth2app=open(PATH_SYNTH2APP, O_RDONLY))<0) {
      DEBUG_TRACE("Can't open named %s\n", PATH_SYNTH2APP);
      //perror("open");
      return -1;
   }
   //
   if((V_CPU.app2synth=open(PATH_APP2SYNTH, O_WRONLY))<0) {
      DEBUG_TRACE("Can't open named %s\n", PATH_APP2SYNTH);
      return -1;
   }

   //unlock eCos apps
   while(write(1, &cmd, sizeof(virtual_cmd_t)) != sizeof(virtual_cmd_t));
   while(read(0, &cmd, sizeof(virtual_cmd_t)) != sizeof(virtual_cmd_t));

   return 0;
}

//
extern kb_layout_t kb_layout[];
//
int main(int argc, char **argv){
   GtkBuilder *builder = NULL;
   GError     *error = NULL;

   GtkWidget  *main_window = NULL;
   GtkWidget  *main_screen = NULL;
   GtkWidget  *button_list[MAX_BUTTON];
   GtkWidget  *led_list[MAX_LEDS];

#ifndef TRUE_COLOR
   guint32 virtual_color[256];
#endif
   //
   init_sighandler();

   //init gtk context
   gtk_init(&argc, &argv);
   gdk_rgb_init();

   //init color palette
#ifndef TRUE_COLOR
   virtual_palette_set((unsigned int *)virtual_color);
   V_CPU.cmap = gdk_rgb_cmap_new(virtual_color, 256);
#endif

   builder = gtk_builder_new();

   //load UI from file. If error occurs, report it and quit application.
   if(!gtk_builder_add_from_file(builder, UI_FILE, &error)) {
      g_warning("%s", error->message);
      g_free(error);
      return -1;
   }

   //
   gtk_builder_connect_signals(builder,NULL);

   //create main_window and configure it main_screen an buttonq
   main_window = GTK_WIDGET(gtk_builder_get_object(builder,"main_window"));
   gtk_signal_connect(GTK_OBJECT (main_window), "delete_event",
            GTK_SIGNAL_FUNC(destroy),NULL);

   //create main screen and configure it
   main_screen = GTK_WIDGET(gtk_builder_get_object(builder,"main_screen"));
   //gtk_drawing_area_size(GTK_DRAWING_AREA(main_screen), SCREEN_XRES, SCREEN_YRES);
   gtk_signal_connect(GTK_OBJECT (main_screen), "expose_event",
            GTK_SIGNAL_FUNC(on_darea_expose),NULL);

   //refresh screen rate 75 ms
   gtk_timeout_add(75, Repaint,(gpointer)main_screen);

   //read kb layout provide by user interface
   virtual_kb_create_button(builder,button_list,MAX_BUTTON,kb_layout,read_kb);

   //create radio button provide by user interface
   virtual_leds_create_radio_button(builder,led_list,MAX_LEDS);

   //add stdin in watch descriptor
   V_CPU.gui_event = gio_watch;
   V_CPU.gui_event[0].watch = (void*)g_io_channel_unix_new(0);
   g_io_add_watch((GIOChannel *)V_CPU.gui_event[0].watch,G_IO_IN, (GIOFunc)gio_watch[0].func,0);

   gtk_widget_add_events(main_screen, GDK_POINTER_MOTION_MASK|GDK_BUTTON_PRESS_MASK|GDK_BUTTON_RELEASE_MASK);
   gtk_widget_show(main_window);

   //destroy builder, since we don't need it anymore
   g_object_unref(G_OBJECT(builder));

   //init hardware
   init_hardware((void *)&V_CPU);

   //wait for pipe ready
   if(load_pipe()<0) {
      return -1;
   }

   //main gtk loop
   gtk_main();

   return 0;
}

