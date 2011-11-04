//** BASIC CLOCK EXAMPLE**//
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Clock.H>
#include "kernel/core/time.h"

#define	WINDOW_X_SIZE	120
#define	WINDOW_Y_SIZE	120

#define	CLOCK_X_SIZE	100
#define	CLOCK_Y_SIZE	100

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
