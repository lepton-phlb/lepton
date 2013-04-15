//
// "$Id: Fl_nanox.cxx,v 1.3 2010-04-01 07:18:48 jjp Exp $"
//
// Nano-X specific code for the Fast Light Tool Kit (FLTK).
// base on Fl_x.cxx
//
// Copyright 1998-2007 by Bill Spitzak and others.
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
// Please report all bugs and problems to "fltk-bugs@easysw.com".
//
// Tanghao - original port of FLTK 1.0.7
// Vladimir Ananiev (Vovan888 at gmail com) - update to FLTK 1.1.8

#include <config.h>
#include <FL/Fl.H>

//#ifdef NANO_X

#include <nxdraw.h>

#include <FL/x.H>
#include <FL/Fl_Window.H>
//#include <ctype.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <unistd.h>
//#include <sys/time.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "kernel/core/time.h"
#include "kernel/core/libstd.h"
#include "kernel/core/interrupt.h"

#include "lib/libc/stdio/stdio.h"
#include "lib/libc/ctype/ctype.h"


#define CONSOLIDATE_MOTION 1
/**** Define this if your keyboard lacks a backspace key... ****/
/* #define BACKSPACE_HACK 1 */


////////////////////////////////////////////////////////////////
// interface to poll/select call:

#if USE_POLL

#include <poll.h>
static pollfd *pollfds = 0;

#else

#if HAVE_SYS_SELECT_H
//#  include <sys/select.h>
#  include "kernel/core/select.h"
#endif /* HAVE_SYS_SELECT_H */

// The following #define is only needed for HP-UX 9.x and earlier:
//#define select(a,b,c,d,e) select((a),(int *)(b),(int *)(c),(int *)(d),(e))

//static fd_set fdsets[3];
//static int maxfd;
#define POLLIN 1
#define POLLOUT 4
#define POLLERR 8

#endif /* HAVE_POLL */

/* JHC 09/19/00 - Added this array to simplify grabbing keystrokes */
/* Because we care about so many different modifiers, its almost easier */
/* to have an array instead of a million if statements */

/* I think I have everything mapped, but check me for accuracy */

static struct
{
  unsigned short key;
  unsigned short value;
}
keymap_array[] =
{
  {  MWKEY_ENTER, FL_Enter}
  , {  MWKEY_BACKSPACE, FL_BackSpace}
  , {  MWKEY_TAB, FL_Tab}
  , {  MWKEY_SCROLLOCK, FL_Scroll_Lock}
  , {	MWKEY_ESCAPE, FL_Escape}
  , {  MWKEY_HOME, FL_Home}
  , {  MWKEY_LEFT, FL_Left}
  , {  MWKEY_UP, FL_Up}
  , {  MWKEY_RIGHT, FL_Right}
  , {  MWKEY_DOWN, FL_Down}
  , {  MWKEY_PAGEUP, FL_Page_Up}
  , {  MWKEY_PAGEDOWN, FL_Page_Down}
  , {  MWKEY_END, FL_End}
  ,    //  { 99 , FL_Insert},
  {  MWKEY_BACKSPACE, FL_BackSpace}
  , {  MWKEY_KP_ENTER, FL_KP_Enter}
  , {  MWKEY_KP7, FL_KP + '7'}
  , {  MWKEY_KP4, FL_KP + '4'}
  , {  MWKEY_KP8, FL_KP + '8'}
  , {  MWKEY_KP6, FL_KP + '6'}
  , {  MWKEY_KP2, FL_KP + '2'}
  , {  MWKEY_KP9, FL_KP + '9'}
  , {  MWKEY_KP3, FL_KP + '3'}
  , {  MWKEY_KP1, FL_KP + '1'}
  , {  MWKEY_KP5, FL_KP + '5'}
  , {  MWKEY_KP0, FL_KP + '0'}
  , {  MWKEY_KP_PERIOD, FL_KP + '.'}
  , {  MWKEY_KP_MULTIPLY, FL_KP + '*'}
  , {  MWKEY_KP_PLUS, FL_KP + '+'}
  , {  MWKEY_KP_MINUS, FL_KP + '-'}
  , {  MWKEY_KP_DIVIDE, FL_KP + '/'}
  , {  MWKEY_F1, FL_F + 1}
  , {  MWKEY_F2, FL_F + 2}
  , {  MWKEY_F3, FL_F + 3}
  , {  MWKEY_F4, FL_F + 4}
  , {  MWKEY_F5, FL_F + 5}
  , {  MWKEY_F6, FL_F + 6}
  , {  MWKEY_F7, FL_F + 7}
  , {  MWKEY_F8, FL_F + 8}
  , {  MWKEY_F9, FL_F + 9}
  , {  MWKEY_F10, FL_F + 10}
  , {  MWKEY_F11, FL_F + 11}
  , {  MWKEY_F12, FL_F + 12}
  , {  MWKEY_RSHIFT, FL_Shift_R}
  , {  MWKEY_LSHIFT, FL_Shift_L}
  , {  MWKEY_LCTRL, FL_Control_L}
  , {  MWKEY_RCTRL, FL_Control_R}
  , {  MWKEY_CAPSLOCK, FL_Caps_Lock}
  , {  MWKEY_LMETA, FL_Alt_L}
  , {  MWKEY_RMETA, FL_Alt_R}
  , {  MWKEY_DELETE, FL_Delete}
  , {  0, 0}
};

