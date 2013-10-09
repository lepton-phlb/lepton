//
// "$Id: fl_read_image_nanox.cxx,v 1.2 2010-03-09 10:28:50 jjp Exp $"
//
// Nano-X image reading routines for the Fast Light Tool Kit (FLTK).
// based on X11 fl_read_image
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

#include <FL/x.H>
#include <FL/fl_draw.H>
#include "flstring.h"

#ifdef DEBUG
#  include <stdio.h>
#endif // DEBUG

// Defined in fl_color.cxx
extern uchar fl_redmask, fl_greenmask, fl_bluemask;
extern int fl_redshift, fl_greenshift, fl_blueshift, fl_extrashift;

//
// 'fl_read_image()' - Read an image from the current window.
//

uchar *				// O - Pixel buffer or NULL if failed
fl_read_image(uchar *p,		// I - Pixel buffer or NULL to allocate
              int   X,		// I - Left position
	      int   Y,		// I - Top position
	      int   w,		// I - Width of area to read
	      int   h,		// I - Height of area to read
	      int   alpha)	// I - Alpha value for image (0 for none)
{
	int x, y;
	int d;	// Depth of image
	unsigned char *rgb, *pp;
	GR_PALETTE *palette = NULL;

	GR_SCREEN_INFO sinfo;
	GR_WINDOW_INFO winfo;
	GR_PIXELVAL *pixels;

	GrGetScreenInfo(&sinfo);
	GrGetWindowInfo(d, &winfo);

	d = alpha ? 4 : 3;

	// Allocate the image data array as needed...
	if (!p)
		p = new uchar[w * h * d];

	// Initialize the default colors/alpha in the whole image...
	memset(p, alpha, w * h * d);

	pixels = new GR_PIXELVAL [w * h];
	if(!pixels) {
		return NULL;
	}
	/* read window data in PIXELVAL format */
	GrReadArea(fl_window, X, Y, w, h, pixels);

	if(sinfo.pixtype == MWPF_PALETTE) {
		palette = new GR_PALETTE;
		if(!palette) {
			delete [] pixels;
			return NULL;
		}
		GrGetSystemPalette(palette);
	}

	rgb = p;
	pp = (unsigned char *)pixels;

	for(y = 0; y < h; y++) {
		for(x = 0; x < w; x++) {

			switch(sinfo.pixtype) {
			/* FIXME: These may need modifying on big endian. */
				case MWPF_TRUECOLOR0888:
				case MWPF_TRUECOLOR888:
					rgb[0] = pp[2];
					rgb[1] = pp[1];
					rgb[2] = pp[0];
					break;
				case MWPF_PALETTE:
					rgb[0] = palette->palette[pp[0]].r;
					rgb[1] = palette->palette[pp[0]].g;
					rgb[2] = palette->palette[pp[0]].b;
					break;
				case MWPF_TRUECOLOR565:
					rgb[0] = pp[1] & 0xf8;
					rgb[1] = ((pp[1] & 0x07) << 5) |
						((pp[0] & 0xe0) >> 3);
					rgb[2] = (pp[0] & 0x1f) << 3;
					break;
				case MWPF_TRUECOLOR555:
					rgb[0] = (pp[1] & 0x7c) << 1;
					rgb[1] = ((pp[1] & 0x03) << 6) |
						((pp[0] & 0xe0) >> 2);
					rgb[2] = (pp[0] & 0x1f) << 3;
					break;
				case MWPF_TRUECOLOR332:
					rgb[0] = pp[0] & 0xe0;
					rgb[1] = (pp[0] & 0x1c) << 3;
					rgb[2] = (pp[0] & 0x03) << 6;
					break;
				default:
					fprintf(stderr, "Unsupported pixel "
							"format\n");
					return 0;
			}
			
			rgb += d; /* goto next pixel*/
			pp += sizeof(GR_PIXELVAL);
		}
	}
	
	if(palette) 
		delete palette;

  //tauon fix 25/02/2010 memory leak
  if(pixels)
    delete[] pixels;

  return p;
}

//
// End of "$Id: fl_read_image_nanox.cxx,v 1.2 2010-03-09 10:28:50 jjp Exp $".
//
