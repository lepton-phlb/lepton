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

#include "core/fcntl.h"
#include "core/types.h"
#include "core/stat.h"

#include "stdio/stdio.h"
#include "unistd/unistd.h"

#include "device.h"
#include "genfont.h"
#include "genmem.h"
#include "fb.h"

#include "dev_atsam9261_lcd/atsam9261_lcd_lut.h"
#include "dev_atsam9261_lcd/atsam9261_lcd.h"

static PSD fb_tauon_open(PSD psd);
static void fb_tauon_close(PSD psd);
static void fb_tauon_getscreeninfo(PSD psd,PMWSCREENINFO psi);
static void fb_tauon_setpalette(PSD psd,int first, int count, MWPALENTRY *palette);


/*static void fb_tauon_drawpixel(PSD psd,MWCOORD x, MWCOORD y, MWPIXELVAL c);
static MWPIXELVAL fb_tauon_readpixel(PSD psd,MWCOORD x, MWCOORD y);
static void fb_tauon_drawhline(PSD psd,MWCOORD x1, MWCOORD x2, MWCOORD y, MWPIXELVAL c);
static void fb_tauon_drawvline(PSD psd,MWCOORD x,MWCOORD y1,MWCOORD y2,MWPIXELVAL c);
static void fb_tauon_fillrect(PSD psd,MWCOORD x1,MWCOORD y1,MWCOORD x2,MWCOORD y2,
		MWPIXELVAL c);
static void fb_tauon_blit(PSD dstpsd, MWCOORD dstx, MWCOORD dsty, MWCOORD w, MWCOORD h, PSD srcpsd, MWCOORD srcx, MWCOORD srcy, long op);
*/

//
void
ioctl_tst_setpalette(MWPALENTRY *palette);

extern vidinfo_t panel_info;

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
	gen_freememgc
};

// init framebuffer
static PSD fb_tauon_open(PSD psd){
	PSUBDRIVER subdriver;
	//get the  address of fb with ioctl
	//
	psd->xres = psd->xvirtres = panel_info.vl_col;
	psd->yres = psd->yvirtres = panel_info.vl_row;
	
	//bpp
	psd->bpp = 8;//panel_info.vl_bpix;
	//planes
	psd->planes = 1;
	
	//Colors
	psd->ncolors = (psd->bpp >= 24)? (1 << 24): (1 << psd->bpp);
	
	psd->linelen = panel_info.vl_col;
	psd->size = 0;
	
	psd->pixtype = MWPF_PALETTE;//MWPF_TRUECOLOR332;
	
	psd->flags=PSF_SCREEN | PSF_HAVEBLIT;
	psd->addr = (void *)at91sam9261_lcd_get_shadow_frame_buffer();
	//
	subdriver = select_fb_subdriver(psd);
	if(!set_subdriver(psd, subdriver, TRUE)) {
		EPRINTF("Driver initialize failed\n", psd->bpp);
		return NULL;
	}
	//
	psd->addr = (void *)at91sam9261_lcd_get_current_frame_buffer();
	//
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
	psi->fonts = 1;

	/* DBG 640x480*/
	psi->xdpcm = panel_info.vl_width;
	psi->ydpcm = panel_info.vl_height;
//	printf("DBG_getscreeninfo()\n");
}

//convert Microwindows palette to framebuffer format and set it
static void
fb_tauon_setpalette(PSD psd,int first, int count, MWPALENTRY *palette)
{
	printf("%s\n", __FUNCTION__);
}

/*
//
static void fb_tauon_drawpixel(PSD psd,MWCOORD x, MWCOORD y, MWPIXELVAL c) {
	char * fb = (char *)(psd->addr + y*psd->linelen + x);
	*fb = c;
}

//
static MWPIXELVAL fb_tauon_readpixel(PSD psd,MWCOORD x, MWCOORD y) {
	char * fb = (char *)(psd->addr + y*psd->linelen + x);
	return (*fb);
}

//
static void fb_tauon_drawhline(PSD psd,MWCOORD x1, MWCOORD x2, MWCOORD y, MWPIXELVAL c){
	MWCOORD x_min = (x1>x2)?x2:x1;
	char * fb = (char *)(psd->addr + y*psd->linelen + x_min);
	int i = 0;
	int len = abs(x2-x1);
	//
	memset((void *)fb, c, len);
	//for(; i<len;i++) {
	//	*fb++ = c;
	//}
}

//
static void fb_tauon_drawvline(PSD psd,MWCOORD x,MWCOORD y1,MWCOORD y2,MWPIXELVAL c) {
	MWCOORD y_min = (y1>y2)?y2:y1;
	char * fb = (char *)(psd->addr + y_min*psd->linelen + x);
	int i = 0;
	int len = abs(y2-y1);
	//
	for(; i<len;i++) {
		*fb = c;
		fb+=psd->linelen;
	}
}

//
static void fb_tauon_fillrect(PSD psd,MWCOORD x1,MWCOORD y1,MWCOORD x2,MWCOORD y2,
		MWPIXELVAL c) {
	MWCOORD x_min = (x1>x2)?x2:x1;
	MWCOORD y_min = (y1>y2)?y2:y1;
	
	char * fb = (char *)(psd->addr + y_min*psd->linelen + x_min);
	int i,j;
	int lx = abs(x2-x1);
	int ly = abs(y2-y1);
	
	for(i=0;i<(ly+1);i++){
		for(j=0;j<(lx+1);j++) {
			*fb++ = c;
		}
		fb += (psd->linelen-(lx+1));
	}
}

//from fblin8.c
static void
fb_tauon_blit(PSD dstpsd, MWCOORD dstx, MWCOORD dsty, MWCOORD w, MWCOORD h,
	PSD srcpsd, MWCOORD srcx, MWCOORD srcy, long op) {
	
	int	dlinelen = dstpsd->linelen;
	int	slinelen = srcpsd->linelen;

	ADDR8	dst = ((ADDR8)dstpsd->addr) + dstx + dsty * dlinelen;
	ADDR8	src = ((ADDR8)srcpsd->addr) + srcx + srcy * slinelen;

	if (srcy < dsty) {
		src += (h-1) * slinelen;
		dst += (h-1) * dlinelen;
		slinelen *= -1;
		dlinelen *= -1;
	}

	while(--h >= 0) {
		//a _fast_ memcpy is a _must_ in this routine
		memmove(dst, src, w);
		dst += dlinelen;
		src += slinelen;
	}
}


static int fade = 100;


//get framebuffer palette
void
ioctl_getpalette(int start, int len, short *red, short *green, short *blue)
{
    printf("%s - NOT IMPLEMENTED\n", __FUNCTION__);
}

//set framebuffer palette
void
ioctl_tst_setpalette(MWPALENTRY *palette)
{

}
*/
