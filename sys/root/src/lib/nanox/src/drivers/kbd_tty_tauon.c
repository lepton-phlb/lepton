/*
 * Copyright (c) 1999, 2003 Greg Haerr <greg@censoft.com>
 * Copyright (c) 1991 David I. Bell
 * Permission is granted to use, distribute, or modify this source,
 * provided that this copyright notice remains intact.
 *
 * /dev/tty TTY Keyboard Driver
 *
 * if TRANSLATE_ESCAPE_SEQUENCES is set in device.h, then we
 * hard-decode function keys for Linux console and KDE konsole.
 */

#include <stdlib.h>

#include "kernel/core/types.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/errno.h"
#include "kernel/core/devio.h"

#include "unistd/unistd.h"

#include "device.h"


#define CTRL(x)	  ((x) & 0x1f)

extern int escape_quits;

static int  TTY_Open(KBDDEVICE *pkd);
static void TTY_Close(void);
static void TTY_GetModifierInfo(MWKEYMOD *modifiers, MWKEYMOD *curmodifiers);
static int  TTY_Read(MWKEY *kbuf, MWKEYMOD *modifiers, MWSCANCODE *scancode);
static int  TTY_Poll(void);

KBDDEVICE kbddev = {
	TTY_Open,
	TTY_Close,
	TTY_GetModifierInfo,
	TTY_Read,
#if _MINIX
	TTY_Poll
#else
	NULL
#endif
};

static	int		fd;		/* file descriptor for keyboard */

/*
 * Open the keyboard.
 * This is real simple, we just use a special file handle
 * that allows non-blocking I/O, and put the terminal into
 * character mode.
 */
static int
TTY_Open(KBDDEVICE *pkd)
{
	int flag;
	fd = STDIN_FILENO;

	flag = fcntl(fd,F_GETFL);
   fcntl(fd,F_SETFL,flag|O_NONBLOCK);

	return fd;
}

/*
 * Close the keyboard.
 * This resets the terminal modes.
 */
static void
TTY_Close(void)
{
	int flag = fcntl(STDIN_FILENO,F_GETFL);
   fcntl(STDIN_FILENO,F_SETFL,flag&~O_NONBLOCK);
	fd = -1;
}

/*
 * Return the possible modifiers for the keyboard.
 */
static  void
TTY_GetModifierInfo(MWKEYMOD *modifiers, MWKEYMOD *curmodifiers)
{
	if (modifiers)
		*modifiers = 0;		/* no modifiers available */
	if (curmodifiers)
		*curmodifiers = 0;
}

/*
 * This reads one keystroke from the keyboard, and the current state of
 * the modifier keys (ALT, SHIFT, etc).  Returns -1 on error, 0 if no data
 * is ready, 1 on a keypress, and 2 on keyrelease.
 * This is a non-blocking call.
 */
static int
TTY_Read(MWKEY *kbuf, MWKEYMOD *modifiers, MWSCANCODE *scancode)
{
	unsigned char buf;

	int	cc;			/* characters read */
	MWKEY mwkey;
	//
	cc = read(fd, (void *)&buf, 1);
	//
	if (cc <= 0) {
		return 0;
	}
	//
	mwkey = buf;
	if (mwkey == CTRL('P'))			/* ^P -> print*/
		mwkey = MWKEY_PRINT;
	//
	if ((mwkey == MWKEY_ESCAPE) && escape_quits)
		mwkey = MWKEY_QUIT;

	//
	*kbuf = mwkey;		/* no translation*/
	*modifiers = 0;		/* no modifiers*/
	*scancode = 0;		/* no scancode*/
	return 1;		/* keypress*/
}

static int
TTY_Poll(void)
{
	return 1;	/* used by _MINIX only*/
}
