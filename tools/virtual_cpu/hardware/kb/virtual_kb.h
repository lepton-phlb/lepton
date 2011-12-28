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

#ifndef VIRTUAL_KB_H_
#define VIRTUAL_KB_H_

#include <gtk/gtk.h>
#include <glib.h>
#include <gtk/gtktable.h>

#define MAX_BUTTON            32

#define BUTTON_PREFIX_NAME    "button_"
#define BUTTON_NAME_SIZE      16

typedef const unsigned char kb_layout_t;

typedef struct virtual_button_st {
   GtkWidget * button;
   gchar * name;
}virtual_button_t;

typedef struct virtual_new_button_st
{
	GtkWidget * button;
	gchar * name;
	guint left_attach;
	guint right_attach;
	guint top_attach;
	guint bottom_attach;
	GtkAttachOptions xoptions;
	GtkAttachOptions yoptions;
	guint xpadding;
	guint ypadding;
}virtual_new_button_t;

//
//
//int virtual_kb_init(void *vc, void * data, gint (*pfn)(GtkWidget *widget, GdkEventButton * event, gpointer user_data));
int virtual_new_kb_init(void *vc, void * data, gint (*pfn)(GtkWidget *widget, GdkEventButton * event, gpointer user_data));

//
int virtual_kb_create_button(GtkBuilder *builder, GtkWidget ** button_list, int size, kb_layout_t *kb_values,
   gint (*pfn)(GtkWidget *widget, GdkEventButton * event, gpointer user_data));
#endif /* VIRTUAL_KB_H_ */
