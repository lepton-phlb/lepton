//
// "$Id: fl_dnd_nanox.cxx,v 1.1 2009-07-15 13:23:30 jjp Exp $"
//
// Drag & Drop code for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2006 by Bill Spitzak and others.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/x.H>
#include "flstring.h"

/*
extern char fl_i_own_selection[2];
extern char *fl_selection_buffer[2];

extern void fl_sendClientMessage(Window window, Atom message,
                                 unsigned long d0,
                                 unsigned long d1=0,
                                 unsigned long d2=0,
                                 unsigned long d3=0,
                                 unsigned long d4=0);
*/
// return version # of Xdnd this window supports.  Also change the
// window to the proxy if it uses a proxy:
static int dnd_aware(Window& window) {
  return 0;
}

static int grabfunc(int event) {
  return 0;
}

extern int (*fl_local_grab)(int); // in Fl.cxx

// send an event to an fltk window belonging to this program:
static int local_handle(int event, Fl_Window* window) {
  return 0;
}

int Fl::dnd() {
  return 1;
}


//
// End of "$Id: fl_dnd_nanox.cxx,v 1.1 2009-07-15 13:23:30 jjp Exp $".
//
