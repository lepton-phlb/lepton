/*
The contents of this file are subject to the Mozilla Public License Version 1.1 
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
specific language governing rights and limitations under the License.

The Original Code is ______________________________________.

The Initial Developer of the Original Code is ________________________.
Portions created by ______________________ are Copyright (C) ______ _______________________.
All Rights Reserved.

Contributor(s): ______________________________________.

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
#include <string.h>
#include "kernel/core/errno.h"

#include "lib/libc/unistd/getopt.h"

#include "kernel/core/signal.h"
#include "kernel/core/libstd.h"
#include "kernel/core/stat.h"
#include "kernel/core/statvfs.h"
#include "kernel/core/devio.h"
#include "kernel/core/time.h"


#include "lib/libc/stdio/stdio.h"


/*===========================================
Global Declaration
=============================================*/


/*===========================================
Implementation
=============================================*/


/* This 'date' command supports only 2 time setting formats, 
   all the GNU strftime stuff (its in libc, lets use it),
   setting time using UTC and displaying int, as well as
   an RFC 822 complient date output for shell scripting
   mail commands */

/* Input parsing code is always bulky - used heavy duty libc stuff as
   much as possible, missed out a lot of bounds checking */

/* Default input handling to save suprising some people */

/*-------------------------------------------
| Name:date_conv_time
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static struct tm *date_conv_time(struct tm *tm_time, const char *t_string)
{
	int nr;

	nr = sscanf(t_string, "%2d%2d%2d%2d%d",
				&(tm_time->tm_mon),
				&(tm_time->tm_mday),
				&(tm_time->tm_hour),
				&(tm_time->tm_min), &(tm_time->tm_year));

	if (nr < 4 || nr > 5) {
		return (struct tm *)0; //error_msg_and_die(invalid_date, t_string); 
	}

	/* correct for century  - minor Y2K problem here? */
	if (tm_time->tm_year >= 1900)
		tm_time->tm_year -= 1900;
	/* adjust date */
	tm_time->tm_mon -= 1;

	return (tm_time);

}

/*-------------------------------------------
| Name:date_conv_ftime
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static struct tm *date_conv_ftime(struct tm *tm_time, const char *t_string)
{/* The new stuff for LRP */
	struct tm t;

	/* Parse input and assign appropriately to tm_time */

	if (t=*tm_time,sscanf(t_string, "%d:%d:%d",
			   &t.tm_hour, &t.tm_min, &t.tm_sec) == 3) {
					/* no adjustments needed */

	} else if (t=*tm_time,sscanf(t_string, "%d:%d",
					  &t.tm_hour, &t.tm_min) == 2) {
					/* no adjustments needed */


	} else if (t=*tm_time,sscanf(t_string, "%d.%d-%d:%d:%d",
					  &t.tm_mon,
					  &t.tm_mday,
					  &t.tm_hour,
					  &t.tm_min, &t.tm_sec) == 5) {

		t.tm_mon -= 1;	/* Adjust dates from 1-12 to 0-11 */

	} else if (t=*tm_time,sscanf(t_string, "%d.%d-%d:%d",
					  &t.tm_mon,
					  &t.tm_mday,
					  &t.tm_hour, &t.tm_min) == 4) {

		t.tm_mon -= 1;	/* Adjust dates from 1-12 to 0-11 */

	} else if (t=*tm_time,sscanf(t_string, "%d.%d.%d-%d:%d:%d",
					  &t.tm_year,
					  &t.tm_mon,
					  &t.tm_mday,
					  &t.tm_hour,
					  &t.tm_min, &t.tm_sec) == 6) {

		t.tm_year -= 1900;	/* Adjust years */
		t.tm_mon -= 1;	/* Adjust dates from 1-12 to 0-11 */

	} else if (t=*tm_time,sscanf(t_string, "%d.%d.%d-%d:%d",
					  &t.tm_year,
					  &t.tm_mon,
					  &t.tm_mday,
					  &t.tm_hour, &t.tm_min) == 5) {
		t.tm_year -= 1900;	/* Adjust years */
		t.tm_mon -= 1;	/* Adjust dates from 1-12 to 0-11 */

	} else {
		return (struct tm *)0; //error_msg_and_die(invalid_date, t_string); 
	}
	*tm_time = t;
	return (tm_time);
}

