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

//** BASIC CLOCK EXAMPLE**//
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Clock.H>
#include "kernel/core/time.h"

#define WINDOW_X_SIZE   120
#define WINDOW_Y_SIZE   120

#define CLOCK_X_SIZE    100
#define CLOCK_Y_SIZE    100

char **environ;
extern "C" int tstfltk_main(int argc, char *argv[]);

int tstfltk_main(int argc, char *argv[]) {
   Fl_Double_Window window(0,0,WINDOW_X_SIZE,WINDOW_Y_SIZE,"Fl_Clock");
   Fl_Clock c1(0,0,CLOCK_X_SIZE,CLOCK_Y_SIZE);

   window.resizable(c1);
   window.color(FL_GREEN);
   c1.color(FL_CYAN);
   window.end();
   // my machine had a clock* Xresource set for another program, so
   // I don't want the class to be "clock":
   window.xclass("Fl_Clock");
   Fl::visual(FL_DOUBLE|FL_INDEX);
   window.show(argc,argv);

   return Fl::run();
}
