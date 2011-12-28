//
// "$Id: fl_font_nanox.cxx,v 1.3 2010-03-31 15:06:10 jjp Exp $"
//
// Standard X11 font selection code for the Fast Light Tool Kit (FLTK).
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

#include <config.h>

/* Define a class here for the font cache */
#define WIDTH_CACHE_SIZE 20

typedef struct
{
    GR_FONT_ID fontid;
    GR_FONT_INFO fi;
    int age;
    unsigned char widths[256];
}
width_cache_struct;

class font_cache
{
  private:
    width_cache_struct * cache[WIDTH_CACHE_SIZE];

  public:
    width_cache_struct * check_cache(GR_FONT_ID curfont)
    {
	/* For the moment, we index the cache with the font ID */
	int n;

	for (n = 0; n < WIDTH_CACHE_SIZE; n++)
	{
	    if (cache[n]) {
		if (cache[n]->fontid == curfont) {
		    cache[n]->age = 0;
		    return (cache[n]);
		}

		cache[n]->age++;
	    }
	}

	return ((width_cache_struct *) 0);
    }

    void add_cache(GR_FONT_ID curfont, unsigned char *widths,
		   GR_FONT_INFO * fi)
    {
	int n;
	int oldest = -1;


	for (n = 0; n < WIDTH_CACHE_SIZE; n++) {
	    if (!cache[n]) {
		cache[n] =
		    (width_cache_struct *) malloc(sizeof(width_cache_struct));

		if (!cache[n])
		    return;

		cache[n]->fontid = curfont;
		cache[n]->age = 0;

		bcopy(widths, cache[n]->widths, 256);
		memcpy(&cache[n]->fi, fi, sizeof(GR_FONT_INFO));
		return;
	    } else {
		if (oldest != -1) {
		    if (cache[n]->age > cache[oldest]->age)
			oldest = n;
		} else
		    oldest = n;
	    }
	}

	if (oldest == -1)
	    return;

	bzero(cache[oldest], sizeof(width_cache_struct));

	cache[oldest]->fontid = curfont;
	cache[oldest]->age = 0;

	bcopy(widths, cache[oldest]->widths, 256);

	return;
    }

    font_cache() {
	int n;

	for (n = 0; n < WIDTH_CACHE_SIZE; n++)
	    cache[n] = (width_cache_struct *) 0;
    }

    ~font_cache() {
	int n;

	for (n = 0; n < WIDTH_CACHE_SIZE; n++)
	    if (cache[n])
		free(cache[n]);
    }
};

font_cache width_cache;


Fl_FontSize::Fl_FontSize(const char* name, int size) {

    isTTF = false;
    next = 0;			// it can't be in a list yet

    font = GrCreateFont((GR_CHAR *) name, size, NULL);

    // need to determine whether the font is TrueType
    // GrGetFontInfo() needs a way to indicate that
    // so this is a bit of a hack; are all TTF fonts in .ttf files?
    if (font) {
	if (strstr(name, ".ttf")) {
	    minsize = size;
	    maxsize = size;	// TTF fonts always scale
	    isTTF = true;
	} else {
	    minsize = size;
	    maxsize = size;	// not true type always scale
	}
    } else {
	Fl::warning("bad font: %s, using a builtin", name);
//      font = GrCreateFont("HZKFONT", 16, NULL);
	font = GrCreateFont((GR_CHAR *) "fonts/simfang.ttf", 16, NULL);
	if (!font)
	    font = GrCreateFont((GR_CHAR *) GR_FONT_GUI_VAR, 0, NULL);
    }

#  if HAVE_GL
  listbase = 0;
#  endif
}

Fl_FontSize* fl_fontsize;

// attempts to resize the font in microwindows
void
Fl_FontSize::Resize(int size)
{
    GrSetFontSize(font, size);
}


Fl_FontSize::~Fl_FontSize() {
#  if HAVE_GL
// Delete list created by gl_draw().  This is not done by this code
// as it will link in GL unnecessarily.  There should be some kind
// of "free" routine pointer, or a subclass?
// if (listbase) {
//  int base = font->min_char_or_byte2;
//  int size = font->max_char_or_byte2-base+1;
//  int base = 0; int size = 256;
//  glDeleteLists(listbase+base,size);
// }
#  endif
  if (this == fl_fontsize) fl_fontsize = 0;
  if (font)
	GrDestroyFont(font);
}

////////////////////////////////////////////////////////////////