/*-------------------------------------------
| Name:date_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int date_main(int argc, char* argv[]){
	char *date_str = NULL;
	char *date_fmt = NULL;
   char t_buff[128]={0};
	int c;
	int set_time = 0;
	int rfc822 = 0;
	int utc = 0;
	int use_arg = 0;
	time_t tm;
	struct tm tm_time;
   getopt_state_t getopt_state;

   getopt_init(&getopt_state);

	/* Interpret command line args */
	while ((c = getopt(&getopt_state,argc, argv, "Rs:ud:")) != EOF) {
		switch (c) {
			case 'R':
				rfc822 = 1;
				break;
			case 's':
				set_time = 1;
				if ((date_str != NULL) || ((date_str = getopt_state.optarg) == NULL)) {
					//show_usage();
               return -1;
				}
				break;
			case 'u':
				utc = 1;
				/*if (putenv("TZ=UTC0") != 0)
					return -1; //error_msg_and_die(memory_exhausted);
            */
				break;
			case 'd':
				use_arg = 1;
				if ((date_str != NULL) || ((date_str = getopt_state.optarg) == NULL))
					//show_usage();
               return -1;
				break;
			default:
				//show_usage();
            return -1;
		}
	}

	if ((date_fmt == NULL) && (getopt_state.optind < argc) && (argv[getopt_state.optind][0] == '+'))
		date_fmt = &argv[getopt_state.optind][1];   /* Skip over the '+' */
	else if (date_str == NULL) {
		set_time = 1;
		date_str = argv[getopt_state.optind];
	} 
#if 0
	else {
		error_msg("date_str='%s'  date_fmt='%s'\n", date_str, date_fmt);
		//show_usage();
	}
#endif

	/* Now we have parsed all the information except the date format
	   which depends on whether the clock is being set or read */

	time(&tm);
	memcpy(&tm_time, localtime(&tm), sizeof(tm_time));
	/* Zero out fields - take her back to midnight! */
	if (date_str != NULL) {
		tm_time.tm_sec = 0;
		tm_time.tm_min = 0;
		tm_time.tm_hour = 0;
	}

	/* Process any date input to UNIX time since 1 Jan 1970 */
	if (date_str != NULL) {

		if (strchr(date_str, ':') != NULL) {
         if(!date_conv_ftime(&tm_time, date_str)){
            printf("cannot set date\r\n");
            return -1;
         }
		} else {
			if(!date_conv_time(&tm_time, date_str)){
            printf("cannot set date\r\n");
            return -1;
         }
		}

		/* Correct any day of week and day of year etc. fields */
		tm = mktime(&tm_time);
		if (tm < 0)
			return -1; //error_msg_and_die(invalid_date, date_str); 

		/*if ( utc ) {
			if (putenv("TZ=UTC0") != 0)
				//error_msg_and_die(memory_exhausted);
            return -1;
		}*/

		/* if setting time, set it */
		if (set_time) {
			if (stime(&tm) < 0) {
				printf("cannot set date\r\n");
            return -1;
			}
		}
	}

	/* Display output */

	/* Deal with format string */
	if (date_fmt == NULL) {
		date_fmt = (rfc822
					? (utc
					   ? "%a, %e %b %Y %H:%M:%S GMT"
					   : "%a, %e %b %Y %H:%M:%S %z")
					: "%a %b %e %H:%M:%S %Z %Y");

	} else if (*date_fmt == '\0') {
		/* Imitate what GNU 'date' does with NO format string! */
		printf("\n");
		return 0;
	}

	/* Handle special conversions */

	if (strncmp(date_fmt, "%f", 2) == 0) {
		date_fmt = "%Y.%m.%d-%H:%M:%S";
	}

	/* Print OUTPUT (after ALL that!) */
	//t_buff = xmalloc(201);
	strftime(t_buff, sizeof(t_buff)-1, date_fmt, &tm_time);
	puts(t_buff);
   printf("\r\n");
	return 0;
}

/*===========================================
End of Sourcedate.c
=============================================*/
