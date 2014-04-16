/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Chauvin-Arnoux.
Portions created by Chauvin-Arnoux are Copyright (C) 2011. All Rights Reserved.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under
the MPL, indicate your decision by deleting  the provisions above and replace
them with the notice and other provisions required by the [eCos GPL] License.
If you do not delete the provisions above, a recipient may use your version of this file under
either the MPL or the [eCos GPL] License."
*/

/*===========================================
Includes
=============================================*/
#include "kernel/core/errno.h"
#include "kernel/core/libstd.h"
#include "kernel/core/devio.h"
#include "lib/libc/termios/termios.h"

/*===========================================
Global Declaration
=============================================*/

/* This is a gross hack around a kernel bug.  If the cfsetispeed functions is
 * called with the SPEED argument set to zero this means use the same speed as
 * for output.  But we don't have independent input and output speeds and
 * therefore cannot record this.
 *
 * We use an unused bit in the `c_iflag' field to keep track of this use of
 * `cfsetispeed'.  The value here must correspond to the one used in
 * `tcsetattr.c'.  */
#define IBAUD0  020000000000

struct speed_struct
{
   speed_t value;
   speed_t internal;
};

static const struct speed_struct speeds[] =
{
#ifdef B0
   { 0, B0 },
#endif
#ifdef B50
   { 50, B50 },
#endif
#ifdef B75
   { 75, B75 },
#endif
#ifdef B110
   { 110, B110 },
#endif
#ifdef B134
   { 134, B134 },
#endif
#ifdef B150
   { 150, B150 },
#endif
#ifdef B200
   { 200, B200 },
#endif
#ifdef B300
   { 300, B300 },
#endif
#ifdef B600
   { 600, B600 },
#endif
#ifdef B1200
   { 1200, B1200 },
#endif
#ifdef B1200
   { 1200, B1200 },
#endif
#ifdef B1800
   { 1800, B1800 },
#endif
#ifdef B2400
   { 2400, B2400 },
#endif
#ifdef B4800
   { 4800, B4800 },
#endif
#ifdef B9600
   { 9600, B9600 },
#endif
#ifdef B19200
   { 19200, B19200 },
#endif
#ifdef B38400
   { 38400, B38400 },
#endif
#ifdef B57600
   { 57600, B57600 },
#endif
#ifdef B76800
   { 76800, B76800 },
#endif
#ifdef B115200
   { 115200, B115200 },
#endif
#ifdef B153600
   { 153600, B153600 },
#endif
#ifdef B230400
   { 230400, B230400 },
#endif
#ifdef B307200
   { 307200, B307200 },
#endif
#ifdef B460800
   { 460800, B460800 },
#endif
};

/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:isatty
| Description:Return 1 if FD is a terminal, 0 if not.
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int isatty(int fd){
   struct termios term;
   return (tcgetattr (fd, &term) == 0);
}

/*-------------------------------------------
| Name:tcdrain
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int tcdrain (int fd){
   /* With an argument of 1, TCSBRK waits for the output to drain.  */
   return ioctl(fd, TCSBRK, 1);
}

/*-------------------------------------------
| Name:tcflow
| Description: Suspend or restart transmission on FD.
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int tcflow ( int fd, int action){
   return ioctl(fd, TCXONC, action);
}

/*-------------------------------------------
| Name:tcflush
| Description:Flush pending data on FD.
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int tcflush ( int fd, int queue_selector){
   return ioctl(fd, TCFLSH, queue_selector);
}

/*-------------------------------------------
| Name:tcsendbreak
| Description:Send zero bits on FD.
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int tcsendbreak( int fd, int duration){
   /* The break lasts 0.25 to 0.5 seconds if DURATION is zero,
     and an implementation-defined period if DURATION is nonzero.
     We define a positive DURATION to be number of milliseconds to break.  */
   if (duration <= 0)
      return ioctl(fd, TCSBRK, 0);

#ifdef TCSBRKP
   /* Probably Linux-specific: a positive third TCSBRKP ioctl argument is
     defined to be the number of 100ms units to break.  */
   return ioctl(fd, TCSBRKP, (duration + 99) / 100);
#else
   /* ioctl can't send a break of any other duration for us.
     This could be changed to use trickery (e.g. lower speed and
     send a '\0') to send the break, but for now just return an error.  */
   //__set_errno (EINVAL);
   return -1;