static int nfds = 0;

static int fd_array_size = 0;
struct FD {
#  if !USE_POLL
  int fd;
  short events;
#  endif
  void (*cb)(int, void*);
  void* arg;
};

static FD *fd = 0;

void Fl::add_fd(int n, int events, void (*cb) (int, void *), void *v) {
  if (events != POLLIN) return;

  remove_fd (n, events);
  GrRegisterInput(n);

  int i = nfds++;

  if (i >= fd_array_size) {
    fd_array_size = 2 * fd_array_size + 1;
    fd = (FD *) realloc (fd, fd_array_size * sizeof (FD));

#ifdef JEFFM_FUGLY
#if HAVE_POLL
    pollfds = (pollfd *) realloc (pollfds, fd_array_size * sizeof (pollfd));
#endif
#endif

  }

  fd[i].fd = n;
  fd[i].events = events;
  fd[i].cb = cb;
  fd[i].arg = v;

#ifdef JEFFM_FUGLY

#if HAVE_POLL
  fds[i].fd = n;
  fds[i].events = events;
#else
  if (events & POLLIN)
    FD_SET (n, &fdsets[0]);
  if (events & POLLOUT)
    FD_SET (n, &fdsets[1]);
  if (events & POLLERR)
    FD_SET (n, &fdsets[2]);
  if (n > maxfd)
    maxfd = n;
#endif

#endif /* fuggly */
}

void Fl::add_fd(int fd, void (*cb) (int, void *), void *v) {
  Fl::add_fd (fd, POLLIN, cb, v);
}

void Fl::remove_fd(int n, int events) {
  int i, j;
  for (i = j = 0; i < nfds; i++) {
    if (fd[i].fd == n) {
      int e = fd[i].events & ~events;
      if (!e)
	continue;		// if no events left, delete this fd
      fd[i].events = e;

#ifdef JEFFM_FUGLY
#if HAVE_POLL
      fds[j].events = e;
#endif
#endif

    }
    // move it down in the array if necessary:
    if (j < i) {
      fd[j] = fd[i];
#if HAVE_POLL
      fds[j] = fds[i];
#endif
    }
    j++;
  }
  nfds = j;
#ifdef JEFFM_FUGLY

#if !HAVE_POLL
  if (events & POLLIN)
    FD_CLR (n, &fdsets[0]);
  if (events & POLLOUT)
    FD_CLR (n, &fdsets[1]);
  if (events & POLLERR)
    FD_CLR (n, &fdsets[2]);
  if (n == maxfd)
    maxfd--;
#endif
#endif
  GrUnregisterInput(n);
// fix memory leak
  if ( (nfds == 0) && (fd > 0) )
     free(fd);
}

void Fl::remove_fd(int n){
  remove_fd(n, -1);
}

#if CONSOLIDATE_MOTION
static Fl_Window *send_motion;
extern Fl_Window *fl_xmousewin;
#endif

static bool in_a_window; // true if in any of our windows, even destroyed ones
static void do_queued_events() {
  in_a_window = true;
//  GR_EVENT xevent;
//  while ( GrQueueLength() ) {
//    GrGetNextEvent(&xevent);
//    fl_handle(xevent);
//  }
  // we send FL_LEAVE only if the mouse did not enter some other window:
  if (!in_a_window) Fl::handle(FL_LEAVE, 0);
#if CONSOLIDATE_MOTION
  else if (send_motion == fl_xmousewin) {
    send_motion = 0;
    Fl::handle(FL_MOVE, fl_xmousewin);
  }
#endif
}

// these pointers are set by the Fl::lock() function:
static void nothing() {}
void (*fl_lock_function)() = nothing;
void (*fl_unlock_function)() = nothing;

int update = 0;

// This is never called with time_to_wait < 0.0:
// It should return negative on error, 0 if nothing happens before
// timeout, and >0 if any callbacks were done.
int fl_wait(double time_to_wait) {

   //if (GrQueueLength()) {do_queued_events(); return 1;}

   double mtime = 1000.0 * time_to_wait;
   int msec = int (mtime);

   if (msec < 0)
      msec = 0;

   GR_EVENT ev;
   ev.general.type = GR_EVENT_TYPE_NONE;

   fl_unlock_function();

   //Bug fix
   GrGetNextEventTimeout (&ev, msec_to_tick(msec));

   fl_lock_function();

   if (ev.general.type == GR_EVENT_TYPE_TIMEOUT) {
      return 0;
   }
   if (ev.general.type > GR_EVENT_TYPE_NONE) {
      /*there was an event*/
      fl_handle (ev);
      return 1;
   }
   return 0;
}

// fl_ready() is just like fl_wait(0.0) except no callbacks are done:
int fl_ready ()
{

  GR_EVENT ev;
  ev.general.type = GR_EVENT_TYPE_NONE;

  GrPeekWaitEvent(&ev);
  return (ev.general.type > GR_EVENT_TYPE_NONE)? 1 : 0;
}

////////////////////////////////////////////////////////////////
//for nanoX ,by tanghao
static   GR_SCREEN_INFO si; /* information about screen */
static int si_valid = 0;

int fl_display = 0;
int fl_screen;

XVisualInfo *fl_visual;

Colormap fl_colormap;