/* JHC - Major hack! */
//#if defined(CONFIG_PDA) || defined (CONFIG_PHONE)
#if 1
static Fl_Fontdesc built_in_table[] = {
    {GR_FONT_SYSTEM_VAR},
    {GR_FONT_GUI_VAR},
    {GR_FONT_SYSTEM_VAR},
    {GR_FONT_GUI_VAR},
    {GR_FONT_SYSTEM_FIXED},
    {GR_FONT_SYSTEM_FIXED},//5
    {GR_FONT_SYSTEM_FIXED},
    {GR_FONT_SYSTEM_FIXED},
    {GR_FONT_SYSTEM_VAR},
    {GR_FONT_GUI_VAR},
    {GR_FONT_SYSTEM_VAR},//10
    {GR_FONT_GUI_VAR},
    {GR_FONT_OEM_FIXED},
    {GR_FONT_OEM_FIXED},
    {GR_FONT_OEM_FIXED},
    {GR_FONT_OEM_FIXED}, //bug fix add this for FL_ZAPF_DINGBATS see Enumerations.H
#ifdef MWFONT_FIXED_MEDIUM_ENABLE
    {GR_FONT_FIXED_MEDIUM},
#endif
#ifdef MWFONT_FIXED_MEDIUM_JA_ENABLE
    {GR_FONT_FIXED_MEDIUM_JA},
#endif
#ifdef MWFONT_METRIX_SYMBOLS_ENABLE
	{GR_FONT_METRIX_SYMBOLS},
#endif
#ifdef MWFONT_VERA_ENABLE
	{GR_FONT_VERA},
#endif
#ifdef MWFONT_VERA_BD_ENABLE
	{GR_FONT_VERA_BD},
#endif
#ifdef MWFONT_ARIAL_UNICODE_ENABLE
	{GR_FONT_ARIAL_UNICODE},
#endif
};
#else
// WARNING: if you add to this table, you must redefine FL_FREE_FONT
// in Enumerations.H & recompile!!
static Fl_Fontdesc built_in_table[] = {

    {"arial.ttf"},
    {"arialb.ttf"},
    {"ariali.ttf"},
    {"arialz.ttf"},
    {"cour.ttf"},
    {"courb.ttf"},
    {"couri.ttf"},
    {"courz.ttf"},
    {"times.ttf"},
    {"timesb.ttf"},
    {"timesi.ttf"},
    {"timesz.ttf"},
    {"impact.ttf"},
    {"comic.ttf"},
    {"comicbd.ttf"},
    {"Terminal"}

};
#endif /* PDA */

Fl_Fontdesc* fl_fonts = built_in_table;

#define MAXSIZE 32767

// return dash number N, or pointer to ending null if none:
const char* fl_font_word(const char* p, int n) {
  while (*p) {if (*p=='-') {if (!--n) break;} p++;}
  return p;
}

// return a pointer to a number we think is "point size":
char* fl_find_fontsize(char* name) {
  char* c = name;
  // for standard x font names, try after 7th dash:
  if (*c == '-') {
    c = (char*)fl_font_word(c,7);
    if (*c++ && isdigit(*c)) return c;
    return 0; // malformed x font name?
  }
  char* r = 0;
  // find last set of digits:
  for (c++;* c; c++)
    if (isdigit(*c) && !isdigit(*(c-1))) r = c;
  return r;
}

const char* fl_encoding = "iso8859-1";

// return true if this matches fl_encoding:
int fl_correct_encoding(const char* name) {
  if (*name != '-') return 0;
  const char* c = fl_font_word(name,13);
  return (*c++ && !strcmp(c,fl_encoding));
}

// font choosers - if the fonts change in built_in_table,
// these and the enum in Fl/Enumerations.H will have to change

inline bool
IsBold(int fnum)
{
    switch (fnum) {
    case FL_HELVETICA_BOLD:
    case FL_HELVETICA_BOLD_ITALIC:
    case FL_COURIER_BOLD:
    case FL_COURIER_BOLD_ITALIC:
	return true;
    default:
	return false;
    }
}
inline bool
IsItalic(int fnum)
{
    switch (fnum) {
    case FL_HELVETICA_ITALIC:
    case FL_HELVETICA_BOLD_ITALIC:
    case FL_COURIER_ITALIC:
    case FL_COURIER_BOLD_ITALIC:
	return true;
    default:
	return false;
    }
}

