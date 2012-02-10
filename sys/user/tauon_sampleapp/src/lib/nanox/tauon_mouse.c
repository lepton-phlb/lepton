/*
 * Copyright (c) 1999 Greg Haerr <greg@censoft.com>
 *
 * mouseL Mouse Driver
 */

#include "lib/libc/stdio/stdio.h"
#include "device.h"

#define SCALE           3       /* default scaling factor for acceleration */
#define THRESH          5       /* default threshhold for acceleration */

static int      mouse_Open(MOUSEDEVICE *pmd);
static void     mouse_Close(void);
static int      mouse_GetButtonInfo(void);
static void     mouse_GetDefaultAccel(int *pscale,int *pthresh);
static int      mouse_Read(MWCOORD *dx, MWCOORD *dy, MWCOORD *dz, int *bp);
static int      mouse_Poll(void);

MOUSEDEVICE mousedev = {
   mouse_Open,
   mouse_Close,
   mouse_GetButtonInfo,
   mouse_GetDefaultAccel,
   mouse_Read,
   mouse_Poll
};

/*
 * Poll for events
 */

static int mouse_Poll(void) {
   return 0;
}

/*
 * Open up the mouse device.
 */
static int mouse_Open(MOUSEDEVICE *pmd) {
   return -2;
}

/*
 * Close the mouse device.
 */
static void mouse_Close(void) {
}

/*
 * Get mouse buttons supported
 */
static int mouse_GetButtonInfo(void) {
   return 0;
}

/*
 * Get default mouse acceleration settings
 */
static void mouse_GetDefaultAccel(int *pscale,int *pthresh) {
   *pscale = SCALE;
   *pthresh = THRESH;
}

/*
 * Attempt to read bytes from the mouse and interpret them.
 * Returns -1 on error, 0 if either no bytes were read or not enough
 * was read for a complete state, or 1 if the new state was read.
 * When a new state is read, the current buttons and x and y deltas
 * are returned.  This routine does not block.
 */
static int mouse_Read(MWCOORD *dx, MWCOORD *dy, MWCOORD *dz, int *bp) {
   return 0;
}