void fl_open_display() {

  if (fl_display)
    return;

  int d = 0;
  if ((d = GrOpen ()) < 0) {
//    char buffer[256];
    /* run server and window manager */
    //sprintf(buffer, "%s/nano-X -p &; %s/nanowm &", NANOXFOLDER, NANOXFOLDER);
    //tauon
    //system ("nano-X -p &; nanowm &");
    if ((d = GrOpen ()) < 0) {
      printf ("cannot open Nano-X graphics,Please run 'nano-X -p' first.\n");
      exit (1);
    }
  }

  // for nanox,by tanghao
  fl_display = d;

  // for nanox,by tanghao
  fl_screen = 0;

  GrReqShmCmds(4096);

  if (!si_valid) {
	GrGetScreenInfo (&si);
	si_valid = 1;
  }
  //end nanox
}

void fl_close_display() {
//  Fl::remove_fd (fl_display);
  GrClose ();
}


int Fl::x() {
  return 0;
}

int Fl::y() {
  return 0;
}

int Fl::h () {
  fl_open_display();

  return si.rows;
//  return DisplayHeight(fl_display,fl_screen);
}

int Fl::w () {
  fl_open_display();

  return si.cols;
  //  return DisplayWidth(fl_display,fl_screen);
}

void Fl::get_mouse (int &x, int &y) {
  fl_open_display();
  // for nanox,by tanghao

  //  Window root = RootWindow(fl_display, fl_screen);
  //  Window c; int mx,my,cx,cy; unsigned int mask;
  //  XQueryPointer(fl_display,root,&root,&c,&mx,&my,&cx,&cy,&mask);
  //  x = mx;
  //  y = my;
  fprintf (stderr, "Nano-X don't support get_mouse(x,y)in file(Fl_X.cxx)\n");
  GR_WINDOW_INFO info;
  GrGetWindowInfo (fl_window, &info);	//(GR_WINDOW_ID wid, GR_WINDOW_INFO *infoptr);
  x = info.x + info.width / 2;
  y = info.y + info.height / 2;
  //end nanox
}

////////////////////////////////////////////////////////////////
// Code used for paste and DnD into the program:
// TODO vovan888 - fix clipboard

Fl_Widget *fl_selection_requestor;
char *fl_selection_buffer[2];
int fl_selection_length[2];
int fl_selection_buffer_length[2];
char fl_i_own_selection[2];

// Call this when a "paste" operation happens:
void Fl::paste(Fl_Widget &receiver, int clipboard) {
#ifndef NANO_X
  if (fl_i_own_selection[clipboard]) {
    // We already have it, do it quickly without window server.
    // Notice that the text is clobbered if set_selection is
    // called in response to FL_PASTE!
    Fl::e_text = fl_selection_buffer[clipboard];
    Fl::e_length = fl_selection_length[clipboard];
    if (!Fl::e_text) Fl::e_text = (char *)"";
    receiver.handle(FL_PASTE);
    return;
  }
  // otherwise get the window server to return it:
  fl_selection_requestor = &receiver;
  Atom property = clipboard ? CLIPBOARD : XA_PRIMARY;
  XConvertSelection(fl_display, property, XA_STRING, property,
		    fl_xid(Fl::first_window()), fl_event_time);
#endif
}

#ifndef NANO_X
Window fl_dnd_source_window;
Atom *fl_dnd_source_types; // null-terminated list of data types being supplied
Atom fl_dnd_type;
Atom fl_dnd_source_action;
Atom fl_dnd_action;

void fl_sendClientMessage(Window window, Atom message,
                                 unsigned long d0,
                                 unsigned long d1=0,
                                 unsigned long d2=0,
                                 unsigned long d3=0,
                                 unsigned long d4=0)
{
  XEvent e;
  e.xany.type = ClientMessage;
  e.xany.window = window;
  e.xclient.message_type = message;
  e.xclient.format = 32;
  e.xclient.data.l[0] = (long)d0;
  e.xclient.data.l[1] = (long)d1;
  e.xclient.data.l[2] = (long)d2;
  e.xclient.data.l[3] = (long)d3;
  e.xclient.data.l[4] = (long)d4;
  XSendEvent(fl_display, window, 0, 0, &e);
}
#endif

////////////////////////////////////////////////////////////////
// Code for copying to clipboard and DnD out of the program:

void Fl::copy(const char *stuff, int len, int clipboard) {
#ifndef NANO_X
  if (!stuff || len<0) return;
  if (len+1 > fl_selection_buffer_length[clipboard]) {
    delete[] fl_selection_buffer[clipboard];
    fl_selection_buffer[clipboard] = new char[len+100];
    fl_selection_buffer_length[clipboard] = len+100;
  }
  memcpy(fl_selection_buffer[clipboard], stuff, len);
  fl_selection_buffer[clipboard][len] = 0; // needed for direct paste
  fl_selection_length[clipboard] = len;
  fl_i_own_selection[clipboard] = 1;
  Atom property = clipboard ? CLIPBOARD : XA_PRIMARY;
  XSetSelectionOwner(fl_display, property, fl_message_window, fl_event_time);
#endif
}

////////////////////////////////////////////////////////////////

// for nanox,by tanghao
//const XEvent* fl_xevent; // the current x event
//const

GR_EVENT *fl_xevent;		// the current nanox event
ulong fl_event_time;		// the last timestamp from an x event

//end nanox

