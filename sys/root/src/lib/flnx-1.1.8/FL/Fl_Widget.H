//
// "$Id: Fl_Widget.H,v 1.1 2009-07-15 13:23:35 jjp Exp $"
//
// Widget header file for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2005 by Bill Spitzak and others.
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

#ifndef Fl_Widget_H
#define Fl_Widget_H

#include "Enumerations.H"

class Fl_Widget;
class Fl_Window;
class Fl_Group;
class Fl_Image;

typedef void (Fl_Callback )(Fl_Widget*, void*);
typedef Fl_Callback* Fl_Callback_p; // needed for BORLAND
typedef void (Fl_Callback0)(Fl_Widget*);
typedef void (Fl_Callback1)(Fl_Widget*, long);

struct FL_EXPORT Fl_Label {
  /** label text */
  const char* value;
  /** optional image for an active label */
  Fl_Image* image;
  /** optional image for a deactivated label */
  Fl_Image* deimage;
  uchar type;
  fntidx font;
  uchar size;
  unsigned color;
  void draw(int,int,int,int, Fl_Align) const ;
  void measure(int&, int&) const ;
};

class FL_EXPORT Fl_Widget {
  friend class Fl_Group;

  Fl_Group* parent_;
  Fl_Callback* callback_;
  void* user_data_;
  int x_,y_,w_,h_;
  Fl_Label label_;
  int flags_;
  unsigned color_;
  unsigned color2_;
  uchar type_;
  uchar damage_;
  uchar box_;
  Fl_Align align_;
  uchar when_;

  const char *tooltip_;

  // unimplemented copy ctor and assignment operator
  Fl_Widget(const Fl_Widget &);
  Fl_Widget& operator=(const Fl_Widget &);

protected:

  Fl_Widget(int,int,int,int,const char* =0);

  void x(int v) {x_ = v;}
  void y(int v) {y_ = v;}
  void w(int v) {w_ = v;}
  void h(int v) {h_ = v;}

  int flags() const {return flags_;}
  void set_flag(int c) {flags_ |= c;}
  void clear_flag(int c) {flags_ &= ~c;}
  enum {INACTIVE=1, INVISIBLE=2, OUTPUT=4, SHORTCUT_LABEL=64,
        CHANGED=128, VISIBLE_FOCUS=512, COPIED_LABEL = 1024};

  void draw_box() const;
  void draw_box(Fl_Boxtype, Fl_Color) const;
  void draw_box(Fl_Boxtype, int,int,int,int, Fl_Color) const;
  virtual void draw_focus() {draw_focus(box(),x(),y(),w(),h());}
  virtual void draw_focus(Fl_Boxtype, int,int,int,int) const;
  void draw_label() const;
  void draw_label(int, int, int, int) const;

public:

  virtual ~Fl_Widget();

  virtual void draw() = 0;
  virtual int handle(int);
  Fl_Group* parent() const {return parent_;}
  void parent(Fl_Group* p) {parent_ = p;} // for hacks only, Fl_Group::add()

  uchar type() const {return type_;}
  void type(uchar t) {type_ = t;}

  int x() const {return x_;}
  int y() const {return y_;}
  int w() const {return w_;}
  int h() const {return h_;}
  virtual void resize(int,int,int,int);
  int damage_resize(int,int,int,int);
  void position(int X,int Y) {resize(X,Y,w_,h_);}
  void size(int W,int H) {resize(x_,y_,W,H);}

  Fl_Align align() const {return (Fl_Align)align_;}
  void align(Fl_Align a) {align_ = a;}
  Fl_Boxtype box() const {return (Fl_Boxtype)box_;}
  void box(Fl_Boxtype a) {box_ = a;}
  Fl_Color color() const {return (Fl_Color)color_;}
  void color(unsigned a) {color_ = a;}
  Fl_Color selection_color() const {return (Fl_Color)color2_;}
  void selection_color(unsigned a) {color2_ = a;}
  void color(unsigned a, unsigned b) {color_=a; color2_=b;}
  const char* label() const {return label_.value;}
  void label(const char* a);
  void copy_label(const char* a);
  void label(Fl_Labeltype a,const char* b) {label_.type = a; label_.value = b;}
  Fl_Labeltype labeltype() const {return (Fl_Labeltype)label_.type;}
  void labeltype(Fl_Labeltype a) {label_.type = a;}
  Fl_Color labelcolor() const {return (Fl_Color)label_.color;}
  void labelcolor(unsigned a) {label_.color=a;}
  Fl_Font labelfont() const {return (Fl_Font)label_.font;}
  void labelfont(fntidx a) {label_.font=a;}
//  void labelfont(int a) {label_.font=(fntidx)a;}
  uchar labelsize() const {return label_.size;}
  void labelsize(uchar a) {label_.size=a;}
  Fl_Image* image() {return label_.image;}
  void image(Fl_Image* a) {label_.image=a;}
  void image(Fl_Image& a) {label_.image=&a;}
  Fl_Image* deimage() {return label_.deimage;}
  void deimage(Fl_Image* a) {label_.deimage=a;}
  void deimage(Fl_Image& a) {label_.deimage=&a;}
  const char *tooltip() const {return tooltip_;}
  void tooltip(const char *t);
  Fl_Callback_p callback() const {return callback_;}
  void callback(Fl_Callback* c, void* p) {callback_=c; user_data_=p;}
  void callback(Fl_Callback* c) {callback_=c;}
  void callback(Fl_Callback0*c) {callback_=(Fl_Callback*)c;}
  void callback(Fl_Callback1*c, long p=0) {callback_=(Fl_Callback*)c; user_data_=(void*)p;}
  void* user_data() const {return user_data_;}
  void user_data(void* v) {user_data_ = v;}
  long argument() const {return (long)user_data_;}
  void argument(long v) {user_data_ = (void*)v;}
  Fl_When when() const {return (Fl_When)when_;}
  void when(uchar i) {when_ = i;}

