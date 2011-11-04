/*
 * Copyright (c) 1999, 2000 Greg Haerr <greg@censoft.com>
 *
 * Microwindows Screen Driver for Linux kernel framebuffers
 *
 * Portions used from Ben Pfaff's BOGL <pfaffben@debian.org>
 *
 * Modified for eCos by
 *   Gary Thomas <gthomas@redhat.com>
 *   Richard Panton <richard.panton@3glab.org>
 *
 * Note: modify select_fb_driver() to add new framebuffer subdrivers
 */
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

#include "kernel/core/fcntl.h"
#include "kernel/core/types.h"
#include "kernel/core/stat.h"
#include "kernel/core/devio.h"
#include "kernel/core/stropts.h"

#include "lib/libc/stdio/stdio.h"

#include "device.h"
#include "genfont.h"
#include "genmem.h"
#include "fb.h"

#include "kernel/dev/arch/gnu32/dev_linux_screen/_screen_linux.h"
//#include "kernel/dev/arch/gnu32/dev_linux_screen/dev_linux_screen.h"

#include "kernel/core/ioctl_fb.h"

#define FB_DEV_GENERIC     "/dev/fb/fb"
//#define FB_DEV_SPECIFIC    "/dev/fb/fb0"
//#define FB_DEV_NO          1
#define FB_DEV_SPECIFIC    "/dev/fb/fb1"
#define FB_DEV_NO          2

static PSD fb_tauon_open(PSD psd);
static void fb_tauon_close(PSD psd);
static void fb_tauon_getscreeninfo(PSD psd,PMWSCREENINFO psi);
static void fb_tauon_setpalette(PSD psd,int first, int count, MWPALENTRY *palette);
static void fb_tauon_setportrait(PSD psd, int portraitmode);
//
SCREENDEVICE	scrdev = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL,
	fb_tauon_open,
	fb_tauon_close,
	fb_tauon_getscreeninfo,
	fb_tauon_setpalette,
	NULL,//fb_tauon_drawpixel,			/* DrawPixel subdriver*/
	NULL,//fb_tauon_readpixel,			/* ReadPixel subdriver*/
	NULL,//fb_tauon_drawhline,			/* DrawHorzLine subdriver*/
	NULL,//fb_tauon_drawvline,			/* DrawVertLine subdriver*/
	NULL,//fb_tauon_fillrect,			/* FillRect subdriver*/
	gen_fonts,
	NULL,//fb_tauon_blit,			/* Blit subdriver*/
	NULL,			/* PreSelect*/
	NULL,			/* DrawArea subdriver*/
	NULL,			/* SetIOPermissions*/
	gen_allocatememgc,
	fb_mapmemgc,
	gen_freememgc,
	NULL, /*StretchBlit*/
	fb_tauon_setportrait, /*SetPortrait*/
	0,//MWPORTRAIT_LEFT, MWPORTRAIT_RIGHT or MWPORTRAIT_DOWN /*screen mode portrait*/
	NULL, /*subdriver for portrait*/
	NULL /*StretchBlitEx*/
};

//for portait mode
extern SUBDRIVER fbportrait_left, fbportrait_right, fbportrait_down;
static PSUBDRIVER pdrivers[4] = { /* portrait mode subdrivers*/
   NULL, &fbportrait_left, &fbportrait_right, &fbportrait_down
};

//
static int fd;

// init framebuffer
static PSD fb_tauon_open(PSD psd){
	PSUBDRIVER subdriver;

	fb_info_t fb;
	fd = open(FB_DEV_SPECIFIC, O_WRONLY, 0);
	ioctl(fd,FBIOGET_DISPINFO,&fb, FB_DEV_NO);

	//
	psd->xres = psd->xvirtres = fb.x_res;
	psd->yres = psd->yvirtres = fb.y_res;

	//bpp
	psd->bpp = fb.bpp;//panel_info.vl_bpix;
	//planes
	psd->planes = 1;

	//Colors
	psd->ncolors = (psd->bpp >= 24)? (1 << 24): (1 << psd->bpp);

	psd->linelen = fb.line_len;
	psd->size = 0;

	/////A VOIR!!!!
	psd->pixtype = MWPF_PALETTE;//MWPF_TRUECOLOR332;
	psd->flags=PSF_SCREEN | PSF_HAVEBLIT;
	psd->addr = (void *)fb.smem_start;

	//
	subdriver = select_fb_subdriver(psd);
	if(!set_subdriver(psd, subdriver, TRUE)) {
		EPRINTF("Driver initialize failed\r\n", psd->bpp);
		return NULL;
	}
	//for portrait mode
	psd->orgsubdriver = subdriver;
	//
	psd->addr = (void *)fb.smem_start;
	//
	printf("%s\r\n", __FUNCTION__);
	return psd;
}

// close framebuffer
static void fb_tauon_close(PSD psd) {
    printf("%s\n", __FUNCTION__);
}

//
static void
fb_tauon_getscreeninfo(PSD psd,PMWSCREENINFO psi)
{
	psi->rows = psd->yvirtres;
	psi->cols = psd->xvirtres;
	psi->planes = psd->planes;
	psi->bpp = psd->bpp;
	psi->ncolors = psd->ncolors;
	psi->pixtype = psd->pixtype;
	psi->fonts = NUMBER_FONTS;

	/* DBG 640x480*/
	//psi->xdpcm = screen_info->xmm;
	//psi->ydpcm = screen_info->ymm;
	printf("DBG_getscreeninfo()\r\n");
}

//convert Microwindows palette to framebuffer format and set it
static void
fb_tauon_setpalette(PSD psd,int first, int count, MWPALENTRY *palette)
{
   int i=0;
   MWPALENTRY *p = NULL;
   //ioctl on fd to get palette info
   fb_info_t fb;
   ioctl(fd,FBIOGET_DISPINFO,&fb, FB_DEV_NO);
   //
   for(;i<fb.cmap_len;i++) {
      p = &palette[i];
      p->r=fb.cmap[i].red;
      p->g=fb.cmap[i].green;
      p->b=fb.cmap[i].blue;
   }
	printf("%s\r\n", __FUNCTION__);
}

//set portrait mode useless portraitmode parameter for moment
static void fb_tauon_setportrait(PSD psd, int portraitmode) {
   int psd_portrait = psd->portrait;

   /*do nothing*/
   if(psd_portrait == MWPORTRAIT_NONE) return;

   /* swap x and y in left or right portrait modes*/
   if (psd_portrait & (MWPORTRAIT_LEFT|MWPORTRAIT_RIGHT)) {
      /* swap x, y*/
      psd->xvirtres = psd->yres;
      psd->yvirtres = psd->xres;
   } else {
      /*MWPORTRAIT_DOWN est égale à 4*/
      psd->xvirtres = psd->xres;
      psd->yvirtres = psd->yres;
      psd_portrait = 3;
   }
   set_subdriver(psd, pdrivers[psd_portrait], FALSE);
}