char fl_key_vector[32];		// used by Fl::get_key()

// Record event mouse position and state from an XEvent:

static int px, py;
static ulong ptime;

static void set_event_xy() {

#if CONSOLIDATE_MOTION
  send_motion = 0;
#endif
  //for nanox,by tanghao
  //  Fl::e_x_root = fl_xevent->xbutton.x_root;
  //  Fl::e_x = fl_xevent->xbutton.x;
  //  Fl::e_y_root = fl_xevent->xbutton.y_root;
  //  Fl::e_y = fl_xevent->xbutton.y;
  //  Fl::e_state = fl_xevent->xbutton.state << 16;
  //  fl_event_time = fl_xevent->xbutton.time;

  Fl::e_x_root = fl_xevent->button.rootx;
  Fl::e_x = fl_xevent->button.x;
  Fl::e_y_root = fl_xevent->button.rooty;
  Fl::e_y = fl_xevent->button.y;
  ulong state = Fl::e_state & 0xff0000;	// keep shift key states

  //    if(fl_xevent->button.modifiers&GR_MODIFIER_SHIFT)state |= FL_SHIFT;
  //    if(fl_xevent->button.modifiers&GR_MODIFIER_CTRL)state |= FL_CTRL;
  //    if(fl_xevent->button.modifiers&GR_MODIFIER_META)state |= FL_ALT;


  if (fl_xevent->button.buttons & GR_BUTTON_L)
    state |= FL_BUTTON3;
  if (fl_xevent->button.buttons & GR_BUTTON_M)
    state |= FL_BUTTON2;
  if (fl_xevent->button.buttons & GR_BUTTON_R)
    state |= FL_BUTTON1;
  Fl::e_state = state;


  //  fl_event_time = fl_xevent->xbutton.time; maybe not support in nanox
  fl_event_time = 0;

  //end nanox

#ifdef __sgi
  // get the meta key off PC keyboards:
  if (fl_key_vector[18] & 0x18)
    Fl::e_state |= FL_META;
#endif
  // turn off is_click if enough time or mouse movement has passed:
  if (abs (Fl::e_x_root - px) + abs (Fl::e_y_root - py) > 3
      || fl_event_time >= ptime + 1000)
    Fl::e_is_click = 0;

}

// if this is same event as last && is_click, increment click count:
static inline void checkdouble () {
  if (Fl::e_is_click == Fl::e_keysym)
    Fl::e_clicks++;
  else {
    Fl::e_clicks = 0;
    Fl::e_is_click = Fl::e_keysym;
  }
  px = Fl::e_x_root;
  py = Fl::e_y_root;
  ptime = fl_event_time;
}

static Fl_Window *resize_bug_fix;

////////////////////////////////////////////////////////////////

//int fix_exposure = 0;