#endif
}

/*-------------------------------------------
| Name:tcsetpgrp
| Description:Set the foreground process group ID of FD set PGRP_ID.
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int tcsetpgrp ( int fd, pid_t pgrp_id){
   return ioctl (fd, TIOCSPGRP, &pgrp_id);
}

/*-------------------------------------------
| Name:tcgetpgrp
| Description:Return the foreground process group ID of FD.
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
pid_t tcgetpgrp ( int fd){
   int pgrp;

   if (ioctl (fd, TIOCGPGRP, &pgrp) < 0)
      return (pid_t) -1;
   return (pid_t) pgrp;
}

/*-------------------------------------------
| Name:cfgetospeed
| Description:Return the output baud rate stored in *TERMIOS_P.
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
speed_t cfgetospeed ( const struct termios *termios_p){
   return termios_p->c_cflag & (CBAUD | CBAUDEX);
}

/*-------------------------------------------
| Name:cfgetispeed
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
/* Return the input baud rate stored in *TERMIOS_P.
 * Although for Linux there is no difference between input and output
 * speed, the numerical 0 is a special case for the input baud rate. It
 * should set the input baud rate to the output baud rate. */
speed_t cfgetispeed (const struct termios *termios_p){
   return ((termios_p->c_iflag & IBAUD0)
           ? 0 : termios_p->c_cflag & (CBAUD | CBAUDEX));
}

/*-------------------------------------------
| Name:cfsetospeed
| Description:Set the output baud rate stored in *TERMIOS_P to SPEED.
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int cfsetospeed  (struct termios *termios_p, speed_t speed){
   if ((speed & ~CBAUD) != 0
       && (speed < B57600 || speed > B460800))
   {
      //__set_errno(EINVAL);
      return -1;
   }

   termios_p->c_cflag &= ~(CBAUD | CBAUDEX);
   termios_p->c_cflag |= speed;

   return 0;
}

/*-------------------------------------------
| Name:cfsetispeed
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
/* Set the input baud rate stored in *TERMIOS_P to SPEED.
 * Although for Linux there is no difference between input and output
 * speed, the numerical 0 is a special case for the input baud rate.  It
 * should set the input baud rate to the output baud rate.  */
int cfsetispeed ( struct termios *termios_p, speed_t speed)
{
   if ((speed & ~CBAUD) != 0
       && (speed < B57600 || speed > B460800)) {
      //__set_errno(EINVAL);
      return -1;
   }

   if (speed == 0) {
      termios_p->c_iflag |= IBAUD0;
   }else{
      termios_p->c_iflag &= ~IBAUD0;
      termios_p->c_cflag &= ~(CBAUD | CBAUDEX);
      termios_p->c_cflag |= speed;
   }

   return 0;
}

/*-------------------------------------------
| Name:cfsetspeed
| Description:Set both the input and output baud rates stored in *TERMIOS_P to SPEED.
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int cfsetspeed (struct termios *termios_p, speed_t speed){
   size_t cnt;

   for (cnt = 0; cnt < sizeof (speeds) / sizeof (speeds[0]); ++cnt)
      if (speed == speeds[cnt].internal)
      {
         cfsetispeed (termios_p, speed);
         cfsetospeed (termios_p, speed);
         return 0;
      }
      else if (speed == speeds[cnt].value)
      {
         cfsetispeed (termios_p, speeds[cnt].internal);
         cfsetospeed (termios_p, speeds[cnt].internal);
         return 0;
      }

   //__set_errno (EINVAL);

   return -1;
}

/*-------------------------------------------
| Name:cfmakeraw
| Description:Set *T to indicate raw mode.
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
/*
   Copyright (C) 1992, 1996, 1997 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
*/
void cfmakeraw (struct termios *termios_p){
   termios_p->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
   termios_p->c_oflag &= ~OPOST;
   termios_p->c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
   termios_p->c_cflag &= ~(CSIZE|PARENB);
   termios_p->c_cflag |= CS8;
   termios_p->c_cc[VMIN] = 1;           /* read returns when one char is available.  */
   termios_p->c_cc[VTIME] = 0;
}





/*===========================================
End of Source termios.c
=============================================*/
