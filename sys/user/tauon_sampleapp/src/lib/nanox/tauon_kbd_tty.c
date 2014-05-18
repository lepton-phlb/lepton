/*
 * Copyright (c) 1999, 2003 Greg Haerr <greg@censoft.com>
 * Copyright (c) 1991 David I. Bell
 * Permission is granted to use, distribute, or modify this source,
 * provided that this copyright notice remains intact.
 *
 * /dev/kb kb Keyboard Driver
 *
 * if TRANSLATE_ESCAPE_SEQUENCES is set in device.h, then we
 * hard-decode function keys for Linux console and KDE konsole.
 */

#include <stdlib.h>

#include "kernel/core/types.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/errno.h"
#include "kernel/core/devio.h"

#include "lib/libc/stdio/stdio.h"

#include "device.h"


#define CTRL(x)   ((x) & 0x1f)

extern int escape_quits;

static int  kb_Open(KBDDEVICE *pkd);
static void kb_Close(void);
static void kb_GetModifierInfo(MWKEYMOD *modifiers, MWKEYMOD *curmodifiers);
static int  kb_Read(MWKEY *kbuf, MWKEYMOD *modifiers, MWSCANCODE *scancode);
static int  kb_Poll(void);

KBDDEVICE kbddev = {
   kb_Open,
   kb_Close,
   kb_GetModifierInfo,
   kb_Read,
#if _MINIX
   kb_Poll
#else
   NULL
#endif
};

#define KB_DEVICE    "/dev/kb0"
static int fd;                          /* file descriptor for keyboard */
//static MWKEY mwkey_release;
/*
 * Open the keyboard.
 * This is real simple, we just use a special file handle
 * that allows non-blocking I/O, and put the terminal into
 * character mode.
 */
static int kb_Open(KBDDEVICE *pkd) {
#if defined(CPU_GNU32)
   if((fd = open(KB_DEVICE, O_RDONLY|O_NONBLOCK,0))<0) {
      printf("Can't open %s\r\n", KB_DEVICE);
      return -1;
   }
#else
   int flag;
   fd = STDIN_FILENO;

   flag = fcntl(fd,F_GETFL);
   fcntl(fd,F_SETFL,flag|O_NONBLOCK);

#endif
   return fd;
}

/*
 * Close the keyboard.
 * This resets the terminal modes.
 */
static void kb_Close(void) {
#if defined(CPU_GNU32)
   close(fd);
#else
   int flag = fcntl(STDIN_FILENO,F_GETFL);
   fcntl(STDIN_FILENO,F_SETFL,flag&~O_NONBLOCK);
#endif
   fd = -1;
}

/*
 * Return the possible modifiers for the keyboard.
 */
static void kb_GetModifierInfo(MWKEYMOD *modifiers, MWKEYMOD *curmodifiers) {
   if (modifiers)
      *modifiers = 0;
   if (curmodifiers)
      *curmodifiers = 0;
}

/*
 * This reads one keystroke from the keyboard, and the current state of
 * the modifier keys (ALT, SHIFT, etc).  Returns -1 on error, 0 if no data
 * is ready, 1 on a keypress, and 2 on keyrelease.
 * This is a non-blocking call.
 */
static int kb_Read(MWKEY *kbuf, MWKEYMOD *modifiers, MWSCANCODE *scancode) {
   unsigned char buf;

   int cc;                      //characters read
   MWKEY mwkey;

   cc = read(fd, (void *)&buf, 1);

   if (cc <= 0) {
      return 0;
   }

   mwkey = buf;

   if (mwkey == CTRL('P'))                      // ^P -> print
      mwkey = MWKEY_PRINT;

   if ((mwkey == MWKEY_ESCAPE) && escape_quits)
      mwkey = MWKEY_QUIT;

   //
   *kbuf = mwkey;               //no translation
   *modifiers = 0;              // no modifiers
   *scancode = 0;               //no scancode
   return 1;            // keypress
}

static int kb_Poll(void) {
   return 1;
}