int fl_handle(const GR_EVENT & xevent) {
  int i;

//  GR_WINDOW_INFO info; //--vovan888
  fl_xevent = (GR_EVENT *) & xevent;
  Window xid = xevent.general.wid;	//fl_window;

  int button = 0;
  int event = 0;
  Fl_Window *window = fl_find (xid);

  /* #$*#&$ - Some events are not tied to a window */

  if (xevent.type == GR_EVENT_TYPE_FDINPUT) {
    int fdnum = xevent.fdinput.fd;

	for(i = 0; i < nfds; i++) {
    	    if (fd[i].fd == fdnum) {
		if (fd[i].cb) fd[i].cb(fdnum, fd[i].arg);
		    break;
    	    }
	}

//    return Fl::handle (event, window);
    return 0;
  }

  if (window)

    switch (xevent.type) {

    case GR_EVENT_TYPE_CLOSE_REQ:
      event = FL_CLOSE;
      Fl::handle(event,window);
      break;

    case GR_EVENT_TYPE_UPDATE:
      update = 1;
      GR_WINDOW_INFO info;
      switch (xevent.update.utype) {

	case GR_UPDATE_MAP:
	//fix_exposure = 1;
		event = FL_SHOW;
		GrGetWindowInfo(xid,&info);
	//printf("GR_UPDATE_MAP wid: %d\t%d, %d, %d, %d\n", xid,xevent.update.x, xevent.update.y, info.width, info.height);
	//	if(!window->parent())
		  window->Fl_Widget::resize(xevent.update.x, xevent.update.y,
					    info.width, info.height);
//	window->resize_notify(xevent.update.x, xevent.update.y, info.width, info.height);
	break;

	case GR_UPDATE_UNMAP:
		event = FL_HIDE;
		break;

	case GR_UPDATE_REPARENT:	//ReparentNotify
		//ReparentNotify gives the new position of the window relative to
		//the new parent. FLTK cares about the position on the root window.
		resize_bug_fix = window;
		window->position(xevent.update.x, xevent.update.y);
		break;

	case GR_UPDATE_SIZE:
	GrGetWindowInfo(xid,&info);
	//printf("GR_UPDATE_SIZE wid: %d\t%d, %d, %d, %d\n", xid, xevent.update.x, xevent.update.y, info.width, info.height);
	//	if(!window->parent())
	  window->resize(xevent.update.x, xevent.update.y, info.width, info.height);
//	window->resize_notify(xevent.update.x, xevent.update.y, info.width, info.height);
	//window->resize_notify(info.x, info.y, xevent.update.width, xevent.update.height);
	break;
      case GR_UPDATE_MOVE:
	GrGetWindowInfo(xid,&info);
	//printf("GR_UPDATE_MOVE wid: %d\t%d, %d, %d, %d\n", xid, info.x, info.y, xevent.update.width, xevent.update.height);
	/*
	if(!window->parent())
	  window->Fl_Widget::resize(xevent.update.x, xevent.update.y,
				    info.width, info.height);
	window->resize_notify(xevent.update.x, xevent.update.y, info.width, info.height);
	*/

	//	if(!window->parent())
	  window->resize(info.x, info.y,
			xevent.update.width, xevent.update.height);
//	window->resize_notify(info.x, info.y, xevent.update.width, xevent.update.height);

	break;
      default:
	break;
      }
      break;

    case GR_EVENT_TYPE_EXPOSURE:
      Fl_X::i (window)->wait_for_expose = 0;
      //if ( !fix_exposure )
	window->damage (FL_DAMAGE_EXPOSE, xevent.exposure.x,
		      xevent.exposure.y, xevent.exposure.width,
		      xevent.exposure.height);
      //fix_exposure = 0;

/*      if (Fl::first_window ()->non_modal ()
	  && window != Fl::first_window ())
	Fl::first_window ()->show ();
*/
      break;

    case GR_EVENT_TYPE_FOCUS_IN:
      event = FL_FOCUS;
      break;

    case GR_EVENT_TYPE_FOCUS_OUT:
      event = FL_UNFOCUS;
      break;

    case GR_EVENT_TYPE_BUTTON_UP:	//tanghao
      if (xevent.button.changebuttons & 0x04) {
	button = FL_Button + 0x01;
      } else if (xevent.button.changebuttons & 0x02)
	button = FL_Button + 0x02;
      else
	button = FL_Button + 0x03;

      Fl::e_keysym = button;	//tanghao have problem +
      set_event_xy ();
      Fl::e_state &= ~(0x01 << (xevent.button.buttons - 1));	//tanghao have problem
      event = FL_RELEASE;
      break;

    case GR_EVENT_TYPE_BUTTON_DOWN:	//tanghao
      if (xevent.button.changebuttons & 0x04) {
	button = FL_Button + 0x01;
      } else if (xevent.button.changebuttons & 0x02) {
	button = FL_Button + 0x02;
      } else {
	button = FL_Button + 0x03;
      }

      Fl::e_keysym = button;
      set_event_xy ();
      checkdouble ();
      Fl::e_state |= (0x01 << (xevent.button.buttons - 1));
      event = FL_PUSH;
      break;

	case GR_EVENT_TYPE_MOUSE_MOTION:
		//TODO ???
	break;

    case GR_EVENT_TYPE_MOUSE_POSITION:	//tanghao
      fl_window = xevent.mouse.wid;
      set_event_xy ();
      in_a_window = true;

#if CONSOLIDATE_MOTION
      send_motion = fl_xmousewin = window;
      return 0;
#else
      event = FL_MOVE;
      fl_xmousewin = window;
      break;
#endif

    case GR_EVENT_TYPE_KEY_UP:
      {
	int keycode = xevent.keystroke.ch;
	fl_key_vector[keycode / 8] &= ~(1 << (keycode % 8));
	set_event_xy ();
	break;
      }

    case GR_EVENT_TYPE_KEY_DOWN:
      {
	unsigned short keycode = xevent.keystroke.ch;
	static char buffer[21];
	int len = 0;

	buffer[len++] = keycode;
	buffer[len] = 0;

	/* Modifiers, passed from Nano-X */

	Fl::e_state = 0;

	 if ( (keycode == MWKEY_LCTRL) || (keycode == MWKEY_RCTRL) )
	   break;
	 else if ( (keycode == MWKEY_LALT) || (keycode == MWKEY_RALT ) )
	   break;
	 else if (keycode == MWKEY_LSHIFT || (keycode == MWKEY_RSHIFT) )
	   break;

	 if (xevent.keystroke.modifiers & MWKMOD_CTRL)
	   Fl::e_state |= FL_CTRL;
	 if (xevent.keystroke.modifiers & MWKMOD_SHIFT)
	   Fl::e_state |= FL_SHIFT;
	 if (xevent.keystroke.modifiers & MWKMOD_CAPS)
	   Fl::e_state |= FL_CAPS_LOCK;
	 if (xevent.keystroke.modifiers & MWKMOD_NUM)
	   Fl::e_state |= FL_NUM_LOCK;
	 if (xevent.keystroke.modifiers & (MWKMOD_ALT|MWKMOD_META))
	   Fl::e_state |= FL_META;

	 /* This goes through the new keymap_array, and
	   handles those keys that are defined.  Otherwise,
	   we just drop out and set the keysem to the raw value */

	int i = 0;

	while (keymap_array[i].value) {
	  if (keycode == keymap_array[i].key) {
	    Fl::e_keysym = keymap_array[i].value;
	    break;
	  }

	  i++;
	}

	if (keymap_array[i].value == 0) {
	  //bug fix 01/04/2010 keep mask for short value
	  Fl::e_keysym = (keycode & 0xFFFF);
	  //Fl::e_keysym = (keycode & 0x00FF);
	}
#ifdef OLDOLDOLD

	if (keycode == '\r')
	  Fl::e_keysym = (int) 65293;	//tanghao Enter
	else if (keycode == '\b')
	  Fl::e_keysym = (int) 65288;	//tanghao backspace
	else if (keycode == 82)
	  Fl::e_keysym = (int) 65362;	//tanghao up
	else if (keycode == 84)
	  Fl::e_keysym = (int) 65364;	//tanghao down
	else if (keycode == 81)
	  Fl::e_keysym = (int) 65361;	//tanghao left
	else if (keycode == 83)
	  Fl::e_keysym = (int) 65363;	//tanghao right
	else if (keycode == 227)
	  Fl::e_keysym = (int) FL_Control_L;	// left ctrl
	else if (keycode == 225)
	  Fl::e_keysym = (int) FL_Alt_L;	// left alt
	else if (keycode == 233)
	  Fl::e_keysym = (int) FL_Shift_L;	// left shift
	else
	  Fl::e_keysym = (int) (keycode & 0x00FF);	//tanghao
#endif

	Fl::e_text = buffer;
	Fl::e_length = len;

	Fl::e_is_click = 0;

	event = FL_KEYBOARD;
	break;
      }
    case GR_EVENT_TYPE_MOUSE_ENTER:
      set_event_xy ();
      //    Fl::e_state = xevent.xcrossing.state << 16;
      event = FL_ENTER;
      fl_xmousewin = window;
      in_a_window = true;
      break;

    case GR_EVENT_TYPE_MOUSE_EXIT:
      set_event_xy ();
      //    Fl::e_state = xevent.xcrossing.state << 16;
      event = FL_LEAVE;
      fl_xmousewin = 0;
      in_a_window = false; // make do_queued_events produce FL_LEAVE event
      break;


    }

  return Fl::handle (event, window);

}