// locate or create an Fl_FontSize for a given Fl_Fontdesc and size:
static Fl_FontSize * find(int fnum, int size)
{
    Fl_Fontdesc *s = fl_fonts + fnum;
    if (!s->name)
	s = fl_fonts;		// use font 0 if still undefined
    Fl_FontSize *f;
    for (f = s->first; f; f = f->next) {
	if (f->minsize <= size && f->maxsize >= size) {
	    return f;
	}
    }

    fl_open_display();

    // time to create one
    if (!s->first) {
	s->first = new Fl_FontSize(s->name, size);
	return s->first;
    }


    for (f = s->first; f->next; f = f->next);
    f->next = new Fl_FontSize(s->name, size);



    return f;

}

////////////////////////////////////////////////////////////////
// Public interface:

int fl_font_ = 0;
int fl_size_ = 0;
GR_FONT_ID fl_xfont = 0;
static GC font_gc;

void fl_font(int fnum, int size) {
  if (fnum == fl_font_ && size == fl_size_) return;
  fl_font_ = fnum; fl_size_ = size;
  Fl_FontSize* f = find(fnum, size);
  if (f != fl_fontsize) {
    fl_fontsize = f;
    fl_xfont = f->font;
    font_gc = 0;
  }
}

int fl_height() {
  if (!fl_xfont)
	return -1;
    GR_FONT_INFO fi;
    width_cache_struct *wc = width_cache.check_cache(fl_xfont);

    if (!wc) {
	GrGetFontInfo(fl_xfont, &fi);
	width_cache.add_cache(fl_xfont, fi.widths, &fi);
	return fi.height;
    } else {
	return wc->fi.height;
    }
}

int fl_descent() {
  if (!fl_xfont)
	return -1;
    GR_FONT_INFO fi;
    width_cache_struct *wc = width_cache.check_cache(fl_xfont);

    if (!wc) {
	GrGetFontInfo(fl_xfont, &fi);
	width_cache.add_cache(fl_xfont, fi.widths, &fi);
	return fi.height - fi.baseline;
    } else {
	return wc->fi.height - wc->fi.baseline;
    }
}

double fl_width(const char* c, int n) {
    double w = 0;
    GR_FONT_INFO fi;
    width_cache_struct *wc = width_cache.check_cache(fl_xfont);
    unsigned char *fwidths;
    GR_FONT *font;
    MWCOORD pwidth=0;
    MWCOORD pheight=0;
    MWCOORD pbase=0;

    font=GsFindFont(fl_xfont);
    if(font)
    {
    	GdGetTextSize(font->pfont, (void*)c, n, &pwidth,&pheight, &pbase, GR_TFUTF8);
    }
    w=(double)pwidth;

    return w;
}

double fl_width(uchar c) {
    GR_FONT_INFO fi;
    width_cache_struct *wc = width_cache.check_cache(fl_xfont);
    unsigned char *fwidths;

    if (!wc) {
	GrGetFontInfo(fl_xfont, &fi);
	width_cache.add_cache(fl_xfont, fi.widths, &fi);

	fwidths = fi.widths;
    } else {
	fwidths = wc->widths;
    }

    return fwidths[c];
}

double fl_width(unsigned int c) {
	int width, height, base;
	if (fl_xfont) {
		GrGetGCTextSize(fl_gc, &c, 1, GR_TFUC16, &width, &height, &base);
		return (double)width;
	} else
		return -1;
}

void fl_draw(const char* str, int n, int x, int y) {
  if (font_gc != fl_gc) {
	  if (!fl_xfont) fl_font(FL_HELVETICA, FL_NORMAL_SIZE);
    font_gc = fl_gc;
//    XSetFont(fl_display, fl_gc, fl_xfont->fid);
    GrSetGCFont(fl_gc, fl_xfont);//++vovan888
  }
    //FIXME hack because nano-X will not draw a true type font
    // correctly without first setting its background!!!
    GR_GC_INFO info;

    GrGetGCInfo(fl_gc, &info);
    if (info.background == MWRGB(255, 255, 255))
	GrSetGCBackground(fl_gc, MWRGB(255, 0, 0));
    else
	GrSetGCBackground(fl_gc, MWRGB(255, 255, 255));
    GrSetGCUseBackground(fl_gc, GR_FALSE);
    /** \todo check flags */
    //tauon fix
    //forcer le flag GR_DBCSMASK car GR_TFUTF8 est force en encodage utf16
    GrText(fl_window, fl_gc, x, y, (GR_CHAR *) str, n, GR_TFUTF8 /*| GR_TFANTIALIAS*/ );
//  XDrawString(fl_display, fl_window, fl_gc, x, y, str, n);
}

void fl_draw(const char* str, int n, float x, float y) {
  fl_draw(str, n, (int)x, (int)y);
}

//
// End of "$Id: fl_font_nanox.cxx,v 1.3 2010-03-31 15:06:10 jjp Exp $".
//