  int visible() const {return !(flags_&INVISIBLE);}
  int visible_r() const;
  void show();
  void hide();
  void set_visible() {flags_ &= ~INVISIBLE;}
  void clear_visible() {flags_ |= INVISIBLE;}
  int active() const {return !(flags_&INACTIVE);}
  int active_r() const;
  void activate();
  void deactivate();
  int output() const {return (flags_&OUTPUT);}
  void set_output() {flags_ |= OUTPUT;}
  void clear_output() {flags_ &= ~OUTPUT;}
  int takesevents() const {return !(flags_&(INACTIVE|INVISIBLE|OUTPUT));}
  int changed() const {return flags_&CHANGED;}
  void set_changed() {flags_ |= CHANGED;}
  void clear_changed() {flags_ &= ~CHANGED;}
  int take_focus();
  void set_visible_focus() { flags_ |= VISIBLE_FOCUS; }
  void clear_visible_focus() { flags_ &= ~VISIBLE_FOCUS; }
  void visible_focus(int v) { if (v) set_visible_focus(); else clear_visible_focus(); }
  int  visible_focus() { return flags_ & VISIBLE_FOCUS; }

  static void default_callback(Fl_Widget*, void*);
  void do_callback() {callback_(this,user_data_); if (callback_ != default_callback) clear_changed();}
  void do_callback(Fl_Widget* o,void* arg=0) {callback_(o,arg); if (callback_ != default_callback) clear_changed();}
  void do_callback(Fl_Widget* o,long arg) {callback_(o,(void*)arg); if (callback_ != default_callback) clear_changed();}

  /** Internal use only. */
  int test_shortcut();
  /** Internal use only. */
  static char label_shortcut(const char *t);
  /** Internal use only. */
  static int test_shortcut(const char*);

  /** Checks if w is a child of this widget.
      \param[in] w potential child widget
      \return Returns 1 if \p w is a child of this widget, or is
      equal to this widget. Returns 0 if \p w is NULL.
   */
  int contains(const Fl_Widget*) const ;

  /** Checks if this widget is a child of w.
      Returns 1 if this widget is a child of \p w, or is
      equal to \p w. Returns 0 if \p w is NULL.
      \param[in] w the possible parent widget.
      \see contains()
   */
  int inside(const Fl_Widget* o) const {return o ? o->contains(this) : 0;}

  /** Schedules the drawing of the widget.
      Marks the widget as needing its draw() routine called.
   */
  void redraw();

  /** Schedules the drawing of the label.
     Marks the widget or the parent as needing a redraw for the label area 
     of a widget.
   */
  void redraw_label();

  /** Returns non-zero if draw() needs to be called. 
      The damage value is actually a bit field that the widget 
      subclass can use to figure out what parts to draw.
      \return a bitmap of flags describing the kind of damage to the widget
      \see damage(uchar), clear_damage(uchar)
   */
  uchar damage() const {return damage_;}

  /** Clears or sets the damage flags.
      Damage flags are cleared when parts of the widget drawing is repaired.

      The optional argument \p c specifies the bits that <b>are set</b>
      after the call (default: 0) and \b not the bits that are cleared!

      \note Therefore it is possible to set damage bits with this method, but
      this should be avoided. Use damage(uchar) instead.
      
      \param[in] c new bitmask of damage flags (default: 0)
      \see damage(uchar), damage()
   */
  void clear_damage(uchar c = 0) {damage_ = c;}

  /** Sets the damage bits for the widget.
      Setting damage bits will schedule the widget for the next redraw.
      \param[in] c bitmask of flags to set
      \see damage(), clear_damage(uchar)
   */
  void damage(uchar c);

  /** Sets the damage bits for an area inside the widget.
      Setting damage bits will schedule the widget for the next redraw.
      \param[in] c bitmask of flags to set
      \param[in] x, y, w, h size of damaged area
      \see damage(), clear_damage(uchar)
   */
  void damage(uchar c,int,int,int,int);
  void draw_label(int, int, int, int, Fl_Align) const;

  /** Sets width ww and height hh accordingly with the label size.
      Labels with images will return w() and h() of the image.
   */
  void measure_label(int& ww, int& hh) {label_.measure(ww, hh);}

  /** Returns a pointer to the primary Fl_Window widget.
      \retval  NULL if no window is associated with this widget.  
      \note for an Fl_Window widget, this returns its <I>parent</I> window 
            (if any), not <I>this</I> window.
   */
  Fl_Window* window() const ;

  
  /** For back compatibility only.
      \deprecated Use selection_color() instead.
  */
  Fl_Color color2() const {return (Fl_Color)color2_;}

  /** For back compatibility only.
      \deprecated Use selection_color(unsigned) instead.
  */
  void color2(unsigned a) {color2_ = a;}
};

/**
    Reserved type numbers (necessary for my cheapo RTTI) start here.
    Grep the header files for "RESERVED_TYPE" to find the next available
    number.
*/
#define FL_RESERVED_TYPE 100

#endif

//
// End of "$Id: Fl_Widget.H,v 1.1 2009-07-15 13:23:35 jjp Exp $".
//