////////////////////////////////////////////////////////////////

void
Fl_Window::resize (int X, int Y, int W, int H)
{
  int is_a_move = (X != x() || Y != y());
  int is_a_resize = (W != w() || H != h());
  int resize_from_program = (this != resize_bug_fix);
  if (!resize_from_program) resize_bug_fix = 0;
  if (is_a_move && resize_from_program) set_flag(FL_FORCE_POSITION);
  else if (!is_a_resize && !is_a_move) return;
  if (is_a_resize) {
    Fl_Group::resize(X,Y,W,H);
    if (shown()) {redraw(); i->wait_for_expose = 1;}
  } else {
    x(X); y(Y);
  }

  if (resize_from_program && is_a_resize && !resizable()) {
    size_range(w(), h(), w(), h());
  }

  if (resize_from_program && shown()) {
    if (is_a_resize) {
      if (!resizable()) size_range(w(),h(),w(),h());
      if (is_a_move) {
	GrMoveWindow (i->xid, X, Y);
	GrResizeWindow (i->xid, W > 0 ? W : 1, H > 0 ? H : 1);
      } else {
	GrResizeWindow(i->xid, W>0 ? W : 1, H>0 ? H : 1);
      }
    } else
      GrMoveWindow(i->xid, X, Y);
  }
}

////////////////////////////////////////////////////////////////

// A subclass of Fl_Window may call this to associate an X window it
// creates with the Fl_Window:

int   Fl_X::mw_parent = 1;
int   Fl_X::mw_parent_xid = 0;
int   Fl_X::mw_parent_top = 0;

void fl_fix_focus(); // in Fl.cxx

Fl_X* Fl_X::set_xid(Fl_Window* win, Window winxid) {
  Fl_X* xp = new Fl_X;
  xp->xid = winxid;
  xp->other_xid = 0;
  xp->setwindow(win);
  xp->next = Fl_X::first;
  xp->region = 0;
  xp->wait_for_expose = 1;
  xp->backbuffer_bad = 1;
  Fl_X::first = xp;
  if (win->modal()) {Fl::modal_ = win; fl_fix_focus();}
  return xp;
}

// More commonly a subclass calls this, because it hides the really
// ugly parts of X and sets all the stuff for a window that is set
// normally.  The global variables like fl_show_iconic are so that
// subclasses of *that* class may change the behavior...

char fl_show_iconic;		// hack for iconize()
int fl_background_pixel = -1;	// hack to speed up bg box drawing
int fl_disable_transient_for;	// secret method of removing TRANSIENT_FOR

//tanghao static const int childEventMask = ExposureMask;

static const int childEventMask = GR_EVENT_MASK_EXPOSURE;	//tanghao
/*				GR_EVENT_MASK_KEY_DOWN |
                      GR_EVENT_MASK_KEY_UP | GR_EVENT_MASK_TIMEOUT |
                      GR_EVENT_MASK_FOCUS_IN | GR_EVENT_MASK_FOCUS_OUT |
                      GR_EVENT_MASK_EXPOSURE | GR_EVENT_MASK_CLOSE_REQ |
                      GR_EVENT_MASK_UPDATE | GR_EVENT_MASK_FDINPUT*/

static const int XEventMask = GR_EVENT_MASK_ALL;	//tanghao

