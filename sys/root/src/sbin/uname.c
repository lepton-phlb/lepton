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


/*============================================
| Includes    
==============================================*/
#include <stdlib.h>
#include "kernel/core/errno.h"
#include "lib/libc/unistd/getopt.h"
#include "kernel/core/libstd.h"
#include "kernel/core/sys/utsname.h"

#include "lib/libc/stdio/stdio.h"

/*============================================
| Global Declaration 
==============================================*/
#define	PRINT_SYSNAME	0x01
#define	PRINT_NODENAME	0x02
#define	PRINT_RELEASE	0x04
#define	PRINT_VERSION	0x08
#define	PRINT_MACHINE	0x10
#define	PRINT_PROCESSOR	0x20
#define	PRINT_ALL	0x3f


/*============================================
| Implementation 
==============================================*/
/*--------------------------------------------
| Name:        usage
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static void usage(){
	fprintf(stderr, "usage: uname [-amnprsv]\r\n");
	exit(1);
}

/*--------------------------------------------
| Name:        uname_main
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int uname_main(int argc,char* argv[]){
	struct utsname u;
	int c;
	int space = 0;
	int print_mask = 0;

   getopt_state_t getopt_state;

   //
   getopt_init(&getopt_state);

   //
	while ((c = getopt(&getopt_state,argc,argv,"amnprsv")) != -1 ) {
		switch ( c ) {
		case 'a':
			print_mask |= PRINT_ALL;
			break;
		case 'm':
			print_mask |= PRINT_MACHINE;
			break;
		case 'n':
			print_mask |= PRINT_NODENAME;
			break;
		case 'p': 
			print_mask |= PRINT_PROCESSOR;
			break;
		case 'r': 
			print_mask |= PRINT_RELEASE;
			break;
		case 's': 
			print_mask |= PRINT_SYSNAME;
			break;
		case 'v':
			print_mask |= PRINT_VERSION;
			break;
		default:
			usage();
			/* NOTREACHED */
		}
	}

	if (!print_mask) {
		print_mask = PRINT_SYSNAME;
	}

	if (uname(&u)<0) {
		printf("error: uname not supported\r\n");
		return 0;
	}

	if (print_mask & PRINT_SYSNAME) {
		space++;
		fputs(u.sysname, stdout);
	}
	if (print_mask & PRINT_NODENAME) {
		if (space++) putchar(' ');
		fputs(u.nodename, stdout);
	}
	if (print_mask & PRINT_RELEASE) {
		if (space++) putchar(' ');
		fputs(u.release, stdout);
	}
	if (print_mask & PRINT_VERSION) {
		if (space++) putchar(' ');
		fputs(u.version, stdout);
	}
	if (print_mask & PRINT_MACHINE) {
		if (space++) putchar(' ');
		fputs(u.machine, stdout);
	}
	if (print_mask & PRINT_PROCESSOR) {
		if (space++) putchar(' ');
	}
   putchar('\r');
	putchar('\n');

	return 0;
}

/*============================================
| End of Source  : uname.c
==============================================*/