void Fl_X::make_xid (Fl_Window * w, XVisualInfo * visual, Colormap colormap)
{

  Fl_Group::current (0);	// get rid of very common user bug: forgot end()

  int X = w->x();
  int Y = w->y();
  int W = w->w();
  if (W <= 0) W = 1; // X don't like zero...
  int H = w->h();
  if (H <= 0) H = 1; // X don't like zero...

//  if (!mw_parent && !Fl::grab ()) {
  if (!w->parent() && !Fl::grab()) {
    // center windows in case window manager does not do anything:
#ifdef FL_CENTER_WINDOWS
    if (!(w->flags() & Fl_Window::FL_FORCE_POSITION)) {
      w->x(X = scr_x+(scr_w-W)/2);
      w->y(Y = scr_y+(scr_h-H)/2);
    }
#endif // FL_CENTER_WINDOWS

//  if ( !mw_parent && Fl::grab() )
//  root = mw_parent_xid;
//  else

    // force the window to be on-screen.  Usually the X window manager
    // does this, but a few don't, so we do it here for consistency:
    int scr_x, scr_y, scr_w, scr_h;
    Fl::screen_xywh(scr_x, scr_y, scr_w, scr_h, X, Y);

    if (w->border ()) {
      // ensure border is on screen:
      // (assumme extremely minimal dimensions for this border)
      const int top = 20;
      const int left = 1;
      const int right = 1;
      const int bottom = 1;
      if (X+W+right > scr_x+scr_w) X = scr_x+scr_w-right-W;
      if (X-left < scr_x) X = scr_x+left;
      if (Y+H+bottom > scr_y+scr_h) Y = scr_y+scr_h-bottom-H;
      if (Y-top < scr_y) Y = scr_y+top;
    }
    // now insure contents are on-screen (more important than border):
    if (X+W > scr_x+scr_w) X = scr_x+scr_w-W;
    if (X < scr_x) X = scr_x;
    if (Y+H > scr_y+scr_h) Y = scr_y+scr_h-H;
    if (Y < scr_y) Y = scr_y;
  }

  // if the window is a subwindow and our parent is not mapped yet, we
  // mark this window visible, so that mapping the parent at a later
  // point in time will call this function again to finally map the subwindow.
  if (w->parent() && !Fl_X::i(w->window())) {
    w->set_visible();
    return;
  }
  // root = either current window id or the MicroWindows root window id.
  ulong root = w->parent () ?
    fl_xid (w->window ()) : GR_ROOT_WINDOW_ID;

  GR_COLOR	bg = WHITE;
  GR_COLOR	fg = BLACK;
  GR_WM_PROPERTIES props; /* window manager properties */

  // set the default flag to WM props
  props.flags = GR_WM_FLAGS_PROPS | GR_WM_FLAGS_TITLE;
  props.props = GR_WM_PROPS_APPWINDOW;

  /* Set window title */
  GR_CHAR *title = (GR_CHAR *) w->label ();
  props.title = title;

  int override_redirect = 0;

  props.props = w->parent() ? GR_WM_PROPS_NODECORATE : GR_WM_PROPS_APPWINDOW; /* Set the default */

/*  attr.colormap = colormap;	// nano-X dont need colormap
  attr.border_pixel = 0;
  attr.bit_gravity = 0; // StaticGravity;
*/

  if (w->override()) {
    override_redirect = 1;
/*    attr.save_under = 1;
    // for InputOutput or InputOnly windows
    mask |= CWOverrideRedirect | CWSaveUnder;*/
    props.props = GR_WM_PROPS_NODECORATE;
  } else override_redirect = 0;
  if (Fl::grab()) {
/*    attr.save_under = 1; mask |= CWSaveUnder;*/ /* for InputOutput windows */
    if (!w->border()) {override_redirect = 1; props.props = GR_WM_PROPS_NODECORATE;}
  }
  if (fl_background_pixel >= 0) {
    bg = fl_background_pixel;
    fl_background_pixel = -1;
  }

    GR_WINDOW_ID wid;

    wid = GrNewWindow (root, X, Y, W, H, 0 /* borderwidth */, bg, fg);
//    printf("%d = GrNewWindow(%ld)\n", wid, root);

    // Start up a MicrowWindow's select events as each window is created.
    // This is related with the fl_wait() function above.
    GrSelectEvents(wid, w->parent() ? childEventMask : XEventMask );

    Fl_X *x = set_xid (w, wid);

    if (mw_parent_top == 0) { /*TODO do we need this ?*/
      mw_parent_xid = wid;
      mw_parent_top = 1;
    }

    int showit = 1;

  if (!w->parent() && !override_redirect) {
    // Communicate all kinds 'o junk to the X Window Manager:

    w->label(w->label(), w->iconlabel());

    // send size limits and border:
    x->sendxjunk();

    // set the class property, which controls the icon used:
    if (w->xclass()) {
    /*TODO ?*/
    }

    if (w->non_modal() && x->next && !fl_disable_transient_for) {
     /*TODO ?*/
      // find some other window to be "transient for":
      Fl_Window* wp = x->next->w;
      while (wp->parent()) wp = wp->window();
      /*Set by application programs to indicate to the window manager
         that a transient top-level window, such as a dialog box.*/
      /*XSetTransientForHint(fl_display, xp->xid, fl_xid(wp));*/
      if (!wp->visible()) showit = 0; // guess that wm will not show it
    }

    // Make sure that borderless windows do not show in the task bar
    if (!w->border()) { /*TODO ?*/
/*      Atom net_wm_state = XInternAtom (fl_display, "_NET_WM_STATE", 0);
      Atom net_wm_state_skip_taskbar = XInternAtom (fl_display, "_NET_WM_STATE_SKIP_TASKBAR", 0);
      XChangeProperty (fl_display, xp->xid, net_wm_state, XA_ATOM, 32,
          PropModeAppend, (unsigned char*) &net_wm_state_skip_taskbar, 1);*/
    }

    // Make it receptive to DnD:
    /*TODO*/
  }

/*    if (!mw_parent && Fl::grab ()) {
      mw_parent = 1;
      props.props = GR_WM_PROPS_NODECORATE;
    } else {
      mw_parent = 1;
    }
*/
//    if(!w->border ()) // ++vovan888 - make child windows (like Popup Menu) not redecoratable
//	props.props = GR_WM_PROPS_NODECORATE;

    props.props |= w->wm_props;

//  GR_WM_PROPS flags;            /**< Which properties valid in struct for set*/
//  GR_WM_PROPS props;            /**< Window property bits*/
//  GR_CHAR *title;               /**< Window title*/
//  GR_COLOR background;          /**< Window background color*/
//  GR_SIZE bordersize;           /**< Window border size*/
//  GR_COLOR bordercolor;         /**< Window border color*/

    GrSetWMProperties (wid, &props);

    GrMapWindow (wid);
//    fl_window = wid;		//tanghao

    if (showit) {
	    w->set_visible();
	    int old_event = Fl::e_number;
	    w->handle(Fl::e_number = FL_SHOW); // get child windows to appear
	    Fl::e_number = old_event;
	    w->redraw();
    }

}

////////////////////////////////////////////////////////////////
// Send X window stuff that can be changed over time:

void Fl_X::sendxjunk () {
  if (w->parent() || w->override()) return; // it's not a window manager window!

  if (!w->size_range_set) {	// default size_range based on resizable():
    if (w->resizable ()) {
      Fl_Widget *o = w->resizable ();
      int minw = o->w ();
      if (minw > 100)
	minw = 100;
      int minh = o->h ();
      if (minh > 100)
	minh = 100;
      w->size_range (w->w () - o->w () + minw, w->h () - o->h () + minh,
		     0, 0);
    } else {
      w->size_range (w->w (), w->h (), w->w (), w->h ());
    }
    return;			// because this recursively called here
  }
}

void Fl_Window::size_range_ () {
  size_range_set = 1;
  if (shown()) i->sendxjunk ();
}

////////////////////////////////////////////////////////////////

// returns pointer to the filename, or null if name ends with '/'
const char * fl_filename_name (const char *name)
{
  const char *p, *q;
  if (!name) return (0);
  for (p = q = name; *p;) if (*p++ == '/') q = p;
  return q;
}

void Fl_Window::label (const char *name, const char *iname) {
  Fl_Widget::label (name);
  iconlabel_ = iname;
  if (shown () && !parent ()) {
    if (!name) name = "";
    /* Set window title */
    GrSetWindowTitle(i->xid, name);

//tanghao    XChangeProperty(fl_display, i->xid, XA_WM_NAME,
//tanghao                   XA_STRING, 8, 0, (uchar*)name, strlen(name));
    if (!iname)
      iname = fl_filename_name (name);
//tanghao    XChangeProperty(fl_display, i->xid, XA_WM_ICON_NAME,
//tanghao                   XA_STRING, 8, 0, (uchar*)iname, strlen(iname));

  }
}

////////////////////////////////////////////////////////////////
// Implement the virtual functions for the base Fl_Window class:

// If the box is a filled rectangle, we can make the redisplay *look*
// faster by using X's background pixel erasing.  We can make it
// actually *be* faster by drawing the frame only, this is done by
// setting fl_boxcheat, which is seen by code in fl_drawbox.cxx:
//
// On XFree86 (and prehaps all X's) this has a problem if the window
// is resized while a save-behind window is atop it.  The previous
// contents are restored to the area, but this assummes the area
// is cleared to background color.  So this is disabled in this version.
// Fl_Window *fl_boxcheat;
static inline int can_boxcheat (uchar b) {return (b==1 || (b&2) && b<=15);}

void Fl_Window::show() {
  image(Fl::scheme_bg_);
  if (Fl::scheme_bg_) {
    labeltype(FL_NORMAL_LABEL);
    align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
  } else {
    labeltype(FL_NO_LABEL);
  }
  Fl_Tooltip::exit(this);
  if (!shown()) {
    fl_open_display ();
    // Don't set background pixel for double-buffered windows...
    if (type() == FL_WINDOW && can_boxcheat(box())) {
//    if (can_boxcheat(box())) {
      fl_background_pixel = int (fl_xpixel (color ()));
    }
    Fl_X::make_xid (this);
  } else {
     //tanghao   XMapRaised(fl_display, i->xid);
     GrMapWindow(i->xid);
     GrRaiseWindow (i->xid);
  }
}

Window fl_window;
//Gr_Window
Fl_Window *Fl_Window::current_;
GC fl_gc;

// make X drawing go into this window (called by subclass flush() impl.)
void Fl_Window::make_current () {
  static GC gc;		// the GC used by all X windows
  if (!gc) gc = GrNewGC ();

  fl_window = i->xid;
  fl_gc = gc;

  current_ = this;
  fl_clip_region (0);
}


void fl_reset_spot()
{
}

void fl_set_spot(int font, int size, int x, int y, int w, int h)
{
}
